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

#ifndef TOADLET_EGG_NET_BCSTREAMCONNECTION_H
#define TOADLET_EGG_NET_BCSTREAMCONNECTION_H

#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/Event.h>
#include <toadlet/egg/net/StreamConnection.h>
#include <toadlet/egg/net/plugins/broadcom/BCIncludes.h>

#pragma pack()

namespace toadlet{
namespace egg{
namespace net{

class BCServer;
class BCClient;

class BCStreamConnection:public StreamConnection,public CRfCommPort,protected toadlet::egg::io::InputStream,protected toadlet::egg::io::OutputStream{
public:
	BCStreamConnection();
	virtual ~BCStreamConnection();

	virtual void close();
	virtual bool isClosed() const;

	virtual toadlet::egg::io::InputStream *getInputStream();

	virtual toadlet::egg::io::OutputStream *getOutputStream();

protected:
	virtual int read(char *buffer,int length);
	virtual bool reset();
	virtual bool seek(int offs);
	virtual int available();

	virtual int write(const char *buffer,int length);

	void OnDataReceived(void *data,UINT16 length);
	void OnEventReceived(UINT32 event);

	BCServer *mServerToNotify;
	BCClient *mClientToNotify;

	CRfCommIf *mRfCommIf;
	bool mClosed;
	bool mReading;
	char *mBuffer;
	int mBufferAmount;
	int mBufferLength;

	static Mutex mLock;
	static Event mLockEvent;
	static bool mDeleted;

	friend BCServer;
	friend BCClient;
};

}
}
}

#endif
