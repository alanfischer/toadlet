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

package org.toadlet.egg.math;

#include <org/toadlet/Types.h>

public final class Capsule{
	public Capsule(){
		origin=new Vector3();
		direction=new Vector3();
	}

	public Capsule(Capsule capsule){
		origin=new Vector3(capsule.origin);
		direction=new Vector3(capsule.direction);
		radius=capsule.radius;
	}

	public Capsule(Vector3 origin1,Vector3 direction1,real radius1){
		origin=new Vector3(origin1);
		direction=new Vector3(direction1);
		radius=radius1;
	}

	public Capsule set(Capsule capsule){
		origin.x=capsule.origin.x;
		origin.y=capsule.origin.y;
		origin.z=capsule.origin.z;

		direction.x=capsule.direction.x;
		direction.y=capsule.direction.y;
		direction.z=capsule.direction.z;

		radius=capsule.radius;

		return this;
	}

	public Capsule set(Vector3 origin1,Vector3 direction1,real radius1){
		origin.x=origin1.x;
		origin.y=origin1.y;
		origin.z=origin1.z;

		direction.x=direction1.x;
		direction.y=direction1.y;
		direction.z=direction1.z;

		radius=radius1;

		return this;
	}

	public Capsule reset(){
		origin.x=0;
		origin.y=0;
		origin.z=0;

		direction.x=0;
		direction.y=0;
		direction.z=0;
		
		radius=0;

		return this;
	}

	public void setOrigin(Vector3 origin1){
		origin.x=origin1.x;
		origin.y=origin1.y;
		origin.z=origin1.z;
	}

	public void setOrigin(real x,real y,real z){
		origin.x=x;
		origin.y=y;
		origin.z=z;
	}

	public Vector3 getOrigin(){
		return origin;
	}

	public void setDirection(Vector3 direction1){
		direction.x=direction1.x;
		direction.y=direction1.y;
		direction.z=direction1.z;
	}

	public void setDirection(real x,real y,real z){
		direction.x=x;
		direction.y=y;
		direction.z=z;
	}

	public Vector3 getDirection(){
		return direction;
	}

	public void setRadius(real radius1){
		radius=radius1;
	}

	public real getRadius(){
		return radius;
	}

	public void setLength(real length){
		if(length>=(radius*2.0f)){
			if(direction.x==0 && direction.y==0 && direction.z==0){
				direction.z=(length-(radius*2.0f));
			}
			else{
				real s=Math.div(length-(radius*2.0f),Math.length(direction));
				Math.mul(direction,s);
			}
		}
		else{
			direction.x=0;
			direction.y=0;
			direction.z=0;
		}
	}

	public real getLength(){
		return Math.length(direction)+(radius*2.0f);
	}
	
	public void rotate(Matrix3x3 rotation){
		Math.mul(direction,rotation);
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		Capsule capsule=(Capsule)object;
		return origin.equals(capsule.origin) && direction.equals(capsule.direction) && radius==capsule.radius;
	}

	public int hashCode(){
		return origin.hashCode() + direction.hashCode() + (int)radius;
	}

	public Vector3 origin;
	public Vector3 direction;
	public real radius;
}
