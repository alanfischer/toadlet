/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright (C) 2006, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 *
 * All source code for the Toadlet Engine, including
 * this file, is the sole property of Lightning Toads
 * Productions, LLC. It has been developed on our own
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed
 * without our explicit permission.
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

package com.lightningtoads.toadlet.tadpole;

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;

public final class Matrix4x3{
	public static void mul(Matrix4x3 r,Matrix4x3 m1,Matrix4x3 m2){
		r.setAt(0,0,(((((int)((((long)m1.at(0,0))*((long)m2.at(0,0)))>>16)))+(((int)((((long)m1.at(0,1))*((long)m2.at(1,0)))>>16))))+(((int)((((long)m1.at(0,2))*((long)m2.at(2,0)))>>16)))));
		r.setAt(0,1,(((((int)((((long)m1.at(0,0))*((long)m2.at(0,1)))>>16)))+(((int)((((long)m1.at(0,1))*((long)m2.at(1,1)))>>16))))+(((int)((((long)m1.at(0,2))*((long)m2.at(2,1)))>>16)))));
		r.setAt(0,2,(((((int)((((long)m1.at(0,0))*((long)m2.at(0,2)))>>16)))+(((int)((((long)m1.at(0,1))*((long)m2.at(1,2)))>>16))))+(((int)((((long)m1.at(0,2))*((long)m2.at(2,2)))>>16)))));
		r.setAt(0,3,((((((int)((((long)m1.at(0,0))*((long)m2.at(0,3)))>>16)))+(((int)((((long)m1.at(0,1))*((long)m2.at(1,3)))>>16))))+(((int)((((long)m1.at(0,2))*((long)m2.at(2,3)))>>16))))+m1.at(0,3)));

		r.setAt(1,0,(((((int)((((long)m1.at(1,0))*((long)m2.at(0,0)))>>16)))+(((int)((((long)m1.at(1,1))*((long)m2.at(1,0)))>>16))))+(((int)((((long)m1.at(1,2))*((long)m2.at(2,0)))>>16)))));
		r.setAt(1,1,(((((int)((((long)m1.at(1,0))*((long)m2.at(0,1)))>>16)))+(((int)((((long)m1.at(1,1))*((long)m2.at(1,1)))>>16))))+(((int)((((long)m1.at(1,2))*((long)m2.at(2,1)))>>16)))));
		r.setAt(1,2,(((((int)((((long)m1.at(1,0))*((long)m2.at(0,2)))>>16)))+(((int)((((long)m1.at(1,1))*((long)m2.at(1,2)))>>16))))+(((int)((((long)m1.at(1,2))*((long)m2.at(2,2)))>>16)))));
		r.setAt(1,3,((((((int)((((long)m1.at(1,0))*((long)m2.at(0,3)))>>16)))+(((int)((((long)m1.at(1,1))*((long)m2.at(1,3)))>>16))))+(((int)((((long)m1.at(1,2))*((long)m2.at(2,3)))>>16))))+m1.at(1,3)));

		r.setAt(2,0,(((((int)((((long)m1.at(2,0))*((long)m2.at(0,0)))>>16)))+(((int)((((long)m1.at(2,1))*((long)m2.at(1,0)))>>16))))+(((int)((((long)m1.at(2,2))*((long)m2.at(2,0)))>>16)))));
		r.setAt(2,1,(((((int)((((long)m1.at(2,0))*((long)m2.at(0,1)))>>16)))+(((int)((((long)m1.at(2,1))*((long)m2.at(1,1)))>>16))))+(((int)((((long)m1.at(2,2))*((long)m2.at(2,1)))>>16)))));
		r.setAt(2,2,(((((int)((((long)m1.at(2,0))*((long)m2.at(0,2)))>>16)))+(((int)((((long)m1.at(2,1))*((long)m2.at(1,2)))>>16))))+(((int)((((long)m1.at(2,2))*((long)m2.at(2,2)))>>16)))));
		r.setAt(2,3,((((((int)((((long)m1.at(2,0))*((long)m2.at(0,3)))>>16)))+(((int)((((long)m1.at(2,1))*((long)m2.at(1,3)))>>16))))+(((int)((((long)m1.at(2,2))*((long)m2.at(2,3)))>>16))))+m1.at(2,3)));
	}


	public Matrix4x3(){
		data[0]=Math.ONE;
		data[4]=Math.ONE;
		data[8]=Math.ONE;
	}

	public Matrix4x3(int x1,int x2,int x3,int x4,int y1,int y2,int y3,int y4,int z1,int z2,int z3,int z4){
		data[0]=x1;data[3]=x2;data[6]=x3;data[9]=x4;
		data[1]=y1;data[4]=y2;data[7]=y3;data[10]=y4;
		data[2]=z1;data[5]=z2;data[8]=z3;data[11]=z4;
	}

	public void set(Matrix4x3 m){
		System.arraycopy(m.data,0,data,0,12);
	}

	public void set(int x1,int x2,int x3,int x4,int y1,int y2,int y3,int y4,int z1,int z2,int z3,int z4){
		data[0]=x1;data[3]=x2;data[6]=x3;data[9]=x4;
		data[1]=y1;data[4]=y2;data[7]=y3;data[10]=y4;
		data[2]=z1;data[5]=z2;data[8]=z3;data[11]=z4;
	}

	public void reset(){
		data[0]=Math.ONE;data[3]=0;data[6]=0;data[9]=0;
		data[1]=0;data[4]=Math.ONE;data[7]=0;data[10]=0;
		data[2]=0;data[5]=0;data[8]=Math.ONE;data[11]=0;
	}

	public boolean equals(Matrix4x3 m2){
		return
			data[0]==m2.data[0] && data[1]==m2.data[1] && data[2]==m2.data[2] && 
			data[3]==m2.data[3] && data[4]==m2.data[4] && data[5]==m2.data[5] &&
			data[6]==m2.data[6] && data[7]==m2.data[7] && data[8]==m2.data[8] &&
			data[9]==m2.data[9] && data[10]==m2.data[10] && data[11]==m2.data[11];
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
	
	public int[] data=new int[12];
}
