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

#if 0

#include <toadlet/egg/System.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/net/Socket.h>
#include <toadlet/egg/net/platform/win32/Win32BluetoothServerQuery.h>

namespace toadlet{
namespace egg{
namespace net{

TOADLET_C_API BluetoothServerQuery *new_BluetoothServerQuery(const String &guid){return new Win32BluetoothServerQuery(guid);}

Win32BluetoothServerQuery::Win32BluetoothServerQuery(const String &guid){
	mLookup=0;
	CLSIDFromString((LPOLESTR)guid.wc_str(),&mGUID);
}

Win32BluetoothServerQuery::~Win32BluetoothServerQuery(){
	cancelQuery();

	clearServers();
}

int Win32BluetoothServerQuery::startQuery(){
	cancelQuery();

	clearServers();

	Logger::debug(Categories::TOADLET_EGG,
		"Win32BluetoothServerQuery: Starting server query");

	WSAQUERYSET wsaq={0};
	wsaq.dwSize=sizeof(wsaq);
	wsaq.dwNameSpace=NS_BTH;
	wsaq.lpcsaBuffer=NULL;
	wsaq.lpServiceClassId=&mGUID;

	// initialize searching procedure
	int result=WSALookupServiceBegin(&wsaq,LUP_CONTAINERS,&mLookup);
	if(result==0){
		Error::unknown(Categories::TOADLET_EGG,
			String("WSALookupServiceBegin error:")+WSAGetLastError());
		return -1;
	}

	union{
		CHAR buffer[5000];			// returned struct can be quite large
		SOCKADDR_BTH __unused;		// properly align buffer to BT_ADDR requirements
	};

	LPWSAQUERYSET pwsaResults;
	DWORD dwSize=0;
	while(true){
		pwsaResults=(LPWSAQUERYSET)buffer;

		dwSize=sizeof(buffer);

		ZeroMemory(pwsaResults,sizeof(WSAQUERYSET));
		pwsaResults->dwSize=sizeof(WSAQUERYSET);
		// namespace MUST be NS_BTH for bluetooth queries
		pwsaResults->dwNameSpace=NS_BTH;
		pwsaResults->lpBlob=NULL;

		// iterate through all found devices, returning name and address
		// (this sample only uses the name, but address could be used for
		// further queries)
		result=WSALookupServiceNext(mLookup,LUP_RETURN_NAME|LUP_RETURN_ADDR,&dwSize,pwsaResults);

		if(result!=0){
			result=WSAGetLastError();
			if(result==WSAENETDOWN){
				WSALookupServiceEnd(mLookup);
				mLookup=0;
				Error::bluetoothDisabled(Categories::TOADLET_EGG,
					"Bluetooth disabled");
				return -1;
			}
			else if(result!=WSA_E_NO_MORE){
			}
			// we're finished
			break;
		}

		SOCKADDR_BTH *bthaddr=(SOCKADDR_BTH*)pwsaResults->lpcsaBuffer->RemoteAddr.lpSockaddr;

		mLock.lock();
			mServerNames.add(pwsaResults->lpszServiceInstanceName);
			mServers.add(new BluetoothAddress(bthaddr->btAddr,bthaddr->port));
		mLock.unlock();
	}

	WSALookupServiceEnd(mLookup);
	mLookup=0;

	return mServers.size();
}

void Win32BluetoothServerQuery::cancelQuery(){
	mLock.lock();
		if(mLookup!=0){
			// We're not actually going to call this, because it DOESNT FREAKING WORK
			// It either hangs, or it doesnt unblock the call below.  So we'll just let it be
			//WSALookupServiceEnd(mLookup);
			mLookup=0;
		}
	mLock.unlock();
}

int Win32BluetoothServerQuery::getNumServers(){
	int num=0;
	mLock.lock();
		num=mServers.size();
	mLock.unlock();
	return num;
}

const String &Win32BluetoothServerQuery::getServerName(int i){
	mLock.lock();
		cacheName=mServerNames[i];
	mLock.unlock();
	return cacheName;
}

BluetoothAddress *Win32BluetoothServerQuery::getServerAddress(int i){
	BluetoothAddress *address=NULL;
	mLock.lock();
		address=mServers[i];
	mLock.unlock();
	return address;
}

void Win32BluetoothServerQuery::clearServers(){
	mLock.lock();
		int i;
		for(i=0;i<mServers.size();++i){
			delete mServers[i];
		}
		mServers.clear();
	mLock.unlock();
}

}
}
}

#endif
