/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include <toadlet/knot/TCPConnection.h>
#include <string.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::net;

namespace toadlet{
namespace knot{

const int TCPConnection::CONNECTION_FRAME=-1;
const char *TCPConnection::CONNECTION_PACKET="toadlet::knot::tcp";
const int TCPConnection::CONNECTION_PACKET_LENGTH=18;
const int TCPConnection::CONNECTION_VERSION=1;

TCPConnection::TCPConnection(Socket::ptr socket):
	mRun(true),

	mDebugPacketDelayMinTime(0),
	mDebugPacketDelayMaxTime(0)
{
	mSocket=socket;

	int maxSize=1024;
	mOutPacket=MemoryStream::ptr(new MemoryStream(new char[maxSize],maxSize,true));
	mDataOutPacket=DataStream::ptr(new DataStream(Stream::ptr(mOutPacket)));
	mInPacket=MemoryStream::ptr(new MemoryStream(new char[maxSize],maxSize,true));
	mDataInPacket=DataStream::ptr(new DataStream(Stream::ptr(mInPacket)));

	mMutex=Mutex::ptr(new Mutex());
	mThread=Thread::ptr(new Thread(this));
}

TCPConnection::~TCPConnection(){
	disconnect();
}

bool TCPConnection::connect(const String &address,int port){
	return connect(Socket::stringToIP(address),port);
}

bool TCPConnection::connect(int ip,int port){
	uint32 remoteIP=ip;
	int remotePort=port;

	Logger::debug(Categories::TOADLET_KNOT,
		String("connect: protocol ")+CONNECTION_PACKET);

	bool result=false;
	int amount=0;
	TOADLET_TRY
		result=mSocket->connect(remoteIP,remotePort);
		if(result){
			result=false;
			int size=buildConnectionPacket(mDataOutPacket);

			amount=mSocket->send(mOutPacket->getOriginalDataPointer(),size);
			if(amount>0){
				Logger::debug(Categories::TOADLET_KNOT,
					"connect: sent connection packet");

				amount=mSocket->receive(mInPacket->getOriginalDataPointer(),size);
				if(amount>0){
					Logger::debug(Categories::TOADLET_KNOT,
						"connect: received connection packet");

					if(verifyConnectionPacket(mDataInPacket)){
						Logger::debug(Categories::TOADLET_KNOT,
							"connect: verified connection packet");

						result=true;
					}
					else{
						Logger::alert(Categories::TOADLET_KNOT,
							"connect: error verifying connection packet");
					}
				}
			}
		}
	TOADLET_CATCH(const Exception &){result=false;}

	mInPacket->reset();
	mOutPacket->reset();

	if(result){
		skipStart();
	}

	return result;
}

bool TCPConnection::accept(){
	mSocket->listen(1);

	Logger::debug(Categories::TOADLET_KNOT,
		String("accept: protocol ")+CONNECTION_PACKET);

	bool result=false;
	TOADLET_TRY
		Socket::ptr socket=Socket::ptr(mSocket->accept());
		if(socket!=NULL){
			mSocket=socket;
			int amount=0;

			int size=buildConnectionPacket(mDataOutPacket);

			amount=mSocket->receive(mInPacket->getOriginalDataPointer(),size);
			if(amount>0){
				Logger::debug(Categories::TOADLET_KNOT,
					"accept: received connection packet");

				if(verifyConnectionPacket(mDataInPacket)){
					Logger::debug(Categories::TOADLET_KNOT,
						"accept: verified connection packet");

					amount=mSocket->send(mOutPacket->getOriginalDataPointer(),size);
					if(amount>0){
						Logger::debug(Categories::TOADLET_KNOT,
							"accept: sent connection packet");

						result=true;
					}
				}
				else{
					Logger::alert(Categories::TOADLET_KNOT,
						"connect: error verifying connection packet");
				}
			}
		}
	TOADLET_CATCH(const Exception &){result=false;}

	mInPacket->reset();
	mOutPacket->reset();

	if(result){
		skipStart();
	}

	return result;
}

void TCPConnection::skipStart(){
	mThread->start();
}

bool TCPConnection::disconnect(){
	TOADLET_TRY
		if(mSocket!=NULL){
			mSocket->close();
		}
	TOADLET_CATCH(const Exception &){}

	mRun=false;
	while(mThread->isAlive()){
		System::msleep(10);
	}

	return true;
}

int TCPConnection::send(const char *data,int length){
	mMutex->lock();
		mDataOutPacket->writeBigInt16(length);
		mDataOutPacket->write(data,length);

		TOADLET_TRY
			if((length=mSocket->send(mOutPacket->getOriginalDataPointer(),mOutPacket->length()))<0)
		TOADLET_CATCH(const Exception &){length=-1;}

		mOutPacket->reset();
	mMutex->unlock();

	return length;
}

int TCPConnection::receive(char *data,int length){
	int amount=0;
	mMutex->lock();
		if(mPackets.size()>0){
			Packet::ptr packet=mPackets[0];
			if(packet->debugDeliverTime<=System::mtime()){
				memcpy(data,packet->data,packet->length);
				amount=packet->length;
				mPackets.removeAt(0);
				mFreePackets.add(packet);
			}
		}
	mMutex->unlock();

	return amount;
}

void TCPConnection::run(){
	while(mRun){
		if(updatePacketReceive()==false){
			break;
		}
	}
}

void TCPConnection::debugSetPacketDelayTime(int minTime,int maxTime){
	mDebugPacketDelayMinTime=minTime;
	mDebugPacketDelayMaxTime=maxTime;
}

int TCPConnection::buildConnectionPacket(DataStream *stream){
	int size=0;

	size+=stream->writeBigInt32(CONNECTION_FRAME);
	size+=stream->write(CONNECTION_PACKET,CONNECTION_PACKET_LENGTH);
	size+=stream->writeBigInt32(CONNECTION_VERSION);

	return size; 
}

bool TCPConnection::verifyConnectionPacket(DataStream *stream){
	int header=stream->readBigInt32();
	if(header!=CONNECTION_FRAME){
		return false;
	}

	char packet[CONNECTION_PACKET_LENGTH];
	stream->read(packet,CONNECTION_PACKET_LENGTH);
	if(memcmp(packet,CONNECTION_PACKET,CONNECTION_PACKET_LENGTH)!=0){
		return false;
	}

	int version=stream->readBigInt32();
	if(version!=CONNECTION_VERSION){
		return false;
	}

	return true;
}

bool TCPConnection::updatePacketReceive(){
	int amount=0;

	TOADLET_TRY
		amount=mSocket->receive(mInPacket->getOriginalDataPointer(),2);
		if(amount>0){
			Packet::ptr packet;
			if(mFreePackets.size()>0){
				packet=mFreePackets[0];
				packet->reset();
				mFreePackets.removeAt(0);
			}
			else{
				packet=Packet::ptr(new Packet());
			}
			packet->length=mDataInPacket->readBigInt16();
			mInPacket->reset();

			amount=mSocket->receive(packet->data,packet->length);
			if(amount>0){
				mMutex->lock();
					if(mDebugPacketDelayMaxTime>0){
						packet->debugDeliverTime=System::mtime() + mDebugRandom.nextInt(mDebugPacketDelayMinTime,mDebugPacketDelayMaxTime);
					}
					mPackets.add(packet);
				mMutex->unlock();
			}
			else{
				mFreePackets.add(packet);
			}
		}
	TOADLET_CATCH(const Exception &){amount=0;}

	return amount>0;
}

}
}
