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

package com.lightningtoads.toadlet.tadpole.millihop{

import com.lightningtoads.toadlet.egg.mathfixed.*;

public final class Collision{
	public function Collision(){}

	public function setC(c:Collision):void{
		time=c.time;
		point.x=c.point.x;point.y=c.point.y;point.z=c.point.z;
		normal.x=c.normal.x;normal.y=c.normal.y;normal.z=c.normal.z;
		velocity.x=c.velocity.x;velocity.y=c.velocity.y;velocity.z=c.velocity.z;
		solid=c.solid;
		collider=c.collider;
	}

	public function reset():void{
		time=-tMath.ONE;
		point.x=0;point.y=0;point.z=0;
		normal.x=0;normal.y=0;normal.z=0;
		velocity.x=0;velocity.y=0;velocity.z=0;
		solid=null;
		collider=null;
	}

	public var time:int=-tMath.ONE;
	public var point:Vector3=new Vector3();
	public var normal:Vector3=new Vector3();
	public var velocity:Vector3=new Vector3();
	public var solid:HopEntity=null;
	public var collider:HopEntity=null;
}

}
