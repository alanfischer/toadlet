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

#ifndef TOADLET_KNOT_TCPCONNECTOR_H
#define TOADLET_KNOT_TCPCONNECTOR_H

#include <toadlet/egg/Thread.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/knot/Connector.h>
#include <toadlet/knot/ConnectionListener.h>
#include <toadlet/knot/TCPConnection.h>

namespace toadlet{
namespace knot{

class TOADLET_API TCPConnector:public Connector,public Runnable{
public:
	TOADLET_SHARED_POINTERS(TCPConnector);

	TCPConnector();
	/// Start accepting on the port
	TCPConnector(int localPort);
	/// Start connecting to the remote host
	TCPConnector(int remoteHost,int remotePort); // Client mode
	virtual ~TCPConnector();

	bool accept(int localPort);
	bool connect(int remoteHost,int remotePort);

	bool opened(){return mRun;}
	void close();

	void addConnectionListener(ConnectionListener *listener,bool notifyAboutCurrent);
	void removeConnectionListener(ConnectionListener *listener,bool notifyAboutCurrent);

	TCPConnection::ptr getConnection(int i);

	void run();

	void connected(Connection *connection);
	void disconnected(Connection *connection);

protected:
	void notifyListenersConnected(TCPConnection::ptr connection);
	void notifyListenersDisconnected(TCPConnection::ptr connection);

	Socket::ptr mServerSocket;
	Thread::ptr mServerThread;
	bool mRun;
	Collection<TCPConnection::ptr> mConnections;
	Mutex mConnectionsMutex;
	Collection<ConnectionListener*> mListeners;
	Mutex mListenersMutex;

	friend class TCPConnection;
};

}
}

#endif