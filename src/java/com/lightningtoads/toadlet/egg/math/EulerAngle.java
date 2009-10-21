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

public final class EulerAngle{
	public real x,y,z;

	public EulerAngle(){}

	public EulerAngle(EulerAngle angle){
		x=angle.x;
		y=angle.y;
		z=angle.z;
	}

	public EulerAngle(real x1,real y1,real z1){
		x=x1;
		y=y1;
		z=z1;
	}

	public EulerAngle set(EulerAngle angle){
		x=angle.x;
		y=angle.y;
		z=angle.z;

		return this;
	}

	public EulerAngle set(real x1,real y1,real z1){
		x=x1;
		y=y1;
		z=z1;

		return this;
	}

	public EulerAngle reset(){
		x=0;
		y=0;
		z=0;

		return this;
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		EulerAngle angle=(EulerAngle)object;
		return x==angle.x && y==angle.y && z==angle.z;
	}

	public int hashCode(){
		return (int)(x + y + z);
	}
}
