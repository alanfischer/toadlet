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
#include <toadlet/knot/TCPConnector.h>
#include <toadlet/knot/ConnectorListener.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::net;

namespace toadlet{
namespace knot{

TCPConnector::TCPConnector():
	//mServerSocket,
	//mServerThread,
	mRun(false)
	//mConnections,
	//mConnectionsMutex,
	//mListeners,
	//mListenersMutex
{}

TCPConnector::TCPConnector(int localPort):
	//mServerSocket,
	//mServerThread,
	mRun(false)
	//mConnections,
	//mConnectionsMutex,
	//mListeners,
	//mListenersMutex
{
	accept(localPort);
}

TCPConnector::TCPConnector(int remoteHost,int remotePort)
	//mSocket,
	//mConnections,
	//mListeners
{
	connect(remoteHost,remotePort);
}

TCPConnector::~TCPConnector(){
	close();
}

bool TCPConnector::accept(int localPort){
	close();

	mServerSocket=Socket::ptr(Socket::makeTCPSocket());
	mServerSocket->bind(localPort);
	bool result=mServerSocket->listen(32);
	if(result){
		mRun=true;
		mServerThread=Thread::ptr(new Thread(this));
		mServerThread->start();
	}
	return result;
}

bool TCPConnector::connect(int remoteHost,int remotePort){
	close();

	TCPConnection::ptr connection(new TCPConnection(this));
	bool result=connection->connect(remoteHost,remotePort);
	if(result){
		mConnectionsMutex.lock();
			mConnections.add(connection);
			notifyListenersConnected(connection);
		mConnectionsMutex.unlock();
	}
	else{
		connection->disconnect();
	}
	return result;
}

bool TCPConnector::isOpen() const{
	return mRun;
}

void TCPConnector::close(){
	TOADLET_TRY
		if(mServerSocket!=NULL){
			mServerSocket->close();
		}
	TOADLET_CATCH(const Exception &){}

	mRun=false;
	while(mServerThread!=NULL && mServerThread->isAlive()){
		System::msleep(10);
	}
	mServerSocket=NULL;

	mConnectionsMutex.lock();
		int i;
		for(i=0;i<mConnections.size();++i){
			mConnections[i]->disconnect();
		}
	mConnectionsMutex.unlock();
}

void TCPConnector::addConnectorListener(ConnectorListener *listener,bool notifyAboutCurrent){
	mListenersMutex.lock();
		mListeners.add(listener);
	mListenersMutex.unlock();

	if(notifyAboutCurrent){
		mConnectionsMutex.lock();
			int i;
			for(i=0;i<mConnections.size();++i){
				listener->connected(mConnections[i]);
			}
		mConnectionsMutex.unlock();
	}
}

void TCPConnector::removeConnectorListener(ConnectorListener *listener,bool notifyAboutCurrent){
	if(notifyAboutCurrent){
		mConnectionsMutex.lock();
			int i;
			for(i=0;i<mConnections.size();++i){
				listener->disconnected(mConnections[i]);
			}
		mConnectionsMutex.unlock();
	}

	mListenersMutex.lock();
		mListeners.remove(listener);
	mListenersMutex.unlock();
}

void TCPConnector::run(){
	while(mRun){
		Socket::ptr clientSocket;
		TOADLET_TRY
			clientSocket=Socket::ptr(mServerSocket->accept());
		TOADLET_CATCH(const Exception &){clientSocket=NULL;}
		if(clientSocket!=NULL){
			TCPConnection::ptr connection(new TCPConnection(this));
			bool result=connection->accept(clientSocket);
			if(result){
				mConnectionsMutex.lock();
					mConnections.add(connection);
					notifyListenersConnected(connection);
				mConnectionsMutex.unlock();
			}
			else{
				connection->disconnect();
			}
		}
	}
}

void TCPConnector::receiveError(TCPConnection *connection){
	TCPConnection::ptr conn;

	mConnectionsMutex.lock();
		// Find the Connection::ptr from the connection
		int i;
		for(i=0;i<mConnections.size();++i){
			if(mConnections[i]==connection) break;
		}
		if(i<mConnections.size()){
			conn=mConnections[i];
			mConnections.removeAt(i);
		}
	mConnectionsMutex.unlock();

	if(conn!=NULL){
		notifyListenersDisconnected(conn);
	}
}

void TCPConnector::dataReady(TCPConnection *connection){
	mListenersMutex.lock();
		int i;
		for(i=0;i<mListeners.size();++i){
			mListeners[i]->dataReady(connection);
		}
	mListenersMutex.unlock();
}

void TCPConnector::notifyListenersConnected(TCPConnection::ptr connection){
	mListenersMutex.lock();
		int i;
		for(i=0;i<mListeners.size();++i){
			mListeners[i]->connected(connection);
		}
	mListenersMutex.unlock();
}

void TCPConnector::notifyListenersDisconnected(TCPConnection::ptr connection){
	mListenersMutex.lock();
		int i;
		for(i=0;i<mListeners.size();++i){
			mListeners[i]->disconnected(connection);
		}
	mListenersMutex.unlock();
}

}
}
