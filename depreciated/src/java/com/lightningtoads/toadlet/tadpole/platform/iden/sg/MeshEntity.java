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
import com.motorola.iden.micro3d.*;

public class MeshEntity extends RenderableEntity{
	public MeshEntity(Engine engine){
		super(engine);
		mType|=TYPE_MESH;
	}

	public void destroy(){
		super.destroy();

		stopAnimation();
	}

	// A limitation of MeshEntities using Micro3D is there can not be duplicate animated MeshEntities.
	// If this functionality becomes necessary in the future, it would be necessary to load multiple Figures for each new animated MeshEntity
	public void load(String name){
		stopAnimation();

		mName=name;
		mFigure=mEngine.getFigure(name);
		mObject3D=mFigure;

		if(mFigure==null){
			throw new RuntimeException("Figure "+name+" not cached");
		}
	}

	public Figure getFigure(){
		return mFigure;
	}

	public void startAnimation(int animation,boolean loop){
		startAnimation(animation,loop,Math.ONE,false);
	}
	
	public void startAnimation(int animation,boolean loop,int timeScale,boolean keepLastFrame){
		if(mActionTable==null){
			mActionTable=mEngine.getActionTable(mName);
			if(mActionTable!=null){
				mFigure.setActionTable(mActionTable);
				mSceneManager=mEngine.getSceneManager();
				mSceneManager.registerAnimatedMeshEntity(this);
			}
		}

		if(mActionTable!=null){
			mAnimationMax=mActionTable.getNumberOfFrames(animation);
		}

		mAnimation=animation;
		mAnimationTime=0;
		mAnimationTimeScale=timeScale;
		mLoopAnimation=loop;
		mKeepLastFrame=keepLastFrame;
		mHoldingLastFrame=false;
		mFigure.setPosture(mAnimation,0);
	}

	public void stopAnimation(){
		if(mActionTable!=null){
			if(mSceneManager!=null){
				mSceneManager.unregisterAnimatedMeshEntity(this);
				mSceneManager=null;
			}
			if(mDestroyed==false){
				mFigure.setPosture(mAnimation,0);
			}
			mActionTable=null;
		}
	}

	String mName;
	Figure mFigure;
	ActionTable mActionTable;
	int mAnimation;
	int mAnimationTime;
	int mAnimationTimeScale;
	int mAnimationMax;
	boolean mLoopAnimation;
	boolean mKeepLastFrame;
	boolean mHoldingLastFrame;

	SceneManager mSceneManager;
}
