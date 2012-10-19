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

public final class Matrix3x3{
	public Matrix3x3(){
		data[0]=Math.ONE;
		data[4]=Math.ONE;
		data[8]=Math.ONE;
	}

	public Matrix3x3(fixed x1,fixed x2,fixed x3,fixed y1,fixed y2,fixed y3,fixed z1,fixed z2,fixed z3){
		data[0]=x1;data[3]=x2;data[6]=x3;
		data[1]=y1;data[4]=y2;data[7]=y3;
		data[2]=z1;data[5]=z2;data[8]=z3;
	}

	public Matrix3x3 set(Matrix3x3 m){
		System.arraycopy(m.data,0,data,0,9);

		return this;
	}

	public Matrix3x3 set(fixed x1,fixed x2,fixed x3,fixed y1,fixed y2,fixed y3,fixed z1,fixed z2,fixed z3){
		data[0]=x1;data[3]=x2;data[6]=x3;
		data[1]=y1;data[4]=y2;data[7]=y3;
		data[2]=z1;data[5]=z2;data[8]=z3;

		return this;
	}

	public Matrix3x3 reset(){
		data[0]=Math.ONE;data[3]=0;data[6]=0;
		data[1]=0;data[4]=Math.ONE;data[7]=0;
		data[2]=0;data[5]=0;data[8]=Math.ONE;

		return this;
	}

	public fixed at(int row,int col){
		return data[row + col*3];
	}

	public void setAt(int row,int col,fixed v){
		data[row + col*3]=v;
	}

	public fixed[] getData(){
		return data;
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		Matrix3x3 m2=(Matrix3x3)object;
		return
			data[0]==m2.data[0] && data[1]==m2.data[1] && data[2]==m2.data[2] &&
			data[3]==m2.data[3] && data[4]==m2.data[4] && data[5]==m2.data[5] &&
			data[6]==m2.data[6] && data[7]==m2.data[7] && data[8]==m2.data[8];
	}

	public int hashCode(){
		return data[0] + data[1] + data[2] + data[3] + data[4] + data[5] + data[6] + data[7] + data[8];
	}

	public fixed[] data=new fixed[9];
}
