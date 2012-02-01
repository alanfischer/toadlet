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

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/System.h>
#include <toadlet/knot/LANPeerEventConnector.h>
#include <toadlet/knot/PeerPacketConnection.h>
#include <toadlet/knot/TCPConnection.h>
#include <toadlet/knot/ConnectionListener.h>

#if defined(TOADLET_PLATFORM_IOS)
	#import <SystemConfiguration/SCNetworkReachability.h>
	#define ReachableViaWiFiNetwork	2
	#define ReachableDirectWWAN	(1<<18)
#elif defined(TOADLET_PLATFORM_WINCE)
	#include <wininet.h>
	#pragma comment(lib,"wininet.lib")
#endif

namespace toadlet{
namespace knot{

class ConnectionEvent:public Event{
public:
	TOADLET_SHARED_POINTERS(ConnectionEvent);

	ConnectionEvent(EventFactory *factory):Event(LANPeerEventConnector::Event_CONNECTION),
		version(0),
		randomSeed(0),
		//payload,
		order(0),
		factory(NULL)
	{
		this->factory=factory;
	}

	ConnectionEvent(int version,int64 randomSeed,Event::ptr payload,EventFactory *factory):Event(LANPeerEventConnector::Event_CONNECTION){
		this->version=version;
		this->randomSeed=randomSeed;
		this->payload=payload;
		order=0;
		this->factory=factory;
	}

	int read(DataStream *stream){
		int payloadType=0;
		int amt=0;
		version=stream->readBInt32();amt+=4;
		randomSeed=stream->readBInt64();amt+=4;
		payloadType=stream->readBInt32();amt+=4;
		if(payloadType!=0){
			payload=factory->createEventType(payloadType);
			amt+=payload->read(stream);
		}
		return amt;
	}

	int write(DataStream *stream){
		int amt=0;
		amt+=stream->writeBInt32(version);
		amt+=stream->writeBInt64(randomSeed);
		if(payload!=NULL){
			amt+=stream->writeBInt32(payload->getType());
			amt+=payload->write(stream);
		}
		else{
			amt+=stream->writeBInt32(0);
		}
		return amt;
	}

	int version;
	int64 randomSeed;
	Event::ptr payload;
	int order;
	EventFactory *factory;
};

class FindLANGameThread:public Thread{
public:
	FindLANGameThread(LANPeerEventConnector *c):connector(c){}
	void run(){connector->findLANGameThread();}
	LANPeerEventConnector *connector;
};

class IPClientThread:public Thread{
public:
	IPClientThread(LANPeerEventConnector *c,int i,int p){connector=c;ip=i;port=p;}
	void run(){connector->ipClientThread(ip,port);}
	LANPeerEventConnector *connector;
	int ip;
	int port;
};

class IPServerThread:public Thread{
public:
	IPServerThread(LANPeerEventConnector *c,int p){connector=c;port=p;}
	void run(){connector->ipServerThread(port);}
	LANPeerEventConnector *connector;
	int port;
};

LANPeerEventConnector::LANPeerEventConnector():
	mUDP(false),
	mBroadcastIP(Socket::stringToIP("255.255.255.255")),
	mBroadcastPort(0),
	//mUUID,
	mVersion(0),
	mLocalSeed(0),
	//mLocalPayload,
	mEventFactory(NULL),
	//mLANListenerSocket,
	//mFindLANGameThread,

	mServerPort(0),
	//mIPServerSocket,
	//mIPServerThread,

	//mIPClientSocket,
	//mIPClientThread,

