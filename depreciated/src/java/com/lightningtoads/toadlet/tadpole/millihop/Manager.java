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

public interface Manager{
	public void traceLine(Segment segment,Collision result);
	public void traceSolid(Segment segment,HopEntity solid,Collision result);
	public void preUpdate(int dt,int fdt);
	public void postUpdate(int dt,int fdt);
	public void preUpdate(HopEntity solid,int dt,int fdt);
	public void intraUpdate(HopEntity solid,int dt,int fdt);
	public void postUpdate(HopEntity solid,int dt,int fdt);
}
