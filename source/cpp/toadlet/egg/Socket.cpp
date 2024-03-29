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

#include <toadlet/egg/Socket.h>
#include <toadlet/egg/Categories.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Log.h>
#include <time.h>

#if defined(TOADLET_PLATFORM_WIN32)
	#ifndef WIN32_LEAN_AND_MEAN
		#define WIN32_LEAN_AND_MEAN 1
	#endif
	#include <windows.h>
	#include <winsock.h>

	#define TOADLET_SOCKET_ERROR SOCKET_ERROR
	#define TOADLET_INVALID_SOCKET INVALID_SOCKET
	#define TOADLET_EINPROGRESS WSAEINPROGRESS-10000
	#define TOADLET_EWOULDBLOCK WSAEWOULDBLOCK-10000
	#define TOADLET_SOCKLEN int
#else
	#include <unistd.h>
	#include <netdb.h>
	#include <errno.h>
	#include <sys/socket.h>
	#include <sys/ioctl.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#if defined(TOADLET_PLATFORM_ANDROID)
		#include <net/if.h>
	#else
		#include <ifaddrs.h>
	#endif

	#define TOADLET_SOCKET_ERROR -1
	#define TOADLET_INVALID_SOCKET -1
	#define TOADLET_EINPROGRESS EINPROGRESS
	#define TOADLET_EWOULDBLOCK EWOULDBLOCK
	#define TOADLET_SOCKLEN socklen_t
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

#if defined(TOADLET_PLATFORM_WIN32)
	// We'll keep these errors in TOADLET_EGG, since they are more significant than the socket errors
	Socket::WSAHandler::WSAHandler(){
		mWSADATA=new WSADATA();
		int result=WSAStartup(MAKEWORD(1,1),mWSADATA);
		if(result!=0){
			Log::error(Categories::TOADLET_EGG,
				String("WSAHandler::WSAHandler(): error ")+result);
		}
	}

	Socket::WSAHandler::~WSAHandler(){
		int result=WSACleanup();
		delete mWSADATA;
		if(result!=0){
			Log::error(Categories::TOADLET_EGG,
				String("WSAHandler::~WSAHandler(): error ")+result);
		}
	}

	Socket::WSAHandler Socket::mWSAHandler;
#endif

Socket::Socket():
	mHandle(TOADLET_INVALID_SOCKET),
	mBlocking(true),
	mHostIPAddress(0),
	mHostPort(0)
{
}

