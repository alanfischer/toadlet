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
import javax.microedition.m3g.*;
//#ifdef RESIZE_IMAGE
	import javax.microedition.lcdui.*;
//#endif

// We store the mFrameWidth, mFrameHeight, etc in this sprite,
// since they may need to be modified for a parallel projection sprite
public class SpriteEntity extends RenderableEntity{
	public SpriteEntity(Engine engine){
		super(engine);
		mType|=TYPE_SPRITE;

		mTargetFrame=-1;

		// By default we assign a layer+1 the normal layer of meshes.  This seems to avoid rendering problems
		setLayer(2);

		mSceneManager=mEngine.getSceneManager();
		mSceneManager.registerSpriteEntity(this);
	}

	public void destroy(){
		if(mSceneManager!=null){
			mSceneManager.unregisterSpriteEntity(this);
		}

		clean();

		super.destroy();
	}

	public void load(int width,int height,boolean scaled,String name){
		clean();

		mName=name;
		mSprite=mEngine.getSprite(name);

		if(mSprite==null){
			throw new RuntimeException("Sprite "+name+" not cached");
		}

		// TODO: This needs to be modified to work in world space, not screen space, and reenabled
		//#ifdef RESIZE_IMAGE
			// If we're in parallel mode, we must resize the image to the correct pixel size on screen
			// since JSR-184's no-scale mode only does it per-pixel
			if(scaled==false){
				mFrameWidth=mSprite.image.getWidth()/mSprite.widthFrames;
				mFrameHeight=mSprite.image.getHeight()/mSprite.heightFrames;

				Image oi=null;
				try{
					oi=mEngine.loadRegularImage(name);
				}
				catch(IOException ex){
					//#ifdef TOADLET_DEBUG
						ex.printStackTrace();
					//#endif
				}

				int w=oi.getWidth();
				int h=oi.getHeight();
				int od[]=new int[w*h];
				int nd[]=new int[mTextureWidth*mTextureHeight];
				
				oi.getRGB(od,0,w,0,0,w,h);
				
				int i,j;
				for(j=0;j<mTextureHeight;++j){
					for(i=0;i<mTextureWidth;++i){
						int a=w*i/mTextureWidth;
						int b=h*j/mTextureHeight;

						nd[j*mTextureHeight+i]=od[b*h+a];
					}
				}
				
				Image ni=Image.createRGBImage(nd,mTextureWidth,mTextureHeight,true);
				Image2D image=new Image2D(mSprite.image.getFormat(),ni);

				mSprite3D=new Sprite3D(false,image,mAppearance);
			}
		//#else
			// Otherwise we assume the file is already the correct size
			// Since it doesnt seem possible to rescale images on some nokia devices
			if(scaled==false){
				mFrameWidth=mSprite.image.getWidth()/mSprite.widthFrames;
				mFrameHeight=mSprite.image.getHeight()/mSprite.heightFrames;

				mSprite3D=new Sprite3D(false,mSprite.image,mAppearance);
				mCheckedOut=false;
			}
		//#endif
		else{
			mFrameWidth=mSprite.image.getWidth()/mSprite.widthFrames;
			mFrameHeight=mSprite.image.getHeight()/mSprite.heightFrames;

			mSprite3D=mEngine.checkoutSprite3D(name);
			
			if(mSprite3D==null){
				mSprite3D=new Sprite3D(true,mSprite.image,mAppearance);
				mCheckedOut=false;
			}
			else{
				mSprite3D.setAppearance(mAppearance);
				mCheckedOut=true;
			}

			mSprite3D.setScale(Math.fixedToFloat(width),Math.fixedToFloat(height),1);
		}

		mNode=mSprite3D;

		if(mParent!=null){
			mParent.mGroup.addChild(mNode);
		}

		mFrame=0;
		mFrameTime=0;
		mTime=0;
		mDieOnFinish=false;

		mSprite3D.setCrop(0,0,mFrameWidth,mFrameHeight);
		
		setFrame(0);
	}
	
	void clean(){
		if(mNode!=null && mNode.getParent()!=null){
			((Group)(mNode.getParent())).removeChild(mNode);
		}
		mNode=null;

		if(mCheckedOut){
			mEngine.checkinSprite3D(mName,mSprite3D);
		}
		mCheckedOut=false;
		mSprite3D=null;
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

	public void setLayer(int layer){
		mAppearance.setLayer(layer);
	}

	public int getLayer(){
		return mAppearance.getLayer();
	}

	public void setVisible(boolean visible){
		mNode.setRenderingEnable(visible);
	}

	public boolean getVisible(){
		return mNode.isRenderingEnabled();
	}

	public void setBlending(boolean blend,boolean add){
		if(mCompositing==null){
			mCompositing=new CompositingMode();
		}
		
		if(blend){
			if(add){
				mCompositing.setBlending(CompositingMode.ALPHA_ADD);
			}
			else{
				mCompositing.setBlending(CompositingMode.ALPHA);
			}
		}
		else{
			mCompositing.setBlending(CompositingMode.REPLACE);
		}
		
		mAppearance.setCompositingMode(mCompositing);
		mSprite3D.setAppearance(mAppearance);
	}

	public void setTransparent(boolean transparent){
		if(mCompositing==null){
			mCompositing=new CompositingMode();
		}

		if(true){
			mCompositing.setAlphaThreshold(0.5f);
//			mCompositing.setDepthWriteEnable(false);
		}
		else{
			mCompositing.setAlphaThreshold(0.0f);
//			mCompositing.setDepthWriteEnable(true);
		}

		mAppearance.setCompositingMode(mCompositing);
		mSprite3D.setAppearance(mAppearance);
	}

	public Sprite3D getSprite3D(){
		return mSprite3D;
	}

	public Appearance getAppearance(){
		return mAppearance;
	}

	void updateFrame(){
		if(mSprite3D==null){
			return;
		}

		int x=((mFrame+mSprite.startFrame)%mSprite.widthFrames)*mFrameWidth;
		int y=((mFrame+mSprite.startFrame)/mSprite.widthFrames)*mFrameHeight;

		mSprite3D.setCrop(x,y,mFrameWidth,mFrameHeight);
	}

	int mFrameWidth;
	int mFrameHeight;
	int mFrameTime;
	int mFrame;
	int mMaxFrame;
	int mTargetFrame;
	int mTime;
	boolean mDieOnFinish;
	boolean mCheckedOut;
	
	Sprite3D mSprite3D;
	Sprite mSprite;
	String mName;

	Appearance mAppearance=new Appearance();
	CompositingMode mCompositing;

	SceneManager mSceneManager;
}
