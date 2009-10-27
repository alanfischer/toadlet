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

#ifndef TOADLET_KNOT_TCPCONNECTION_H
#define TOADLET_KNOT_TCPCONNECTION_H

#include <toadlet/egg/String.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/Runnable.h>
#include <toadlet/egg/Random.h>
#include <toadlet/egg/io/DataOutputStream.h>
#include <toadlet/egg/io/MemoryOutputStream.h>
#include <toadlet/egg/io/DataInputStream.h>
#include <toadlet/egg/io/MemoryInputStream.h>
#include <toadlet/egg/net/Socket.h>
#include <toadlet/knot/Connection.h>

namespace toadlet{
namespace knot{

class TOADLET_API TCPConnection:public Connection,egg::Runnable{
public:
	TOADLET_SHARED_POINTERS(TCPConnection,Connection);

	TCPConnection(egg::net::Socket::ptr socket);
	virtual ~TCPConnection();

	bool connect(const egg::String &address,int port);
	bool accept();
	void skipStart();

	bool disconnect();

	int send(const char *data,int length);
	int receive(char *data,int length);

	int getPing() const{return -1;}

	void run();

	/// Debug methods
	void debugSetPacketDelayTime(int minTime,int maxTime);

protected:
	class Packet{
	public:
		TOADLET_SHARED_POINTERS(Packet,Packet);

		Packet():length(0),debugDeliverTime(0){}

		void reset(){
			length=0;
			debugDeliverTime=0;
		}

		char data[1024];
		int length;
		uint64 debugDeliverTime;
	};

	const static int CONNECTION_FRAME;
	const static char *CONNECTION_PACKET;
	const static int CONNECTION_PACKET_LENGTH;
	const static int CONNECTION_VERSION;

	int buildConnectionPacket(egg::io::DataOutputStream *out);
	bool verifyConnectionPacket(egg::io::DataInputStream *in);

	bool updatePacketReceive();

	egg::net::Socket::ptr mSocket;
	egg::io::MemoryOutputStream::ptr mOutPacket;
	egg::io::DataOutputStream::ptr mDataOutPacket;
	egg::io::MemoryInputStream::ptr mInPacket;
	egg::io::DataInputStream::ptr mDataInPacket;

	egg::Mutex::ptr mMutex;
	egg::Thread::ptr mThread;
	bool mRun;

	egg::Collection<Packet::ptr> mPackets;
	egg::Collection<Packet::ptr> mFreePackets;

	egg::Random mDebugRandom;
	int mDebugPacketDelayMinTime;
	int mDebugPacketDelayMaxTime;
};

}
}

#endif
