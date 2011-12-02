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

public abstract class RenderableEntity extends Entity{
	public final static int MAX_LAYERS=4;

	public RenderableEntity(Engine engine){
		super(engine);
		mType|=TYPE_RENDERABLE;
	}

	public void setVisible(boolean visible){
		mVisible=visible;
	}

	public boolean getVisible(){
		return mVisible;
	}

	public void setLayer(int layer){
		mLayer=layer;
	}

	public int getLayer(){
		return mLayer;
	}

	int mLayer=1;
	boolean mVisible=true;
}
