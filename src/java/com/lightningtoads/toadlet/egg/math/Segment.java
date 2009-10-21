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

import com.lightningtoads.toadlet.egg.math.Math;

public final class Segment{
	public Segment(){
		origin=new Vector3();
		direction=new Vector3();
	}

	public Segment(Segment segment){
		origin=new Vector3(segment.origin);
		direction=new Vector3(segment.direction);
	}

	public Segment set(Segment segment){
		origin.x=segment.origin.x;
		origin.y=segment.origin.y;
		origin.z=segment.origin.z;

		direction.x=segment.direction.x;
		direction.y=segment.direction.y;
		direction.z=segment.direction.z;

		return this;
	}

	public Segment setStartEnd(Vector3 start,Vector3 end){
		origin.x=start.x;
		origin.y=start.y;
		origin.z=start.z;

		direction.x=end.x-start.x;
		direction.y=end.y-start.y;
		direction.z=end.z-start.z;

		return this;
	}

	public Segment setStartEnd(float startX,float startY,float startZ,float endX,float endY,float endZ){
		origin.x=startX;
		origin.y=startY;
		origin.z=startZ;

		direction.x=endX-startX;
		direction.y=endY-startY;
		direction.z=endZ-startZ;

		return this;
	}

	public Segment setStartDir(Vector3 start,Vector3 dir){
		origin.x=start.x;
		origin.y=start.y;
		origin.z=start.z;

		direction.x=dir.x;
		direction.y=dir.y;
		direction.z=dir.z;

		return this;
	}

	public Segment setStartDir(float startX,float startY,float startZ,float dirX,float dirY,float dirZ){
		origin.x=startX;
		origin.y=startY;
		origin.z=startZ;

		direction.x=dirX;
		direction.y=dirY;
		direction.z=dirZ;

		return this;
	}

	public Segment reset(){
		origin.x=0;
		origin.y=0;
		origin.z=0;

		direction.x=0;
		direction.y=0;
		direction.z=0;

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

	public void getEndPoint(Vector3 endPoint){
		endPoint.x=origin.x+direction.x;
		endPoint.y=origin.y+direction.y;
		endPoint.z=origin.z+direction.z;
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		Segment s=(Segment)object;
		return origin.equals(s.origin) && direction.equals(s.direction);
	}

	public int hashCode(){
		return (int)(origin.x + origin.y + origin.z + direction.x + direction.y + direction.z);
	}

	public Vector3 origin;
	public Vector3 direction;
}

