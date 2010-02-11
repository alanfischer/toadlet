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

package org.toadlet.tadpole.plugins.hop;

#include <org/toadlet/tadpole/Types.h>

import org.toadlet.tadpole.*;

public class HopCollision{
	public HopCollision(){}

	public void set(HopCollision c){
		time=c.time;
		point.set(c.point);
		normal.set(c.normal);
		velocity.set(c.velocity);
		collider=c.collider;
		collidee=c.collidee;
	}

	public void reset(){
		time=-Math.ONE;
		point.set(Math.ZERO_VECTOR3);
		normal.set(Math.ZERO_VECTOR3);
		velocity.set(Math.ZERO_VECTOR3);
		collider=null;
		collidee=null;
	}

	public scalar time=-Math.ONE;
	public Vector3 point=new Vector3(); // This is the point at which the tracing solid would stop
	public Vector3 normal=new Vector3();
	public Vector3 velocity=new Vector3(); // The velocity difference between the two solids
	public HopEntity collider=null; // The solid that blocked the trace
	public HopEntity collidee=null; // The solid being traced
}
