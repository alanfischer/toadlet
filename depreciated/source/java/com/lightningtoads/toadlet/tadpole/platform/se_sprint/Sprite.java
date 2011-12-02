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

package com.lightningtoads.toadlet.tadpole;

import com.mascotcapsule.micro3d.v3.*;

public final class Sprite{
	public Sprite(int widthFrames,int heightFrames,int startFrame,int numFrames,Texture texture,int frameWidth,int frameHeight){
		this.widthFrames=widthFrames;
		this.heightFrames=heightFrames;
		this.startFrame=startFrame;
		this.numFrames=numFrames;
		this.texture=texture;
		this.frameWidth=frameWidth;
		this.frameHeight=frameHeight;
	}
	
	public int widthFrames;
	public int heightFrames;
	public int startFrame;
	public int numFrames;
	public Texture texture;
	public int frameWidth;
	public int frameHeight;
}
