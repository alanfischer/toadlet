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
#include <toadlet/knot/TCPConnector.h>
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

TCPConnection::TCPConnection(egg::net::Socket::ptr socket):
	mClient(false),
	//mSocket,
	//mOutPacket,
	//mDataOutPacket,
	//mInPacket,
	//mDataInPacket,
	mConnectionListener(NULL),

	//mMutex,
	//mPackets,
	//mFreePackets,
	mReceiveError(false),

	//mDebugRandom,
	mDebugPacketDelayMinTime(0),
	mDebugPacketDelayMaxTime(0)
{
	mPackets.reserve(32);

	int maxSize=4096*16;
	mOutPacket=MemoryStream::ptr(new MemoryStream(new uint8[maxSize],maxSize,0,true));
	mDataOutPacket=DataStream::ptr(new DataStream(Stream::ptr(mOutPacket)));
	mInPacket=MemoryStream::ptr(new MemoryStream(new uint8[maxSize],maxSize,maxSize,true));
	mDataInPacket=DataStream::ptr(new DataStream(Stream::ptr(mInPacket)));

	mSocket=socket;
}

TCPConnection::~TCPConnection(){
	close();
}

bool TCPConnection::connect(uint32 remoteHost,int remotePort){
	Socket::ptr socket;
	if(mSocket==NULL){
		socket=Socket::ptr(Socket::createTCPSocket());
	}
	else{
		socket=mSocket;
	}

	bool result=false;
	TOADLET_TRY
		result=socket->connect(remoteHost,remotePort);
	TOADLET_CATCH(const Exception &){result=false;}
	if(result){
		connect(socket);
	}

	return result;
}

bool TCPConnection::connect(Socket::ptr socket){
	mSocket=socket;

	Logger::debug(Categories::TOADLET_KNOT,
		String("connect: protocol ")+CONNECTION_PACKET);

	bool result=false;
	int amount=0;
	TOADLET_TRY
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
	TOADLET_CATCH(const Exception &){result=false;}

	mInPacket->reset();
	mOutPacket->reset();

	mClient=true;

	if(result && mConnectionListener!=NULL){
		mConnectionListener->connected(this);
	}

	return result;
}

bool TCPConnection::accept(int localPort){
	Socket::ptr socket;
	if(mSocket==NULL){
		socket=Socket::ptr(Socket::createTCPSocket());
	}
	else{
		socket=mSocket;
	}

	Socket::ptr clientSocket;
	TOADLET_TRY
		// If we were passed in a server Socket to use, then we assume it has already been bound.
		// This allows users to have more control over external sockets
		if(socket!=mSocket){
			socket->bind(localPort);
		}
		socket->listen(1);
		clientSocket=Socket::ptr(socket->accept());
	TOADLET_CATCH(const Exception &){clientSocket=NULL;}
	bool result=false;
	if(clientSocket!=NULL){
		result=accept(clientSocket);
	}

	return result;
}

bool TCPConnection::accept(Socket::ptr socket){
	mSocket=socket;

	Logger::debug(Categories::TOADLET_KNOT,
		String("accept: protocol ")+CONNECTION_PACKET);

	bool result=false;
	TOADLET_TRY
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
					"accept: error verifying connection packet");
			}
		}
	TOADLET_CATCH(const Exception &){result=false;}

	mInPacket->reset();
	mOutPacket->reset();

	mClient=false;

	if(result && mConnectionListener!=NULL){
		mConnectionListener->connected(this);
	}

	return result;
}

void TCPConnection::close(){
	TOADLET_TRY
		if(mSocket!=NULL){
			mSocket->close();
		}
	TOADLET_CATCH(const Exception &){}

	if(mConnectionListener!=NULL){
		TCPConnector *listener=mConnectionListener;
		mConnectionListener=NULL;
		listener->disconnected(this);
	}
}

int TCPConnection::send(const byte *data,int length){
	int amount=0;

	mDataOutPacket->writeBigInt16(length);
	amount=mDataOutPacket->write(data,length);
	if(amount>=length){
		TOADLET_TRY
			if(mSocket!=NULL){
				amount=mSocket->send(mOutPacket->getOriginalDataPointer(),mOutPacket->length());
			}
		TOADLET_CATCH(const Exception &){amount=-1;}

		// We need to account for the bigInt we write first
		if(amount>0){ amount-=2; }
	}
	else{
		amount=-2;
	}

	mOutPacket->reset();

	if(amount<0 && closed()==false){
		close();
	}

	return amount;
}

int TCPConnection::receive(byte *data,int length){
	int amount=0;
	int numPackets=0;

	TOADLET_TRY
		if(mSocket!=NULL && mSocket->pollRead(0)==true){
			amount=mSocket->receive(mInPacket->getOriginalDataPointer(),2);
			if(amount>0){
				int packetLength=mDataInPacket->readBigInt16();
				mInPacket->reset();

				amount=mSocket->receive(data,packetLength<length?packetLength:length);
				if(amount>=0){
					int remaining=amount-packetLength;
					while(remaining>0){
						remaining-=mSocket->receive(mDummyData,remaining<mDummyDataLength?remaining:mDummyDataLength);
					}
				}
				amount=packetLength;
			}
		}
	TOADLET_CATCH(const Exception &){amount=-1;}

	if(amount<0 && closed()==false){
		close();
	}

	return amount;
}

