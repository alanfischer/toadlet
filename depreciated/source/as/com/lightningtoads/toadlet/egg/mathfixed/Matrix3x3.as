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

package com.lightningtoads.toadlet.egg.mathfixed{

public final class Matrix3x3{
	// We cannot use Math.ONE, since Matrix3x3 is needed in the Math constructor.
	private static var ONE:int=65536;

	public function Matrix3x3(){
		a00=ONE;a01=0;a02=0;
		a10=0;a11=ONE;a12=0;
		a20=0;a21=0;a22=ONE;
	}

	public static function Matrix3x3S9(x1:int,x2:int,x3:int,y1:int,y2:int,y3:int,z1:int,z2:int,z3:int):Matrix3x3{
		var m:Matrix3x3=new Matrix3x3();
		m.a00=x1;m.a01=x2;m.a02=x3;
		m.a10=y1;m.a11=y2;m.a12=y3;
		m.a20=z1;m.a21=z2;m.a22=z3;
		return m;
	}

	public function setM(m:Matrix3x3):void{
		a00=m.a00;a01=m.a01;a02=m.a02;
		a10=m.a10;a11=m.a11;a12=m.a12;
		a20=m.a20;a21=m.a21;a22=m.a22;
	}

	public function setS9(x1:int,x2:int,x3:int,y1:int,y2:int,y3:int,z1:int,z2:int,z3:int):void{
		a00=x1;a01=x2;a02=x3;
		a10=y1;a11=y2;a12=y3;
		a20=z1;a21=z2;a22=z3;
	}

	public function reset():void{
		a00=ONE;a01=0;a02=0;
		a10=0;a11=ONE;a12=0;
		a20=0;a21=0;a22=ONE;
	}

	public function equals(m2:Matrix3x3):Boolean{
		return(
			a00==m2.a00 && a01==m2.a01 && a02==m2.a02 &&
			a10==m2.a10 && a11==m2.a11 && a12==m2.a12 &&
			a20==m2.a20 && a21==m2.a21 && a22==m2.a22);
	}

	public var a00:int,a01:int,a02:int;
	public var a10:int,a11:int,a12:int;
	public var a20:int,a21:int,a22:int;
}

}
