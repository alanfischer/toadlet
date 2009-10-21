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
import com.motorola.iden.micro3d.*;
//#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
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

		mProjParameters[0]=256;
		mProjParameters[1]=4096;
		mProjParameters[2]=engine.mWidth<<12;
		mProjParameters[3]=engine.mHeight<<12;
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

			entity.mLayout.setProjection(mProjType,mProjParameters);
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

		mProjParameters[0]=mLastNear>>mRenderShiftBits;
		mProjParameters[1]=mLastFar>>mRenderShiftBits;
		mProjParameters[2]=(px>>mRenderShiftBits)<<12;
		mProjParameters[3]=(py>>mRenderShiftBits)<<12;

		int i;
		for(i=0;i<mNumRegisteredEntities;++i){
			RenderableEntity entity=mRegisteredEntities[i];
			entity.mLayout.setProjection(mProjType,mProjParameters);
		}
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
					mesh.mFigure.setPosture(mesh.mAnimation,mesh.mAnimationTime);
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
				Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"render called on a destroyed SceneManager");
				throw new RuntimeException("render called on a destroyed SceneManager");
			}
		//#endif

		//#ifdef TOADLET_DEBUG
			Profile.beginSection(Profile.SECTION_RENDER);
		//#endif

		mRenderer=mEngine.getRenderer();

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
			mBackgroundNode.setTranslate(mCamera.mViewTransform.get(AffineTransform.M03)<<mRenderShiftBits,mCamera.mViewTransform.get(AffineTransform.M13)<<mRenderShiftBits,mCamera.mViewTransform.get(AffineTransform.M23)<<mRenderShiftBits);
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
		g.setClip(viewportX,viewportY,viewportWidth-1,viewportHeight-1);
		g.translate(-translateX,-translateY);
		mHWidth=(viewportWidth-1)/2 + viewportX;
		mHHeight=(viewportHeight-1)/2 + viewportY;
		mRenderingScope=mCamera.getScope();

		// Update light parameters
		if(mLight!=null){
			AffineTransform c=camera.mViewTransform;

			Vector3D result=c.transformPoint(mLight.mDirectionD);
			result.set(
				result.getX()-c.get(AffineTransform.M03),
				result.getY()-c.get(AffineTransform.M13),
				result.getZ()-c.get(AffineTransform.M23));
			mLight.mLight.setDirectionVector(result);
		}

		render(this);

		try{
			renderRenderables(g);
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

		mCamera=null;

		g.translate(translateX,translateY);
		g.setClip(clipX,clipY,clipWidth,clipHeight);

		mRenderer=null;
		
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
			AffineTransform.multiply(entity.mWorldTransform,entity.mParent.mWorldTransform,entity.mTransform);
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
				AffineTransform.multiply(node.mWorldTransform,mCamera.mViewTransform,node.mTransform);
			}
			// AffineTransform has no set(AffineTransform) method for mIdentityTransform
			else{
				AffineTransform.multiply(node.mWorldTransform,node.mParent.mWorldTransform,node.mTransform);
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
					renderable.mLayout.setViewPointTransform(mCamera.mViewTransform);
				}
				else if(renderable.mIdentityTransform){
					renderable.mLayout.setViewPointTransform(entity.mParent.mWorldTransform);
				}
				else{
					AffineTransform.multiply(entity.mWorldTransform,entity.mParent.mWorldTransform,entity.mTransform);
					renderable.mLayout.setViewPointTransform(entity.mWorldTransform);
				}
				mLayers[renderable.mLayer][mLayersSize[renderable.mLayer]++]=renderable;
			}
		}
	}

	public final void renderRenderables(Graphics g){
		int i,j;

		RenderableEntity renderable=null;
		for(i=0;i<RenderableEntity.MAX_LAYERS;++i){
			for(j=mLayersSize[i]-1;j>=0;--j){
				renderable=mLayers[i][j];
				mLayers[i][j]=null;
				renderable.mObject3D.setLayout(renderable.mLayout);
				if(renderable.mTexture!=null){
					renderable.mObject3D.setTexture(renderable.mTexture);
				}
				else if(renderable.mTextures!=null){
					renderable.mObject3D.setTexture(renderable.mTextures);
				}
				// Dont set it to null otherwise our sprites will get hosed

				mRenderer.draw(renderable.mObject3D,mHWidth,mHHeight);
			}

			if(mLayersSize[i]>0){
				mRenderer.paint(g);
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
						renderable.mLayout.setLight(mLight.mLight);
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
						renderable.mLayout.setLight(null);
					}
				}
			}
		}
	}

	int mTime=0;

	RenderableEntity[][] mLayers;
	int mLayersSize[];
	Renderer mRenderer;
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

	int mProjType=Layout3D.PERSPECTIVE_WIDTH_HEIGHT;
	int[] mProjParameters=new int[4];

	LightEntity mLight;
	int mAmbientIntensity;

	Vector3 mCacheDiff=new Vector3();
}
