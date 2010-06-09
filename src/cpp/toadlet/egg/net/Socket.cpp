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

#include <toadlet/egg/net/Socket.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <time.h>
#include <string.h> // memset

#if !defined(TOADLET_PLATFORM_WIN32)
	#include <sys/ioctl.h>
	#include <errno.h>
#endif

#if defined(TOADLET_PLATFORM_WIN32)
	#if defined(TOADLET_PLATFORM_WINCE)
		#pragma comment(lib,"ws2.lib")
	#else
		#pragma comment(lib,"wsock32.lib")
	#endif
#endif

namespace toadlet{
namespace egg{
namespace net{

#if defined(TOADLET_PLATFORM_WIN32)
	// We'll keep these errors in TOADLET_EGG, since they are more significant than the socket errors
	Socket::WSAHandler::WSAHandler(){
		int result=WSAStartup(MAKEWORD(1,1),&mWSADATA);
		if(result!=0){
			Logger::error(Categories::TOADLET_EGG,
				String("WSAHandler::WSAHandler(): error ")+result);
		}
	}

	Socket::WSAHandler::~WSAHandler(){
		int result=WSACleanup();
		if(result!=0){
			Logger::error(Categories::TOADLET_EGG,
				String("WSAHandler::~WSAHandler(): error ")+result);
		}
	}

