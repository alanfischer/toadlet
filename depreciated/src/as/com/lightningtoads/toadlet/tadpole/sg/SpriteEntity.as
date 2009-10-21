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

public class SpriteEntity extends RenderableEntity{
	public function SpriteEntity(engine:Engine){
		super(engine);
		mType|=TYPE_SPRITE;

		mTargetFrame=-1;

		mEngine.getSceneManager().registerSpriteEntity(this);
	}

	public override function destroy():void{
		mEngine.getSceneManager().unregisterSpriteEntity(this);
		super.destroy();
	}
	
	public function load(width:int,height:int,scaled:Boolean,name:String):void{
		mWidth=width;
		mHeight=height;

		mSprite=mEngine.getSprite(name);

		mScaled=scaled;
		if(scaled){
			var hw:int=width>>7;
			var hh:int=height>>7;
			mBoundingRadius=tMath.sqrt(((hw*hw)>>4) + ((hh*hh)>>4));
		}
		else{
			mBoundingRadius=-tMath.ONE;
		}

		mFrame=0;
		mFrameTime=0;
		mTime=0;
		mDieOnFinish=false;

		setFrame(0);
	}

	public function setBlending(blend:Boolean,add:Boolean):void{
	}

	public function setTransparent(transparent:Boolean):void{
	}

	public function setFrameTime(frameTime:int):void{
		mFrameTime=frameTime;
	}

	public function setFrame(frame:int):void{
		mFrame=frame;
		updateFrame();
	}

	// If -1, then just advance as normal
	public function setTargetFrame(frame:int):void{
		mTargetFrame=frame;
	}

	public function setDieOnFinish(die:Boolean):void{
		mDieOnFinish=die;
	}

	internal function updateFrame():void{
		mFrameX=((mFrame+mSprite.startFrame)%mSprite.widthFrames);
		mFrameY=((mFrame+mSprite.startFrame)/mSprite.widthFrames);
	}

	internal function updateAnimation(dt:int):void{
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

	public var mFrame:int;
	internal var mFrameTime:int;
	internal var mTargetFrame:int;
	internal var mTime:int;
	public var mWidth:int;
	public var mHeight:int;
	public var mSprite:Sprite;
	public var mScaled:Boolean;
	public var mFrameX:int;
	public var mFrameY:int;
	public var mDieOnFinish:Boolean;
}

}
