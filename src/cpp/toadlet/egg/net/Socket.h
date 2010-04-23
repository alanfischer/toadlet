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

#ifndef TOADLET_EGG_NET_SOCKET_H
#define TOADLET_EGG_NET_SOCKET_H

#include <toadlet/Types.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/WeakPointer.h>

#if defined(TOADLET_PLATFORM_WIN32)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <windows.h>
	#include <winsock.h>
#else
	#include <unistd.h>
	#include <netdb.h> 
	#include <sys/socket.h> 
	#include <netinet/in.h>
	#include <arpa/inet.h>
#endif

#if defined(TOADLET_PLATFORM_WIN32)
	#define TOADLET_SOCKET_ERROR SOCKET_ERROR
	#define TOADLET_INVALID_SOCKET INVALID_SOCKET
	#define TOADLET_SOCKLEN int
#else
	#define TOADLET_SOCKET_ERROR -1
	#define TOADLET_INVALID_SOCKET -1
	#define TOADLET_SOCKLEN socklen_t
#endif

namespace toadlet{
namespace egg{
namespace net{

class TOADLET_API Socket{
public:
	TOADLET_SHARED_POINTERS(Socket);

	Socket();
	Socket(int domain,int type,int protocol);
	Socket(int handle,struct sockaddr_in *address);
	virtual ~Socket();

	static Socket *createTCPSocket();
	static Socket *createUDPSocket();

	virtual bool bind(int port=0);
	virtual bool bound(){return mBound;}

	virtual bool connect(uint32 ipAddress,int port);
	virtual bool connect(const String &name,int port);
	virtual bool connected(){return mConnected;}

	virtual bool listen(int backlog);
	virtual Socket *accept();

	virtual void close();
	virtual bool closed(){return mHandle==TOADLET_INVALID_SOCKET;}

	virtual uint32 getHostIPAddress() const{return mHostIPAddress;}
	virtual int getHostPort() const{return mHostPort;}

	virtual bool setSendBufferSize(int size);
	virtual int getSendBufferSize() const;

	virtual bool setRecieveBufferSize(int size);
	virtual int getRecieveBufferSize() const;

	virtual bool setBroadcast(bool broadcast);
	virtual bool getBroadcast() const;

	virtual bool setTimeout(int millis);
	virtual int getTimeout() const;

	virtual bool pollRead(int millis);
	virtual bool pollWrite(int millis);

	virtual int receive(byte *buffer,int length);
	virtual int receiveFrom(byte *buffer,int length,uint32 &ipAddress,int &port);

	virtual int send(const byte *buffer,int length);
	virtual int sendTo(const byte *buffer,int length,uint32 ipAddress,int port);

public:
	static String ipToString(uint32 ip);
	static uint32 stringToIP(const String &string);
	static bool getHostAdaptorsByName(Collection<uint32> &adaptors,const String &name);
	static bool getHostAdaptorsByIP(Collection<uint32> &adaptors,uint32 ip);

protected:
	Socket(int socket,uint32 ipAddress,int port);

	static void error(const String &function);

	int mHandle;
	bool mBound;
	bool mConnected;
	uint32 mHostIPAddress;
	uint32 mHostPort;

	#if defined(TOADLET_PLATFORM_WIN32)
		static class WSAHandler{
		public:
			WSAHandler();
			~WSAHandler();
			WSADATA mWSADATA;
		} mWSAHandler;
	#endif
};

}
}
}

#endif
