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
#include <toadlet/egg/Random.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/egg/net/Socket.h>
#include <toadlet/knot/Connection.h>
#include <toadlet/knot/ConnectionListener.h>

namespace toadlet{
namespace knot{

class TCPConnector;

/**
 The TCPConnection is a Connection abstraction over TCP Sockets.
 It can either be created via a TCPConnector, or a One to One connection can be achieved via the connect & accept methods.
 It currently is synchronous.
*/
class TOADLET_API TCPConnection:public Connection{
public:
	TOADLET_SHARED_POINTERS(TCPConnection);

	TCPConnection(egg::net::Socket::ptr socket=egg::net::Socket::ptr()); // Supply a socket to use, this lets the socket be controlled externally
	virtual ~TCPConnection();

	bool connect(uint32 remoteHost,int remotePort);
	bool connect(egg::net::Socket::ptr socket);
	bool accept(int localPort);
	bool accept(egg::net::Socket::ptr socket);

	void close();
	bool closed(){return mSocket->closed();}

	int send(const tbyte *data,int length);
	int receive(tbyte *data,int length);

	egg::net::Socket::ptr getSocket(){return mSocket;}

	// Due to the use of unintrusive smart pointers, we don't actually expose a setConnectionListener(ConnectionListener*) method
	//  Instead that is left to the Connectors, since they can figure out which smart pointer cooresponds to a raw pointer
	void setConnectionListener(TCPConnector *listener);

	/// Debug methods
	void debugSetPacketDelayTime(int minTime,int maxTime);

protected:
	static const int maxLength=8192;

	class Packet{
	public:
		TOADLET_SHARED_POINTERS(Packet);

		Packet():length(0),debugDeliverTime(0){}

		void reset(){
			length=0;
			debugDeliverTime=0;
		}

		tbyte data[maxLength];
		int length;
		uint64 debugDeliverTime;
	};

	const static int CONNECTION_FRAME;
	const static char *CONNECTION_PACKET;
	const static int CONNECTION_PACKET_LENGTH;
	const static int CONNECTION_VERSION;

	int buildConnectionPacket(egg::io::DataStream *stream);
	bool verifyConnectionPacket(egg::io::DataStream *stream);

	bool mClient;
	bool mBlocking;
	egg::net::Socket::ptr mSocket;
	egg::io::MemoryStream::ptr mOutPacket;
	egg::io::DataStream::ptr mDataOutPacket;
	egg::io::MemoryStream::ptr mInPacket;
	egg::io::DataStream::ptr mDataInPacket;
	TCPConnector *mConnectionListener;

	egg::Mutex::ptr mMutex;
	egg::Collection<Packet::ptr> mPackets;
	egg::Collection<Packet::ptr> mFreePackets;
	bool mReceiving;
	bool mReceiveError;

	egg::Random mDebugRandom;
	int mDebugPacketDelayMinTime;
	int mDebugPacketDelayMaxTime;

	static const int mDummyDataLength=1024;
	tbyte mDummyData[mDummyDataLength];
};

}
}

#endif
