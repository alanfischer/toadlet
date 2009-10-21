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

package com.lightningtoads.toadlet.tadpole{

public class Sprite{
	public function Sprite(widthFrames:int,heightFrames:int,startFrame:int,numFrames:int){
		this.widthFrames=widthFrames;
		this.heightFrames=heightFrames;
		this.startFrame=startFrame;
		this.numFrames=numFrames;
	}
	
	public var widthFrames:int;
	public var heightFrames:int;
	public var startFrame:int;
	public var numFrames:int;
}

}
