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
#include <toadlet/knot/PeerPacketConnection.h>

namespace toadlet{
namespace knot{

class DebugThread:public Thread{
public:
	DebugThread(PeerPacketConnection *connection){mConnection=connection;}
	void run(){mConnection->debugRun();}
	PeerPacketConnection *mConnection;
};

PeerPacketConnection::PeerPacket::PeerPacket():
	debugDeliverTime(0),

	mData(NULL),
	mDataLength(0),
	mDataMaxSize(0),

	mFrame(0),
	mFrameBits(0),
	mFrameBitsReferenceFrame(0),

	mTimeHandled(0)
{}

PeerPacketConnection::PeerPacket::~PeerPacket(){
	if(mData!=NULL){
		delete[] mData;
		mData=NULL;
	}
}

void PeerPacketConnection::PeerPacket::set(PeerPacket *packet){
	setData(packet->getData(),packet->getDataLength());
	setFrame(packet->getFrame());
	setFrameBits(packet->getFrameBits(),packet->getFrameBitsReferenceFrame());
}

int PeerPacketConnection::PeerPacket::setData(const tbyte *data,int length){
	if(mDataMaxSize<length){
		if(mData!=NULL){
			delete[] mData;
			mData=NULL;
		}
		mData=new tbyte[length];
		mDataMaxSize=length;
	}
	memcpy(mData,data,length);
	mDataLength=length;
	return length;
}

void PeerPacketConnection::PeerPacket::setFrameBits(int bits,int referenceFrame){
	mFrameBits=bits;
	mFrameBitsReferenceFrame=referenceFrame;
}


const int PeerPacketConnection::CONNECTION_FRAME=-1;
const char *PeerPacketConnection::CONNECTION_PACKET="toadlet::knot::ppp";
const int PeerPacketConnection::CONNECTION_PACKET_LENGTH=18;
const int PeerPacketConnection::CONNECTION_VERSION=1;

PeerPacketConnection::PeerPacketConnection(Socket::ptr socket):
	//mMutex,
	//mThread,
	mRun(false),

	mPacketResendTime(1000),
	mPing(-1),
	mPingWeighting(0.1),

	mLocalFrame(0),
	mMasterFrame(0),
	mWindowSize(0),
	mHalfWindowSize(0),
	mNewFrameBits(0),
	mReceivedFrameBits(0),
	mRemoteReceivedFrameBits(0),
	mNumExtraPackets(0),

