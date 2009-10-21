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
import com.mascotcapsule.micro3d.v3.*;

public class SpriteEntity extends RenderableEntity{
	final static int[] POINT={0,0,0};
	final static int[] NORMAL=new int[]{4096};
	final static int[] COLOR=new int[]{0x00};

	int mCommand=Graphics3D.PRIMITVE_POINT_SPRITES|Graphics3D.PDATA_POINT_SPRITE_PARAMS_PER_CMD;
	int[] mTextureCommand={128,128,0,0,0,128,128,0};

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
	///   In mascot it appears POINT_SPRITE_LOCAL_SIZE doesnt work with POINT_SPRITE_NO_PERS, so we use POINT_SPRITE_PIXEL_SIZE
	public void load(int width,int height,boolean scaled,String name){
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

			mTextureCommand[7]=Graphics3D.POINT_SPRITE_LOCAL_SIZE|Graphics3D.POINT_SPRITE_PERSPECTIVE;

			width>>=bits;
			height>>=bits;
		}
		else{
			mBoundingRadius=-Math.ONE;

			mTextureCommand[7]=Graphics3D.POINT_SPRITE_PIXEL_SIZE|Graphics3D.POINT_SPRITE_NO_PERS;

			width=mSprite.frameWidth;
			height=mSprite.frameHeight;
		}

		mTextureCommand[0]=width;
		mTextureCommand[1]=height;

		mTexture=mSprite.texture;

		mFrame=0;
		mFrameTime=0;
		mTime=0;
		mDieOnFinish=false;
		
		setFrame(0);
	}

	public void setBlending(boolean blend,boolean add){
		mCommand&=~(Graphics3D.PATTR_BLEND_HALF|Graphics3D.PATTR_BLEND_ADD);
		if(blend){
			if(add){
				mCommand|=Graphics3D.PATTR_BLEND_ADD;
			}
			else{
				mCommand|=Graphics3D.PATTR_BLEND_HALF;
			}
		}
		mEffect.setSemiTransparentEnabled(blend);
	}

	public void setTransparent(boolean transparent){
		if(transparent){
			mCommand|=Graphics3D.PATTR_COLORKEY;
		}
		else{
			mCommand&=~Graphics3D.PATTR_COLORKEY;
		}
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
		int x=((mFrame+mSprite.startFrame)%mSprite.widthFrames)*mSprite.frameWidth;
		int y=((mFrame+mSprite.startFrame)/mSprite.widthFrames)*mSprite.frameHeight;

		mTextureCommand[3]=x;
		mTextureCommand[4]=y;

		mTextureCommand[5]=x+mSprite.frameWidth-1;
		mTextureCommand[6]=y+mSprite.frameHeight-1;
	}

	public void render(Graphics3D g,int hwidth,int hheight){
		g.renderPrimitives(mTexture,hwidth,hheight,mLayout,mEffect,mCommand,1,POINT,NORMAL,mTextureCommand,COLOR);
	}

	Sprite mSprite;
	int mFrameTime;
	int mFrame;
	int mTargetFrame;
	int mTime;
	boolean mDieOnFinish;

	SceneManager mSceneManager;
}
