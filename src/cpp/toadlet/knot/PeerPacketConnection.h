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

#ifndef TOADLET_KNOT_PEERPACKETCONNECTION_H
#define TOADLET_KNOT_PEERPACKETCONNECTION_H

#include <toadlet/egg/String.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/Runnable.h>
#include <toadlet/egg/Random.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/egg/net/Socket.h>
#include <toadlet/knot/Connection.h>

namespace toadlet{
namespace knot{

/*!
 * A specialized Connection that operates over UDP, and enforces reliable delivery of packets.
 * It's usage requires that both sides of the connection send the same amount of packets, since
 * the packet header is used to determine which packets need to be resent.
 */
class TOADLET_API PeerPacketConnection:public Connection,egg::Runnable{
public:
	TOADLET_SHARED_POINTERS(PeerPacketConnection);

	PeerPacketConnection(egg::net::Socket::ptr socket);
	virtual ~PeerPacketConnection();

	/// This is the amount of time(ms) that the Connection will wait before it assumes a packet is lost
	void setPacketResendTime(int time);
	inline int getPacketResendTime() const{return mPacketResendTime;}

	/// This is the number of previous packets to send each frame, useful if you are predicting several frames ahead, and have a high packet drop
	void setNumExtraPackets(int extraPackets);
	inline int getNumExtraPackets() const{return mNumExtraPackets;}

	/// This weights the calculated ping value, to make it more stable
	void setPingWeighting(float weighting);
	inline float getPingWeighting() const{return mPingWeighting;}

	/// Connect to an address:port
	bool connect(uint32 remoteHost,int remotePort);
	/// Wait for a connection
	bool accept(int localPort);

	void close();
	bool closed(){return mRun==false;}

	// This connection is always non-blocking
	int send(const tbyte *data,int length);
	int receive(tbyte *data,int length);

	/*!
	 * This is not an accurate ping value.  Since the soonest an acknowledgement of a received packet can be sent is
	 * dependent on the logic dt of the application, the sum of the ping value of both peers will always be greater
	 * the logic dt.
	 * So if your logic dt is 100 ms, you can be playing locally, and the ping on one client will be reported as
	 * anywhere from 0 - 100 ms, on average about 50.
	*/
	int getPing() const{return mPing;}

	/// Log current status
	void output();

	void run();

	/// Debug methods
	void debugSetPacketDelayTime(int time);
	void debugSetPacketDropAmount(float amount);
	void debugDropNextPacket();
	void debugRun();

protected:
	class PeerPacket{
	public:
		TOADLET_SHARED_POINTERS(PeerPacket);

		PeerPacket();
		~PeerPacket();

		void set(PeerPacket *packet);
		
		inline tbyte *getData() const{return mData;}
		inline int getDataLength() const{return mDataLength;}

		int setData(const tbyte *data,int length);

		inline int getFrame() const{return mFrame;}
		void setFrame(int frame){mFrame=frame;}

		inline int getFrameBits() const{return mFrameBits;}
		inline int getFrameBitsReferenceFrame() const{return mFrameBitsReferenceFrame;}
		void setFrameBits(int bits,int referenceFrame);

		inline uint64 getTimeHandled() const{return mTimeHandled;}
		void setTimeHandled(uint64 time){mTimeHandled=time;}

		int debugDeliverTime;

	protected:
		tbyte *mData;
		int mDataLength;
		int mDataMaxSize;

		int mFrame;
		int mFrameBits;
		int mFrameBitsReferenceFrame;

		uint64 mTimeHandled;
	};

	const static int CONNECTION_FRAME;
	const static char *CONNECTION_PACKET;
	const static int CONNECTION_PACKET_LENGTH;
	const static int CONNECTION_VERSION;

	int buildConnectionPacket(egg::io::DataStream *stream);
	bool verifyConnectionPacket(egg::io::DataStream *stream);

	egg::String toBinaryString(int n);
	inline int frameToIndex(int frame){return frame-mMasterFrame-1 + mHalfWindowSize;}

	bool updatePacketReceive();
	bool updatePacketResend();
	void updatePacketInfo(PeerPacket *packet);

	int sendPacketsToSocket(const egg::Collection<PeerPacket::ptr> &packets,int numPackets);
	int receivePacketsFromSocket(const egg::Collection<PeerPacket::ptr> &packets,int numPackets);

	egg::net::Socket::ptr mSocket;
	egg::io::MemoryStream::ptr mOutPacket;
	egg::io::DataStream::ptr mDataOutPacket;
	egg::io::MemoryStream::ptr mInPacket;
	egg::io::DataStream::ptr mDataInPacket;

	egg::Mutex::ptr mMutex;
	egg::Thread::ptr mThread;
	bool mRun;

	int mPacketResendTime;
	int mPing;
	float mPingWeighting;

	int mLocalFrame;
	int mMasterFrame;
	int mWindowSize;
	int mHalfWindowSize;
	int mNewFrameBits;
	int mReceivedFrameBits;
	int mRemoteReceivedFrameBits;
	int mNumExtraPackets;
	egg::Collection<PeerPacket::ptr> mLocalPackets;
	egg::Collection<PeerPacket::ptr> mRemotePackets;
	egg::Collection<PeerPacket::ptr> mSendingPackets;
	egg::Collection<PeerPacket::ptr> mReceivingPackets;

	egg::Random mDebugRandom;
	int mDebugPacketDelayTime;
	float mDebugPacketDropAmount;
	bool mDebugDropNextPacket;
	egg::Thread::ptr mDebugThread;
	bool mDebugRun;
};

}
}

#endif
