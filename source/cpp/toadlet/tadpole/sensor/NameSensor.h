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

#ifndef TOADLET_TADPOLE_SENSOR_NAMESENSOR_H
#define TOADLET_TADPOLE_SENSOR_NAMESENSOR_H

#include <toadlet/tadpole/sensor/Sensor.h>

namespace toadlet{
namespace tadpole{
namespace sensor{

class TOADLET_API NameSensor:public Object,public Sensor{
public:
	TOADLET_IOBJECT(NameSensor);

	NameSensor(Scene *scene,const String &name=(char*)NULL);

	void setName(const String &name){mName=name;}
	const String &getName() const{return mName;}

protected:
	bool senseNode(Node *node);

	String mName;
};

}
}
}

#endif
