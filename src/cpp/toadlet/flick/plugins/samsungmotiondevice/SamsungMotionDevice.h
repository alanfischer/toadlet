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

#ifndef TOADLET_FLICK_SAMSUNGMOTIONDEVICE_H
#define TOADLET_FLICK_SAMSUNGMOTIONDEVICE_H

#include <toadlet/flick/FilteredMotionDevice.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/DynamicLibrary.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/Logger.h>
#include <windows.h>

#define TILT_SENSOR        TEXT("ACS1:")
#define LIGHT_SENSOR       TEXT("BKL1:")

// The tilt needs to be scaled by 9.8 * 3.3793103448275862068965517241379
#define CONTROL_TILT_ON    0x44E
#define CONTROL_TILT_OFF   0x44F
#define CONTROL_TILT_READ  0x3F7

// The lumens result ranges between 0 and 15
#define CONTROL_LIGHT_READ 0x220004

namespace toadlet{
namespace flick{

class SamsungMotionDevice:public FilteredMotionDevice,public Runnable{
public:
	SamsungMotionDevice();
	virtual ~SamsungMotionDevice();

	bool available();

	bool create();

	bool startup();

	bool shutdown();

	void destroy();

	void setPollSleep(int ms);

	void setNativeOrientation(bool native);

	void setListener(MotionDeviceListener *listener);

	State getState();

	void run();

protected:
	State mState;

	int mPollSleep;
	bool mNative;
	MotionDeviceListener *mListener;

	HANDLE mSensor;

	// The Outer/Inner mutex setup is done so we can have the motion detecting loop call
	// the listener, and have that callback safely null out the listener in the motion Device
	Mutex mOuterMutex;
	Mutex mInnerMutex;
	bool mListenerLocked;
	Thread *mPollThread;
};

}
}

#endif

