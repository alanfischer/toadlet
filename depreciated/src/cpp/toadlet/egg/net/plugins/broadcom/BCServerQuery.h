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

#ifndef TOADLET_EGG_NET_BCSERVERQUERY_H
#define TOADLET_EGG_NET_BCSERVERQUERY_H

#include <toadlet/egg/Thread.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/Event.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/net/BluetoothServerQuery.h>
#include <toadlet/egg/net/plugins/broadcom/BCServerEntry.h>
#include <toadlet/egg/net/plugins/broadcom/BCIncludes.h>

#pragma pack()

namespace toadlet{
namespace egg{
namespace net{

class BCServerQuery:public BluetoothServerQuery,public CBtIf{
public:
	BCServerQuery(const toadlet::egg::String &guid);
	virtual ~BCServerQuery();

	int startQuery();
	void cancelQuery();

	int getNumServerEntries();
	BluetoothServerEntry *getServerEntry(int i);

protected:
	void OnDeviceResponded(BD_ADDR bda,DEV_CLASS devClass,BD_NAME bdName,BOOL bConnected);
	void OnInquiryComplete(BOOL success,short num_responses);
	void OnDiscoveryComplete();

	String mServiceName;
	GUID mServiceGUID;
	toadlet::egg::Mutex mLock;
	toadlet::egg::Event mQueryFinishedEvent;
	toadlet::egg::Collection<BCServerEntry> mServers;
	toadlet::egg::Collection<BCServerEntry> mCachedServers;
};

}
}
}

#endif
