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
import javax.microedition.khronos.opengles.*;

public class SpriteEntity extends RenderableEntity{
	public SpriteEntity(Engine engine){
		super(engine);
		mType|=TYPE_SPRITE;

		mTargetFrame=-1;
		mScaled=true;

		VertexFormat vertexFormat=new VertexFormat(2);
		vertexFormat.addVertexElement(new VertexElement(VertexElement.TYPE_POSITION,VertexElement.FORMAT_BIT_FIXED_32|VertexElement.FORMAT_BIT_COUNT_3));
		vertexFormat.addVertexElement(new VertexElement(VertexElement.TYPE_TEX_COORD,VertexElement.FORMAT_BIT_FIXED_32|VertexElement.FORMAT_BIT_COUNT_2));
		Buffer vertexBuffer=new Buffer(Buffer.UT_STATIC,Buffer.AT_WRITE_ONLY,vertexFormat,4);
		mVertexData=new VertexData(vertexBuffer);

		mIndexData=new IndexData(IndexData.PRIMITIVE_TRISTRIP,null,0,4);

		mMaterial=new Material();
		mMaterial.setFaceCulling(Material.FC_NONE);

		mSceneManager=mEngine.getSceneManager();
		mSceneManager.registerSpriteEntity(this);
	}

	public void destroy(){
		if(mSceneManager!=null){
			mSceneManager.unregisterSpriteEntity(this);
		}

		super.destroy();
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

	public void load(int width,int height,boolean scaled,String name){
		mSprite=mEngine.getSprite(name);

		if(mSprite==null){
			throw new RuntimeException("Invalid Sprite");
		}

		if(scaled){
			int hw=width>>1;
			int hh=height>>1;
			mBoundingRadius=Math.sqrt(Math.square(hw) + Math.square(hh));
		}
		else{
			mBoundingRadius=-Math.ONE;
		}

		mWidth=width;
		mHeight=height;
		mScaled=scaled;

		updateFrame();

		mMaterial.setTexture(mSprite.texture);
	}

	public void setTransparent(boolean transparent){
		if(transparent){
			mMaterial.setAlphaTest(Material.AT_GEQUAL,Math.HALF);
		}
		else{
			mMaterial.setAlphaTest(Material.AT_NONE,Math.HALF);
		}
	}

	public void setBlending(boolean blend,boolean add){
		if(blend){
			if(add){
				mMaterial.setBlend(Material.COMBINATION_ALPHA_ADDITIVE);
			}
			else{
				mMaterial.setBlend(Material.COMBINATION_ALPHA);
			}
			mMaterial.setDepthWrite(false);
		}
		else{
			mMaterial.setBlend(Material.DISABLED_BLEND);
			mMaterial.setDepthWrite(true);
		}
	}

	public Material getMaterial(){return mMaterial;}

	void updateFrame(){
		if(mSprite==null){
			return;
		}

		int x=((mFrame+mSprite.startFrame)%mSprite.widthFrames);
		int y=((mFrame+mSprite.startFrame)/mSprite.widthFrames);

		int tx1=Math.div(Math.intToFixed(x),Math.intToFixed(mSprite.widthFrames));
		int ty1=Math.div(Math.intToFixed(y),Math.intToFixed(mSprite.heightFrames));
		int tx2=Math.div(Math.intToFixed(x+1),Math.intToFixed(mSprite.widthFrames));
		int ty2=Math.div(Math.intToFixed(y+1),Math.intToFixed(mSprite.heightFrames));

		{
			java.nio.IntBuffer data=(java.nio.IntBuffer)mVertexData.getVertexBuffer(0).lock(Buffer.LT_WRITE_ONLY);

			data.put(0,-Math.HALF);
			data.put(1,Math.HALF);
			data.put(2,0);

			data.put(3,tx1);
			data.put(4,ty1);

			data.put(5,Math.HALF);
			data.put(6,Math.HALF);
			data.put(7,0);

			data.put(8,tx2);
			data.put(9,ty1);

			data.put(10,-Math.HALF);
			data.put(11,-Math.HALF);
			data.put(12,0);

			data.put(13,tx1);
			data.put(14,ty2);

			data.put(15,Math.HALF);
			data.put(16,-Math.HALF);
			data.put(17,0);

			data.put(18,tx2);
			data.put(19,ty2);

			mVertexData.getVertexBuffer(0).unlock();
		}
	}

	void updateAnimation(int dt){
		if(mSprite==null){
			return;
		}

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

	Sprite mSprite;

	int mFrameTime;
	int mFrame;
	int mTargetFrame;
	int mTime;
	int mWidth;
	int mHeight;
	boolean mScaled;
	boolean mDieOnFinish;

	Material mMaterial;
	VertexData mVertexData;
	IndexData mIndexData;

	SceneManager mSceneManager;
}
