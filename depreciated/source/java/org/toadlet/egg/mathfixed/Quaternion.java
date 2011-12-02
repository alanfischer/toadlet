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

package org.toadlet.egg.mathfixed;

#include <org/toadlet/Types.h>

public final class Quaternion{
	public Quaternion(){}
	
	public Quaternion(Quaternion q){
		x=q.x;
		y=q.y;
		z=q.z;
		w=q.w;
	}
	
	public Quaternion(fixed x1,fixed y1,fixed z1,fixed w1){
		x=x1;
		y=y1;
		z=z1;
		w=w1;
	}
	
	public Quaternion set(Quaternion q){
		x=q.x;
		y=q.y;
		z=q.z;
		w=q.w;

		return this;
	}

	public Quaternion set(fixed x1,fixed y1,fixed z1,fixed w1){
		x=x1;
		y=y1;
		z=z1;
		w=w1;

		return this;
	}

	public Quaternion reset(){
		x=0;
		y=0;
		z=0;
		w=0;

		return this;
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		Quaternion q=(Quaternion)object;
		return x==q.x && y==q.y && z==q.z && w==q.w;
	}

	public int hashCode(){
		return x + y + z + w;
	}

	public fixed x,y,z,w;
}
