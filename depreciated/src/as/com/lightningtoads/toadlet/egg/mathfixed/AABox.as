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

public final class AABox{
	public function AABox(){
		mins=new Vector3();
		maxs=new Vector3();
	}

	public static function AABoxA(box:AABox):AABox{
		var a:AABox=new AABox();
		a.mins.setV(box.mins);
		a.maxs.setV(box.maxs);
		return a;
	}

	public static function AABoxVV(mins1:Vector3,maxs1:Vector3):AABox{
		var a:AABox=new AABox();
		a.mins.setV(mins1);
		a.maxs.setV(maxs1);
		return a;
	}

	public static function AABoxXYZXYZ(x1:int,y1:int,z1:int,x2:int,y2:int,z2:int):AABox{
		var a:AABox=new AABox();
		a.mins.setXYZ(x1,y1,z1);
		a.maxs.setXYZ(x2,y2,z2);
		return a;
	}

	public static function AABoxR(radius:int):AABox{
		var a:AABox=new AABox();
		a.mins.setXYZ(-radius,-radius,-radius);
		a.maxs.setXYZ(radius,radius,radius);
		return a;
	}
	
	public function setA(box:AABox):void{
		mins.x=box.mins.x;
		mins.y=box.mins.y;
		mins.z=box.mins.z;

		maxs.x=box.maxs.x;
		maxs.y=box.maxs.y;
		maxs.z=box.maxs.z;
	}

	public function setVV(mins1:Vector3,maxs1:Vector3):void{
		mins.x=mins1.x;
		mins.y=mins1.y;
		mins.z=mins1.z;

		maxs.x=maxs1.x;
		maxs.y=maxs1.y;
		maxs.z=maxs1.z;
	}

	public function setXYZXYZ(minx:int,miny:int,minz:int,maxx:int,maxy:int,maxz:int):void{
		mins.x=minx;
		mins.y=miny;
		mins.z=minz;

		maxs.x=maxx;
		maxs.y=maxy;
		maxs.z=maxz;
	}

	public function setR(radius:int):void{
		mins.x=-radius;
		mins.y=-radius;
		mins.z=-radius;

		maxs.x=radius;
		maxs.y=radius;
		maxs.z=radius;
	}

	public function reset():void{
		mins.x=0;
		mins.y=0;
		mins.z=0;

		maxs.x=0;
		maxs.y=0;
		maxs.z=0;
	}

	public function setMins(mins1:Vector3):void{
		mins.x=mins1.x;
		mins.y=mins1.y;
		mins.z=mins1.z;
	}

	public function getMins():Vector3{
		return mins;
	}

	public function setMaxs(maxs1:Vector3):void{
		maxs.x=maxs1.x;
		maxs.y=maxs1.y;
		maxs.z=maxs1.z;
	}

	public function getMaxs():Vector3{
		return maxs;
	}

	public function mergeWithA(box:AABox):void{
		if(box.mins.x<mins.x)mins.x=box.mins.x;
		if(box.mins.y<mins.y)mins.y=box.mins.y;
		if(box.mins.z<mins.z)mins.z=box.mins.z;
		if(box.maxs.x>maxs.x)maxs.x=box.maxs.x;
		if(box.maxs.y>maxs.y)maxs.y=box.maxs.y;
		if(box.maxs.z>maxs.z)maxs.z=box.maxs.z;
	}

	public function mergeWithV(vec:Vector3):void{
		if(vec.x<mins.x)mins.x=vec.x;
		if(vec.y<mins.y)mins.y=vec.y;
		if(vec.z<mins.z)mins.z=vec.z;
		if(vec.x>maxs.x)maxs.x=vec.x;
		if(vec.y>maxs.y)maxs.y=vec.y;
		if(vec.z>maxs.z)maxs.z=vec.z;
	}

	public var mins:Vector3;
	public var maxs:Vector3;
}

}