	Socket::WSAHandler Socket::mWSAHandler;
#endif

Socket::Socket():
	mHandle(TOADLET_INVALID_SOCKET),
	mBound(false),
	mConnected(false),
	mBlocking(true),
	mHostIPAddress(0),
	mHostPort(0)
{
}

Socket::Socket(int domain,int type,int protocol):
	mHandle(TOADLET_INVALID_SOCKET),
	mBound(false),
	mConnected(false),
	mBlocking(true),
	mHostIPAddress(0),
	mHostPort(0)
{
	mHandle=socket(domain,type,protocol);
	if(mHandle==TOADLET_INVALID_SOCKET){
		error("socket");
	}

	// It seems on win32, or at least wince, if I have a server socket on port X,
	//  and a client socket trying to connect to port Y on a separate machine,
	//  it will give an address in use error unless I set this.
	int value=1;
	setsockopt(mHandle,SOL_SOCKET,SO_REUSEADDR,(char*)&value,sizeof(int));
	#if defined(SO_NOSIGPIPE)
		setsockopt(mHandle,SOL_SOCKET,SO_NOSIGPIPE,(char*)&value,sizeof(int));
	#endif
}

Socket::Socket(int handle,struct sockaddr_in *address):
	mHandle(handle),
	mBound(true),
	mConnected(true),
	mBlocking(true),
	mHostIPAddress(address->sin_addr.s_addr),
	mHostPort(address->sin_port)
{
	// It seems on win32, or at least wince, if I have a server socket on port X,
	//  and a client socket trying to connect to port Y on a separate machine,
	//  it will give an address in use error unless I set this.
	int value=1;
	setsockopt(mHandle,SOL_SOCKET,SO_REUSEADDR,(char*)&value,sizeof(int));
	#if defined(SO_NOSIGPIPE)
		setsockopt(mHandle,SOL_SOCKET,SO_NOSIGPIPE,(char*)&value,sizeof(int));
	#endif
}

Socket *Socket::createTCPSocket(){
	return new Socket(PF_INET,SOCK_STREAM,IPPROTO_TCP);
}

Socket *Socket::createUDPSocket(){
	return new Socket(PF_INET,SOCK_DGRAM,IPPROTO_UDP);
}

Socket::~Socket(){
	close();
}

void Socket::close(){
	int result=0;
	if(mHandle!=TOADLET_INVALID_SOCKET){
		#if defined(TOADLET_PLATFORM_WIN32)
			shutdown(mHandle,2);
			result=::closesocket(mHandle);
		#else
			shutdown(mHandle,SHUT_RDWR);
			result=::close(mHandle);
		#endif
		mHandle=TOADLET_INVALID_SOCKET;
	}
	mBound=false;
	mConnected=false;
	mHostIPAddress=0;
	mHostPort=0;

	if(result==TOADLET_SOCKET_ERROR){
		error("closesocket");
	}
}

bool Socket::setBlocking(bool blocking){
	unsigned long value=!blocking;
	int result=0;
	#if defined(TOADLET_PLATFORM_WIN32)
		result=ioctlsocket(mHandle,FIONBIO,&value);
	#else
		result=ioctl(mHandle,FIONBIO,&value);
	#endif
	if(result==TOADLET_SOCKET_ERROR){
		error("ioctlsocket");
		return false;
	}
	mBlocking=blocking;
	return true;
}

bool Socket::getBlocking() const{
	return mBlocking;
}

bool Socket::setSendBufferSize(int size){
	int result=setsockopt(mHandle,SOL_SOCKET,SO_SNDBUF,(char*)&size,sizeof(size));
	if(result==TOADLET_SOCKET_ERROR){
		error("setsockopt");
		return false;
	}
	return true;
}

int Socket::getSendBufferSize() const{
	int value=0;
	TOADLET_SOCKLEN size=sizeof(value);
	int result=getsockopt(mHandle,SOL_SOCKET,SO_SNDBUF,(char*)&value,&size);
	if(result==TOADLET_SOCKET_ERROR){
		error("getsockopt");
		return -1;
	}
	return size;
}

bool Socket::setRecieveBufferSize(int size){
	int result=setsockopt(mHandle,SOL_SOCKET,SO_RCVBUF,(char*)&size,sizeof(size));
	if(result==TOADLET_SOCKET_ERROR){
		error("setsockopt");
		return false;
	}
	return true;
}

int Socket::getRecieveBufferSize() const{
	int value=0;
	TOADLET_SOCKLEN size=sizeof(value);
	int result=getsockopt(mHandle,SOL_SOCKET,SO_RCVBUF,(char*)&value,&size);
	if(result==TOADLET_SOCKET_ERROR){
		error("getsockopt");
		return -1;
	}
	return value;
}

bool Socket::setBroadcast(bool broadcast){
	int value=broadcast;
	int result=setsockopt(mHandle,SOL_SOCKET,SO_BROADCAST,(char*)&value,sizeof(value));
	if(result==TOADLET_SOCKET_ERROR){
		error("setsockopt");
		return false;
	}
	return true;
}

bool Socket::getBroadcast() const{
	int value=0;
	TOADLET_SOCKLEN size=sizeof(value);
	int result=getsockopt(mHandle,SOL_SOCKET,SO_BROADCAST,(char*)&value,&size);
	if(result==TOADLET_SOCKET_ERROR){
		error("getsockopt");
		return -1;
	}
	return value>0;
}

bool Socket::setTimeout(int milliseconds){
	struct timeval value={0};
	value.tv_sec=milliseconds/1000;
	value.tv_usec=(milliseconds%1000)*1000;
	int result=setsockopt(mHandle,SOL_SOCKET,SO_RCVTIMEO,(char*)&value,sizeof(value));
	if(result==TOADLET_SOCKET_ERROR){
		error("setsockopt");
		return false;
	}
	return true;
}

int Socket::getTimeout() const{
	struct timeval value={0};
	TOADLET_SOCKLEN size=sizeof(value);
	int result=getsockopt(mHandle,SOL_SOCKET,SO_RCVTIMEO,(char*)&value,&size);
	if(result==TOADLET_SOCKET_ERROR){
		error("getsockopt");
		return -1;
	}
	return value.tv_sec*1000 + value.tv_usec/1000;
}

bool Socket::pollRead(int millis){
	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(mHandle,&fd);

	timeval tv={0};
	tv.tv_sec=millis/1000;
	tv.tv_usec=(millis%1000)*1000;

	return select(mHandle+1,&fd,NULL,NULL,&tv)!=0;
}

bool Socket::pollWrite(int millis){
	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(mHandle,&fd);

	timeval tv={0};
	tv.tv_sec=millis/1000;
	tv.tv_usec=(millis%1000)*1000;

	return select(mHandle+1,NULL,&fd,NULL,&tv)!=0;
}

bool Socket::bind(int port){
	struct sockaddr_in address={0};
	address.sin_family=AF_INET;
	address.sin_addr.s_addr=htonl(INADDR_ANY);
	address.sin_port=htons(port);

	int result=::bind(mHandle,(struct sockaddr*)&address,sizeof(address));
	if(result==TOADLET_SOCKET_ERROR){
		error("bind");
		return false;
	}

	mBound=true;

	return true;
}

bool Socket::connect(uint32 ipAddress,int port){
	mHostIPAddress=ipAddress;
	mHostPort=port;

	struct sockaddr_in address={0};
	address.sin_family=AF_INET;
	address.sin_addr.s_addr=mHostIPAddress;
	address.sin_port=htons(mHostPort);

	int result=::connect(mHandle,(struct sockaddr*)&address,sizeof(address));
	if(result==TOADLET_SOCKET_ERROR){
		error("connect");
		return false;
	}

	mBound=true;
	mConnected=true;

	return true;
}


bool Socket::connect(const String &name,int port){
	Collection<uint32> adaptors;
	getHostAdaptorsByName(adaptors,name);
	if(adaptors.size()>0){
		return connect(adaptors[0],port);
	}
	else{
		return false;
	}
}

bool Socket::listen(int backlog){
	int result=::listen(mHandle,backlog);
	if(result==TOADLET_SOCKET_ERROR){
		error("listen");
		return false;
	}

	return true;
}

Socket *Socket::accept(){
	struct sockaddr_in clientAddress;
	TOADLET_SOCKLEN clientAddressLength=sizeof(clientAddress);

	int clientHandle=::accept(mHandle,(struct sockaddr*)&clientAddress,&clientAddressLength);
	if(clientHandle==TOADLET_SOCKET_ERROR){
		error("accept");
		return NULL;
	}
	else{
		Socket *socket=new Socket(clientHandle,&clientAddress);
		return socket;
	}
}

int Socket::receive(byte *buffer,int length){
	int flags=0;
	int result=::recv(mHandle,(char*)buffer,length,flags);
	if(result==TOADLET_SOCKET_ERROR){
		error("recv");
	}
	return result;
}

int Socket::receiveFrom(byte *buffer,int length,uint32 &ipAddress,int &port){
	int flags=0;
	struct sockaddr_in from={0};
	TOADLET_SOCKLEN fromLength=sizeof(sockaddr_in);
	int result=::recvfrom(mHandle,(char*)buffer,length,flags,(struct sockaddr*)&from,&fromLength);
	if(result==TOADLET_SOCKET_ERROR){
		error("recvfrom");
	}
	ipAddress=from.sin_addr.s_addr;
	port=ntohs(from.sin_port);
	return result;
}

int Socket::send(const byte *buffer,int length){
	#if defined(MSG_NOSIGNAL)
		int flags=MSG_NOSIGNAL;
	#else
		int flags=0;
	#endif
	int result=::send(mHandle,(char*)buffer,length,flags);
	if(result==TOADLET_SOCKET_ERROR){
		error("send");
	}
	return result;
}

int Socket::sendTo(const byte *buffer,int length,uint32 ipAddress,int port){
	#if defined(MSG_NOSIGNAL)
		int flags=MSG_NOSIGNAL;
	#else
		int flags=0;
	#endif
	struct sockaddr_in address={0};
	address.sin_family=AF_INET;
	address.sin_addr.s_addr=ipAddress;
	address.sin_port=htons(port);
	int result=::sendto(mHandle,(char*)buffer,length,flags,(struct sockaddr*)&address,sizeof(address));
	if(result==TOADLET_SOCKET_ERROR){
		error("sendto");
	}
	return result;
}

String Socket::ipToString(uint32 ip){
	return inet_ntoa(*(in_addr*)&ip);
}

uint32 Socket::stringToIP(const String &string){
	return inet_addr(string);
}

bool Socket::getHostAdaptorsByName(Collection<uint32> &adaptors,const String &name){
	struct hostent *he=gethostbyname(name);
	if(he!=NULL){
		char **list=he->h_addr_list;
		while((*list)!=NULL){
			adaptors.add(*(uint32*)(*list));
			++list;
		}
		return true;
	}
	else{
		return false;
	}
}

bool Socket::getHostAdaptorsByIP(Collection<uint32> &adaptors,uint32 ip){
	struct hostent *he=gethostbyaddr((char*)&ip,4,AF_INET);
	if(he!=NULL){
		char **list=he->h_addr_list;
		while((*list)!=NULL){
			adaptors.add(*(uint32*)(*list));
			++list;
		}
		return true;
	}
	else{
		return false;
	}
}

void Socket::error(const String &function){
	#if defined(TOADLET_PLATFORM_WIN32)
		Error::unknown(Categories::TOADLET_EGG_NET,
			String("Socket::")+function+"(): error "+(int)WSAGetLastError());
	#else
		Error::unknown(Categories::TOADLET_EGG_NET,
			String("Socket::")+function+"(): error "+(int)errno);
	#endif
}

}
}
}
