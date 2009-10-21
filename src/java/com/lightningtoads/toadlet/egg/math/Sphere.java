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

package com.lightningtoads.toadlet.egg.math;

#include <com/lightningtoads/toadlet/Types.h>

public final class Sphere{
	public Sphere(){
		origin=new Vector3();
	}

	public Sphere(Sphere sphere){
		origin=new Vector3(sphere.origin);
		radius=sphere.radius;
	}

	public Sphere(real radius1){
		origin=new Vector3();
		radius=radius1;
	}

	public Sphere(Vector3 origin1,real radius1){
		origin=new Vector3(origin1);
		radius=radius1;
	}

	public Sphere set(Sphere sphere){
		origin.x=sphere.origin.x;
		origin.y=sphere.origin.y;
		origin.z=sphere.origin.z;

		radius=sphere.radius;

		return this;
	}

	public Sphere set(Vector3 origin1,real radius1){
		origin.x=origin1.x;
		origin.y=origin1.y;
		origin.z=origin1.z;

		radius=radius1;

		return this;
	}

	public Sphere reset(){
		origin.x=0;
		origin.y=0;
		origin.z=0;
		
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

	public void setRadius(real radius1){
		radius=radius1;
	}

	public real getRadius(){
		return radius;
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		Sphere s=(Sphere)object;
		return origin.equals(s.origin) && radius==s.radius;
	}

	public int hashCode(){
		return (int)(origin.x + origin.y + origin.z + radius);
	}

	public Vector3 origin;
	public real radius;
}
