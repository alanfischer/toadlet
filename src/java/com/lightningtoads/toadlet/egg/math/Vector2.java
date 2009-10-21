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

public final class Vector2{
	public real x,y;

	public Vector2(){}
	
	public Vector2(Vector2 v){
		x=v.x;
		y=v.y;
	}
	
	public Vector2(real x1,real y1){
		x=x1;
		y=y1;
	}
	
	public Vector2 set(Vector2 v){
		x=v.x;
		y=v.y;

		return this;
	}

	public Vector2 set(real x1,real y1){
		x=x1;
		y=y1;

		return this;
	}

	public Vector2 reset(){
		x=0;
		y=0;

		return this;
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		Vector2 v=(Vector2)object;
		return x==v.x && y==v.y;
	}

	public int hashCode(){
		return (int)(x + y);
	}
}
