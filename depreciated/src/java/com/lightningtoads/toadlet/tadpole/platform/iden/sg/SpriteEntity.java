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

import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.*;
import com.motorola.iden.micro3d.*;

public class SpriteEntity extends RenderableEntity{
	public SpriteEntity(Engine engine){
		super(engine);
		mType|=TYPE_SPRITE;

		mTargetFrame=-1;

		mSceneManager=mEngine.getSceneManager();
		mSceneManager.registerSpriteEntity(this);
	}

	public void destroy(){
		if(mSceneManager!=null){
			mSceneManager.unregisterSpriteEntity(this);
		}

		super.destroy();
	}

	/// @todo non-scasled sprites are not in the same units as c++.
	///   In mascot it appears LOCAL_SIZE doesnt work with PARALLEL_PROJECTION, so we use PIXEL_SIZE
	public void load(int width,int height,boolean scaled,String name){
		int type=0;
		
		mSprite=mEngine.getSprite(name);

		if(mSprite==null){
			throw new RuntimeException("Sprite "+name+" not cached");
		}

		int bits=mEngine.getSceneManager().mRenderShiftBits;
		if(scaled){
			int hw=width>>1;
			int hh=height>>1;
			mBoundingRadius=Math.sqrt(
				//#exec fpcomp.bat hw*hw + hh*hh
			);

			type|=PointSprite.LOCAL_SIZE|PointSprite.PERSPECTIVE_PROJECTION;

			width>>=bits;
			height>>=bits;
		}
		else{
			mBoundingRadius=-Math.ONE;

			type|=PointSprite.PIXEL_SIZE|PointSprite.PARALLEL_PROJECTION;

			width=mSprite.frameWidth;
			height=mSprite.frameHeight;
		}

		// Sub 1 from width & height so we dont get invalid tex-coords
		PointSprite sprite=new PointSprite(new Vector3D(),width,height,0,0,0,mSprite.frameWidth-1,mSprite.frameHeight-1,type,null,mSprite.texture);
		sprite.enableColorKeyTransparency(mTransparent);
		mObject3D=sprite;

		mFrame=0;
		mFrameTime=0;
		mTime=0;
		mDieOnFinish=false;

		setFrame(0);
	}

	public void setBlending(boolean blend,boolean add){
		PointSprite sprite=(PointSprite )mObject3D;
		if(blend){
			mLayout.setSemiTransparent(true);
			if(add){
				sprite.setBlendingType(Primitive.BLENDING_ADD);
			}
			else{
				sprite.setBlendingType(Primitive.BLENDING_HALF);
			}
		}
		else{
			mLayout.setSemiTransparent(false);
		}
	}

	public void setTransparent(boolean transparent){
		mTransparent=transparent;
		((PointSprite)mObject3D).enableColorKeyTransparency(transparent);
	}

	public void setFrameTime(int frameTime){
		mFrameTime=frameTime;
	}

	public void setFrame(int frame){
		mFrame=frame;
		updateFrame();
	}

	// If -1, then just advance as normal
	public void setTargetFrame(int frame){
		mTargetFrame=frame;
	}

	public void setDieOnFinish(boolean die){
		mDieOnFinish=die;
	}

	void updateFrame(){
		PointSprite ps=(PointSprite)mObject3D;

		int x=((mFrame+mSprite.startFrame)%mSprite.widthFrames)*mSprite.frameWidth;
		int y=((mFrame+mSprite.startFrame)/mSprite.widthFrames)*mSprite.frameHeight;

		ps.setTextureCoordinates(PointSprite.VERTEX_A,x,y);
	}

	Sprite mSprite;
	int mFrameTime;
	int mFrame;
	int mTargetFrame;
	int mTime;
	boolean mDieOnFinish;
	boolean mTransparent;

	SceneManager mSceneManager;
}
