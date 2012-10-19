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

#include <toadlet/egg/net/plugins/broadcom/BCClient.h>
#include <toadlet/egg/net/plugins/broadcom/BCServerEntry.h>
#include <toadlet/egg/System.h>

namespace toadlet{
namespace egg{
namespace net{

TOADLET_C_API BluetoothClient *new_BluetoothClient(){return new BCClient();}

BCClient::BCClient(){
	mConnectionCompleted=false;
	mDiscoveryComplete=false;
	mConnection=NULL;
	mMTU=RFCOMM_DEFAULT_MTU;
}

BCClient::~BCClient(){
	disconnect();
}

bool BCClient::connect(BluetoothServerEntry *serverEntry){
	disconnect();

	BCServerEntry *bcServerEntry=(BCServerEntry*)serverEntry;

	mConnection=new BCStreamConnection();
	mConnection->mClientToNotify=this;

	BOOL result=StartDiscovery(bcServerEntry->address,&bcServerEntry->serviceGUID);
	if(result==FALSE){
		disconnect();
		return false;
	}

	// Just so we can't get stuck here forever
	mDiscoveryComplete=false;
	int t=GetTickCount();
	while((GetTickCount()-t)<10000 && mDiscoveryComplete==false){
		System::msleep(10);
	}

	CSdpDiscoveryRec record;
	ReadDiscoveryRecords(bcServerEntry->address,1,&record,&bcServerEntry->serviceGUID);
	UINT8 scn=0;
	result=record.FindRFCommScn(&scn);
	// We'll continue on anyway, this seems to happen occasionally
//	if(result==FALSE){
//		disconnect();
//		return false;
//	}

	result=mConnection->mRfCommIf->AssignScnValue(&bcServerEntry->serviceGUID,scn);
	if(result==false){
		disconnect();
		return false;
	}

	bool isServer=false;
	result=mConnection->mRfCommIf->SetSecurityLevel(toBT_CHAR(bcServerEntry->serviceName),BTM_SEC_NONE,isServer);
	if(result==false){
		disconnect();
		return false;
	}

	mConnectionCompleted=false;
	int portResult=mConnection->OpenClient(scn,bcServerEntry->address,mMTU);
	if(portResult!=SUCCESS){
		disconnect();
		return false;
	}

	t=GetTickCount();
	while((GetTickCount()-t)<10000 && mConnectionCompleted==false){
		System::msleep(10);
	}

	if(mConnectionCompleted==false){
		disconnect();
	}

	return mConnection!=NULL;
}

bool BCClient::isConnected() const{
	return mConnectionCompleted;
}

bool BCClient::disconnect(){
	if(mConnection!=NULL){
		delete mConnection;
		mConnection=NULL;
	}

	return true;
}

StreamConnection *BCClient::getStreamConnection(){
	return mConnection;
}

DatagramConnection *BCClient::getDatagramConnection(){
	return NULL;
}

void BCClient::OnDiscoveryComplete(){
	mDiscoveryComplete=true;
}

void BCClient::connected(){
	mConnectionCompleted=true;
}

}
}
}
