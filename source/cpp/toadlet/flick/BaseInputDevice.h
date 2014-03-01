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

#ifndef TOADLET_FLICK_BASEINPUTDEVICE_H
#define TOADLET_FLICK_BASEINPUTDEVICE_H

#include <toadlet/flick/InputDevice.h>

namespace toadlet{
namespace flick{

class BaseInputDevice:public Object,public InputDevice{
public:
	TOADLET_IOBJECT(BaseInputDevice);

	virtual ~BaseInputDevice(){}

	bool create(){return true;}
	void destroy(){}

	InputType getType(){return (InputType)0;}
	bool start(){return true;}
	void update(int dt){}
	void stop(){}

	void setListener(InputDeviceListener *listener){}
	void setSampleTime(int dt){}
	void setAlpha(float alpha){}
};

}
}

#endif
