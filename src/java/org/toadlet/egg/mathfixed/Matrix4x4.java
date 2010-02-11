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

public final class Matrix4x4{
	public Matrix4x4(){
		data[0]=Math.ONE;
		data[5]=Math.ONE;
		data[10]=Math.ONE;
		data[15]=Math.ONE;
	}

	public Matrix4x4(fixed x1,fixed x2,fixed x3,fixed x4,fixed y1,fixed y2,fixed y3,fixed y4,fixed z1,fixed z2,fixed z3,fixed z4,fixed w1,fixed w2,fixed w3,fixed w4){
		data[0]=x1;data[4]=x2;data[8]=x3;data[12]=x4;
		data[1]=y1;data[5]=y2;data[9]=y3;data[13]=y4;
		data[2]=z1;data[6]=z2;data[10]=z3;data[14]=z4;
		data[3]=w1;data[7]=w2;data[11]=w3;data[15]=w4;
	}

	public Matrix4x4 set(Matrix4x4 m){
		System.arraycopy(m.data,0,data,0,16);

		return this;
	}

	public Matrix4x4 set(fixed x1,fixed x2,fixed x3,fixed x4,fixed y1,fixed y2,fixed y3,fixed y4,fixed z1,fixed z2,fixed z3,fixed z4,fixed w1,fixed w2,fixed w3,fixed w4){
		data[0]=x1;data[4]=x2;data[8]=x3;data[12]=x4;
		data[1]=y1;data[5]=y2;data[9]=y3;data[13]=y4;
		data[2]=z1;data[6]=z2;data[10]=z3;data[14]=z4;
		data[3]=w1;data[7]=w2;data[11]=w3;data[15]=w4;

		return this;
	}

	public Matrix4x4 reset(){
		data[0]=Math.ONE;data[4]=0;data[8]=0;data[12]=0;
		data[1]=0;data[5]=Math.ONE;data[9]=0;data[13]=0;
		data[2]=0;data[6]=0;data[10]=Math.ONE;data[14]=0;
		data[3]=0;data[7]=0;data[11]=0;data[15]=Math.ONE;

		return this;
	}

	public fixed at(int row,int col){
		return data[row + col*4];
	}

	public void setAt(int row,int col,fixed v){
		data[row + col*4]=v;
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
		Matrix4x4 m2=(Matrix4x4)object;
		return
			data[0]==m2.data[0] && data[1]==m2.data[1] && data[2]==m2.data[2] && data[3]==m2.data[3] &&
			data[4]==m2.data[4] && data[5]==m2.data[5] && data[6]==m2.data[6] && data[7]==m2.data[7] &&
			data[8]==m2.data[8] && data[9]==m2.data[9] && data[10]==m2.data[10] && data[11]==m2.data[11] &&
			data[12]==m2.data[12] && data[13]==m2.data[13] && data[14]==m2.data[14] && data[15]==m2.data[15];
	}

	public int hashCode(){
		return data[0] + data[1] + data[2] + data[3] + data[4] + data[5] + data[6] + data[7] + data[8] + data[9] + data[10] + data[11] + data[12] + data[13] + data[14] + data[15];
	}

	public fixed[] data=new fixed[16];
}
