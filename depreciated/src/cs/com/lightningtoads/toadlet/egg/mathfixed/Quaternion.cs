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
public sealed class Quaternion{
	public Quaternion(){}
	
	public Quaternion(Quaternion q){
		x=q.x;
		y=q.y;
		z=q.z;
		w=q.w;
	}
	
	public Quaternion(int x1,int y1,int z1,int w1){
		x=x1;
		y=y1;
		z=z1;
		w=w1;
	}
	
	public void set(Quaternion q){
		x=q.x;
		y=q.y;
		z=q.z;
		w=q.w;
	}

	public void set(int x1,int y1,int z1,int w1){
		x=x1;
		y=y1;
		z=z1;
		w=w1;
	}

	public void reset(){
		x=0;
		y=0;
		z=0;
		w=0;
	}

	public bool equals(Quaternion q){
		return x==q.x && y==q.y && z==q.z && w==q.w;
	}

	public int x,y,z,w;
}
}