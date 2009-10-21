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

#ifndef TOADLET_EGG_NET_BCCLIENT_H
#define TOADLET_EGG_NET_BCCLIENT_H

#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/net/BluetoothClient.h>
#include <toadlet/egg/net/plugins/broadcom/BCIncludes.h>
#include <toadlet/egg/net/plugins/broadcom/BCStreamConnection.h>

#pragma pack()

namespace toadlet{
namespace egg{
namespace net{

class BCClient:public BluetoothClient,public CBtIf{
public:
	BCClient();
	virtual ~BCClient();

	bool connect(BluetoothServerEntry *serverEntry);
	bool isConnected() const;

	bool disconnect();

	StreamConnection *getStreamConnection();
	DatagramConnection *getDatagramConnection();

protected:
	void OnDiscoveryComplete();
	void connected();

	bool mConnectionCompleted;
	bool mDiscoveryComplete;
	BCStreamConnection *mConnection;
	int mMTU;

	friend BCStreamConnection;
};

}
}
}

#endif
