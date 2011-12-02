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

#include <toadlet/egg/net/platform/wince/WinCEConnectionManager.h>
#include <toadlet/egg/Error.h>
#include <initguid.h>
#include <connmgr.h>

#pragma comment(lib,"cellcore.lib")

namespace toadlet{
namespace egg{
namespace net{

WinCEConnectionManager::WinCEConnectionManager(){
	mConnection=NULL;
}

WinCEConnectionManager::~WinCEConnectionManager(){
	disconnect();
}

bool WinCEConnectionManager::connect(int timeout){
	CONNMGR_CONNECTIONINFO connectionInfo={0};
	connectionInfo.cbSize=sizeof(connectionInfo);
	connectionInfo.dwPriority=CONNMGR_PRIORITY_USERINTERACTIVE;
	connectionInfo.dwParams=CONNMGR_PARAM_GUIDDESTNET;
	connectionInfo.guidDestNet=IID_DestNetInternet;
	connectionInfo.bExclusive=false;
	connectionInfo.bDisabled=false;

	DWORD status=0;

	HRESULT hr=ConnMgrEstablishConnectionSync(&connectionInfo,&mConnection,timeout,&status);
	if(FAILED(hr)){
		Error::unknown(Categories::TOADLET_EGG,
			String("connection failed with:")+(int)status);
		return false;
	}

	return true;
}

bool WinCEConnectionManager::disconnect(){
	if(mConnection!=NULL){
		HRESULT hr=ConnMgrReleaseConnection(mConnection,true);
		mConnection=NULL;
		return SUCCEEDED(hr);
	}

	return false;
}

}
}
}
