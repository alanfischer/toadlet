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

#ifndef TOADLET_KNOT_LANPEEREVENTCONNECTOR
#define TOADLET_KNOT_LANPEEREVENTCONNECTOR

#include <toadlet/egg/EventFactory.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/Random.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/net/Socket.h>
#include <toadlet/knot/Connector.h>
#include <toadlet/knot/SynchronizedEventPeer.h>

namespace toadlet{
namespace knot{

// This is a class that can be used to easily set up a peer to peer game over a LAN, with one client randomly being the primary one.

// TODO: This class can be separated out to base PeerEventConnector, which will contain the Events & connected(), and then specialize it for BluetoothPeerConnector, LANPeerConnector, etc

// TODO: The LANPeerConnector could really just be a LANConnector, and would do broadcasting, and generate client/server connections.  Then we would have a PeerNegotiator, which would take connections, and do the whole "send events and see who is server" thing, and return a PeerEventSynchronizer.
//  That would let us re-use the LANConnector in the server stuff to automatically find a server on the LAN
class TOADLET_API LANPeerEventConnector:public Connector,public egg::EventFactory{
public:
	TOADLET_SHARED_POINTERS(LANPeerEventConnector);

	static const int Event_CONNECTION=1;

	static const int Error_SERVER_CONNECT=-2;
	static const int Error_CLIENT_CONNECT=-3;
	static const int Error_RECEIVING_CONNECTION=-4;
	static const int Error_INCOMPATIBLE_VERSION=-5;
	static const int Error_IDENTICAL_SEED=-6;
	static const int Error_UNKNOWN=-7;

	LANPeerEventConnector();
	virtual ~LANPeerEventConnector();

	// TODO: Replace int uuid with an actual UUID class
	bool create(bool udp,int broadcastPort,int serverPort,const egg::String &uuid,int version,EventFactory *factory);

	bool opened(){return mConnection!=NULL;}
	void close();

	void addConnectorListener(ConnectorListener *listener,bool notifyAboutCurrent);
	void removeConnectorListener(ConnectorListener *listener,bool notifyAboutCurrent);

	// Start searching for a game
	bool search(int seed,egg::Event::ptr localPayload);
	// Cancel the search
	void cancel();
	// Returns a 1 upon connection, and the get* commands will return valid objects
	//  Or returns a negative number upon error, and can check with the Errors above
	int update();

	int getOrder() const{return mOrder;}
	int getSeed() const{return mSeed;}
	egg::Event::ptr getPayload(){return mPayload;}
	Connection::ptr getConnection(){return mConnection;}
	SynchronizedEventPeer::ptr getEventPeer(){return mEventPeer;}

	egg::Event::ptr createEventType(int type);

protected:
	void pushThreadEvent(egg::Event::ptr event);

	bool startIPServer(int port);
	void stopIPServer();
	void ipServerThread(int port);
	bool startIPClient(int ip,int port);
	void stopIPClient();
	void ipClientThread(int ip,int port);

	void findLANGameThread();

	void connected(Connection::ptr connection);

	bool ensureConnectionAbility();

	bool mUDP;
	egg::Random mRandom;
	int mBroadcastIP;
	int mBroadcastPort;
	egg::String mUUID;
	int mVersion;
	int mLocalSeed;
	egg::Event::ptr mLocalPayload;
	egg::EventFactory *mEventFactory;
	egg::net::Socket::ptr mLANListenerSocket;
	egg::Thread::ptr mFindLANGameThread;

	int mServerPort;
	egg::net::Socket::ptr mIPServerSocket;
	egg::Thread::ptr mIPServerThread;

	egg::net::Socket::ptr mIPClientSocket;
	egg::Thread::ptr mIPClientThread;

	int mOrder;
	int mSeed;
	egg::Event::ptr mPayload;
	Connection::ptr mConnection;
	egg::Mutex mConnectionMutex;
	egg::Collection<ConnectorListener*> mListeners;
	egg::Mutex mListenersMutex;
	SynchronizedEventPeer::ptr mEventPeer;
	egg::Mutex mEventsMutex;
	egg::Collection<egg::Event::ptr> mEvents;

	friend class FindLANGameThread;
	friend class IPClientThread;
	friend class IPServerThread;
};

}
}

#endif
