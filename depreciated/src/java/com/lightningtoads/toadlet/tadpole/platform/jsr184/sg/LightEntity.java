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
import javax.microedition.m3g.*;

public class LightEntity extends Entity{
	public LightEntity(Engine engine){
		super(engine);
		mType|=TYPE_LIGHT;
		
		mLight=new Light();
		mNode=mLight;
		
		mLight.setMode(Light.DIRECTIONAL);
	}

	public void setDirection(Vector3 dir){
		Math.normalizeCarefully(mDirection,dir,0);

		if(mDirection.equals(Math.NEG_Z_UNIT_VECTOR3)){
			mNode.setOrientation(0,0,0,0);
		}
		else if(mDirection.equals(Math.Z_UNIT_VECTOR3)){
			mNode.setOrientation(180,1,0,0);
		}
		else{
			Math.normalizeCarefully(mCacheUp,Math.Z_UNIT_VECTOR3,0);
			Math.cross(mCacheRight,mDirection,mCacheUp);
			Math.normalizeCarefully(mCacheRight,0);
			Math.cross(mCacheUp,mCacheRight,mDirection);

			if(mTransform==null){
				mTransform=new Transform();
				mTransformData=new float[16];
				mTransformData[15]=1.0f;
			}
			
			mTransformData[0]=Math.fixedToFloat(mCacheRight.x);
			mTransformData[4]=Math.fixedToFloat(mCacheRight.y);
			mTransformData[8]=Math.fixedToFloat(mCacheRight.z);
			mTransformData[1]=Math.fixedToFloat(mCacheUp.x);
			mTransformData[5]=Math.fixedToFloat(mCacheUp.y);
			mTransformData[9]=Math.fixedToFloat(mCacheUp.z);
			mTransformData[2]=-Math.fixedToFloat(mDirection.x);
			mTransformData[6]=-Math.fixedToFloat(mDirection.y);
			mTransformData[10]=-Math.fixedToFloat(mDirection.z);

			mTransformData[3]=0.0f;
			mTransformData[7]=0.0f;
			mTransformData[11]=0.0f;

			mTransform.set(mTransformData);
			mNode.setTransform(mTransform);
		}
	}

	public Vector3 getDirection(){
		return mDirection;
	}

	public void setDiffuseColor(int diffuse){
		mLight.setColor(diffuse);
	}

	Vector3 mDirection=new Vector3();
	Light mLight=new Light();

	Vector3 mCacheRight=new Vector3();
	Vector3 mCacheUp=new Vector3();
}
