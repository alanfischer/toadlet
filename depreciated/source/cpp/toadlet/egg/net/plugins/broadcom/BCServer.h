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

#ifndef TOADLET_EGG_NET_BCSERVER_H
#define TOADLET_EGG_NET_BCSERVER_H

#include <toadlet/egg/String.h>
#include <toadlet/egg/net/BluetoothServer.h>
#include <toadlet/egg/net/plugins/broadcom/BCStreamConnection.h>
#include <toadlet/egg/net/plugins/broadcom/BCIncludes.h>

#pragma pack()

namespace toadlet{
namespace egg{
namespace net{

class BCServer:public BluetoothServer,public CBtIf{
public:
	BCServer();
	virtual ~BCServer();

	bool accept(const String &guid);
	bool isConnected() const;

	bool disconnect();

	StreamConnection *getStreamConnection();
	DatagramConnection *getDatagramConnection();

protected:
	void connected();

	bool mServerCanceled;
	bool mServerConnected;
	String mGUID;
	BCStreamConnection *mConnection;
	CSdpService *mSdpService;
	int mMTU;

	friend BCStreamConnection;
};

}
}
}

#endif
