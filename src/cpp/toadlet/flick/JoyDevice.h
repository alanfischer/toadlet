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

#ifndef TOADLET_FLICK_JOYDEVICE_H
#define TOADLET_FLICK_JOYDEVICE_H

#include <toadlet/flick/Types.h>

namespace toadlet{
namespace flick{

class JoyDeviceListener;

class JoyDevice{
public:
	virtual ~JoyDevice(){}

	virtual bool create()=0;
	virtual void destroy()=0;

	virtual bool start()=0;
	virtual void update(int dt)=0;
	virtual bool stop()=0;
	virtual bool isRunning()=0;

	virtual void setListener(JoyDeviceListener *listener)=0;
};

}
}

#endif