Socket::Socket(int domain,int type,int protocol):
	mHandle(TOADLET_INVALID_SOCKET),
	mBlocking(true),
	mHostIPAddress(0),
	mHostPort(0)
{
	mHandle=socket(domain,type,protocol);
	if(mHandle==TOADLET_INVALID_SOCKET){
		Error::socket(Categories::TOADLET_EGG,
			String("Socket:")+error());
		return;
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
	mBlocking(true),
	mHostIPAddress(address->sin_addr.s_addr),
	mHostPort(address->sin_port)
{
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
	if(mHandle!=TOADLET_INVALID_SOCKET){
		#if defined(TOADLET_PLATFORM_WIN32)
			shutdown(mHandle,2);
			::closesocket(mHandle);
		#else
			shutdown(mHandle,SHUT_RDWR);
			::close(mHandle);
		#endif
		mHandle=TOADLET_INVALID_SOCKET;
	}
	mHostIPAddress=0;
	mHostPort=0;
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
		Error::socket(Categories::TOADLET_EGG,
			String("setBlocking:")+error());
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
		Error::socket(Categories::TOADLET_EGG,
			String("setSendBufferSize:")+error());
		return false;
	}
	return true;
}

int Socket::getSendBufferSize() const{
	int value=0;
	TOADLET_SOCKLEN size=sizeof(value);
	int result=getsockopt(mHandle,SOL_SOCKET,SO_SNDBUF,(char*)&value,&size);
	if(result==TOADLET_SOCKET_ERROR){
		Error::socket(Categories::TOADLET_EGG,
			String("getSendBufferSize:")+error());
		return -1;
	}
	return size;
}

bool Socket::setRecieveBufferSize(int size){
	int result=setsockopt(mHandle,SOL_SOCKET,SO_RCVBUF,(char*)&size,sizeof(size));
	if(result==TOADLET_SOCKET_ERROR){
		Error::socket(Categories::TOADLET_EGG,
			String("setReceiveBufferSize:")+error());
		return false;
	}
	return true;
}

int Socket::getRecieveBufferSize() const{
	int value=0;
	TOADLET_SOCKLEN size=sizeof(value);
	int result=getsockopt(mHandle,SOL_SOCKET,SO_RCVBUF,(char*)&value,&size);
	if(result==TOADLET_SOCKET_ERROR){
		Error::socket(Categories::TOADLET_EGG,
			String("getRecieveBufferSize:")+error());
		return -1;
	}
	return value;
}

bool Socket::setBroadcast(bool broadcast){
	int value=broadcast;
	int result=setsockopt(mHandle,SOL_SOCKET,SO_BROADCAST,(char*)&value,sizeof(value));
	if(result==TOADLET_SOCKET_ERROR){
		Error::socket(Categories::TOADLET_EGG,
			String("setBroadcast:")+error());
		return false;
	}
	return true;
}

bool Socket::getBroadcast() const{
	int value=0;
	TOADLET_SOCKLEN size=sizeof(value);
	int result=getsockopt(mHandle,SOL_SOCKET,SO_BROADCAST,(char*)&value,&size);
	if(result==TOADLET_SOCKET_ERROR){
		Error::socket(Categories::TOADLET_EGG,
			String("getBroadcast:")+error());
		return false;
	}
	return value>0;
}

bool Socket::setTimeout(int milliseconds){
	struct timeval value={0};
	value.tv_sec=milliseconds/1000;
	value.tv_usec=(milliseconds%1000)*1000;
	int result=setsockopt(mHandle,SOL_SOCKET,SO_RCVTIMEO,(char*)&value,sizeof(value));
	if(result==TOADLET_SOCKET_ERROR){
		Error::socket(Categories::TOADLET_EGG,
			String("setTimeout:")+error());
		return false;
	}
	return true;
}

int Socket::getTimeout() const{
	struct timeval value={0};
	TOADLET_SOCKLEN size=sizeof(value);
	int result=getsockopt(mHandle,SOL_SOCKET,SO_RCVTIMEO,(char*)&value,&size);
	if(result==TOADLET_SOCKET_ERROR){
		Error::socket(Categories::TOADLET_EGG,
			String("getTimeout:")+error());
		return -1;
	}
	return value.tv_sec*1000 + value.tv_usec/1000;
}

bool Socket::setKeepAlive(bool keepAlive){
	int value=keepAlive;
	int result=setsockopt(mHandle,SOL_SOCKET,SO_KEEPALIVE,(char*)&value,sizeof(value));
	if(result==TOADLET_SOCKET_ERROR){
		Error::socket(Categories::TOADLET_EGG,
			String("setKeepAlive:")+error());
		return false;
	}
	return true;
}

bool Socket::getKeepAlive() const{
	int value=0;
	TOADLET_SOCKLEN size=sizeof(value);
	int result=getsockopt(mHandle,SOL_SOCKET,SO_KEEPALIVE,(char*)&value,&size);
	if(result==TOADLET_SOCKET_ERROR){
		Error::socket(Categories::TOADLET_EGG,
			String("getKeepAlive:")+error());
		return -1;
	}
	return value!=0;
}

bool Socket::addMembership(uint32 ipAddress){
	struct ip_mreq mreq={{0}};
	mreq.imr_multiaddr.s_addr=ipAddress;
	mreq.imr_interface.s_addr=htonl(INADDR_ANY);

	int result=setsockopt(mHandle,IPPROTO_IP,IP_ADD_MEMBERSHIP,(const char *)&mreq,sizeof(struct ip_mreq));
	if(result==TOADLET_SOCKET_ERROR){
		Error::socket(Categories::TOADLET_EGG,
			String("addMembership:")+strerror(error()));
		return false;
	}
	return true;
}

bool Socket::pollRead(int millis){
	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(mHandle,&fd);

	timeval tv={0};
	tv.tv_sec=millis/1000;
	tv.tv_usec=(millis%1000)*1000;

	return select(mHandle+1,&fd,NULL,NULL,&tv)>0;
}

bool Socket::pollWrite(int millis){
	fd_set fd;
	FD_ZERO(&fd);
	FD_SET(mHandle,&fd);

	timeval tv={0};
	tv.tv_sec=millis/1000;
	tv.tv_usec=(millis%1000)*1000;

	return select(mHandle+1,NULL,&fd,NULL,&tv)>0;
}

bool Socket::bind(int port){
	return bind(htonl(INADDR_ANY),port);
}

bool Socket::bind(uint32 ipAddress,int port){
	struct sockaddr_in address={0};
	address.sin_family=AF_INET;
	address.sin_addr.s_addr=ipAddress;
	address.sin_port=htons(port);

	int result=::bind(mHandle,(struct sockaddr*)&address,sizeof(address));
	if(result==TOADLET_SOCKET_ERROR){
		result=error();
		Error::socket(Categories::TOADLET_EGG,
			String("bind:")+result);
		return false;
	}

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
		result=error();
		if(result!=TOADLET_EINPROGRESS){
			Error::socket(Categories::TOADLET_EGG,
				String("connect:")+result);
		}
		return false;
	}

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
		result=error();
		Error::socket(Categories::TOADLET_EGG,
			String("listen:")+result);
		return false;
	}

	return true;
}

