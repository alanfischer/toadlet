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
//#ifdef TOADLET_PLATFORM_BLACKBERRY
	import net.rim.device.api.ui.*;
//#else
	import javax.microedition.lcdui.Graphics;
//#endif

public class SpriteEntity extends RenderableEntity{
	public SpriteEntity(Engine engine){
		super(engine);
		mType|=TYPE_SPRITE;

		mTargetFrame=-1;

		mEngine.getSceneManager().registerSpriteEntity(this);
	}

	public void destroy(){
		mEngine.getSceneManager().unregisterSpriteEntity(this);
		super.destroy();
	}

	public void load(int width,int height,boolean scaled,String name){
		mWidth=width;
		mHeight=height;

		mSprite=mEngine.getSprite(name);

		if(scaled){
			int hw=width>>1;
			int hh=height>>1;
			mBoundingRadius=Math.sqrt(
				//#exec fpcomp.bat hw*hw + hh*hh
			);
		}
		else{
			mBoundingRadius=-Math.ONE;
		}

		mFrame=0;
		mFrameTime=0;
		mTime=0;
		mDieOnFinish=false;

		setFrame(0);
	}

	public void setBlending(boolean blend,boolean add){
	}

	public void setTransparent(boolean transparent){
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

	public void setSwitchBias(int offset,int scale){
		mSwitchOffset=offset;
		mSwitchScale=scale;
	}

	public void setAlignment(int alignment){
		mAlignment=alignment;
	}

	void updateFrame(){
		mFrameX=((mFrame+mSprite.startFrame)%mSprite.widthFrames);
		mFrameY=((mFrame+mSprite.startFrame)/mSprite.widthFrames);
	}

	void updateAnimation(int dt){
		if(mFrameTime!=0){
			mTime+=dt;
			if(mTime>mFrameTime){
				mTime-=mFrameTime;
				if(mTargetFrame==-1){
					mFrame++;
					if(mFrame>=mSprite.numFrames){
						if(mDieOnFinish){
							destroy();
							return;
						}
						mFrame=0;
					}
				}
				else{
					if(mTargetFrame<mFrame){
						mFrame--;
					}
					else if(mTargetFrame>mFrame){
						mFrame++;
					}
					else if(mDieOnFinish){
						destroy();
						return;
					}
				}
				updateFrame();
			}
		}
	}

	int mFrame;
	int mFrameTime;
	int mTargetFrame;
	int mTime;
	public int mWidth;
	public int mHeight;
	public Sprite mSprite;
	public int mFrameX;
	public int mFrameY;
	boolean mDieOnFinish;
	public int mSwitchOffset=0;
	public int mSwitchScale=Math.ONE;
	public int mAlignment=Graphics.HCENTER|Graphics.VCENTER;
}
