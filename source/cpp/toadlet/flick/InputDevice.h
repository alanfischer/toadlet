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

#ifndef TOADLET_FLICK_INPUTDEVICE_H
#define TOADLET_FLICK_INPUTDEVICE_H

#include <toadlet/egg/Interface.h>
#include <toadlet/flick/Types.h>

namespace toadlet{
namespace flick{

class InputDeviceListener;

class InputDevice:public Interface{
public:
	TOADLET_INTERFACE(InputDevice);

	enum InputType{
		InputType_JOY=0,
		InputType_LINEAR,
		InputType_ANGULAR,
		InputType_LIGHT,
		InputType_PROXIMITY,
		InputType_MAGNETIC,
		InputType_MAX,
	};

	virtual ~InputDevice(){}

	virtual bool create()=0;
	virtual void destroy()=0;

	virtual InputType getType()=0;
	virtual bool start()=0;
	virtual void update(int dt)=0;
	virtual void stop()=0;

	virtual void setListener(InputDeviceListener *listener)=0;
	virtual void setSampleTime(int dt)=0;
	virtual void setAlpha(scalar alpha)=0;
};

}
}

#endif
