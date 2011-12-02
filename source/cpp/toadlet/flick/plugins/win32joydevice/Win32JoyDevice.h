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

#ifndef TOADLET_FLICK_WIN32JOYDEVICE_H
#define TOADLET_FLICK_WIN32JOYDEVICE_H

#include <toadlet/flick/JoyDevice.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Logger.h>
#include <windows.h>

namespace toadlet{
namespace flick{

class Win32JoyDevice:public JoyDevice{
public:
	Win32JoyDevice();
	virtual ~Win32JoyDevice();

	bool create();
	void destroy();

	bool start();
	void update(int dt);
	bool stop();
	bool isRunning(){return mRunning;}

	void setListener(JoyDeviceListener *listener);

	scalar joyToScalar(int joy);

protected:
	bool mRunning;
	JoyDeviceListener *mListener;
	int mJoyID;
	JOYINFOEX mJoyInfo,mLastJoyInfo;
};

}
}

#endif