Socket *Socket::accept(){
	struct sockaddr_in clientAddress;
	TOADLET_SOCKLEN clientAddressLength=sizeof(clientAddress);

	int clientHandle=::accept(mHandle,(struct sockaddr*)&clientAddress,&clientAddressLength);
	if(clientHandle==TOADLET_SOCKET_ERROR){
		int result=error();
		if(result!=TOADLET_EWOULDBLOCK){
			Error::socket(Categories::TOADLET_EGG,
				String("accept:")+result);
		}
		return NULL;
	}
	else{
		Socket *socket=new Socket(clientHandle,&clientAddress);
		return socket;
	}
}

int Socket::receive(tbyte *buffer,int length){
	int flags=0;
	int result=::recv(mHandle,(char*)buffer,length,flags);
	if(result<0){
		result=-error();
		Log::debug(Categories::TOADLET_EGG,
			String("receive:")+result);
	}
	return result;
}

int Socket::receiveFrom(tbyte *buffer,int length,uint32 &ipAddress,int &port){
	int flags=0;
	struct sockaddr_in from={0};
	TOADLET_SOCKLEN fromLength=sizeof(sockaddr_in);
	int result=::recvfrom(mHandle,(char*)buffer,length,flags,(struct sockaddr*)&from,&fromLength);
	ipAddress=from.sin_addr.s_addr;
	port=ntohs(from.sin_port);
	if(result<0){
		result=-error();
		Log::debug(Categories::TOADLET_EGG,
			String("receiveFrom:")+result);
	}
	return result;
}

int Socket::send(const tbyte *buffer,int length){
	#if defined(MSG_NOSIGNAL)
		int flags=MSG_NOSIGNAL;
	#else
		int flags=0;
	#endif
	int result=::send(mHandle,(char*)buffer,length,flags);
	if(result<0){
		result=-error();
		Log::debug(Categories::TOADLET_EGG,
			String("send:")+result);
	}
	return result;
}

int Socket::sendTo(const tbyte *buffer,int length,uint32 ipAddress,int port){
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
	if(result<0){
		result=-error();
		Log::debug(Categories::TOADLET_EGG,
			String("sendTo:")+result);
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
			adaptors.push_back(*(uint32*)(*list));
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
			adaptors.push_back(*(uint32*)(*list));
			++list;
		}
		return true;
	}
	else{
		return false;
	}
}


bool Socket::getLocalAdaptors(Collection<uint32> &adaptors){
	#if defined(TOADLET_PLATFORM_WIN32)
		return getHostAdaptorsByName(adaptors,"");
	#elif defined(TOADLET_PLATFORM_ANDROID)
		struct ifreq ifreqs[20];
		struct ifconf ifconf;

		memset(&ifconf,0,sizeof(ifconf));
		ifconf.ifc_buf = (char*) (ifreqs);
		ifconf.ifc_len = sizeof(ifreqs);

		int sock=::socket(AF_INET,SOCK_STREAM,0);
		ioctl(sock,SIOCGIFCONF,(char*)&ifconf);
		::close(sock);

		int count=ifconf.ifc_len/sizeof(struct ifreq);
		for(int i=0;i<count;++i){
			adaptors.push_back(((struct sockaddr_in*)&ifreqs[i].ifr_addr)->sin_addr.s_addr);
		}
	#else
		struct ifaddrs *addrs,*a=NULL;
		getifaddrs(&addrs);
		for(a=addrs;a!=NULL;a=a->ifa_next){
			if(a->ifa_addr->sa_family==AF_INET){
				adaptors.push_back(((struct sockaddr_in*)a->ifa_addr)->sin_addr.s_addr);
			}
		}
		freeifaddrs(addrs);
		return true;
	#endif
}


int Socket::error() const{
	#if defined(TOADLET_PLATFORM_WIN32)
		return WSAGetLastError()-10000;
	#else
		return errno;
	#endif
}

}
}
