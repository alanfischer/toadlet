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

#include <toadlet/egg/net/plugins/broadcom/BCStreamConnection.h>
#include <toadlet/egg/net/plugins/broadcom/BCServer.h>
#include <toadlet/egg/net/plugins/broadcom/BCClient.h>
#include <toadlet/egg/System.h>

using namespace toadlet::egg::io;

namespace toadlet{
namespace egg{
namespace net{

TOADLET_C_API StreamConnection *new_StreamConnection(){return new BCStreamConnection();}

// These are static so we can safely delete a BCStreamConnection when we get OnEventReceived
// calls right at destruction
Mutex BCStreamConnection::mLock;
Event BCStreamConnection::mLockEvent;
bool BCStreamConnection::mDeleted=false;

BCStreamConnection::BCStreamConnection(){
	mServerToNotify=NULL;
	mClientToNotify=NULL;
	mRfCommIf=new CRfCommIf();
	mClosed=false;
	mBuffer=NULL;
	mBufferAmount=0;
	mBufferLength=0;

	mReading=false;
	mDeleted=false;
}

BCStreamConnection::~BCStreamConnection(){
	Close();

	while(mReading){
		System::msleep(50);
	}

	mServerToNotify=NULL;
	mClientToNotify=NULL;
	delete mRfCommIf;
	mRfCommIf=NULL;
	if(mBuffer!=NULL){
		delete[] mBuffer;
		mBuffer=NULL;
	}
	mBufferAmount=0;
	mBufferLength=0;

	mLock.lock();
		mDeleted=true;
	mLock.unlock();
}

void BCStreamConnection::close(){
	mClosed=true;
	// We don't actually close until deletion
}

bool BCStreamConnection::isClosed() const{
	return mClosed;
}

InputStream *BCStreamConnection::getInputStream(){
	return this;
}

OutputStream *BCStreamConnection::getOutputStream(){
	return this;
}

int BCStreamConnection::read(char *buffer,int length){
	if(mClosed){
		return -1;
	}

	mReading=true;

	mLock.lock();
		while(true){
			mLockEvent.wait(&mLock,1000);

			if(mDeleted || mClosed){
				mLock.unlock();
				mReading=false;
				return 0;
			}

			if(length<=mBufferAmount){
				break;
			}
		}

		memcpy(buffer,mBuffer,length);
		mBufferAmount-=length;
		memmove(mBuffer,mBuffer+length,mBufferAmount);
	mLock.unlock();

	mReading=false;

	return length;
}

bool BCStreamConnection::reset(){
	return false;
}

bool BCStreamConnection::seek(int ofs){
	return false;
}

int BCStreamConnection::available(){
	int amount=0;
	mLock.lock();
		amount=mBufferAmount;
	mLock.unlock();
	return amount;
}

int BCStreamConnection::write(const char *buffer,int length){
	if(mClosed){
		return -1;
	}

	UINT16 written;
	Write((void*)buffer,length,&written);
	return written;
}

void BCStreamConnection::OnDataReceived(void *data,UINT16 length){
	mLock.lock();
		if(mBufferLength-mBufferAmount<length){
			char *buffer=new char[mBufferAmount+length];
			if(mBuffer!=NULL){
				memcpy(buffer,mBuffer,mBufferAmount);
				delete[] mBuffer;
			}
			mBuffer=buffer;
		}
		memcpy(mBuffer+mBufferAmount,data,length);
		mBufferAmount+=length;

		mLockEvent.notify();
	mLock.unlock();
}

void BCStreamConnection::OnEventReceived(UINT32 event){
	BCServer *serverToNotify=NULL;
	BCClient *clientToNotify=NULL;

	mLock.lock();
		if(mDeleted==false){
			if(event==PORT_EV_CONNECTED){
				if(mServerToNotify!=NULL){
					serverToNotify=mServerToNotify;
				}
				if(mClientToNotify!=NULL){
					clientToNotify=mClientToNotify;
				}
			}
			else if(event==PORT_EV_CONNECT_ERR){
				close();
			}
		}
	mLock.unlock();

	if(serverToNotify!=NULL){
		serverToNotify->connected();
	}
	if(clientToNotify!=NULL){
		clientToNotify->connected();
	}
}

}
}
}
