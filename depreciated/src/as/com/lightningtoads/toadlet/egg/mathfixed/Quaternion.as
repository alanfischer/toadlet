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

public final class Quaternion{
	public function Quaternion(){
		x=0;
		y=0;
		z=0;
		w=0;
	}

	public static function QuaternionXYZW(x1:int,y1:int,z1:int,w1:int):Quaternion{
		var q:Quaternion=new Quaternion();
		q.x=x1;
		q.y=y1;
		q.z=z1;
		q.w=w1;
		return q;
	}

	public static function QuaternionQ(q1:Quaternion):Quaternion{
		var q:Quaternion=new Quaternion();
		q.x=q1.x;
		q.y=q1.y;
		q.z=q1.z;
		q.w=q1.w;
		return q;
	}
	
	public function setQ(q:Quaternion):void{
		x=q.x;
		y=q.y;
		z=q.z;
		w=q.w;
	}

	public function setXYZW(x1:int,y1:int,z1:int,w1:int):void{
		x=x1;
		y=y1;
		z=z1;
		w=w1;
	}

	public function reset():void{
		x=0;
		y=0;
		z=0;
		w=0;
	}

	public function equals(q:Quaternion):Boolean{
		return x==q.x && y==q.y && z==q.z && w==q.w;
	}

	public var x:int,y:int,z:int,w:int;
}

}
