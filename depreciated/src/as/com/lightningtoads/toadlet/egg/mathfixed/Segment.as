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

public final class Segment{
	public function Segment(){
		origin=new Vector3();
		direction=new Vector3();
	}

	public function setS(segment:Segment):void{
		origin.x=segment.origin.x;
		origin.y=segment.origin.y;
		origin.z=segment.origin.z;

		direction.x=segment.direction.x;
		direction.y=segment.direction.y;
		direction.z=segment.direction.z;
	}

	public function setVV(start:Vector3,end:Vector3):void{
		origin.x=start.x;
		origin.y=start.y;
		origin.z=start.z;

		direction.x=end.x-start.x;
		direction.y=end.y-start.y;
		direction.z=end.z-start.z;
	}

	public function setStartEnd(start:Vector3,end:Vector3):void{
		origin.x=start.x;
		origin.y=start.y;
		origin.z=start.z;

		direction.x=end.x-start.x;
		direction.y=end.y-start.y;
		direction.z=end.z-start.z;
	}

	public function setStartDir(start:Vector3,end:Vector3):void{
		origin.x=start.x;
		origin.y=start.y;
		origin.z=start.z;

		direction.x=dir.x;
		direction.y=dir.y;
		direction.z=dir.z;
	}

	public function reset():void{
		origin.x=0;
		origin.y=0;
		origin.z=0;

		direction.x=0;
		direction.y=0;
		direction.z=0;
	}

	public function setOrigin(origin1:Vector3):void{
		origin.x=origin1.x;
		origin.y=origin1.y;
		origin.z=origin1.z;
	}

	public function getOrigin():Vector3{
		return origin;
	}

	public function setDirection(direction1:Vector3):void{
		direction.x=direction1.x;
		direction.y=direction1.y;
		direction.z=direction1.z;
	}

	public function getDirection():Vector3{
		return direction;
	}

	public function getEndPoint(endPoint:Vector3):void{
		endPoint.x=origin.x+direction.x;
		endPoint.y=origin.y+direction.y;
		endPoint.z=origin.z+direction.z;
	}

	public var origin:Vector3;
	public var direction:Vector3;
}

}
