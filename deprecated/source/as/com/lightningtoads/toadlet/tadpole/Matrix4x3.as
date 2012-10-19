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

package com.lightningtoads.toadlet.tadpole{

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.tMath;

public final class Matrix4x3{
	public static function mulMMM(r:Matrix4x3,m1:Matrix4x3,m2:Matrix4x3):void{
		r.a00=(((m1.a00>>2)*(m2.a00>>2))>>12) + (((m1.a01>>2)*(m2.a10>>2))>>12) + (((m1.a02>>2)*(m2.a20>>2))>>12);
		r.a01=(((m1.a00>>2)*(m2.a01>>2))>>12) + (((m1.a01>>2)*(m2.a11>>2))>>12) + (((m1.a02>>2)*(m2.a21>>2))>>12);
		r.a02=(((m1.a00>>2)*(m2.a02>>2))>>12) + (((m1.a01>>2)*(m2.a12>>2))>>12) + (((m1.a02>>2)*(m2.a22>>2))>>12);
		r.a03=(((m1.a00>>2)*(m2.a03>>6))>>8) + (((m1.a01>>2)*(m2.a13>>6))>>8) + (((m1.a02>>2)*(m2.a23>>6))>>8) + m1.a03;

		r.a10=(((m1.a10>>2)*(m2.a00>>2))>>12) + (((m1.a11>>2)*(m2.a10>>2))>>12) + (((m1.a12>>2)*(m2.a20>>2))>>12);
		r.a11=(((m1.a10>>2)*(m2.a01>>2))>>12) + (((m1.a11>>2)*(m2.a11>>2))>>12) + (((m1.a12>>2)*(m2.a21>>2))>>12);
		r.a12=(((m1.a10>>2)*(m2.a02>>2))>>12) + (((m1.a11>>2)*(m2.a12>>2))>>12) + (((m1.a12>>2)*(m2.a22>>2))>>12);
		r.a13=(((m1.a10>>2)*(m2.a03>>6))>>8) + (((m1.a11>>2)*(m2.a13>>6))>>8) + (((m1.a12>>2)*(m2.a23>>6))>>8) + m1.a13;

		r.a20=(((m1.a20>>2)*(m2.a00>>2))>>12) + (((m1.a21>>2)*(m2.a10>>2))>>12) + (((m1.a22>>2)*(m2.a20>>2))>>12);
		r.a21=(((m1.a20>>2)*(m2.a01>>2))>>12) + (((m1.a21>>2)*(m2.a11>>2))>>12) + (((m1.a22>>2)*(m2.a21>>2))>>12);
		r.a22=(((m1.a20>>2)*(m2.a02>>2))>>12) + (((m1.a21>>2)*(m2.a12>>2))>>12) + (((m1.a22>>2)*(m2.a22>>2))>>12);
		r.a23=(((m1.a20>>2)*(m2.a03>>6))>>8) + (((m1.a21>>2)*(m2.a13>>6))>>8) + (((m1.a22>>2)*(m2.a23>>6))>>8) + m1.a23;
	}

	public function Matrix4x3(){
		a00=tMath.ONE;a01=0;a02=0;a03=0;
		a10=0;a11=tMath.ONE;a12=0;a13=0;
		a20=0;a21=0;a22=tMath.ONE;a23=0;
	}

	public function Matrix4x3I12(x1:int,x2:int,x3:int,x4:int,y1:int,y2:int,y3:int,y4:int,z1:int,z2:int,z3:int,z4:int):Matrix4x3{
		var m:Matrix4x3=new Matrix4x3();
		m.a00=x1;m.a01=x2;m.a02=x3;m.a03=x4;
		m.a10=y1;m.a11=y2;m.a12=y3;m.a13=y4;
		m.a20=z1;m.a21=z2;m.a22=z3;m.a23=z4;
		return m;
	}

	public function setM(m:Matrix4x3):void{
		a00=m.a00;a01=m.a01;a02=m.a02;a03=m.a03;
		a10=m.a10;a11=m.a11;a12=m.a12;a13=m.a13;
		a20=m.a20;a21=m.a21;a22=m.a22;a23=m.a23;
	}

	public function setI12(x1:int,x2:int,x3:int,x4:int,y1:int,y2:int,y3:int,y4:int,z1:int,z2:int,z3:int,z4:int):void{
		a00=x1;a01=x2;a02=x3;a03=x4;
		a10=y1;a11=y2;a12=y3;a13=y4;
		a20=z1;a21=z2;a22=z3;a23=z4;
	}

	public function reset():void{
		a00=tMath.ONE;a01=0;a02=0;a03=0;
		a10=0;a11=tMath.ONE;a12=0;a13=0;
		a20=0;a21=0;a22=tMath.ONE;a23=0;
	}

	public function equals(m2:Matrix4x3):Boolean{
		return(
			a00==m2.a00 && a01==m2.a01 && a02==m2.a02 && a03==m2.a03 &&
			a10==m2.a10 && a11==m2.a11 && a12==m2.a12 && a13==m2.a13 &&
			a20==m2.a20 && a21==m2.a21 && a22==m2.a22 && a23==m2.a23);
	}

	public var a00:int,a01:int,a02:int,a03:int;
	public var a10:int,a11:int,a12:int,a13:int;
	public var a20:int,a21:int,a22:int,a23:int;
}

}
