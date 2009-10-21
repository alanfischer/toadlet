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

public class Triangle{
	public var sx1:int,sy1:int,sx2:int,sy2:int,sx3:int,sy3:int;
	public var tu1:int,tv1:int,tu2:int,tv2:int,tu3:int,tv3:int;
	public var color1:int,color2:int,color3:int;

	public var texture:Object=null;
	
	public var next:Triangle;
};

}
