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
import com.lightningtoads.toadlet.tadpole.sg.*;
import com.motorola.iden.micro3d.*;

public class LightEntity extends Entity{
	public LightEntity(Engine engine){
		super(engine);
		mType|=TYPE_LIGHT;

		mEngine.getSceneManager().registerLightEntity(this);
	}

	public void destroy(){
		if(mEngine.getSceneManager()!=null){
			mEngine.getSceneManager().unregisterLightEntity(this);
		}
		
		super.destroy();
	}

	public void setDirection(Vector3 dir){
		Math.normalizeCarefully(mDirection,dir,0);
		mDirectionD.set(mDirection.x>>4,mDirection.y>>4,mDirection.z>>4);
		mLight.setDirectionVector(mDirectionD);
	}

	public Vector3 getDirection(){
		return mDirection;
	}

	public void setDiffuseColor(int diffuse){
		int diffuseIntensity=(((diffuse&0x00FF0000)>>16) + ((diffuse&0x0000FF00)>>8) + ((diffuse&0x000000FF)>>0)) / 3;
		diffuseIntensity=(diffuseIntensity*Math.ONE)/255;
		mLight.setDirectionalIntensity(diffuseIntensity>>4);
	}

	Vector3 mDirection=new Vector3();
	Vector3D mDirectionD=new Vector3D();
	Light mLight=new Light(0,null,0);
}
