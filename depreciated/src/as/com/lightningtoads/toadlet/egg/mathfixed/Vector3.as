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

public final class Vector3{
	public function Vector3(){
		x=0;
		y=0;
		z=0;
	}

	public static function Vector3XYZ(x1:int,y1:int,z1:int):Vector3{
		var v:Vector3=new Vector3();
		v.x=x1;
		v.y=y1;
		v.z=z1;
		return v;
	}

	public static function Vector3V(v1:Vector3):Vector3{
		var v:Vector3=new Vector3();
		v.x=v1.x;
		v.y=v1.y;
		v.z=v1.z;
		return v;
	}
	
	public function setV(v:Vector3):void{
		x=v.x;
		y=v.y;
		z=v.z;
	}

	public function setXYZ(x1:int,y1:int,z1:int):void{
		x=x1;
		y=y1;
		z=z1;
	}

	public function reset():void{
		x=0;
		y=0;
		z=0;
	}

	public function equals(v:Vector3):Boolean{
		return x==v.x && y==v.y && z==v.z;
	}

	public var x:int,y:int,z:int;
}

}
