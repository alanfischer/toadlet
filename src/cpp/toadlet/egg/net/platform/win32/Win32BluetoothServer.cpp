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

#include <toadlet/egg/net/platform/win32/Win32BluetoothServer.h>

namespace toadlet{
namespace egg{
namespace net{

TOADLET_C_API BluetoothServer *new_BluetoothServer(){return new Win32BluetoothServer();}

Win32BluetoothServer::Win32BluetoothServer():
	Socket(PF_BTH,SOCK_STREAM,BTHPROTO_RFCOMM),
	//mGUID,
	//mLock,
	mClient(NULL)
{}

Win32BluetoothServer::~Win32BluetoothServer(){
	close();
}

bool Win32BluetoothServer::accept(const String &guid){
	GUID serviceGUID;
	CLSIDFromString((LPOLESTR)guid.wc_str(),&serviceGUID);

	SOCKADDR_BTH address={0};
	address.addressFamily=AF_BTH;

	int result=::bind(mHandle,(struct sockaddr*)&address,sizeof(address));
	if(result==TOADLET_SOCKET_ERROR){
		error("bind");
		return false;
	}

	int nameLength=sizeof(address);
	result=getsockname(mHandle,(struct sockaddr*)&address,&nameLength);
	if(result==TOADLET_SOCKET_ERROR){
		error("getsockname");
		return false;
	}

	result=registerService(serviceGUID,(UCHAR)address.port);
	if(result!=0){ // Not TOADLET_SOCKET_ERROR here
		error("registerService");
		return false;
	}

	result=::listen(mHandle,SOMAXCONN);
	if(result==TOADLET_SOCKET_ERROR){
		error("listen");
		return false;
	}

	SOCKADDR_BTH clientAddress={0};
	int clientAddressLength=sizeof(clientAddress);

	int clientHandle=::accept(mHandle,(struct sockaddr*)&clientAddress,&clientAddressLength);
	if(clientHandle==TOADLET_SOCKET_ERROR){
		error("accept");
		return false;
	}
	else{
		mClient=new Socket(clientHandle,0,0);

		return true;
	}
}

bool Win32BluetoothServer::isConnected() const{
	return mClient!=NULL && mClient->isConnected();
}

void Win32BluetoothServer::close(){
	if(mClient!=NULL){
		mClient->close();
		delete mClient;
		mClient=NULL;
	}

	Socket::close();
}

int Win32BluetoothServer::receive(char *buffer,int length){
	if(mClient!=NULL){
		return mClient->receive(buffer,length);
	}
	else{
		return -1;
	}
}

int Win32BluetoothServer::send(const char *buffer,int length){
	if(mClient!=NULL){
		return mClient->send(buffer,length);
	}
	else{
		return -1;
	}
}

int Win32BluetoothServer::registerService(const GUID &guid,unsigned char port){
	char record[]={
						0x35, 0x00, 0x09, 0x00, 0x01, 0x35, 0x11, 0x1c,
/* GUID HERE */			0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
/* L2CAP + RFCOMM */	0x09, 0x00, 0x04, 0x35, 0x0c, 0x35, 0x03, 0x19,
						0x01, 0x00, 0x35, 0x05, 0x19, 0x00, 0x03, 0x08,
/* CHANNEL NUM HERE */	0x00
	};

	record[1]=sizeof(record)-2;

	char *guidArray=(char*)&guid;

	record[8]=guidArray[3];
	record[9]=guidArray[2];
	record[10]=guidArray[1];
	record[11]=guidArray[0];

	record[12]=guidArray[5];
	record[13]=guidArray[4];

	record[14]=guidArray[7];
	record[15]=guidArray[6];

	memcpy(&record[16],&guidArray[8],8);

	record[sizeof(record)-1]=port;

	ULONG recordHandle=0;

	struct bigBlob{
		BTHNS_SETBLOB b;
	} *pBigBlob;

	pBigBlob=(bigBlob*)malloc(sizeof(struct bigBlob)+sizeof(record));
	ULONG ulSdpVersion=BTH_SDP_VERSION;
	pBigBlob->b.pRecordHandle   = &recordHandle;
	pBigBlob->b.pSdpVersion     = &ulSdpVersion;
	pBigBlob->b.fSecurity       = 0;
	pBigBlob->b.fOptions        = 0;
	pBigBlob->b.ulRecordLength  = sizeof(record);

	memcpy(pBigBlob->b.pRecord,record,sizeof(record));

	BLOB blob;
	blob.cbSize    = sizeof(BTHNS_SETBLOB) + sizeof(record) - 1;
	blob.pBlobData = (PBYTE) pBigBlob;

	WSAQUERYSET service={0};
	service.dwSize=sizeof(service);
	service.lpBlob=&blob;
	service.dwNameSpace=NS_BTH;

	int result=WSASetService(&service,RNRSERVICE_REGISTER,0);
	free(pBigBlob);
	if(result==TOADLET_SOCKET_ERROR){
		return WSAGetLastError();
	}
	else{
		return 0;
	}
}

}
}
}
