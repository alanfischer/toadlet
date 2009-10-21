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

//#ifdef TOADLET_PLATFORM_BLACKBERRY
	import net.rim.device.api.system.Bitmap;
//#else
	import javax.microedition.lcdui.Image;
//#endif

public final class Sprite{
	//#ifdef TOADLET_PLATFORM_BLACKBERRY
		public Sprite(int widthFrames,int heightFrames,int startFrame,int numFrames,Bitmap[] images){
	//#else
		public Sprite(int widthFrames,int heightFrames,int startFrame,int numFrames,Image[] images){
	//#endif
		this.widthFrames=widthFrames;
		this.heightFrames=heightFrames;
		this.startFrame=startFrame;
		this.numFrames=numFrames;
		this.images=images;
	}
	
	public int widthFrames;
	public int heightFrames;
	public int startFrame;
	public int numFrames;
	//#ifdef TOADLET_PLATFORM_BLACKBERRY
		public Bitmap[] images;
	//#else
		public Image[] images;
	//#endif
}
