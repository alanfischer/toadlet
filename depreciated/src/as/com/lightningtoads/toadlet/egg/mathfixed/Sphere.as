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

public final class Sphere{
	public function Sphere(){
		origin=new Vector3();
	}

	public static function SphereS(sphere:Sphere):Sphere{
		var s:Sphere=new Sphere();
		s.origin.setV(sphere.origin);
		s.radius=sphere.radius;
		return s;
	}

	public static function SphereR(radius1:int):Sphere{
		var s:Sphere=new Sphere();
		s.radius=radius1;
		return s;
	}

	public static function SphereVR(origin1:Vector3,radius1:int):Sphere{
		var s:Sphere=new Sphere();
		s.origin.setV(origin1);
		s.radius=radius1;
		return s;
	}

	public function setS(sphere:Sphere):void{
		origin.x=sphere.origin.x;
		origin.y=sphere.origin.y;
		origin.z=sphere.origin.z;

		radius=sphere.radius;
	}

	public function setVR(origin1:Vector3,radius1:int):void{
		origin.x=origin1.x;
		origin.y=origin1.y;
		origin.z=origin1.z;

		radius=radius1;
	}

	public function reset():void{
		origin.x=0;
		origin.y=0;
		origin.z=0;
		
		radius=0;
	}

	public function setOriginV(origin1:Vector3):void{
		origin.x=origin1.x;
		origin.y=origin1.y;
		origin.z=origin1.z;
	}

	public function setOriginXYZ(x:int,y:int,z:int):void{
		origin.x=x;
		origin.y=y;
		origin.z=z;
	}

	public function getOrigin():Vector3{
		return origin;
	}

	public function setRadius(radius1:int):void{
		radius=radius1;
	}

	public function getRadius():int{
		return radius;
	}

	public var origin:Vector3;
	public var radius:int;
}

}
