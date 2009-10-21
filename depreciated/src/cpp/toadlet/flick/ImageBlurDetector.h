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

#ifndef TOADLET_FLICK_IMAGEBLURDETECTOR_H
#define TOADLET_FLICK_IMAGEBLURDETECTOR_H

#include <toadlet/flick/ImageBlurDetectorListener.h>

namespace toadlet{
namespace flick{

class ImageBlurDetector{
public:
	enum State{
		State_DESTROYED=0,
		State_STOPPED=1,
		State_RUNNING=2,
	};

	enum LightingResult{
		LightingResult_TOO_DIM=0,
		LightingResult_TOO_BRIGHT=1,
		LightingResult_GOOD=2,
	};

	static const int INVALID_ANGLE_DETECTED=-360;

	virtual ~ImageBlurDetector(){}

	virtual bool available()=0;

	virtual bool create()=0;

	virtual bool startup()=0;

	virtual bool shutdown()=0;

	virtual bool destroy()=0;

	virtual void setSleepTimes(int prestart,int precapture)=0;

	virtual void setListener(ImageBlurDetectorListener *listener)=0;

	virtual bool startDetection()=0;

	virtual int getDetectedAngle()=0;

	virtual int getDetectedAngleClusterSize()=0;

	virtual int getDetectedAngleClusterWidth()=0;

	virtual bool startLightingCheck()=0;

	virtual LightingResult getLightingResult()=0;

	virtual State getState()=0;
};

}
}

#endif
