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

#ifndef TOADLET_HOP_COLLISION_H
#define TOADLET_HOP_COLLISION_H

#include <toadlet/hop/Types.h>

namespace toadlet{
namespace hop{

class Solid;

class Collision{
public:
	Collision():
		time(Math::ONE),
		//point,
		//normal,
		//velocity,
		collider(NULL),
		collidee(NULL),
		scope(0){}

	inline Collision &set(const Collision &c){
		time=c.time;
		point.set(c.point);
		normal.set(c.normal);
		velocity.set(c.velocity);
		collider=c.collider;
		collidee=c.collidee;
		scope=c.scope;

		return *this;
	}

	inline Collision &reset(){
		time=Math::ONE;
		point.reset();
		normal.reset();
		velocity.reset();
		collider=NULL;
		collidee=NULL;
		scope=0;

		return *this;
	}

	inline void invert(){
		Solid *s=collider;
		collider=collidee;
		collidee=s;

		Math::neg(normal);
		Math::neg(velocity);
	}

	scalar time;
	Vector3 point; // This is the point at which the tracing solid would stop
	Vector3 normal;
	Vector3 velocity; // The velocity difference between the two solids
	IntrusivePointer<Solid> collider; // The solid that blocked the trace
	IntrusivePointer<Solid> collidee; // The solid being traced
	int scope; // The OR'd scope of the collision point
};

}
}

#endif