	mDebugPacketDelayTime(0),
	mDebugPacketDropAmount(0),
	mDebugDropNextPacket(false),
	//mDebugThread
	mDebugRun(false)
{
	int i;

	mSocket=socket;

	int maxSize=1024;
	mOutPacket=MemoryStream::ptr(new MemoryStream(new tbyte[maxSize],maxSize,0,true));
	mDataOutPacket=DataStream::ptr(new DataStream(Stream::ptr(mOutPacket)));
	mInPacket=MemoryStream::ptr(new MemoryStream(new tbyte[maxSize],maxSize,maxSize,true));
	mDataInPacket=DataStream::ptr(new DataStream(Stream::ptr(mInPacket)));

	mWindowSize=32;
	mHalfWindowSize=mWindowSize>>1;
	mLocalPackets.resize(mWindowSize);
	mRemotePackets.resize(mWindowSize);
	for(i=0;i<mLocalPackets.size();++i){
		mLocalPackets[i]=PeerPacket::ptr(new PeerPacket());
		mRemotePackets[i]=PeerPacket::ptr(new PeerPacket());
	}

	int numExtraPackets=32;
	mSendingPackets.resize(numExtraPackets);
	mReceivingPackets.resize(numExtraPackets);
	for(i=0;i<mSendingPackets.size();++i){
		mSendingPackets[i]=PeerPacket::ptr(new PeerPacket());
		mReceivingPackets[i]=PeerPacket::ptr(new PeerPacket());
	}

	setNumExtraPackets(8);

	mMutex=Mutex::ptr(new Mutex());
	mThread=Thread::ptr(new Thread(this));
}

PeerPacketConnection::~PeerPacketConnection(){
	close();
}

void PeerPacketConnection::setPacketResendTime(int time){
	mPacketResendTime=time;
}

void PeerPacketConnection::setNumExtraPackets(int extraPackets){
	mNumExtraPackets=extraPackets;
}

void PeerPacketConnection::setPingWeighting(float weighting){
	mPingWeighting=weighting;
}

bool PeerPacketConnection::connect(uint32 remoteHost,int remotePort){
	Logger::debug(Categories::TOADLET_KNOT,
		String("connect: protocol ")+CONNECTION_PACKET);

	bool result=false;
	int amount=0;
	TOADLET_TRY
		int size=buildConnectionPacket(mDataOutPacket);

		int tries=0;
		while(tries<3){
			amount=mSocket->sendTo(mOutPacket->getOriginalDataPointer(),size,remoteHost,remotePort);
			if(amount>0){
				Logger::debug(Categories::TOADLET_KNOT,
					String("connect: sent connection packet to:")+Socket::ipToString(remoteHost)+":"+remotePort);

				amount=mSocket->receiveFrom(mInPacket->getOriginalDataPointer(),size,remoteHost,remotePort);
				if(amount>0){
					Logger::debug(Categories::TOADLET_KNOT,
						String("connect: received connection packet from:")+Socket::ipToString(remoteHost)+":"+remotePort);

					if(verifyConnectionPacket(mDataInPacket)){
						Logger::debug(Categories::TOADLET_KNOT,
							"connect: verified connection packet");

						result=true;
						break;
					}
					else{
						Logger::alert(Categories::TOADLET_KNOT,
							"connect: error verifying connection packet");
					}

					mInPacket->reset();
				}
				else{
					Logger::alert(Categories::TOADLET_KNOT,
						"connect: error receiving connection packet");
				}
			}

			tries++;
		}
	TOADLET_CATCH(const Exception &){result=false;}

	mInPacket->reset();
	mOutPacket->reset();

	mDebugRandom.setSeed(System::mtime());

	if(result){
		mSocket->connect(remoteHost,remotePort);

		mRun=true;
		mThread->start();
	}

	return result;
}

// localPort is currently unused, since its expected that the socket is already bound
bool PeerPacketConnection::accept(int localPort){
	uint32 remoteHost=0;
	int remotePort=0;

	Logger::debug(Categories::TOADLET_KNOT,
		String("accept: protocol ")+CONNECTION_PACKET);

	bool result=false;
	int amount=0;
	TOADLET_TRY
		int size=buildConnectionPacket(mDataOutPacket);

		int tries=0;
		while(tries<3){
			TOADLET_TRY
				amount=mSocket->receiveFrom(mInPacket->getOriginalDataPointer(),size,remoteHost,remotePort);
			TOADLET_CATCH(const Exception &){}
			if(amount>0){
				Logger::debug(Categories::TOADLET_KNOT,
					String("accept: received connection packet from:")+Socket::ipToString(remoteHost)+":"+remotePort);

				if(verifyConnectionPacket(mDataInPacket)){
					Logger::debug(Categories::TOADLET_KNOT,
						"accept: verified connection packet");

					amount=mSocket->sendTo(mOutPacket->getOriginalDataPointer(),size,remoteHost,remotePort);
					if(amount>0){
						Logger::debug(Categories::TOADLET_KNOT,
							String("accept: sent connection packet to:")+Socket::ipToString(remoteHost)+":"+remotePort);

						result=true;
						break;
					}
				}
				else{
					Logger::alert(Categories::TOADLET_KNOT,
						"accept: error verifying connection packet");
				}

				mInPacket->reset();
			}
			else{
				Logger::alert(Categories::TOADLET_KNOT,
					"accept: error receiving connection packet");
			}

			tries++;
		}
	TOADLET_CATCH(const Exception &){result=false;}

	mInPacket->reset();
	mOutPacket->reset();

	mDebugRandom.setSeed(System::mtime());

	if(result){
		mSocket->connect(remoteHost,remotePort);

		mRun=true;
		mThread->start();
	}

	return result;
}

void PeerPacketConnection::close(){
	mRun=false;
	mDebugRun=false;

	TOADLET_TRY
		if(mSocket!=NULL){
			mSocket->close();
		}
	TOADLET_CATCH(const Exception &){}

	if(mThread!=NULL){
		Thread::ptr thread=mThread;
		mThread=NULL;
		thread->join();
	}

	if(mDebugThread!=NULL){
		Thread::ptr thread=mDebugThread;
		mDebugThread=NULL;
		thread->join();
	}

	mSocket=NULL;
}

int PeerPacketConnection::send(const tbyte *data,int length){
	mMutex->lock();

	mLocalFrame++;

	int index=frameToIndex(mLocalFrame);
	if(index<0 || index>=mLocalPackets.size()){
		mMutex->unlock();

		Error::unknown(Categories::TOADLET_KNOT,
			String("send: local packet out of bounds. ") +
				"local packet frame:"+mLocalFrame+" master frame:"+mMasterFrame); 
		return -1;
	}

	PeerPacket::ptr packet=mLocalPackets[index];
	packet->setFrame(mLocalFrame);
	packet->setData(data,length);
	updatePacketInfo(packet);

	int i;
	for(i=0;i<mNumExtraPackets+1 && (index-i)>=0 && mLocalPackets[index-i]->getFrame()>0;++i){
		mSendingPackets[i]->set(mLocalPackets[index-i]);
	}

	mMutex->unlock();

	sendPacketsToSocket(mSendingPackets,i);

	updatePacketResend();

	return length;
}

int PeerPacketConnection::receive(tbyte *data,int length){
	int index=mHalfWindowSize;

	mMutex->lock();

	if((mNewFrameBits&(0x1<<index))!=0 && mRemotePackets[index]->debugDeliverTime<=(int)System::mtime()){ // If the NewPacketBit for the current packet is on
		mNewFrameBits&=(~(0x1<<index)); // Turn it off

		mMasterFrame++;
		mNewFrameBits>>=1; // Shift the new packet bits
		mReceivedFrameBits>>=1; // Shift the recevied frame bits
		mRemoteReceivedFrameBits>>=1; // Shift the remote received frame bits

		PeerPacket::ptr packet=mRemotePackets[index];
		if(packet->getFrame()!=mMasterFrame){
			mMutex->unlock();

			Error::unknown(Categories::TOADLET_KNOT,
				String("receive: remote packet out of order. ") +
					"remote packet frame:"+packet->getFrame()+" master frame:"+mMasterFrame); 
			return -1;
		}
		else{
			length=packet->getDataLength()<length?packet->getDataLength():length;
			memcpy(data,packet->getData(),length);
		}

		// Rotate the packet stacks
		{
			PeerPacket::ptr packet=mLocalPackets.get(0);
			packet->setFrame(0); // Invalidate packet
			mLocalPackets.add(packet);
			mLocalPackets.removeAt(0);
		}
		{
			PeerPacket::ptr packet=mRemotePackets.get(0);
			packet->setFrame(0); // Invalidate packet
			mRemotePackets.add(packet);
			mRemotePackets.removeAt(0);
		}
	}
	else{
		length=0;
	}

	mMutex->unlock();

	updatePacketResend();

	return length;
}

void PeerPacketConnection::output(){
	int i;

	mMutex->lock();

	String string;

	string=string+"-------------------\n";
	string=string+String("Master Frame     :")+mMasterFrame+"\n";
	string=string+String("Local Frame      :")+mLocalFrame+"\n";
	string=string+String("Window Size      :")+mWindowSize+"\n";
	string=string+String("New Frame Bits   :")+toBinaryString(mNewFrameBits)+"\n";
	string=string+String("Recv Frame Bits  :")+toBinaryString(mReceivedFrameBits)+"\n";
	string=string+String("Remote Recv Bits :")+toBinaryString(mRemoteReceivedFrameBits)+"\n";

	string=string+String("Loc :");
	for(i=mLocalPackets.size()-5;i>=5;--i){
		int frame=mLocalPackets[i]->getFrame();
		if(frame>=10){
			string=string+frame+" ";
		}
		else{
			string=string+frame+"  ";
		}
	}
	string=string+"\n";

	string=string+String("Rmt :");
	for(i=mRemotePackets.size()-5;i>=5;--i){
		int frame=mRemotePackets[i]->getFrame();
		if(frame>=10){
			string=string+frame+" ";
		}
		else{
			string=string+frame+"  ";
		}
	}
	string=string+"\n";

	mMutex->unlock();

	Logger::alert(Categories::TOADLET_KNOT,string);
}

String PeerPacketConnection::toBinaryString(int n){
	String r;

	int i;
	for(i=0;i<(int)sizeof(n)*8;i++,n<<=1){
		r=r+((i%8==0)?" ":"")+(int)((n>>((sizeof(n)*8)-1))&0x1);
	}

	return r;
}

int PeerPacketConnection::buildConnectionPacket(DataStream *stream){
	int size=0;

	size+=stream->writeBInt32(CONNECTION_FRAME);
	size+=stream->write((tbyte*)CONNECTION_PACKET,CONNECTION_PACKET_LENGTH);
	size+=stream->writeBInt32(CONNECTION_VERSION);

	return size; 
}

bool PeerPacketConnection::verifyConnectionPacket(DataStream *stream){
	int header=stream->readBInt32();
	if(header!=CONNECTION_FRAME){
		return false;
	}

	char packet[CONNECTION_PACKET_LENGTH];
	stream->read((tbyte*)packet,CONNECTION_PACKET_LENGTH);
	if(memcmp(packet,CONNECTION_PACKET,CONNECTION_PACKET_LENGTH)!=0){
		return false;
	}

	int version=stream->readBInt32();
	if(version!=CONNECTION_VERSION){
		return false;
	}

	return true;
}

bool PeerPacketConnection::updatePacketReceive(){
	int i,j;

	int numPackets=receivePacketsFromSocket(mReceivingPackets,mReceivingPackets.size());
	if(numPackets>0){
		mMutex->lock();

		int index=frameToIndex(mReceivingPackets[0]->getFrame());
		if(index>=mRemotePackets.size()){
			mMutex->unlock();

			Error::unknown(Categories::TOADLET_KNOT,
				String("update: remote packet out of bounds. ") +
					"remote packet frame:"+mReceivingPackets[0]->getFrame()+" master frame:"+mMasterFrame); 
			return -1;
		}
		else{
			int time=System::mtime();
			for(i=0;i<numPackets;++i){
				index=frameToIndex(mReceivingPackets[i]->getFrame());
				if(index>=0 && mReceivingPackets[i]->getFrame()!=mRemotePackets[index]->getFrame()){  // We do not already have this packet
					PeerPacket::ptr packet=mRemotePackets[index];
					packet->set(mReceivingPackets[i]);

					mNewFrameBits|=(0x1<<index);
					mReceivedFrameBits|=(0x1<<index);
					int bits=packet->getFrameBits();
					int shift=mMasterFrame-packet->getFrameBitsReferenceFrame();
					if(shift>0){
						// Shift the bits right
						mRemoteReceivedFrameBits=bits>>shift;
					}
					else if(shift<0){
						// Shift the bits left, and fill in the right blanks with ones
						mRemoteReceivedFrameBits=bits<<-shift;
						mRemoteReceivedFrameBits|=(~((~0)<<-shift));
					}
					else{
						mRemoteReceivedFrameBits=bits;
					}

					for(j=mWindowSize-1;j>0;--j){
						if(((mRemoteReceivedFrameBits>>j)&0x1)>0){
							int newPing=(time-mLocalPackets[j]->getTimeHandled()) + mDebugPacketDelayTime;
							if(mPing<0){
								mPing=newPing;
							}
							else{
								mPing=(int)((float)mPing*(1.0-mPingWeighting)+(float)newPing*mPingWeighting);
							}
							break;
						}
					}

					if(mDebugPacketDelayTime>0){
						packet->debugDeliverTime=System::mtime() + mDebugPacketDelayTime;
					}
					else{
						packet->debugDeliverTime=0;
					}
				}
			}
		}

		mMutex->unlock();
	}

	return numPackets>0;
}

bool PeerPacketConnection::updatePacketResend(){
	mMutex->lock();

	bool result=false;
	int frame=mMasterFrame-mHalfWindowSize+1>=1?mMasterFrame-mHalfWindowSize+1:1;
	int index=0;
	for(;frame<mLocalFrame;++frame){
		index=frameToIndex(frame);
		if(((mRemoteReceivedFrameBits>>index)&0x1)==0){
			// Send the packet here instead of breaking and sending one at the end.
			//  This way we are guaranteed to attempt to send all missing packets
			//  instead of just the most ancient one.  This prevents a deadlock issue
			//  that can be caused by a small FrameBuffer in the application, where one
			//  peer ends up waiting for a specific dropped packet, but the second peer
			//  keeps resending an earlier packet peer one doesnt need.
			if(index>0 && frame<mLocalFrame){
				PeerPacket *packet=mLocalPackets[index];
				if(packet->getFrame()>0 && (System::mtime()-packet->getTimeHandled())>(uint32)(mPacketResendTime)){
					updatePacketInfo(packet);
					mLocalPackets[0]->set(packet);
					result=sendPacketsToSocket(mLocalPackets,1)>0;
				}
			}
		}
	}

	mMutex->unlock();

	return result;
}

void PeerPacketConnection::updatePacketInfo(PeerPacket *packet){
	packet->setFrameBits(mReceivedFrameBits,mMasterFrame);
	packet->setTimeHandled(System::mtime());
}

int PeerPacketConnection::sendPacketsToSocket(const Collection<PeerPacket::ptr> &packets,int numPackets){
	int i=0;
	PeerPacket *packet=packets[0];
	int amount=0;

	mDataOutPacket->writeBInt32(packet->getFrame());
	mDataOutPacket->writeBInt32(packet->getFrameBits());
	mDataOutPacket->writeBInt32(packet->getFrameBitsReferenceFrame());
	mDataOutPacket->writeInt8(numPackets);
	for(i=0;i<numPackets;++i){
		packet=packets[i];
		mDataOutPacket->writeBInt16(packet->getDataLength());
		if(mDataOutPacket->write(packet->getData(),packet->getDataLength())<packet->getDataLength()){
			Error::unknown(Categories::TOADLET_KNOT,
				"out packet buffer overflow");
			return -1;
		}
	}

	String frameNames;
	for(i=0;i<numPackets;++i){frameNames=frameNames+packets[i]->getFrame()+",";}

	if(mDebugDropNextPacket==false && (mDebugPacketDropAmount==0 || mDebugRandom.nextFloat(0,1)>mDebugPacketDropAmount)){
		Logger::excess(Categories::TOADLET_KNOT,String("sending frames : ")+frameNames+" size:"+mOutPacket->length());

		amount=mSocket->send(mOutPacket->getOriginalDataPointer(),mOutPacket->length());
	}
	else{
		Logger::excess(Categories::TOADLET_KNOT,String("dropping frames: ")+frameNames);

		mDebugDropNextPacket=false;
	}
	mOutPacket->reset();

	return numPackets;
}

int PeerPacketConnection::receivePacketsFromSocket(const Collection<PeerPacket::ptr> &packets,int maxNumPackets){
	int i;
	bool result=false;
	int numPackets=0;
	int amount=0;
	int time=System::mtime();

	TOADLET_TRY
		amount=mSocket->receive(mInPacket->getOriginalDataPointer(),mInPacket->length());
		if(amount>0){
			int frame=mDataInPacket->readBInt32();
			if(frame>0){ // Is a data packet
				int frameBits=mDataInPacket->readBInt32();
				int frameBitsReferenceFrame=mDataInPacket->readBInt32();
				numPackets=mDataInPacket->readInt8();
				numPackets=maxNumPackets<numPackets?maxNumPackets:numPackets;

				for(i=0;i<numPackets;++i){
					int length=mDataInPacket->readBInt16();

					PeerPacket *packet=packets[i];
					packet->setData(mInPacket->getCurrentDataPointer(),length);
					mInPacket->seek(mInPacket->position()+length);
					packet->setFrame(frame-i);
					packet->setFrameBits(frameBits,frameBitsReferenceFrame);
					packet->setTimeHandled(time);
				}

				result=true;
			}
			mInPacket->reset();
		}
	TOADLET_CATCH(const Exception &){}
	// We ignore receive errors in this connection, since they can possibly be ignored and the packet will get resent

	#if defined(TOADLET_DEBUG)
		if(numPackets>0){
			String frameNames;
			for(i=0;i<numPackets;++i) frameNames=frameNames+packets[i]->getFrame()+",";
			Logger::excess(Categories::TOADLET_KNOT,String("receive frames : ")+frameNames);
		}
	#endif

	return numPackets;
}

void PeerPacketConnection::run(){
	while(mRun){
		updatePacketReceive();
	}
}

void PeerPacketConnection::debugSetPacketDelayTime(int time){
	mMutex->lock();
		mDebugPacketDelayTime=time;

		if(mDebugPacketDelayTime>0 && mDebugThread==NULL){
			mDebugThread=Thread::ptr(new DebugThread(this));
			mDebugRun=true;
			mDebugThread->start();
		}
		else if(mDebugPacketDelayTime==0 && mDebugThread!=NULL){
			mDebugRun=false;
			Thread::ptr thread=mDebugThread;
			mDebugThread=NULL;
			thread->join();

			int i;
			for(i=0;i<mRemotePackets.size();++i){
				PeerPacket::ptr packet=mRemotePackets[i];
				if(packet->debugDeliverTime!=0){
					packet->debugDeliverTime=0;
				}
			}
		}
	mMutex->unlock();
}

void PeerPacketConnection::debugSetPacketDropAmount(float amount){
	mMutex->lock();
		mDebugPacketDropAmount=amount;
	mMutex->unlock();
}

void PeerPacketConnection::debugDropNextPacket(){
	mMutex->lock();
		mDebugDropNextPacket=true;
	mMutex->unlock();
}

void PeerPacketConnection::debugRun(){
	while(mDebugRun){
//		mMutex->lock();
//			int i;
//			for(i=0;i<mRemotePackets.size();++i){
//				PeerPacket::ptr packet=mPackets[i];
//				if(packet->debugDeliverTime<=System::mtime()){
//					packet->debugDeliverTime=0;
//					mConnector->dataReady(this);
//				}
//			}
//		mMutex->unlock();
		System::msleep(10);
	}
}

}
}