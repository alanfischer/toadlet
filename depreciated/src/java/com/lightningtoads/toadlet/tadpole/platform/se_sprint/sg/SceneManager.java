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

import javax.microedition.lcdui.*;
import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.*;
import com.mascotcapsule.micro3d.v3.*;
//#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
//#endif

//#ifdef TOADLET_PLATFORM_SPRINT
	// We need to use this to get around the failure to implement setProjection(near,far,width,height)
	//#define TOADLET_USE_SPRINT_HACK
//#endif

public class SceneManager extends NodeEntity{
	public SceneManager(Engine engine){
		super(engine);

		mBackgroundNode=new NodeEntity(engine);
		attach(mBackgroundNode);

		mLayers=new RenderableEntity[RenderableEntity.MAX_LAYERS][];
		mLayersSize=new int[RenderableEntity.MAX_LAYERS];
		int i;
		for(i=0;i<RenderableEntity.MAX_LAYERS;++i){
			mLayers[i]=new RenderableEntity[RenderableEntity.MAX_RENDERABLES_IN_LAYER];
		}

		mProjNear=256;
		mProjFar=4096;
		//#ifdef TOADLET_USE_SPRINT_HACK
			mProjFov=512;
		//#else
			mProjWidth=engine.mWidth<<12;
			mProjHeight=engine.mHeight<<12;
		//#endif
	}

	public void registerRenderableEntity(RenderableEntity entity){
		int i;
		for(i=0;i<mNumRegisteredEntities;++i){
			if(mRegisteredEntities[i]==entity)break;
		}

		if(i==mNumRegisteredEntities){
			if(mRegisteredEntities.length<mNumRegisteredEntities+1){
				RenderableEntity[] entities=new RenderableEntity[mNumRegisteredEntities+1];
				System.arraycopy(mRegisteredEntities,0,entities,0,mRegisteredEntities.length);
				mRegisteredEntities=entities;
			}

			mRegisteredEntities[mNumRegisteredEntities]=entity;
			mNumRegisteredEntities++;

			//#ifdef TOADLET_USE_SPRINT_HACK
				entity.mLayout.setPerspective(mProjNear,mProjFar,mProjFov);
			//#else
				entity.mLayout.setPerspective(mProjNear,mProjFar,mProjWidth,mProjHeight);
			//#endif
		}
	}

	public void unregisterRenderableEntity(RenderableEntity entity){
		int i;
		for(i=0;i<mNumRegisteredEntities;++i){
			if(mRegisteredEntities[i]==entity)break;
		}
		if(i!=mNumRegisteredEntities){
			mRegisteredEntities[i]=null;
			System.arraycopy(mRegisteredEntities,i+1,mRegisteredEntities,i,mNumRegisteredEntities-(i+1));
			mNumRegisteredEntities--;
			mRegisteredEntities[mNumRegisteredEntities]=null;
		}
	}

	final void updateProjectionParameters(){
		//#ifdef TOADLET_DEBUG
			Logger.getInstance().addLogString(Logger.LEVEL_ALERT,"Updating projection parameters");
		//#endif

		//#ifdef TOADLET_PLATFORM_SPRINT
			//#ifdef TOADLET_USE_SPRINT_HACK
				if(mLastProjectionType==CameraEntity.PT_FOVY){
					//#ifdef TOADLET_DEBUG
						Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"PT_FOVY unsupported on sprint");
					//#endif
					throw new RuntimeException("PT_FOVY unsupported on sprint");
				}
			
				int rx=Math.div(Math.intToFixed(mLastCanvasWidth),Math.intToFixed(mLastViewportWidth));

				int q=Math.arctan(Math.mul(rx,Math.tan(mLastFov>>1)))<<1;

				mProjNear=mLastNear>>mRenderShiftBits;
				mProjFar=mLastFar>>mRenderShiftBits;
				mProjFov=Math.div(q,Math.TWO_PI)>>4;

				int i;
				for(i=0;i<mNumRegisteredEntities;++i){
					RenderableEntity entity=mRegisteredEntities[i];
					entity.mLayout.setPerspective(mProjNear,mProjFar,mProjFov);
				}
			//#else
				int rx=Math.div(Math.intToFixed(mLastCanvasWidth),Math.intToFixed(mLastViewportWidth));
				int ry=Math.div(Math.intToFixed(mLastCanvasHeight),Math.intToFixed(mLastViewportHeight));

