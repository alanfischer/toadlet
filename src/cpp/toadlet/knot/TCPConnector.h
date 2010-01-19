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

#include <toadlet/knot/Connector.h>
#include <toadlet/knot/TCPConnection.h>

namespace toadlet{
namespace knot{

class TOADLET_API TCPConnector:public Connector,egg::Runnable{
public:
	TOADLET_SHARED_POINTERS(TCPConnector);

	TCPConnector();
	TCPConnector(int localPort); // Server mode
	TCPConnector(int remoteHost,int remotePort); // Client mode
	virtual ~TCPConnector();

	bool accept(int localPort);
	bool connect(int remoteHost,int remotePort);

	bool isOpen() const;
	void close();

	void addConnectorListener(ConnectorListener *listener);
	void removeConnectorListener(ConnectorListener *listener);
	
	void run();

	void receiveError(TCPConnection *connection);
	void dataReady(TCPConnection *connection);
	
protected:
	void notifyListenersConnected(TCPConnection::ptr connection);
	void notifyListenersDisconnected(TCPConnection::ptr connection);

	egg::net::Socket::ptr mServerSocket;
	egg::Thread::ptr mServerThread;
	bool mRun;
	egg::Collection<TCPConnection::ptr> mConnections;
	egg::Mutex mConnectionsMutex;
	egg::Collection<ConnectorListener*> mListeners;
	egg::Mutex mListenersMutex;
};

}
}

#endif
