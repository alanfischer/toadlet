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

public final class Capsule{
	public function Capsule(){
		origin=new Vector3();
		direction=new Vector3();
	}

	public static function CapsuleC(capsule:Capsule):Capsule{
		var c:Capsule=new Capsule();
		c.origin.setV(capsule.origin);
		c.direction.setV(capsule.direction);
		c.radius=capsule.radius;
		return c;
	}

	public static function CapsuleVVR(origin1:Vector3,direction1:Vector3,radius1:int):Capsule{
		var c:Capsule=new Capsule();
		c.origin.setV(origin1);
		c.direction.setV(direction1);
		c.radius=radius1;
		return c;
	}

	public function setC(capsule:Capsule):void{
		origin.x=capsule.origin.x;
		origin.y=capsule.origin.y;
		origin.z=capsule.origin.z;

		direction.x=capsule.direction.x;
		direction.y=capsule.direction.y;
		direction.z=capsule.direction.z;

		radius=capsule.radius;
	}

	public function setVVR(origin1:Vector3,direction1:Vector3,radius1:int):void{
		origin.x=origin1.x;
		origin.y=origin1.y;
		origin.z=origin1.z;

		direction.x=direction1.x;
		direction.y=direction1.y;
		direction.z=direction1.z;

		radius=radius1;
	}

	public function reset():void{
		origin.x=0;
		origin.y=0;
		origin.z=0;

		direction.x=0;
		direction.y=0;
		direction.z=0;

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

	public function setDirectionV(direction1:Vector3):void{
		direction.x=direction1.x;
		direction.y=direction1.y;
		direction.z=direction1.z;
	}

	public function setDirectionXYZ(x:int,y:int,z:int):void{
		direction.x=x;
		direction.y=y;
		direction.z=z;
	}

	public function getDirection():Vector3{
		return direction;
	}

	public function setRadius(radius1:int):void{
		radius=radius1;
	}

	public function getRadius():int{
		return radius;
	}

	public function setLength(length:int):void{
		if(length>=(radius<<1)){
			if(direction.x==0 && direction.y==0 && direction.z==0){
				direction.z=(length-(radius<<1));
			}
			else{
				var s:int=tMath.div(length-(radius<<1),tMath.lengthV(direction));
				tMath.mulVS(direction,s);
			}
		}
		else{
			direction.x=0;
			direction.y=0;
			direction.z=0;
		}
	}

	public function getLength():int{
		return tMath.lengthV(direction)+(radius<<1);
	}
	
	public function rotate(rotation:Matrix3x3):void{
		tMath.mulVM(direction,rotation);
	}

	public var origin:Vector3;
	public var direction:Vector3;
	public var radius:int;
}

}
