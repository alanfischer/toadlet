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

#ifndef TOADLET_FLICK_HTCMOTIONDETECTOR_H
#define TOADLET_FLICK_HTCMOTIONDETECTOR_H

#include <toadlet/flick/FilteredMotionDetector.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/DynamicLibrary.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/Thread.h>
#include <toadlet/egg/Logger.h>
#include <windows.h>

// ---- HTC Touch Diamond sensor --------------------------------------------
// ------- by Scott Seligman <scott@scottandmichelle.net> -------------------

// -- Polling the sensor using the SDK dll ------------------------
#define SENSOR_DLL      L"HTCSensorSDK"

typedef struct _TILTSENSORDATA
{
    SHORT   TiltX;          // From -1000 to 1000 (about), 0 is flat
    SHORT   TiltY;          // From -1000 to 1000 (about), 0 is flat
    SHORT   TiltZ;          // From -1000 to 1000 (about)
                            // 0 = Straight up, -1000 = Flat, 1000 = Upside down
    WORD    Unknown1;       // Always zero
    DWORD   AngleY;         // From 0 to 359
    DWORD   AngleX;         // From 0 to 359
    DWORD   Unknown2;       // Bit field?
} TILTSENSORDATA, *PTILTSENSORDATA;

typedef struct _LIGHTSENSORDATA
{
	DWORD   Reserved0;      // This value is always 3
	BYTE    Luminance;      // This value ranges between 0 and 30
} LIGHTSENSORDATA, *PLIGHTSENSORDATA;

#define SENSOR_TILT     1
#define SENSOR_LIGHT    2

typedef HANDLE (WINAPI * PFN_HTCSensorOpen)(DWORD);
typedef void (WINAPI * PFN_HTCSensorClose)(HANDLE);
typedef DWORD (WINAPI * PFN_HTCSensorGetDataOutput)(HANDLE, LPVOID);

// -- Notifications via the sensor's event ------------------------
#define SN_GSENSOR_ROOT     HKEY_LOCAL_MACHINE
#define SN_GSENSOR_PATH     _T("Software\\HTC\\HTCSensor\\GSensor")
#define SN_GSENSOR_VALUE    _T("EventChanged")
#define SN_GSENSOR_BITMASK  0xF

#define SENSOR_START        _T("HTC_GSENSOR_SERVICESTART")
#define SENSOR_STOP         _T("HTC_GSENSOR_SERVICESTOP")

#define ORIENTATION_LANDSCAPE           0
#define ORIENTATION_REVERSE_LANDSCAPE   1
#define ORIENTATION_PORTRAIT            2
#define ORIENTATION_UPSIDE_DOWN         3
#define ORIENTATION_FACE_DOWN           4
#define ORIENTATION_FACE_UP             5

#define SN_GSENSOR          1 // TODO: Modify this value to your needs
#define WM_STATECHANGE      (WM_USER + 1) // TODO: Modify this value to your needs

// -- Notifications for gestures around the d-pad -----------------
#define HTCAPI_DLL      L"HTCAPI.dll"

typedef DWORD (WINAPI * PFN_HTCNavOpen)(HWND, DWORD);
typedef DWORD (WINAPI * PFN_HTCNavSetMode)(HWND, DWORD);
typedef DWORD (WINAPI * PFN_HTCNavClose)(DWORD);

#define GESTURE_API         1
#define GESTURE_GESTURE     4
#define WM_HTC_GESTURE      (WM_USER + 200)

#define ROTATION_MASK       0x00000F00  // WPARAM
#define ROTATION_CLOCKWISE  0x00000800
#define ROTATION_COUNTER    0x00000900

#define COMPLETION_MASK     0x00F00000 // LPARAM
#define DIRECTION_MASK      0x0000F000 // LPARAM

// ---- End of HTC Touch Diamond sensor -------------------------------------
// --------------------------------------------------------------------------

typedef void REGISTRYNOTIFYCALLBACK(
  HANDLE hNotify,
  DWORD dwUserData,
  const PBYTE pData,
  const UINT cbData
);

typedef enum tagREG_COMPARISONTYPE { 
   REG_CT_ANYCHANGE,
   REG_CT_EQUAL,
   REG_CT_NOT_EQUAL,
   REG_CT_GREATER,
   REG_CT_GREATER_OR_EQUAL,
   REG_CT_LESS,
   REG_CT_LESS_OR_EQUAL,
   REG_CT_CONTAINS,
   REG_CT_STARTS_WITH,
   REG_CT_ENDS_WITH
} REG_COMPARISONTYPE;

typedef struct tagNOTIFICATIONCONDITION {
   REG_COMPARISONTYPE ctComparisonType;
   DWORD dwMask;
   union TargetValue {
      LPCTSTR psz;
      DWORD dw; 
      };
} NOTIFICATIONCONDITION;

namespace toadlet{
namespace flick{

class HTCMotionDetector:public FilteredMotionDetector,public egg::Runnable{
public:
	HTCMotionDetector();
	virtual ~HTCMotionDetector();

	bool available();

	bool create();

	bool startup();

	bool shutdown();

	bool destroy();

	void setPollSleep(int ms);

	void setFilterAlpha(egg::math::real alpha);

	void setNativeOrientation(bool native);

	void setListener(MotionDetectorListener *listener);

	State getState();

	void run();

protected:
	State mState;

	int mPollSleep;
	bool mNative;
	MotionDetectorListener *mListener;

	egg::DynamicLibrary mSensorLibrary;
	PFN_HTCSensorOpen pfnHTCSensorOpen;
	PFN_HTCSensorClose pfnHTCSensorClose;
	PFN_HTCSensorGetDataOutput pfnHTCSensorGetDataOutput;

	HANDLE mSensor;

	TILTSENSORDATA mSensorData;

	// The Outer/Inner mutex setup is done so we can have the motion detecting loop call
	// the listener, and have that callback safely null out the listener in the motion detector
	egg::Mutex mOuterMutex;
	egg::Mutex mInnerMutex;
	bool mListenerLocked;
	egg::Thread *mPollThread;
};

}
}

#endif

