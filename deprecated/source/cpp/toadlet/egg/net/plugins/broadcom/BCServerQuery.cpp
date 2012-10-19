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

#include <toadlet/egg/System.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/net/plugins/broadcom/BCServerQuery.h>

// TODO: Get server pruning working
//#define USE_SERVER_PRUNE

namespace toadlet{
namespace egg{
namespace net{

TOADLET_C_API BluetoothServerQuery *new_BluetoothServerQuery(const String &guid){return new BCServerQuery(guid);}

BCServerQuery::BCServerQuery(const String &guid){
	mServiceName=guid;
	CLSIDFromString((wchar_t*)guid.wc_str(),&mServiceGUID);
}

BCServerQuery::~BCServerQuery(){
	cancelQuery();
}

int BCServerQuery::startQuery(){
	cancelQuery();

	mLock.lock();
		mServers.clear();
	mLock.unlock();

	TOADLET_LOG(NULL,Logger::LEVEL_DEBUG,
		"BCServerQuery: Starting server query");

	StartInquiry();

	mLock.lock();
		mQueryFinishedEvent.wait(&mLock);
	mLock.unlock();

	return mServers.size();
}

void BCServerQuery::cancelQuery(){
	StopInquiry();

	mLock.lock();
		mQueryFinishedEvent.notify();
	mLock.unlock();
}

int BCServerQuery::getNumServerEntries(){
	int num=0;
	mLock.lock();
		num=mServers.size();
	mLock.unlock();
	return num;
}

BluetoothServerEntry *BCServerQuery::getServerEntry(int i){
	if(mCachedServers.size()<=i){
		mCachedServers.resize(i+1);
	}

	mLock.lock();
		mCachedServers[i]=mServers[i];
	mLock.unlock();

	return &mCachedServers[i];
}

void BCServerQuery::OnDeviceResponded(BD_ADDR bda,DEV_CLASS devClass,BD_NAME bdName,BOOL bConnected){
	mLock.lock();
		int i;
		for(i=0;i<mServers.size();++i){
			if(memcmp(mServers[i].address,bda,sizeof(bda))==0){
				break;
			}
		}
		if(i==mServers.size()){
			mServers.addElement(BCServerEntry((char*)bdName,mServiceGUID,mServiceName,bda));
		}
	mLock.unlock();
}

void BCServerQuery::OnInquiryComplete(BOOL success,short num_responses){
	StopInquiry();

	mLock.lock();
		TOADLET_LOG(NULL,Logger::LEVEL_DEBUG,
			String("Num servers found:")+mServers.size());

		#ifdef USE_SERVER_PRUNE
			if(mServers.size()==0){
				if(mListener!=NULL){
					mListener->serverQueryFinished();
				}
			}
			else{
				int i;
				for(i=0;i<mServers.size();++i){
					StartDiscovery(*(BD_ADDR*)&mServers[i].mAddress,mServiceName,&mServiceName);
				}
			}
		#else
			mQueryFinishedEvent.notify();
		#endif
	mLock.unlock();
}

void BCServerQuery::OnDiscoveryComplete(){
#ifdef USE_SERVER_PRUNE
	mLock.lock();
		int i;
		for(i=0;i<mServers.size();++i){
			CSdpDiscoveryRec record;
			int num=ReadDiscoveryRecords(*(BD_ADDR*)&mServers[i].mAddress,1,&record,&mServiceName);
			if(num<=0){
				mServers.erase(mServers.begin()+i);
				i--;
			}
		}

		TOADLET_LOG(NULL,Logger::LEVEL_DEBUG,
			String("Num servers found after pruning:")+mServers.size());

		mQueryFinishedEvent.notify();
	mLock.unlock();
#endif
}

}
}
}
