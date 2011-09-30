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

#include <toadlet/egg/Error.h>
#include <toadlet/flick/JoyDeviceListener.h>
#include "Win32JoyDevice.h"
#pragma comment(lib,"winmm.lib")

namespace toadlet{
namespace flick{

TOADLET_C_API JoyDevice *new_Win32JoyDevice(){
	return new Win32JoyDevice();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API JoyDevice *new_JoyDevice(){
		return new Win32JoyDevice();
	}
#endif

Win32JoyDevice::Win32JoyDevice():
	mRunning(false),
	mListener(NULL),
	mJoyID(0)
{
	memset(&mJoyInfo,0,sizeof(JOYINFOEX));
	memset(&mLastJoyInfo,0,sizeof(JOYINFOEX));
}

Win32JoyDevice::~Win32JoyDevice(){
	destroy();
}

bool Win32JoyDevice::create(){
	mJoyInfo.dwSize=sizeof(JOYINFOEX);
	mJoyInfo.dwFlags=JOY_RETURNALL;
	memcpy(&mLastJoyInfo,&mJoyInfo,sizeof(JOYINFOEX));
	int numJoys=0;
	int joyID=JOYSTICKID1;
	HRESULT result=S_OK;
	while((result=joyGetPosEx(joyID++,&mJoyInfo))!=JOYERR_PARMS){
		if(result==JOYERR_NOERROR){numJoys++;}
	}
	if(numJoys>0){
		mJoyID=JOYSTICKID1;
	}
	else{
		mJoyID=-1;
	}
	Logger::alert(Categories::TOADLET_FLICK,
		String("detected ")+numJoys+" joysticks");

	return true;
}

void Win32JoyDevice::destroy(){
	if(mRunning){
		stop();
	}
}

bool Win32JoyDevice::start(){
	mRunning=true;

	return mRunning;
}

bool Win32JoyDevice::stop(){
	mRunning=false;

	return mRunning;
}

void Win32JoyDevice::update(int dt){
	if(mJoyID>=JOYSTICKID1){
		JOYINFOEX *joyInfo=&mJoyInfo,*lastJoyInfo=&mLastJoyInfo;
		MMRESULT result=joyGetPosEx(mJoyID,joyInfo);
		if(	joyInfo->dwXpos!=lastJoyInfo->dwXpos || joyInfo->dwYpos!=lastJoyInfo->dwYpos || joyInfo->dwZpos!=lastJoyInfo->dwZpos ||
			joyInfo->dwRpos!=lastJoyInfo->dwRpos || joyInfo->dwUpos!=lastJoyInfo->dwUpos || joyInfo->dwVpos!=lastJoyInfo->dwVpos){
			if(mListener!=NULL){
				mListener->joyMoved(joyToScalar(joyInfo->dwXpos),joyToScalar(joyInfo->dwYpos),joyToScalar(joyInfo->dwZpos),joyToScalar(joyInfo->dwRpos),joyToScalar(joyInfo->dwUpos),joyToScalar(joyInfo->dwVpos));
			}
		}
		if(joyInfo->dwButtons!=lastJoyInfo->dwButtons){
			int pressedButtons=(joyInfo->dwButtons^lastJoyInfo->dwButtons)&joyInfo->dwButtons;
			int releasedButtons=(joyInfo->dwButtons^lastJoyInfo->dwButtons)&lastJoyInfo->dwButtons;
			int button=0;
			while(pressedButtons>0 || releasedButtons>0){
				if((pressedButtons&1)>0){
					if(mListener!=NULL){
						mListener->joyPressed(button);
					}
				}
				if((releasedButtons&1)>0){
					if(mListener!=NULL){
						mListener->joyReleased(button);
					}
				}
				pressedButtons>>=1;
				releasedButtons>>=1;
				button++;
			}
		}
		memcpy(lastJoyInfo,joyInfo,sizeof(JOYINFOEX));
	}
}

void Win32JoyDevice::setListener(JoyDeviceListener *listener){
	mListener=listener;
}

scalar Win32JoyDevice::joyToScalar(int joy){
	return Math::fromFloat(((float)joy/(float)65536)*2-1);
}

}
}
