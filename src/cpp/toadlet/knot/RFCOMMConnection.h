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

#ifndef TOADLET_KNOT_RFCOMMCONNECTION_H
#define TOADLET_KNOT_RFCOMMCONNECTION_H

#include <toadlet/egg/String.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/Runnable.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/egg/io/MemoryStream.h>
#include <toadlet/egg/net/BluetoothClient.h>
#include <toadlet/egg/net/BluetoothServer.h>
#include <toadlet/knot/Connection.h>

namespace toadlet{
namespace knot{

class TOADLET_API RFCOMMConnection:public Connection,egg::Runnable{
public:
	TOADLET_SHARED_POINTERS(RFCOMMConnection);

	RFCOMMConnection(egg::net::BluetoothClient::ptr client);
	RFCOMMConnection(egg::net::BluetoothServer::ptr server);
	virtual ~RFCOMMConnection();

	bool connect(egg::net::BluetoothAddress *address);
	bool accept(const egg::String &guid);
	bool disconnect();

	int send(const byte *data,int length);
	int receive(byte *data,int length);

	int getPing() const{return -1;}

	void run();

protected:
	class Packet{
	public:
		TOADLET_SHARED_POINTERS(Packet);
		byte data[1024];
		int length;
	};

	const static int CONNECTION_FRAME;
	const static char *CONNECTION_PACKET;
	const static int CONNECTION_PACKET_LENGTH;
	const static int CONNECTION_VERSION;

	int buildConnectionPacket(egg::io::DataStream *stream);
	bool verifyConnectionPacket(egg::io::DataStream *stream);

	bool updatePacketReceive();

	egg::net::BluetoothClient::ptr mClient;
	egg::net::BluetoothServer::ptr mServer;
	egg::io::MemoryStream::ptr mOutPacket;
	egg::io::DataStream::ptr mDataOutPacket;
	egg::io::MemoryStream::ptr mInPacket;
	egg::io::DataStream::ptr mDataInPacket;

	egg::Mutex::ptr mMutex;
	egg::Thread::ptr mThread;
	bool mRun;

	egg::Collection<Packet::ptr> mPackets;
};

}
}

#endif
