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

public final class Plane{
	public Plane(){
		normal=new Vector3();
	}
	
	public Plane(Plane p){
		normal=new Vector3(p.normal);
		d=p.d;
	}

	public Plane(Vector3 normal1,real d1){
		normal=new Vector3(normal1);
		d=d1;
	}

	public Plane(real x1,real y1,real z1,real d1){
		normal=new Vector3(x1,y1,z1);
		d=d1;
	}

	public Plane(Vector3 point,Vector3 normal1){
		normal=new Vector3(normal1);
		d=Math.dot(normal,point);
	}

	public Plane(Vector3 point1,Vector3 point2,Vector3 point3){	
		normal=new Vector3();
		Vector3 v1=new Vector3(),v2=new Vector3();

		Math.sub(v1,point3,point1);
		Math.sub(v2,point2,point1);

		Math.cross(normal,v2,v1);

		d=Math.dot(normal,point1);

		Math.normalize(this);
	}

	public Plane set(Plane p){
		normal.x=p.normal.x;
		normal.y=p.normal.y;
		normal.z=p.normal.z;

		d=p.d;

		return this;
	}
	
	public Plane set(Vector3 normal1,real d1){
		normal.x=normal1.x;
		normal.y=normal1.y;
		normal.z=normal1.z;

		d=d1;

		return this;
	}
	
	public Plane set(real x1,real y1,real z1,real d1){
		normal.x=x1;
		normal.y=y1;
		normal.z=z1;

		d=d1;

		return this;
	}

	public Plane reset(){
		normal.x=0;
		normal.y=0;
		normal.z=0;

		d=0;

		return this;
	}

	public boolean isPointOutside(Vector3 point){
		return isPointOutside(point,0);
	}

	public boolean isPointOutside(Vector3 point,real epsilon){
		return Math.mul(normal.x,point.x)+Math.mul(normal.y,point.y)+Math.mul(normal.z,point.z)-d>epsilon;
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		Plane p=(Plane)object;
		return normal.equals(p.normal) && d==p.d;
	}

	public int hashCode(){
		return (int)(normal.x + normal.y + normal.z + d);
	}

	public Vector3 normal;
	public real d;
}
