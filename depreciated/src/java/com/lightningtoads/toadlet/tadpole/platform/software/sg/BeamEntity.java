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

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.*;

public class BeamEntity extends RenderableEntity{
	public BeamEntity(Engine engine){
		super(engine);
		mType|=TYPE_BEAM;
	}

	public void destroy(){
		super.destroy();
	}

	public void startBeam(int color,int scale,Vector3 start,Vector3 end){
		mColor=color;

		mStart.set(start);
		mEnd.set(end);

		mScale=scale;
	}

	public void setBlending(boolean blend,boolean add){
	}

	public void setTransparent(boolean transparent){
	}

	public void setStartPoint(Vector3 start){
		mStart.set(start);
	}

	public void setEndPoint(Vector3 end){
		mEnd.set(end);
	}

	public int mScale=Math.ONE;
	public Vector3 mStart=new Vector3();
 	public Vector3 mEnd=new Vector3();
	public int mColor=0xFFFFFFFF;
}
