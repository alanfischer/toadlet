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

package com.lightningtoads.toadlet.tadpole.millihop;

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;

public final class Collision{
	public Collision(){}

	public void set(Collision c){
		time=c.time;
		point.x=c.point.x;point.y=c.point.y;point.z=c.point.z;
		normal.x=c.normal.x;normal.y=c.normal.y;normal.z=c.normal.z;
		velocity.x=c.velocity.x;velocity.y=c.velocity.y;velocity.z=c.velocity.z;
		solid=c.solid;
		collider=c.collider;
	}

	public void reset(){
		time=-Math.ONE;
		point.x=0;point.y=0;point.z=0;
		normal.x=0;normal.y=0;normal.z=0;
		velocity.x=0;velocity.y=0;velocity.z=0;
		solid=null;
		collider=null;
	}

	public int time=-Math.ONE;
	public Vector3 point=new Vector3();
	public Vector3 normal=new Vector3();
	public Vector3 velocity=new Vector3();
	public HopEntity solid=null;
	public HopEntity collider=null;
}