				int neartan2=Math.mul(mLastNear,Math.tan(mLastFov>>1));
				
				int px=0;
				int py=0;
				if(mLastProjectionType==CameraEntity.PT_FOVX){
					px=Math.mul(rx,neartan2)<<1;
					py=Math.mul(Math.mul(ry,neartan2)<<1,mLastAspect);
				}
				else if(mLastProjectionType==CameraEntity.PT_FOVY){
					px=Math.mul(Math.mul(rx,neartan2)<<1,mLastAspect);
					py=Math.mul(ry,neartan2)<<1;
				}
				else{
					//#ifdef TOADLET_DEBUG
						Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"Unknown projection type");
					//#endif
					throw new RuntimeException("Unknown projection type");
				}

				mProjNear=mLastNear>>mRenderShiftBits;
				mProjFar=mLastFar>>mRenderShiftBits;
				mProjWidth=(px>>mRenderShiftBits)<<12;
				mProjHeight=(py>>mRenderShiftBits)<<12;

				int i;
				for(i=0;i<mNumRegisteredEntities;++i){
					RenderableEntity entity=mRegisteredEntities[i];
					entity.mLayout.setPerspective(mProjNear,mProjFar,mProjWidth,mProjHeight);
				}
			//#endif
		//#elifdef TOADLET_PLATFORM_SE
			int rx=0;
			int ry=0;
			if(mEngine.mSonyEricssonJP3){
				// This section is a work-in-progress to get the viewport projection working correctly in JP3 mascot engines.  This seems to be right so far
				rx=Math.div(Math.intToFixed(mLastViewportWidth),Math.intToFixed(mLastViewportWidth-mLastViewportX));
				ry=Math.div(Math.intToFixed(mLastViewportHeight),Math.intToFixed(mLastViewportHeight-mLastViewportY));
			}
			else{
				rx=Math.div(Math.intToFixed(mLastCanvasWidth),Math.intToFixed(mLastViewportWidth));
				ry=Math.div(Math.intToFixed(mLastCanvasHeight),Math.intToFixed(mLastViewportHeight));
			}

			int neartan2=Math.mul(mLastNear,Math.tan(mLastFov>>1));
			
			int px=0;
			int py=0;
			if(mLastProjectionType==CameraEntity.PT_FOVX){
				px=Math.mul(rx,neartan2)<<1;
				py=Math.mul(Math.mul(ry,neartan2)<<1,mLastAspect);
			}
			else if(mLastProjectionType==CameraEntity.PT_FOVY){
				px=Math.mul(Math.mul(rx,neartan2)<<1,mLastAspect);
				py=Math.mul(ry,neartan2)<<1;
			}
			else{
				//#ifdef TOADLET_DEBUG
					Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"Unknown projection type");
				//#endif
				throw new RuntimeException("Unknown projection type");
			}

			mProjNear=mLastNear>>mRenderShiftBits;
			mProjFar=mLastFar>>mRenderShiftBits;
			mProjWidth=(px>>mRenderShiftBits)<<12;
			mProjHeight=(py>>mRenderShiftBits)<<12;

			int i;
			for(i=0;i<mNumRegisteredEntities;++i){
				RenderableEntity entity=mRegisteredEntities[i];
				entity.mLayout.setPerspective(mProjNear,mProjFar,mProjWidth,mProjHeight);
			}
		//#endif
	}

	public final NodeEntity getBackgroundNode(){
		return mBackgroundNode;
	}

	public void setTime(int time){
		mTime=time;
	}

	public int getTime(){
		return mTime;
	}

	public final void setRenderShiftBits(int b){
		mRenderShiftBits=b;
	}

	public final int getRenderShiftBits(){
		return mRenderShiftBits;
	}

	public final void setAmbientColor(int ambient){
		mAmbientIntensity=(((ambient&0x00FF0000)>>16) + ((ambient&0x0000FF00)>>8) + ((ambient&0x000000FF)>>0)) / 3;
		mAmbientIntensity=(mAmbientIntensity*Math.ONE)/255;
		if(mLight!=null){
			mLight.mLight.setAmbientIntensity(mAmbientIntensity>>4);
		}
	}

	public final int getAmbientColor(){
		int ambient=(mAmbientIntensity*255)/Math.ONE;
		return (ambient<<16) + (ambient<<8) + ambient;
	}

	public void update(int dt){
		//#ifdef TOADLET_DEBUG
			if(mDestroyed){
				Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"update called on a destroyed SceneManager");
				throw new RuntimeException("update called on a destroyed SceneManager");
			}
		//#endif

		mTime+=dt;

		// Update sprites
		int i;
		for(i=0;i<mNumSpriteEntities;++i){
			SpriteEntity sprite=mSpriteEntities[i];
			if(sprite.mFrameTime!=0){
				sprite.mTime+=dt;
				if(sprite.mTime>sprite.mFrameTime){
					sprite.mTime-=sprite.mFrameTime;
					if(sprite.mTargetFrame==-1){
						sprite.mFrame++;
						if(sprite.mFrame>=sprite.mSprite.numFrames){
							if(sprite.mDieOnFinish){
								sprite.destroy();
								continue;
							}
							sprite.mFrame=0;
						}
					}
					else{
						if(sprite.mTargetFrame<sprite.mFrame){
							sprite.mFrame--;
						}
						else if(sprite.mTargetFrame>sprite.mFrame){
							sprite.mFrame++;
						}
						else if(sprite.mDieOnFinish){
							sprite.destroy();
							continue;
						}
					}

					sprite.updateFrame();
				}
			}
		}

		// Update animated meshes
		int fdt=Math.intToFixed(dt);
		for(i=0;i<mNumAnimatedMeshEntities;++i){
			MeshEntity mesh=mAnimatedMeshEntities[i];
			if(mesh.mHoldingLastFrame==false){
				mesh.mAnimationTime+=(Math.fixedToInt(Math.mul(fdt,mesh.mAnimationTimeScale))*30*65536/1000);
				if(mesh.mAnimationTime>=mesh.mAnimationMax){
					if(mesh.mLoopAnimation==true){
						mesh.mAnimationTime=0;
					}
					else if(mesh.mKeepLastFrame==true){
						mesh.mAnimationTime=mesh.mAnimationMax-1;
						mesh.mHoldingLastFrame=true;
					}
					else{
						mesh.stopAnimation();
					}
				}
				if(mesh.mActionTable!=null){
					mesh.mFigure.setPosture(mesh.mActionTable,mesh.mAnimation,mesh.mAnimationTime);
				}
			}
		}
	}

	public final void render(Graphics g,CameraEntity camera){
		render(g,mEngine.mWidth,mEngine.mHeight,camera);
	}

	public final void render(Graphics g,int canvasWidth,int canvasHeight,CameraEntity camera){
		//#ifdef TOADLET_DEBUG
			if(mDestroyed){
				Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"SceneManager.render called on a destroyed SceneManager");
				throw new RuntimeException("SceneManager.render called on a destroyed SceneManager");
			}
		//#endif

		//#ifdef TOADLET_DEBUG
			Profile.beginSection(Profile.SECTION_RENDER);
		//#endif

		mGraphics3D=mEngine.getGraphics3D();

		int clipX=g.getClipX();
		int clipY=g.getClipY();
		int clipWidth=g.getClipWidth();
		int clipHeight=g.getClipHeight();

		int viewportX=clipX;
		int viewportY=clipY;
		int viewportWidth=clipWidth;
		int viewportHeight=clipHeight;

		int translateX=g.getTranslateX();
		int translateY=g.getTranslateY();

		if(camera.mViewportSet){
			viewportX=camera.getViewportX();
			viewportY=camera.getViewportY();
			viewportWidth=camera.getViewportWidth();
			viewportHeight=camera.getViewportHeight();
		}

		mCamera=camera;

		updateWorldTransform(mCamera);

		mCamera.updateViewTransform();

		if(mBackgroundNode.mNumEntities>0){
			mBackgroundNode.setTranslate(mCamera.mViewTransform.m03<<mRenderShiftBits,mCamera.mViewTransform.m13<<mRenderShiftBits,mCamera.mViewTransform.m23<<mRenderShiftBits);
		}

		// We store backups of the viewport parameters to check because we allow the null viewport above to equal a clip sized viewport
		if(	mLastProjectionType!=mCamera.mProjectionType ||
			mLastFov!=mCamera.mFov || mLastAspect!=mCamera.mAspect ||
			mLastNear!=mCamera.mNear || mLastFar!=mCamera.mFar ||
			mLastViewportX!=viewportX || mLastViewportY!=viewportY || mLastViewportWidth!=viewportWidth || mLastViewportHeight!=viewportHeight ||
			mLastCanvasWidth!=canvasWidth || mLastCanvasHeight!=canvasHeight
		){
			mLastProjectionType=mCamera.mProjectionType;
			mLastFov=mCamera.mFov;
			mLastAspect=mCamera.mAspect;
			mLastNear=mCamera.mNear;
			mLastFar=mCamera.mFar;
			mLastViewportX=viewportX;
			mLastViewportY=viewportY;
			mLastViewportWidth=viewportWidth;
			mLastViewportHeight=viewportHeight;
			mLastCanvasWidth=canvasWidth;
			mLastCanvasHeight=canvasHeight;

			updateProjectionParameters();
		}

		//#ifdef TOADLET_PLATFORM_SE
			if(mEngine.mSonyEricssonJP3){
				if((clipHeight-viewportY)<(viewportY+viewportHeight) || (clipWidth-viewportX)<(viewportX+viewportWidth)){
					//#ifdef TOADLET_DEBUG
						if(mWarnViewport==false){
							mWarnViewport=true;
							Logger.getInstance().addLogString(Logger.LEVEL_WARNING,
								"Problematic viewport for SonyEricsson JP3.  (clipHeight-viewportY) must be greater than (viewportY+viewportHeight)");
						}
					//#endif
				}
				else{
					g.setClip(viewportX,viewportY,viewportX+viewportWidth+1,viewportY+viewportHeight+1);
				}
			}
			else{
				g.setClip(viewportX,viewportY,viewportWidth,viewportHeight);
			}
		//#elifdef TOADLET_PLATFORM_SPRINT
			if(mEngine.mEmulator==false){
				if(clipX!=viewportX || clipY!=viewportY || clipWidth!=viewportWidth || clipHeight!=viewportHeight){
					//#ifdef TOADLET_DEBUG
						if(mWarnViewport==false){
							mWarnViewport=true;
							Logger.getInstance().addLogString(Logger.LEVEL_WARNING,
								"Problematic viewport for a Sprint device.  Bottom & right will be clipped if on a GameCanvas, otherwise no clipping will occur");
						}
					//#endif
				}
				g.setClip(viewportX,viewportY,viewportX+viewportWidth,viewportY+viewportHeight);
			}
			else{
				g.setClip(viewportX,viewportY,viewportWidth,viewportHeight);
			}
		//#endif
		g.translate(-translateX,-translateY);
		mHWidth=viewportX + viewportWidth/2;
		mHHeight=viewportY + viewportHeight/2;
		mRenderingScope=mCamera.getScope();
		
		// Update light direction
		if(mLight!=null){
			AffineTrans c=camera.mViewTransform;

			Vector3D result=c.transform(mLight.mDirectionD);
			result.x-=c.m03;
			result.y-=c.m13;
			result.z-=c.m23;
			mLight.mLight.setParallelLightDirection(result);
		}

		render(this);

		try{
			mGraphics3D.bind(g);
			renderRenderables();
		}
		catch(Exception ex){
			//#ifdef TOADLET_DEBUG
				Logger.getInstance().addLogString(Logger.LEVEL_WARNING,"SceneManager.render:"+ex.toString());
			//#endif
			int i;
			for(i=0;i<RenderableEntity.MAX_LAYERS;++i){
				mLayersSize[i]=0;
			}
		}
		finally{
			mGraphics3D.release(g);
		}

		mCamera=null;

		g.translate(translateX,translateY);
		g.setClip(clipX,clipY,clipWidth,clipHeight);

		mGraphics3D=null;
		
		//#ifdef TOADLET_DEBUG
			Profile.endSection(Profile.SECTION_RENDER);
		//#endif
	}

	void updateWorldTransform(Entity entity){
		if(entity.mParent==null){
			// For mascot we assume the root node always remains at the origin
			entity.mWorldTransform.setIdentity();
		}
		else{
			updateWorldTransform(entity.mParent);

			// AffineTransform has no set(AffineTransform) method for mIdentityTransform
			entity.mWorldTransform.mul(entity.mParent.mWorldTransform,entity.mTransform);
		}
	}

	// We can't cache any of these transforms, since the viewpoint is constantly changing, and they all depend
	// On the root transformation being the cameras transform
	protected final void render(Entity entity){
		if((entity.mScope&mRenderingScope)==0){
			return;
		}

		// Hack to cull entities attached to the scene manager
		// Ideally this should be performed at each entity we traverse, but that would involve transforming the bound back to the camera
		if(entity.mParent==this && entity.mBoundingRadius>=0){
			mCacheDiff.x=mCamera.mBoundingOrigin.x-entity.mTranslate.x;
			mCacheDiff.y=mCamera.mBoundingOrigin.y-entity.mTranslate.y;
			mCacheDiff.z=mCamera.mBoundingOrigin.z-entity.mTranslate.z;
			int sum=mCamera.mBoundingRadius+entity.mBoundingRadius;
			if(
				//#exec fpcomp.bat (mCacheDiff.x*mCacheDiff.x+mCacheDiff.y*mCacheDiff.y+mCacheDiff.z*mCacheDiff.z)
				>
				//#exec fpcomp.bat (sum*sum)
				){
				return;
			}
		}

		if((entity.mType&TYPE_NODE)>0){
			NodeEntity node=(NodeEntity)entity;

			if(node.mParent==null){
				node.mWorldTransform.mul(mCamera.mViewTransform,node.mTransform);
			}
			else{
				node.mWorldTransform.mul(node.mParent.mWorldTransform,node.mTransform);
			}

			int i;
			for(i=node.mNumEntities-1;i>=0;--i){
				render(node.mEntities[i]);
			}
		}
		else if((entity.mType&TYPE_RENDERABLE)>0){
			RenderableEntity renderable=(RenderableEntity)entity;

			if(renderable.mVisible==true){
				if(renderable.mWorldSpace){
					renderable.mLayout.setAffineTrans(mCamera.mViewTransform);
				}
				else if(renderable.mIdentityTransform){
					renderable.mLayout.setAffineTrans(entity.mParent.mWorldTransform);
				}
				else{
					entity.mWorldTransform.mul(entity.mParent.mWorldTransform,entity.mTransform);
					renderable.mLayout.setAffineTrans(entity.mWorldTransform);
				}
				mLayers[renderable.mLayer][mLayersSize[renderable.mLayer]++]=renderable;
			}
		}
	}

	public final void renderRenderables(){
		int i,j;

		RenderableEntity renderable=null;
		for(i=0;i<RenderableEntity.MAX_LAYERS;++i){
			for(j=mLayersSize[i]-1;j>=0;--j){
				renderable=mLayers[i][j];
				mLayers[i][j]=null;

				renderable.render(mGraphics3D,mHWidth,mHHeight);
			}

			if(mLayersSize[i]>0){
				mGraphics3D.flush();
			}

			mLayersSize[i]=0;
		}
	}

	final void registerSpriteEntity(SpriteEntity entity){
		int i;
		for(i=0;i<mNumSpriteEntities;++i){
			if(mSpriteEntities[i]==entity)break;
		}

		if(i==mNumSpriteEntities){
			if(mSpriteEntities.length<mNumSpriteEntities+1){
				SpriteEntity[] entities=new SpriteEntity[mNumSpriteEntities+1];
				System.arraycopy(mSpriteEntities,0,entities,0,mSpriteEntities.length);
				mSpriteEntities=entities;
			}

			mSpriteEntities[mNumSpriteEntities]=entity;
			mNumSpriteEntities++;
		}
	}

	final void unregisterSpriteEntity(SpriteEntity entity){
		int i;
		for(i=0;i<mNumSpriteEntities;++i){
			if(mSpriteEntities[i]==entity)break;
		}

		if(i!=mNumSpriteEntities){
			mSpriteEntities[i]=null;
			System.arraycopy(mSpriteEntities,i+1,mSpriteEntities,i,mNumSpriteEntities-(i+1));
			mNumSpriteEntities--;
			mSpriteEntities[mNumSpriteEntities]=null;
		}
	}

	final void registerAnimatedMeshEntity(MeshEntity entity){
		int i;
		for(i=0;i<mNumAnimatedMeshEntities;++i){
			if(mAnimatedMeshEntities[i]==entity)break;
		}

		if(i==mNumAnimatedMeshEntities){
			if(mAnimatedMeshEntities.length<mNumAnimatedMeshEntities+1){
				MeshEntity[] entities=new MeshEntity[mNumAnimatedMeshEntities+1];
				System.arraycopy(mAnimatedMeshEntities,0,entities,0,mAnimatedMeshEntities.length);
				mAnimatedMeshEntities=entities;
			}

			mAnimatedMeshEntities[mNumAnimatedMeshEntities]=entity;
			mNumAnimatedMeshEntities++;
		}
	}

	final void unregisterAnimatedMeshEntity(MeshEntity entity){
		int i;
		for(i=0;i<mNumAnimatedMeshEntities;++i){
			if(mAnimatedMeshEntities[i]==entity)break;
		}

		if(i!=mNumAnimatedMeshEntities){
			mAnimatedMeshEntities[i]=null;
			System.arraycopy(mAnimatedMeshEntities,i+1,mAnimatedMeshEntities,i,mNumAnimatedMeshEntities-(i+1));
			mNumAnimatedMeshEntities--;
			mAnimatedMeshEntities[mNumAnimatedMeshEntities]=null;
		}
	}

	final void registerLightEntity(LightEntity light){
		if(mLight!=null){
			throw new RuntimeException("Currently limited to one light!");
		}
		else{
			mLight=light;
			mLight.mLight.setAmbientIntensity(mAmbientIntensity>>4);

			int i;
			for(i=0;i<mNumRegisteredEntities;++i){
				Entity entity=mRegisteredEntities[i];

				if((entity.getType()&Entity.TYPE_RENDERABLE)>0){
					RenderableEntity renderable=(RenderableEntity)entity;
					if(renderable.mLit){
						renderable.mEffect.setLight(mLight.mLight);
					}
				}
			}
		}
	}

	final void unregisterLightEntity(LightEntity light){
		if(mLight==light){
			mLight=null;

			int i;
			for(i=0;i<mNumRegisteredEntities;++i){
				Entity entity=mRegisteredEntities[i];

				if((entity.getType()&Entity.TYPE_RENDERABLE)>0){
					RenderableEntity renderable=(RenderableEntity)entity;
					if(renderable.mLit){
						renderable.mEffect.setLight(null);
					}
				}
			}
		}
	}

	int mTime=0;

	RenderableEntity[][] mLayers;
	int mLayersSize[];
	Graphics3D mGraphics3D;
	int mHWidth,mHHeight;
	CameraEntity mCamera;
	int mRenderShiftBits;
	int mRenderingScope;
	NodeEntity mBackgroundNode;

	RenderableEntity[] mRegisteredEntities=new RenderableEntity[0];
	int mNumRegisteredEntities;

	SpriteEntity[] mSpriteEntities=new SpriteEntity[0];
	int mNumSpriteEntities;

	MeshEntity[] mAnimatedMeshEntities=new MeshEntity[0];
	int mNumAnimatedMeshEntities;

	byte mLastProjectionType;
	int mLastFov;
	int mLastAspect;
	int mLastNear;
	int mLastFar;
	int mLastViewportX;
	int mLastViewportY;
	int mLastViewportWidth;
	int mLastViewportHeight;
	int mLastCanvasWidth;
	int mLastCanvasHeight;

	int mProjNear;
	int mProjFar;
	//#ifdef TOADLET_USE_SPRINT_HACK
		int mProjFov;
	//#else
		int mProjWidth;
		int mProjHeight;
	//#endif

	LightEntity mLight;
	int mAmbientIntensity;

	//#ifdef TOADLET_DEBUG
		boolean mWarnViewport=false;
	//#endif

	Vector3 mCacheDiff=new Vector3();
}

