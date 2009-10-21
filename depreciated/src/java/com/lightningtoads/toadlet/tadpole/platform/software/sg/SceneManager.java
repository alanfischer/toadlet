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

//#ifdef TOADLET_PLATFORM_BLACKBERRY
	import net.rim.device.api.ui.*;
//#else
	import javax.microedition.lcdui.*;
//#endif

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.*;
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
	}

	public final NodeEntity getBackgroundNode(){
		return mBackgroundNode;
	}

	public void update(int dt){
		//#ifdef TOADLET_DEBUG
			if(mDestroyed){
				Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"update called on a destroyed SceneManager");
				throw new RuntimeException("update called on a destroyed SceneManager");
			}
		//#endif

		mTime+=dt;

		int i;
		for(i=0;i<mNumAnimatedMeshEntities;++i){
			mAnimatedMeshEntities[i].updateAnimation(dt);
		}
		for(i=0;i<mNumSpriteEntities;++i){
			mSpriteEntities[i].updateAnimation(dt);
		}
	}

	public void setTime(int time){
		mTime=time;
	}

	public int getTime(){
		return mTime;
	}

	public final void setAmbientColor(int ambient){
		mAmbientIntensity=(((ambient&0x00FF0000)>>16) + ((ambient&0x0000FF00)>>8) + ((ambient&0x000000FF)>>0)) / 3;
		mAmbientIntensity=(mAmbientIntensity*Math.ONE)/255;
		mAmbient=ambient;
	}

	public final int getAmbientColor(){
		return mAmbient;
	}

	public final void render(Graphics g,CameraEntity camera){
		//#ifdef TOADLET_DEBUG
			if(mDestroyed){
				Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"render called on a destroyed SceneManager");
				throw new RuntimeException("render called on a destroyed SceneManager");
			}
		//#endif

		//#ifdef TOADLET_DEBUG
			Profile.beginSection(Profile.SECTION_RENDER);
		//#endif

		//#ifdef TOADLET_PLATFORM_BLACKBERRY
			int clipX=g.getTranslateX();
			int clipY=g.getTranslateY();
			int clipWidth=g.getClippingRect().width;
			int clipHeight=g.getClippingRect().height;
		//#else
			int clipX=g.getClipX();
			int clipY=g.getClipY();
			int clipWidth=g.getClipWidth();
			int clipHeight=g.getClipHeight();
		//#endif

		int viewportX=clipX;
		int viewportY=clipY;
		int viewportWidth=clipWidth;
		int viewportHeight=clipHeight;

		if(camera.mViewportSet){
			viewportX=camera.getViewportX();
			viewportY=camera.getViewportY();
			viewportWidth=camera.getViewportWidth();
			viewportHeight=camera.getViewportHeight();
		}

		mRenderer=mEngine.getRenderer();

		//#ifdef TOADLET_PLATFORM_BLACKBERRY
			g.pushRegion(viewportX-clipX,viewportY-clipY,viewportWidth,viewportHeight,0,0);
		//#else
			g.setClip(viewportX,viewportY,viewportWidth,viewportHeight);
			g.translate(viewportX,viewportY);
		//#endif

		mCamera=camera;

		updateWorldTransform(mCamera);

		mCamera.updateViewTransform();

		if(mBackgroundNode.mNumEntities>0){
			mBackgroundNode.setTranslate(mCamera.mViewTransform.at(0,3)<<mEngine.mRenderShiftBits,mCamera.mViewTransform.at(1,3)<<mEngine.mRenderShiftBits,mCamera.mViewTransform.at(2,3)<<mEngine.mRenderShiftBits);
		}

		mRenderingScope=mCamera.getScope();

		mRenderer.setViewportSize(viewportWidth,viewportHeight);
		mRenderer.setProjection(mCamera.mInvHX,mCamera.mInvHY,mCamera.mNear>>mEngine.mRenderShiftBits,mCamera.mFar>>mEngine.mRenderShiftBits);

		if(mLight!=null){
			Matrix4x3 m=mCacheMatrix;
			m.data[0+0*3]=mCamera.mViewTransform.data[0+0*3]>>2;
			m.data[0+1*3]=mCamera.mViewTransform.data[0+1*3]>>2;
			m.data[0+2*3]=mCamera.mViewTransform.data[0+2*3]>>2;
			m.data[1+0*3]=mCamera.mViewTransform.data[1+0*3]>>2;
			m.data[1+1*3]=mCamera.mViewTransform.data[1+1*3]>>2;
			m.data[1+2*3]=mCamera.mViewTransform.data[1+2*3]>>2;
			m.data[2+0*3]=mCamera.mViewTransform.data[2+0*3]>>2;
			m.data[2+1*3]=mCamera.mViewTransform.data[2+1*3]>>2;
			m.data[2+2*3]=mCamera.mViewTransform.data[2+2*3]>>2;
			int x=mLight.getDirection().x>>6;
			int y=mLight.getDirection().y>>6;
			int z=mLight.getDirection().z>>6;

			// Transform the light from world space to camera space
			// Since the Renderer does everything from camera space
			mCacheVector.x=
				(((m.data[0+0*3]*x)>>8)+((m.data[0+1*3]*y)>>8)+((m.data[0+2*3]*z)>>8))
			;
			mCacheVector.y=
				(((m.data[1+0*3]*x)>>8)+((m.data[1+1*3]*y)>>8)+((m.data[1+2*3]*z)>>8))
			;
			mCacheVector.z=
				(((m.data[2+0*3]*x)>>8)+((m.data[2+1*3]*y)>>8)+((m.data[2+2*3]*z)>>8))
			;

			mRenderer.setLighting(true,mCacheVector,mAmbientIntensity);
		}
		else{
			mRenderer.setLighting(false,Math.ZERO_VECTOR3,mAmbientIntensity);
		}

		mRenderer.beginScene(g);

		render(this);

		renderRenderables(g);

		mRenderer.endScene();

		mCamera=null;

		//#ifdef TOADLET_PLATFORM_BLACKBERRY
			g.popContext();
		//#else
			g.translate(-viewportX,-viewportY);
			g.setClip(clipX,clipY,clipWidth,clipHeight);
		//#endif

		mRenderer=null;

		//#ifdef TOADLET_DEBUG
			Profile.endSection(Profile.SECTION_RENDER);
		//#endif
	}

	void updateWorldTransform(Entity entity){
		if(entity.mParent==null){
			entity.mWorldTransform.set(entity.mTransform);
		}
		else{
			updateWorldTransform(entity.mParent);

			if(entity.mIdentityTransform){
				entity.mWorldTransform.set(entity.mParent.mWorldTransform);
			}
			else{
				Matrix4x3.mul(entity.mWorldTransform,entity.mParent.mWorldTransform,entity.mTransform);
			}
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
			mCacheVector.x=mCamera.mBoundingOrigin.x-entity.mTranslate.x;
			mCacheVector.y=mCamera.mBoundingOrigin.y-entity.mTranslate.y;
			mCacheVector.z=mCamera.mBoundingOrigin.z-entity.mTranslate.z;
			int sum=mCamera.mBoundingRadius+entity.mBoundingRadius;
			if(
				//#exec fpcomp.bat (mCacheVector.x*mCacheVector.x+mCacheVector.y*mCacheVector.y+mCacheVector.z*mCacheVector.z)
				>
				//#exec fpcomp.bat (sum*sum)
				){
				return;
			}
		}

		if((entity.mType&TYPE_NODE)>0){
			NodeEntity node=(NodeEntity)entity;

			if(node.mParent==null){
				Matrix4x3.mul(node.mWorldTransform,mCamera.mViewTransform,node.mTransform);
			}
			else if(node.mIdentityTransform){
				node.mWorldTransform.set(node.mParent.mWorldTransform);
			}
			else{
				Matrix4x3.mul(node.mWorldTransform,node.mParent.mWorldTransform,node.mTransform);
			}

			int i;
			for(i=node.mNumEntities-1;i>=0;--i){
				render(node.mEntities[i]);
			}
		}
		else if((entity.mType&TYPE_MESH)>0){
			MeshEntity meshEntity=(MeshEntity)entity;

			if(meshEntity.mVisible==true && meshEntity.mMesh!=null){
				mCacheScale.setAt(0,0,meshEntity.mMesh.worldScale);
				mCacheScale.setAt(1,1,meshEntity.mMesh.worldScale);
				mCacheScale.setAt(2,2,meshEntity.mMesh.worldScale);
				if(meshEntity.mIdentityTransform){
					Matrix4x3.mul(meshEntity.mWorldTransform,meshEntity.mParent.mWorldTransform,mCacheScale);
				}
				else{
					Matrix4x3.mul(mCacheMatrix,meshEntity.mTransform,mCacheScale);
					Matrix4x3.mul(meshEntity.mWorldTransform,meshEntity.mParent.mWorldTransform,mCacheMatrix);
				}

				mLayers[meshEntity.mLayer][mLayersSize[meshEntity.mLayer]++]=meshEntity;
			}
		}
		else if((entity.mType&TYPE_SPRITE)>0){
			SpriteEntity spriteEntity=(SpriteEntity)entity;

			if(spriteEntity.mVisible==true && spriteEntity.mSprite!=null){
				if(spriteEntity.mIdentityTransform){
					spriteEntity.mWorldTransform.set(spriteEntity.mParent.mWorldTransform);
				}
				else{
					Matrix4x3.mul(spriteEntity.mWorldTransform,spriteEntity.mParent.mWorldTransform,spriteEntity.mTransform);
				}

				mLayers[spriteEntity.mLayer][mLayersSize[spriteEntity.mLayer]++]=spriteEntity;
			}
		}
		else if((entity.mType&TYPE_BEAM)>0){
			BeamEntity beamEntity=(BeamEntity)entity;

			if(beamEntity.mVisible==true){
				beamEntity.mWorldTransform.set(mWorldTransform);

				mLayers[beamEntity.mLayer][mLayersSize[beamEntity.mLayer]++]=beamEntity;
			}
		}
	}

	public final void renderRenderables(Graphics g){
		int layer,j,i;

		RenderableEntity renderable=null;
		for(layer=0;layer<RenderableEntity.MAX_LAYERS;++layer){
			// First render meshes, then render sprites and beams on top of them
			for(j=mLayersSize[layer]-1;j>=0;--j){
				renderable=mLayers[layer][j];
				if((renderable.mType&Entity.TYPE_MESH)>0){
					MeshEntity mesh=(MeshEntity)renderable;
					mRenderer.renderMeshEntity(renderable.mWorldTransform,mesh);
				}
			}
			mRenderer.flush();

			for(j=mLayersSize[layer]-1;j>=0;--j){
				renderable=mLayers[layer][j];
				mLayers[layer][j]=null;

				if((renderable.mType&Entity.TYPE_SPRITE)>0){
					SpriteEntity sprite=(SpriteEntity)renderable;
					mRenderer.renderSpriteEntity(renderable.mWorldTransform,sprite);
				}
				else if((renderable.mType&Entity.TYPE_BEAM)>0){
					BeamEntity beam=(BeamEntity)renderable;
					mRenderer.renderBeamEntity(renderable.mWorldTransform,beam);
				}
			}
			mLayersSize[layer]=0;
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

	final void registerLightEntity(LightEntity light){
		if(mLight!=null){
			throw new RuntimeException("Currently limited to one light!");
		}
		else{
			mLight=light;
		}
	}

	final void unregisterLightEntity(LightEntity light){
		if(mLight==light){
			mLight=null;
		}
	}

	int mTime=0;

	CameraEntity mCamera;
	Renderer mRenderer;
	int mRenderingScope;

	NodeEntity mBackgroundNode;

	RenderableEntity[][] mLayers;
	int mLayersSize[];

	MeshEntity[] mAnimatedMeshEntities=new MeshEntity[0];
	int mNumAnimatedMeshEntities;
	SpriteEntity[] mSpriteEntities=new SpriteEntity[0];
	int mNumSpriteEntities;

	int mAmbient;
	int mAmbientIntensity;
	LightEntity mLight;

	Matrix4x3 mModelViewMatrix=new Matrix4x3();

	Vector3 mCacheVector=new Vector3();
	Matrix4x3 mCacheScale=new Matrix4x3();
	Matrix4x3 mCacheMatrix=new Matrix4x3();
}
