/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_FLICK_IPHONEMOTIONDETECTOR_H
#define TOADLET_FLICK_IPHONEMOTIONDETECTOR_H

#include <toadlet/flick/FilteredMotionDetector.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Logger.h>
#import <UIKit/UIKit.h>

namespace toadlet{
namespace flick{

class IPhoneMotionDetector;

}
}

@interface ToadletAccelerometerDelegate:NSObject<UIAccelerometerDelegate>;

- (id) initWithMotionDetector:(toadlet::flick::IPhoneMotionDetector*)motionDetector;
- (void) accelerometer:(UIAccelerometer*)accelerometer didAccelerate:(UIAcceleration*)acceleration;
toadlet::flick::IPhoneMotionDetector *mMotionDetector;

@end

namespace toadlet{
namespace flick{

class IPhoneMotionDetector:public FilteredMotionDetector{
public:
	IPhoneMotionDetector();
	virtual ~IPhoneMotionDetector();

	bool available();

	bool create();

	bool startup();

	bool shutdown();

	bool destroy();

	void setPollSleep(int ms);

	void setNativeOrientation(bool native);

	void setListener(MotionDetectorListener *listener);
	
	State getState();

	void didAccelerate(UIAcceleration *acceleration);

protected:
	State mState;
	MotionDetectorListener *mListener;
	bool mNative;
	ToadletAccelerometerDelegate *mAccelerometerDelegate;
};

}
}

#endif

