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

#ifndef TOADLET_TADPOLE_NAMESENSOR_H
#define TOADLET_TADPOLE_NAMESENSOR_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/SensorResultsListener.h>

namespace toadlet{
namespace tadpole{

class Scene;

/// @todo: perhaps these sensors should be nodes, so they can easily be attached to the scene and then their transforms will be relative, just like opal
class TOADLET_API NameSensor{
public:
	TOADLET_SHARED_POINTERS(NameSensor);

	NameSensor(Scene *scene);
	virtual ~NameSensor();

	virtual bool senseNames(SensorResultsListener *results,const egg::String &name);

protected:
	int senseNames(node::Node *node,SensorResultsListener *results,const egg::String &name);

	Scene *mScene;
};

}
}

#endif
