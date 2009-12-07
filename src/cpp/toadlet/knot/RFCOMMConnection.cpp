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
#include <toadlet/knot/RFCOMMConnection.h>
#include <string.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::net;

namespace toadlet{
namespace knot{

const int RFCOMMConnection::CONNECTION_FRAME=-1;
const char *RFCOMMConnection::CONNECTION_PACKET="toadlet::knot::rfcomm";
const int RFCOMMConnection::CONNECTION_PACKET_LENGTH=21;
const int RFCOMMConnection::CONNECTION_VERSION=1;

RFCOMMConnection::RFCOMMConnection(BluetoothClient::ptr client):
	mRun(true)
{
	mClient=client;

	int maxSize=1024;
	mOutPacket=MemoryOutputStream::ptr(new MemoryOutputStream(new char[maxSize],maxSize,true));
	mDataOutPacket=DataOutputStream::ptr(new DataOutputStream(OutputStream::ptr(mOutPacket)));
	mInPacket=MemoryInputStream::ptr(new MemoryInputStream(new char[maxSize],maxSize,true));
	mDataInPacket=DataInputStream::ptr(new DataInputStream(InputStream::ptr(mInPacket)));

	mMutex=Mutex::ptr(new Mutex());
	mThread=Thread::ptr(new Thread(this));
}

RFCOMMConnection::RFCOMMConnection(BluetoothServer::ptr server):
	mRun(true)
{
	mServer=server;

	int maxSize=1024;
	mOutPacket=MemoryOutputStream::ptr(new MemoryOutputStream(new char[maxSize],maxSize,true));
	mDataOutPacket=DataOutputStream::ptr(new DataOutputStream(OutputStream::ptr(mOutPacket)));
	mInPacket=MemoryInputStream::ptr(new MemoryInputStream(new char[maxSize],maxSize,true));
	mDataInPacket=DataInputStream::ptr(new DataInputStream(InputStream::ptr(mInPacket)));

	mMutex=Mutex::ptr(new Mutex());
	mThread=Thread::ptr(new Thread(this));
}

RFCOMMConnection::~RFCOMMConnection(){
	disconnect();
}

bool RFCOMMConnection::connect(BluetoothAddress *address){
	Logger::debug(Categories::TOADLET_KNOT,
		String("connect: protocol ")+CONNECTION_PACKET);

	bool result=mClient->connect(address);
	if(result){
		result=false;
		int amount=0;

		int size=buildConnectionPacket(mDataOutPacket);

		amount=mClient->send(mOutPacket->getOriginalDataPointer(),size);
		if(amount>0){
			Logger::debug(Categories::TOADLET_KNOT,
				"connect: sent connection packet");

			amount=mClient->receive(mInPacket->getOriginalDataPointer(),size);
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

		mInPacket->reset();
		mOutPacket->reset();
	}

	if(result){
		mThread->start();
	}

	return result;
}

bool RFCOMMConnection::accept(const String &guid){
	Logger::debug(Categories::TOADLET_KNOT,
		String("accept: protocol ")+CONNECTION_PACKET);

	bool result=mServer->accept(guid);
	if(result){
		result=false;
		int amount=0;

		int size=buildConnectionPacket(mDataOutPacket);

		amount=mServer->receive(mInPacket->getOriginalDataPointer(),size);
		if(amount>0){
			Logger::debug(Categories::TOADLET_KNOT,
				"accept: received connection packet");

			if(verifyConnectionPacket(mDataInPacket)){
				Logger::debug(Categories::TOADLET_KNOT,
					"accept: verified connection packet");

				amount=mServer->send(mOutPacket->getOriginalDataPointer(),size);
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

		mInPacket->reset();
		mOutPacket->reset();
	}

	if(result){
		mThread->start();
	}

	return result;
}

bool RFCOMMConnection::disconnect(){
	if(mClient!=NULL){
		mClient->close();
		mClient=NULL;
	}

	if(mServer!=NULL){
		mServer->close();
		mServer=NULL;
	}

	mRun=false;
	while(mThread->isAlive()){
		System::msleep(10);
	}

	return true;
}

int RFCOMMConnection::send(const char *data,int length){
	mMutex->lock();

	mDataOutPacket->writeBigInt16(length);
	mDataOutPacket->write(data,length);

	if(mClient!=NULL){
		length=mClient->send(mOutPacket->getOriginalDataPointer(),mOutPacket->getSize());
	}
	else{
		length=mServer->send(mOutPacket->getOriginalDataPointer(),mOutPacket->getSize());
	}
	mOutPacket->reset();

	mMutex->unlock();

	return length;
}

int RFCOMMConnection::receive(char *data,int length){
	mMutex->lock();

	int amount=0;
	if(mPackets.size()>0){
		memcpy(data,mPackets[0]->data,mPackets[0]->length);
		amount=mPackets[0]->length;
		mPackets.removeAt(0);
	}

	mMutex->unlock();

	return amount;
}

bool RFCOMMConnection::updatePacketReceive(){
	Packet::ptr packet(new Packet());
	int amount=0;

	if(mClient!=NULL){
		amount=mClient->receive(mInPacket->getOriginalDataPointer(),2);
	}
	else{
		amount=mServer->receive(mInPacket->getOriginalDataPointer(),2);
	}
	if(amount>0){
		packet->length=mDataInPacket->readBigInt16();
		mInPacket->reset();

		if(mClient!=NULL){
			amount=mClient->receive(packet->data,packet->length);
		}
		else{
			amount=mServer->receive(packet->data,packet->length);
		}
		if(amount>0){
			mMutex->lock();

			mPackets.add(packet);

			mMutex->unlock();
		}
	}

	return amount>0;
}

int RFCOMMConnection::buildConnectionPacket(DataOutputStream *out){
	int size=0;

	size+=out->writeBigInt32(CONNECTION_FRAME);
	size+=out->write(CONNECTION_PACKET,CONNECTION_PACKET_LENGTH);
	size+=out->writeBigInt32(CONNECTION_VERSION);

	return size; 
}

bool RFCOMMConnection::verifyConnectionPacket(DataInputStream *in){
	int header=in->readBigInt32();
	if(header!=CONNECTION_FRAME){
		return false;
	}

	char packet[CONNECTION_PACKET_LENGTH];
	in->read(packet,CONNECTION_PACKET_LENGTH);
	if(memcmp(packet,CONNECTION_PACKET,CONNECTION_PACKET_LENGTH)!=0){
		return false;
	}

	int version=in->readBigInt32();
	if(version!=CONNECTION_VERSION){
		return false;
	}

	return true;
}

void RFCOMMConnection::run(){
	while(mRun){
		if(updatePacketReceive()==false){
			break;
		}
	}
}

}
}
