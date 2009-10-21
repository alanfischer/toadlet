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

package com.lightningtoads.toadlet.tadpole.sg;

import com.lightningtoads.toadlet.tadpole.*;
import javax.microedition.m3g.*;

public abstract class RenderableEntity extends Entity{
	public RenderableEntity(Engine engine){
		super(engine);
		mType|=TYPE_RENDERABLE;
	}

	public abstract void setLayer(int layer);

	public abstract int getLayer();

	public abstract void setVisible(boolean visible);

	public abstract boolean getVisible();
}
