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

public class LightEntity extends Entity{
	public function LightEntity(engine:Engine){
		super(engine);
		mType|=TYPE_LIGHT;

		mEngine.getSceneManager().registerLightEntity(this);
	}

	public override function destroy():void{
		if(mEngine.getSceneManager()!=null){
			mEngine.getSceneManager().unregisterLightEntity(this);
		}

		super.destroy();
	}

	public function setDirection(dir:Vector3):void{
		tMath.normalizeCarefullyVV(mDirection,dir,0);
	}

	public function getDirection():Vector3{
		return mDirection;
	}

	public function setDiffuseColor(diffuse:int):void{
	}

	protected var mDirection:Vector3=new Vector3();
}

}
