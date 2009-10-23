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

/***********  c# Conversion from Java ************
 * 
 * package changed to namespace
 * Change class type from final to sealed
 * 
 * ***********************************************/

// need to use something other than "fixed" since it's a c# keyword
// unfortunately, this is only vision to this file so needs to be defined
// for each file using it
using tfixed = System.Int32;

namespace com.lightningtoads.toadlet.egg.mathfixed
{
public sealed class Matrix3x3{
	public Matrix3x3(){
		data[0]=Math.ONE;
		data[4]=Math.ONE;
		data[8]=Math.ONE;
	}

	public Matrix3x3(int x1,int x2,int x3,int y1,int y2,int y3,int z1,int z2,int z3){
		data[0]=x1;data[3]=x2;data[6]=x3;
		data[1]=y1;data[4]=y2;data[7]=y3;
		data[2]=z1;data[5]=z2;data[8]=z3;
	}

	public void set(Matrix3x3 m){
		System.Array.Copy(m.data,0,data,0,9);
	}

	public void set(int x1,int x2,int x3,int y1,int y2,int y3,int z1,int z2,int z3){
		data[0]=x1;data[3]=x2;data[6]=x3;
		data[1]=y1;data[4]=y2;data[7]=y3;
		data[2]=z1;data[5]=z2;data[8]=z3;
	}

	public void reset(){
		data[0]=Math.ONE;data[3]=0;data[6]=0;
		data[1]=0;data[4]=Math.ONE;data[7]=0;
		data[2]=0;data[5]=0;data[8]=Math.ONE;
	}

	public bool equals(Matrix3x3 m2){
		return
			data[0]==m2.data[0] && data[1]==m2.data[1] && data[2]==m2.data[2] && 
			data[3]==m2.data[3] && data[4]==m2.data[4] && data[5]==m2.data[5] && 
			data[6]==m2.data[6] && data[7]==m2.data[7] && data[8]==m2.data[8];
	}

	public int at(int row,int col){
		return data[row + col*3];
	}

	public void setAt(int row,int col,int v){
		data[row + col*3]=v;
	}

	public int[] getData(){
		return data;
	}
	
	public int[] data=new int[9];
}
}