/*

	while(true){
		if(mSocket->pollRead(0)==true){
			amount=mSocket->receive(mInPacket->getOriginalDataPointer(),2);
		}
		
	}
	if(mDebugPacketDelayMaxTime>0
	mMutex->lock();
		// Start or notify the debug thread to stop if necessary
		if(mSocket->closed()==false && mDebugPacketDelayMinTime>0 && mDebugThread==NULL){
			mDebugThread=Thread::ptr(new Thread(this));
			mDebugRun=true;
			mDebugThread->start();
		}
		else if(mDebugPacketDelayMaxTime==0 && mDebugThread!=NULL){
			if(mDebugThread->isAlive()){
				mDebugRun=false;
			}
			else{
				mDebugThread=NULL;
			}

			int i;
			for(i=0;i<mPackets.size();++i){
				Packet::ptr packet=mPackets[i];
				if(packet->debugDeliverTime!=0){
					packet->debugDeliverTime=0;
				}
			}
		}
		numPackets=mPackets.size();
	mMutex->unlock();

	bool needReceive=true;

	// If we have any stored packets, or we're in packet waiting mode
	if(numPackets>0 || mDebugThread!=NULL){
		needReceive=false;
		while(true){
			mMutex->lock();
				if(mPackets.size()>0){
					Packet::ptr packet=mPackets[0];
					if(packet->debugDeliverTime<=System::mtime()){
						memcpy(data,packet->data,packet->length<length?packet->length:length);
						amount=packet->length;
						mPackets.removeAt(0);
						mFreePackets.add(packet);
						packet=NULL;

						mMutex->unlock();
						break;
					}
				}
				else if(mReceiveError){
					mReceiveError=false;
					amount=-1;

					mMutex->unlock();
					break;
				}
				// Check in case the debug thread has ended, and has no waiting packets for us, so we dont get stuck looping
				else if(mDebugThread!=NULL && mDebugThread->isAlive()==false && mPackets.size()==0){
					needReceive=true;
					break;
				}
			mMutex->unlock();

			if(mBlocking){
				System::msleep(10);
			}
			else{
				break;
			}
		}
	}

*/
void TCPConnection::setConnectionListener(TCPConnector *listener){
	mConnectionListener=listener;
}

void TCPConnection::debugSetPacketDelayTime(int minTime,int maxTime){
	if(minTime<0) minTime=0;
	if(maxTime<0) maxTime=0;
	if(maxTime<minTime) maxTime=minTime;

	mDebugPacketDelayMinTime=minTime;
	mDebugPacketDelayMaxTime=maxTime;
}
/*
void TCPConnection::run(){
	while(mDebugRun){
		int amount=0;

		TOADLET_TRY
			amount=mSocket->receive(mInPacket->getOriginalDataPointer(),2);
			if(amount>0){
				int packetLength=mDataInPacket->readBigInt16();
				mInPacket->reset();

				Packet::ptr packet;
				mMutex->lock();
					if(mFreePackets.size()>0){
						packet=mFreePackets[0];
						packet->reset();
						mFreePackets.removeAt(0);
					}
					else{
						packet=Packet::ptr(new Packet());
					}
				mMutex->unlock();
				packet->length=packetLength;

				amount=mSocket->receive(packet->data,packetLength);
				mMutex->lock();
				if(amount>=0){
					if(mDebugPacketDelayMaxTime>0){
						packet->debugDeliverTime=System::mtime() + mDebugRandom.nextInt(mDebugPacketDelayMinTime,mDebugPacketDelayMaxTime);
					}
					mPackets.add(packet);
				}
				else{
					mFreePackets.add(packet);
				}
				packet=NULL;
				mMutex->unlock();
			}
		TOADLET_CATCH(const Exception &){amount=-1;}

		if(amount<=0){
			mReceiveError=true;
		}

		System::msleep(0);
	}
}
*/
int TCPConnection::buildConnectionPacket(DataStream *stream){
	int size=0;

	size+=stream->writeBigInt32(CONNECTION_FRAME);
	size+=stream->write((byte*)CONNECTION_PACKET,CONNECTION_PACKET_LENGTH);
	size+=stream->writeBigInt32(CONNECTION_VERSION);

	return size; 
}

bool TCPConnection::verifyConnectionPacket(DataStream *stream){
	int header=stream->readBigInt32();
	if(header!=CONNECTION_FRAME){
		return false;
	}

	char packet[CONNECTION_PACKET_LENGTH];
	stream->read((byte*)packet,CONNECTION_PACKET_LENGTH);
	if(memcmp(packet,CONNECTION_PACKET,CONNECTION_PACKET_LENGTH)!=0){
		return false;
	}

	int version=stream->readBigInt32();
	if(version!=CONNECTION_VERSION){
		return false;
	}

	return true;
}

}
}
