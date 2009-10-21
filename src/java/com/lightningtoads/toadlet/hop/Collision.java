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

package com.lightningtoads.toadlet.hop;

#include <com/lightningtoads/toadlet/hop/Types.h>

public final class Collision{
	public Collision(){}

	public Collision set(Collision c){
		time=c.time;
		point.set(c.point);
		normal.set(c.normal);
		velocity.set(c.velocity);
		collider=c.collider;
		collidee=c.collidee;

		return this;
	}

	public Collision reset(){
		time=-Math.ONE;
		point.reset();
		normal.reset();
		velocity.reset();
		collider=null;
		collidee=null;

		return this;
	}

	public scalar time=-Math.ONE;
	public Vector3 point=new Vector3(); // This is the point at which the tracing solid would stop
	public Vector3 normal=new Vector3();
	public Vector3 velocity=new Vector3(); // The velocity difference between the two solids
	public Solid collider=null;
	public Solid collidee=null;
}
