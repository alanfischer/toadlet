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

#ifndef TOADLET_FLICK_DSIMAGEBLURDETECTOR_H
#define TOADLET_FLICK_DSIMAGEBLURDETECTOR_H

#include "DSIncludes.h"
#include <toadlet/flick/ImageBlurDetector.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/WaitCondition.h>
#include <toadlet/egg/DynamicLibrary.h>
#include <toadlet/egg/Logger.h>

#include "PropertyBag.h"
#include "dump.h"

namespace toadlet{
namespace flick{

class DSImageBlurDetector:public ImageBlurDetector,public egg::Runnable{
public:
	DSImageBlurDetector();
	virtual ~DSImageBlurDetector();

	bool available();

	bool create();

	bool startup();

	bool shutdown();

	bool destroy();

	void setSleepTimes(int prestart,int precapture);

	void setListener(ImageBlurDetectorListener *listener);

	bool startDetection();

	int getDetectedAngle();

	int getDetectedAngleClusterSize();

	int getDetectedAngleClusterWidth();

	bool startLightingCheck();

	LightingResult getLightingResult();

	State getState();

	void run();

protected:
	enum Status{
		Status_STARTING,
		Status_READY,
		Status_BUSY,
		Status_STOPPED,
	};

	void findAngleInImage(int data[],int width,int height,int &angle,int &angleClusterSize,int &angleClusterWidth);
	int sumLine(int x0,int y0,int x1,int y1,int data[],int width,int height);
	void radon(int data[],int width,int height,float theta,int results[],int spacing);

	void drawLine(int x0,int y0,int x1,int y1,int data[],int width,int height);
	void drawLineAtAngle(int data[],int width,int height,float theta);		

	egg::String getFirstCameraDriver();

	HRESULT createCaptureGraph();
	HRESULT runCaptureGraph();
	HRESULT captureStillImage();
	HRESULT stopCaptureGraph();
	HRESULT destroyCaptureGraph();

	State mState;

	egg::Thread *mDetectionThread;
	
	ImageBlurDetectorListener *mListener;
	int mAngle;
	int mAngleClusterSize;
	int mAngleClusterWidth;
	LightingResult mLighting;

	bool mRun;
	bool mAlwaysRunGraph;
	Status mStatus;
	bool mCheckLighting;
	LightingResult mLightingResult;
	bool mGraphBuilt;
	bool mGraphRunning;
	bool mStartImageTaking;

	int *mImageData;
	int mImageWidth;
	int mImageHeight;

	int mPrestartSleepTime;
	int mPrecaptureSleepTime;

	const static int mTotalClusters=5;

	egg::Mutex mLock;
	egg::WaitCondition mLockWaitCondition;
	CComPtr<ICaptureGraphBuilder2>  mCaptureGraphBuilder;
	CComPtr<IBaseFilter>			mVideoCaptureFilter;
	CComPtr<CDump>					mDump;
	CComPtr<IMediaEvent>            mMediaEvent;

	egg::DynamicLibrary mCoreDLL;
	typedef HANDLE(*FindFirstDeviceFunc)(DeviceSearchType,LPCVOID,PDEVMGR_DEVICE_INFORMATION);
	FindFirstDeviceFunc mFindFirstDeviceFunc;
};

}
}

#endif

