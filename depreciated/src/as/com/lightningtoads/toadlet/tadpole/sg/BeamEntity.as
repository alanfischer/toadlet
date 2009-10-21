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

package com.lightningtoads.toadlet.tadpole.sg{

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.tMath;
import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.tadpole.sg.*;

public class BeamEntity extends RenderableEntity{
	public function BeamEntity (engine:Engine){
		super(engine);
		mType|=TYPE_BEAM;
	}

	public override function destroy():void{
		super.destroy();
	}

	public function startBeam(color:int,horizontal:Boolean,name:String,scale:int,start:Vector3,end:Vector3):void{
		mColor=color;

		mStart.setV(start);
		mEnd.setV(end);

		mScale.setXYZ(scale,scale,scale);
	}

	public function setBlending(blend:Boolean,add:Boolean):void{
	}

	public function setTransparent(transparent:Boolean):void{
	}

	public function setStartPoint(start:Vector3):void{
		mStart.setV(start);
	}

	public function setEndPoint(end:Vector3):void{
		mEnd.setV(end);
	}

	public var mStart:Vector3=new Vector3();
 	public var mEnd:Vector3=new Vector3();
	public var mColor:int=0x00FFFFFF;
}

}
