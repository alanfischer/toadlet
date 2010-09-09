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

#ifndef TOADLET_TADPOLE_COLLISION_H
#define TOADLET_TADPOLE_COLLISION_H

#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{
namespace node{

class Node;

}

class Collision{
public:
	Collision():
		time(Math::ONE),
		//point,
		//normal,
		collider(NULL),
		scope(0)
	{}

	inline void set(const Collision &c){
		time=c.time;
		point.set(c.point);
		normal.set(c.normal);
		collider=c.collider;
		scope=c.scope;
	}

	inline void reset(){
		time=Math::ONE;
		point.set(Math::ZERO_VECTOR3);
		normal.set(Math::ZERO_VECTOR3);
		collider=NULL;
		scope=0;
	}

	scalar time;
	Vector3 point; // This is the point at which the tracing node would stop
	Vector3 normal;
	node::Node *collider; // The node that blocked the trace
	int scope; // The OR'd scope of the collision point
};

}
}

#endif
