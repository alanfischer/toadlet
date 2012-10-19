/********** Copyright header - do not remove **********
 *
 * Lawn Darts
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the game Dropscape, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_FLICK_IMAGEBLURDETECTORLISTENER_H
#define TOADLET_FLICK_IMAGEBLURDETECTORLISTENER_H

namespace toadlet{
namespace flick{

class ImageBlurDetectorListener{
public:
	virtual ~ImageBlurDetectorListener(){}

	virtual void imageCaptureStarting()=0;

	virtual void imageCaptureFinished()=0;

	virtual void detectionFinished()=0;
};

}
}

#endif
