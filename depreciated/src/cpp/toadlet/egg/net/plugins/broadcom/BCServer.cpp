/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#include <toadlet/egg/net/plugins/broadcom/BCServer.h>
#include <toadlet/egg/System.h>

namespace toadlet{
namespace egg{
namespace net{

TOADLET_C_API BluetoothServer *new_BluetoothServer(){return new BCServer();}

BCServer::BCServer(){
	mServerCanceled=false;
	mServerConnected=false;
	mConnection=NULL;
	mSdpService=NULL;
	mMTU=RFCOMM_DEFAULT_MTU;
}

BCServer::~BCServer(){
	disconnect();

	// We call this here, since there should only currently be one server, and it will die when everything else does.
	WIDCOMMSDK_ShutDown();
}

bool BCServer::accept(const String &guid){
	// Convert our GUID string to a win32 GUID object
	GUID serviceGUID;
	CLSIDFromString((wchar_t*)guid.wc_str(),&serviceGUID);

	String serviceName=guid;

	disconnect();

	mServerCanceled=false;
	mServerConnected=false;

#	if defined(TOADLET_PLATFORM_WINCE)
		AllowToConnect(CONNECT_ALLOW_ALL);
		SetDiscoverable(true);
#	endif

	mConnection=new BCStreamConnection();
	mConnection->mServerToNotify=this;

	if(!mConnection->mRfCommIf->AssignScnValue(&serviceGUID)){
		disconnect();
		return false;
	}

	mSdpService=new CSdpService();
	if(mSdpService->AddServiceClassIdList(1,&serviceGUID)!=SDP_OK){
		disconnect();
		return false;
	}

	if(mSdpService->AddServiceName(toBT_CHAR(serviceName))!=SDP_OK){
		disconnect();
		return false;
	}

	if(mSdpService->AddRFCommProtocolDescriptor(mConnection->mRfCommIf->GetScn())!=SDP_OK){
		disconnect();
		return false;
	}

	if(mSdpService->MakePublicBrowseable()!=SDP_OK){
		disconnect();
		return false;
	}

	bool isServer=true;
	if(!mConnection->mRfCommIf->SetSecurityLevel(toBT_CHAR(serviceName),BTM_SEC_NONE,isServer)){
		disconnect();
		return false;
	}

	if(mConnection->OpenServer(mConnection->mRfCommIf->GetScn(),mMTU)!=SUCCESS){
		disconnect();
		return false;
	}

	while(mServerConnected==false && mServerCanceled==false){
		System::msleep(10);
	}

	return true;
}

bool BCServer::isConnected() const{
	return mServerConnected;
}

bool BCServer::disconnect(){
	mServerConnected=false;

	if(mSdpService!=NULL){
		delete mSdpService;
		mSdpService=NULL;
	}

	if(mConnection!=NULL){
		delete mConnection;
		mConnection=NULL;
	}

	return true;
}

StreamConnection *BCServer::getStreamConnection(){
	return mConnection;
}

DatagramConnection *BCServer::getDatagramConnection(){
	return NULL;
}

void BCServer::connected(){
	if(mServerCanceled==false){
		mServerConnected=true;
	}
}

}
}
}