	mOrder(0),
	mSeed(0)
	//mPayload,
	//mConnection,
	//mConnectionMutex,
	//mEventConnection,
	//mEventsMutex,
	//mEvents
{
}

LANPeerEventConnector::~LANPeerEventConnector(){
	close();
}

bool LANPeerEventConnector::create(bool udp,int broadcastPort,int serverPort,const String &uuid,int version,EventFactory *factory){
	mUDP=udp;
	mBroadcastPort=broadcastPort;
	mServerPort=serverPort;
	mUUID=uuid;
	mVersion=version;
	mEventFactory=factory;

	return true;
}

void LANPeerEventConnector::close(){
	if(mConnection!=NULL){
		mConnection->close();
		mConnection=NULL;
	}

	mEventConnection=NULL;

	stopIPClient();
	stopIPServer();

	mConnectionMutex.lock();
		if(mLANListenerSocket!=NULL){
			mLANListenerSocket->close();
		}
	mConnectionMutex.unlock();

	if(mFindLANGameThread!=NULL){
		Thread::ptr thread=mFindLANGameThread;
		mFindLANGameThread=NULL;
		thread->join();
	}
}

void LANPeerEventConnector::addConnectionListener(ConnectionListener *listener,bool notifyAboutCurrent){
	mListenersMutex.lock();
		mListeners.add(listener);
	mListenersMutex.unlock();

	if(notifyAboutCurrent){
		mConnectionMutex.lock();
			if(mConnection!=NULL){
				listener->connected(mConnection);
			}
		mConnectionMutex.unlock();
	}
}

void LANPeerEventConnector::removeConnectionListener(ConnectionListener *listener,bool notifyAboutCurrent){
	if(notifyAboutCurrent){
		mConnectionMutex.lock();
			if(mConnection!=NULL){
				listener->disconnected(mConnection);
			}
		mConnectionMutex.unlock();
	}

	mListenersMutex.lock();
		mListeners.remove(listener);
	mListenersMutex.unlock();
}

bool LANPeerEventConnector::search(int localSeed,Event::ptr localPayload){
	if(mConnectionMutex.tryLock()==false){
		return false;
	}

	mLocalSeed=localSeed;
	mLocalPayload=localPayload;

	if(mLANListenerSocket!=NULL){
		mLANListenerSocket->close();
	}

	startIPServer(mServerPort);

	if(mFindLANGameThread==NULL || mFindLANGameThread->isAlive()==false){
		mFindLANGameThread=Thread::ptr(new FindLANGameThread(this));
		mFindLANGameThread->start();
	}

	mConnectionMutex.unlock();

	return true;
}

void LANPeerEventConnector::cancel(){
	close(); // For now close should have the same effect
}

int LANPeerEventConnector::update(){
	int result=0;

	int i;
	mEventsMutex.lock();
		for(i=0;i<mEvents.size();++i){
			Event::ptr event=mEvents[i];
			switch(event->getType()){
				case(Event_CONNECTION):{
					ConnectionEvent *connectionEvent=(ConnectionEvent*)event.get();
					mOrder=connectionEvent->order;
					mSeed=connectionEvent->randomSeed;
					mPayload=connectionEvent->payload;
					result=1;
				} break;
				default:{ // Assume its an error, and return the code
					result=-event->getType();
				} break;
			}
			mEvents.removeAt(0);
			i--;
		}
	mEventsMutex.unlock();
	return result;
}

void LANPeerEventConnector::pushThreadEvent(Event::ptr event){
	mEventsMutex.lock();
		mEvents.add(event);
	mEventsMutex.unlock();
}

Event::ptr LANPeerEventConnector::createEventType(int type){
	if(type==Event_CONNECTION){
		return Event::ptr(new ConnectionEvent(this));
	}
	else{
		return mEventFactory->createEventType(type);
	}
}

bool LANPeerEventConnector::startIPServer(int port){
	stopIPServer();

	Logger::alert(Categories::TOADLET_KNOT,
		String("Starting IPServer:")+port);

	if(mUDP){
		mIPServerSocket=Socket::ptr(Socket::createUDPSocket());
	}
	else{
		mIPServerSocket=Socket::ptr(Socket::createTCPSocket());
	}

	// The reason we do the binding before starting the thread & Connection is due to the following:
	//  If we bind before starting the thread, we know that we can call close on the socket at any time in this thread and verify
	//  that we closed it after it was bound, and that the socket will properly not send/recv data.
	//  If instead we bind the socket in the thread, then we can have a race condition where we could attempt to close the socket,
	//  but the thread would not of even bound the socket yet, so then the thread binds the socket and starts running it.
	//  I suppose this could be prevented by modifying a socket so when it is closed, it can not be used for anything again. (and make them threadsafe)
	if(mIPServerSocket->bind(port)==false){
		Error::unknown("error binding server socket");
		return false;
	}

	mIPServerThread=Thread::ptr(new IPServerThread(this,port));
	mIPServerThread->start();

	return true;
}

void LANPeerEventConnector::stopIPServer(){
	Logger::debug(Categories::TOADLET_KNOT,
		"Stopping IPServer");

	if(mIPServerSocket!=NULL){
		mIPServerSocket->close();
	}

	if(mIPServerThread!=NULL){
		Thread::ptr thread=mIPServerThread;
		mIPServerThread=NULL;
		thread->join();
		Logger::debug(Categories::TOADLET_KNOT,
			"IPServer stopped");
	}
	else{
		Logger::debug(Categories::TOADLET_KNOT,
			"No IPServer thread to stop");
	}
}

void LANPeerEventConnector::ipServerThread(int port){
	ensureConnectionAbility();

	Logger::alert(Categories::TOADLET_KNOT,
		"IPServer waiting");

	Connection::ptr connection=NULL;
	bool result=false;
	// accept() is not a member of Connection, so we have to know the child class to call it
	if(mUDP){
		PeerPacketConnection::ptr con(new PeerPacketConnection(mIPServerSocket));
		result=con->accept(port);
		connection=con;
	}
	else{
		TCPConnection::ptr con(new TCPConnection(mIPServerSocket));
		result=con->accept(port);
		connection=con;
	}

	if(result){
		connected(connection);
	}
	else{
		pushThreadEvent(Event::ptr(new Event(-Error_SERVER_CONNECT)));
	}
}

bool LANPeerEventConnector::startIPClient(int ip,int port){
	stopIPClient();

	Logger::alert(Categories::TOADLET_KNOT,
		String("Starting IPClient:")+ip+":"+port);

	if(mUDP){
		mIPClientSocket=Socket::ptr(Socket::createUDPSocket());
	}
	else{
		mIPClientSocket=Socket::ptr(Socket::createTCPSocket());
	}

	if(mIPClientSocket->bind(0)==false){
		Logger::error("error binding client socket");
		return false;
	}

	mIPClientThread=Thread::ptr(new IPClientThread(this,ip,port));
	mIPClientThread->start();

	return true;
}

void LANPeerEventConnector::stopIPClient(){
	Logger::debug(Categories::TOADLET_KNOT,
		"Stopping IPClient");

	if(mIPClientSocket!=NULL){
		mIPClientSocket->close();
	}

	if(mIPClientThread!=NULL){
		Thread::ptr thread=mIPClientThread;
		mIPClientThread=NULL;
		thread->join();
		Logger::debug(Categories::TOADLET_KNOT,
			"IPClient stopped");
	}
	else{
		Logger::debug(Categories::TOADLET_KNOT,
			"No IPClient thread to stop");
	}
}

void LANPeerEventConnector::ipClientThread(int ip,int port){
	Connection::ptr connection=NULL;
	bool result=false;
	if(mUDP){
		PeerPacketConnection::ptr con(new PeerPacketConnection(mIPClientSocket));
		result=con->connect(ip,port);
		connection=con;
	}
	else{
		TCPConnection::ptr con(new TCPConnection(mIPClientSocket));
		result=con->connect(ip,port);
		connection=con;
	}

	if(result){
		connected(connection);
	}
	else{
		pushThreadEvent(Event::ptr(new Event(-Error_CLIENT_CONNECT)));
	}
}

// This function may fail sometimes if you are attempting to run two instances on the same computer.
//  Since both instances are listening to the same port, the actual connection packet (not the broadcast packet)
//  will get sent to both instances, so the connector may, depending on timing, attempt to connect
//  to itself.  This could be aleviated by not allowing sockets to bind to the same port, and then having the
//  second instance change its server port.
void LANPeerEventConnector::findLANGameThread(){
	Socket::ptr broadcastSocket(Socket::createUDPSocket());

	broadcastSocket->bind(0);
	broadcastSocket->setBroadcast(true);

	String message=mUUID+":"+mServerPort;
	broadcastSocket->sendTo((tbyte*)message.c_str(),message.length(),mBroadcastIP,mBroadcastPort);

	broadcastSocket->close();

	mConnectionMutex.lock();
		mLANListenerSocket=Socket::ptr(Socket::createUDPSocket());
	mConnectionMutex.unlock();
	mLANListenerSocket->bind(mBroadcastPort);

	char receivedData[1024];
	memset(receivedData,0,sizeof(receivedData));
	uint32 ip=0;
	int port=0;
    int amount=0;
	TOADLET_TRY
		amount=mLANListenerSocket->receiveFrom((tbyte*)receivedData,sizeof(receivedData),ip,port);
	TOADLET_CATCH(const Exception &){}

	TOADLET_TRY
		mLANListenerSocket->close();
	TOADLET_CATCH(const Exception &){}

	mConnectionMutex.lock();
		mLANListenerSocket=NULL;
	mConnectionMutex.unlock();

	Logger::alert(Categories::TOADLET_KNOT,
		String("Received broadcast from ")+Socket::ipToString(ip)+":"+port);

	if(amount>0){
		message=receivedData;
		String uuid=(char*)NULL;
		int index=message.find(':');
		if(index>=0){
			uuid=message.substr(0,index);
			port=message.substr(index+1,message.length()).toInt32();
		}
		if(mUUID.equals(uuid)){
			startIPClient(ip,port);
		}
	}
}

void LANPeerEventConnector::connected(Connection::ptr connection){
	if(mConnectionMutex.tryLock()==false || mConnection!=NULL){
		connection->close();
		return;
	}

	SynchronizedPeerEventConnection::ptr eventConnection(new SynchronizedPeerEventConnection(connection,this));

	ConnectionEvent::ptr localConnectionEvent(new ConnectionEvent(mVersion,mLocalSeed,mLocalPayload,this));
	ConnectionEvent::ptr remoteConnectionEvent;

	// Send local event
	eventConnection->send(localConnectionEvent);
	eventConnection->update(); // Send the event

	// Receive remote event
	int i;
	for(i=50;i>=0;--i){ // 5 seconds total to wait for the event
		if(eventConnection->update()==SynchronizedPeerEventConnection::PeerStatus_FRAME_OK){
			remoteConnectionEvent=shared_static_cast<ConnectionEvent>(eventConnection->receive());
			// Check to see if it just gave us the local event first
			if(remoteConnectionEvent==localConnectionEvent){
				// Do it again, this time remote should be coming
				remoteConnectionEvent=shared_static_cast<ConnectionEvent>(eventConnection->receive());
			}
			else{
				// Just need to receive any event, but we know its local
				localConnectionEvent=shared_static_cast<ConnectionEvent>(eventConnection->receive());
			}
			break;
		}
		System::msleep(100);
	}

	if(remoteConnectionEvent!=NULL && remoteConnectionEvent->version==localConnectionEvent->version && remoteConnectionEvent->randomSeed!=localConnectionEvent->randomSeed){
		ConnectionEvent::ptr connectionEvent;
		if(localConnectionEvent->randomSeed<remoteConnectionEvent->randomSeed){
			connectionEvent=localConnectionEvent;
			connectionEvent->order=0;
		}
		else{
			connectionEvent=remoteConnectionEvent;
			connectionEvent->order=1;
		}

		// Now that we have sent the proper connection messages
		eventConnection->reset(4,1);

		/// @todo: Add a disconnected & dataReady notification to the listeners
		mListenersMutex.lock();
			for(i=0;i<mListeners.size();++i){
				mListeners[i]->connected(connection);
			}
		mListenersMutex.unlock();

		mConnection=connection;
		mEventConnection=eventConnection;

		Logger::alert(Categories::TOADLET_KNOT,
			String("Receiving ConnectionEvent:")+(int)connectionEvent->randomSeed);

		pushThreadEvent(connectionEvent);
	}
	else{
		if(remoteConnectionEvent==NULL){
			Logger::alert(Categories::TOADLET_KNOT,
				String("Error receiving ConnectionEvent"));
			pushThreadEvent(Event::ptr(new Event(-Error_RECEIVING_CONNECTION)));
		}
		else if(remoteConnectionEvent->version!=localConnectionEvent->version){
			Logger::alert(Categories::TOADLET_KNOT,
				String("Incompatible version.  remote:")+remoteConnectionEvent->version+" local:"+localConnectionEvent->version);
			pushThreadEvent(Event::ptr(new Event(-Error_INCOMPATIBLE_VERSION)));
		}
		else if(remoteConnectionEvent->randomSeed==localConnectionEvent->randomSeed){
			Logger::alert(Categories::TOADLET_KNOT,
				String("Identical seed error")+remoteConnectionEvent->randomSeed+":"+localConnectionEvent->randomSeed);
			pushThreadEvent(Event::ptr(new Event(-Error_IDENTICAL_SEED)));
		}
		else{
			Logger::alert(Categories::TOADLET_KNOT,
				String("Invalid ConnectionEvent"));
			pushThreadEvent(Event::ptr(new Event(-Error_UNKNOWN)));
		}
	}

	mConnectionMutex.unlock();
}

bool LANPeerEventConnector::ensureConnectionAbility(){
	bool result=true;
	#if defined(TOADLET_PLATFORM_WINCE)
		// This way it simply checks to see if we can connect, not focing a GPRS connection like below.
		// However if a GPRS connection is already open from some other application, and a WIFI connection is not
		//  available, it appears as if we can not tell the difference, from what I have read, so it will just silently fail, oh well.
		#if 1
			result=InternetCheckConnection(TEXT("http://www.google.com"),FLAG_ICC_FORCE_CONNECTION,0)>0;
		#else
			mConnectionManagerMutex.lock();
			if(mConnectionManager==NULL){
				mConnectionManager=SharedPointer<ConnectionManager>(new ConnectionManager());
				result=mConnectionManager->connect(false,60000);
			}
			mConnectionManagerMutex.unlock();
		#endif
	#elif defined(TOADLET_PLATFORM_IOS)
		SCNetworkReachabilityFlags flags=0;
		SCNetworkReachabilityRef reachabilityRef=SCNetworkReachabilityCreateWithName(CFAllocatorGetDefault(),"www.google.com");
		bool gotFlags=SCNetworkReachabilityGetFlags(reachabilityRef,&flags);
		CFRelease(reachabilityRef);
    
		result=!(!gotFlags || (flags & ReachableDirectWWAN) || !(flags & ReachableViaWiFiNetwork));
	#endif
	return result;
}

}
}
