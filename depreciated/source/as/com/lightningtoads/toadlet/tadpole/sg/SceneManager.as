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

import flash.display.Graphics;
import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.tMath;
import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.tadpole.sg.*;
#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
#endif

public class SceneManager extends NodeEntity{
	public function SceneManager(engine:Engine){
		super(engine);

		mBackgroundNode=new NodeEntity(engine);
		attach(mBackgroundNode);

		mLayers=new Array();
		mLayersSize=new Array();
		var i:int;
		for(i=0;i<RenderableEntity.MAX_LAYERS;++i){
			mLayers[i]=new Array();
			mLayersSize[i]=0;
		}
	}

	public function getBackgroundNode():NodeEntity{
		return mBackgroundNode;
	}

	public function update(dt:int):void{
		#ifdef TOADLET_DEBUG
			if(mDestroyed){
				Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"update called on a destroyed SceneManager");
				throw new Error("update called on a destroyed SceneManager");
			}
		#endif

		mTime+=dt;

		var i:int;
		for(i=0;i<mAnimatedMeshEntities.length;++i){
			mAnimatedMeshEntities[i].updateAnimation(dt);
		}
		for(i=0;i<mSpriteEntities.length;++i){
			mSpriteEntities[i].updateAnimation(dt);
		}
	}

	public function setTime(time:int):void{
		mTime=time;
	}

	public function getTime():int{
		return mTime;
	}

	public function setAmbientColor(ambient:int):void{
		mAmbientIntensity=(((ambient&0x00FF0000)>>16) + ((ambient&0x0000FF00)>>8) + ((ambient&0x000000FF)>>0)) / 3;
		mAmbientIntensity=(mAmbientIntensity*tMath.ONE)/255;
		mAmbient=ambient;
	}

	public function getAmbientColor():int{
		return mAmbient;
	}

	public function render(g:Graphics,camera:CameraEntity):void{
		#ifdef TOADLET_DEBUG
			if(mDestroyed){
				Logger.getInstance().addLogString(Logger.LEVEL_ERROR,"render called on a destroyed SceneManager");
				throw new Error("render called on a destroyed SceneManager");
			}
		#endif

		var viewportX:int=0;
		var viewportY:int=0;
		var viewportWidth:int=mEngine.mWidth;
		var viewportHeight:int=mEngine.mHeight;

		if(camera.mViewportSet){
			viewportX=camera.getViewportX();
			viewportY=camera.getViewportY();
			viewportWidth=camera.getViewportWidth();
			viewportHeight=camera.getViewportHeight();
		}
		
		mRenderer=mEngine.getRenderer();
		
		mCamera=camera;

		updateWorldTransform(mCamera);

		mCamera.updateViewTransform();

		mRenderingScope=mCamera.getScope();

		mRenderer.setViewportSize(viewportWidth,viewportHeight);
		mRenderer.setProjection(mCamera.mInvHX,mCamera.mInvHY,mCamera.mNear>>mEngine.mRenderShiftBits,mCamera.mFar>>mEngine.mRenderShiftBits);

		if(mLight!=null){
			var m:Matrix4x3=mCacheMatrix;
			m.a00=mCamera.mViewTransform.a00>>2;
			m.a01=mCamera.mViewTransform.a01>>2;
			m.a02=mCamera.mViewTransform.a02>>2;
			m.a10=mCamera.mViewTransform.a10>>2;
			m.a11=mCamera.mViewTransform.a11>>2;
			m.a12=mCamera.mViewTransform.a12>>2;
			m.a20=mCamera.mViewTransform.a20>>2;
			m.a21=mCamera.mViewTransform.a21>>2;
			m.a22=mCamera.mViewTransform.a22>>2;
			var x:int=mLight.getDirection().x>>6;
			var y:int=mLight.getDirection().y>>6;
			var z:int=mLight.getDirection().z>>6;

			// Transform the light from world space to camera space
			// Since the Renderer does everything from camera space
			mCacheVector.x=
				(((m.a00*x)>>8)+((m.a01*y)>>8)+((m.a02*z)>>8))
			;
			mCacheVector.y=
				(((m.a10*x)>>8)+((m.a11*y)>>8)+((m.a12*z)>>8))
			;
			mCacheVector.z=
				(((m.a20*x)>>8)+((m.a21*y)>>8)+((m.a22*z)>>8))
			;

			mRenderer.setLighting(true,mCacheVector,mAmbientIntensity);
		}
		else{
			mRenderer.setLighting(false,tMath.ZERO_VECTOR3,mAmbientIntensity);
		}

		mRenderer.beginScene(g);

		renderE(this);

		renderRenderables(g);

		mRenderer.endScene();

		mCamera=null;

		mRenderer=null;
	}

	protected function updateWorldTransform(entity:Entity):void{
		var parent:NodeEntity=entity.getParent();

		if(parent==null){
			entity.mWorldTransform.setM(entity.mTransform);
		}
		else{
			updateWorldTransform(parent);

			if(entity.mIdentityTransform){
				entity.mWorldTransform.setM(parent.mWorldTransform);
			}
			else{
				Matrix4x3.mulMMM(entity.mWorldTransform,parent.mWorldTransform,entity.mTransform);
			}
		}
	}

	// We can't cache any of these transforms, since the viewpoint is constantly changing, and they all depend
	// On the root transformation being the cameras transform
	protected function renderE(entity:Entity):void{
		if((entity.mScope&mRenderingScope)==0){
			return;
		}

		var parent:NodeEntity=entity.getParent();

		// Hack to cull entities attached to the scene manager
		// Ideally this should be performed at each entity we traverse, but that would involve transforming the bound back to the camera
		if(parent==this && entity.mBoundingRadius>=0){
			mCacheVector.x=(mCamera.mBoundingOrigin.x-entity.mTranslate.x)>>6;
			mCacheVector.y=(mCamera.mBoundingOrigin.y-entity.mTranslate.y)>>6;
			mCacheVector.z=(mCamera.mBoundingOrigin.z-entity.mTranslate.z)>>6;
			var sum:int=(mCamera.mBoundingRadius+entity.mBoundingRadius)>>6;
			if(((mCacheVector.x*mCacheVector.x) + (mCacheVector.y*mCacheVector.y) + (mCacheVector.z*mCacheVector.z))
				>(sum*sum)){
				return;
			}
		}

		var type:int=entity.getType();

		if((type&TYPE_NODE)>0){
			var node:NodeEntity=NodeEntity(entity);

			if(parent==null){
				Matrix4x3.mulMMM(node.mWorldTransform,mCamera.mViewTransform,node.mTransform);
			}
			else if(node.mIdentityTransform){
				node.mWorldTransform.setM(parent.mWorldTransform);
			}
			else{
				Matrix4x3.mulMMM(node.mWorldTransform,parent.mWorldTransform,node.mTransform);
			}

			var i:int;
			for(i=node.mEntities.length-1;i>=0;--i){
				renderE(node.mEntities[i]);
			}
		}
		else if((type&TYPE_MESH)>0){
			var meshEntity:MeshEntity=MeshEntity(entity);

			if(meshEntity.mVisible==true && meshEntity.mMesh!=null){
				mCacheScale.a00=meshEntity.mMesh.worldScale;
				mCacheScale.a11=meshEntity.mMesh.worldScale;
				mCacheScale.a22=meshEntity.mMesh.worldScale;
				if(meshEntity.mIdentityTransform){
					Matrix4x3.mulMMM(meshEntity.mWorldTransform,parent.mWorldTransform,mCacheScale);
				}
				else{
					Matrix4x3.mulMMM(mCacheMatrix,meshEntity.mTransform,mCacheScale);
					Matrix4x3.mulMMM(meshEntity.mWorldTransform,parent.mWorldTransform,mCacheMatrix);
				}

				mLayers[meshEntity.mLayer][mLayersSize[meshEntity.mLayer]++]=meshEntity;
			}
		}
		else if((type&TYPE_SPRITE)>0){
			var spriteEntity:SpriteEntity=SpriteEntity(entity);

			if(spriteEntity.mVisible==true && spriteEntity.mSprite!=null){
				if(spriteEntity.mIdentityTransform){
					spriteEntity.mWorldTransform.setM(parent.mWorldTransform);
				}
				else{
					Matrix4x3.mulMMM(spriteEntity.mWorldTransform,parent.mWorldTransform,spriteEntity.mTransform);
				}

				mLayers[spriteEntity.mLayer][mLayersSize[spriteEntity.mLayer]++]=spriteEntity;
			}
		}
		else if((type&TYPE_BEAM)>0){
			var beamEntity:BeamEntity=BeamEntity(entity);

			if(beamEntity.mVisible==true){
				beamEntity.mWorldTransform.setM(mWorldTransform);

				mLayers[beamEntity.mLayer][mLayersSize[beamEntity.mLayer]++]=beamEntity;
			}
		}
	}

	public function renderRenderables(g:Graphics):void{
		var layer:int,j:int,i:int;

		var renderable:RenderableEntity=null;
		for(layer=0;layer<RenderableEntity.MAX_LAYERS;++layer){
			// First render meshes, then render sprites and beams on top of them
			for(j=mLayersSize[layer]-1;j>=0;--j){
				renderable=mLayers[layer][j];

				if((renderable.getType()&Entity.TYPE_MESH)>0){
					var mesh:MeshEntity=MeshEntity(renderable);
					mRenderer.renderMeshEntity(renderable.mWorldTransform,mesh);
				}
			}
			mRenderer.flush();

			for(j=mLayersSize[layer]-1;j>=0;--j){
				renderable=mLayers[layer][j];
				mLayers[layer][j]=null;

				var type:int=renderable.getType();

				if((type&Entity.TYPE_SPRITE)>0){
					var sprite:SpriteEntity=SpriteEntity(renderable);
					mRenderer.renderSpriteEntity(renderable.mWorldTransform,sprite);
				}
				else if((type&Entity.TYPE_BEAM)>0){
					var beam:BeamEntity=BeamEntity(renderable);
					mRenderer.renderBeamEntity(renderable.mWorldTransform,beam);
				}
			}
			mLayersSize[layer]=0;
		}
	}

	internal function registerAnimatedMeshEntity(entity:MeshEntity):void{
		var i:int;
		for(i=0;i<mAnimatedMeshEntities.length;++i){
			if(mAnimatedMeshEntities[i]==entity)break;
		}

		if(i==mAnimatedMeshEntities.length){
			mAnimatedMeshEntities[mAnimatedMeshEntities.length]=entity;
		}
	}

	internal function unregisterAnimatedMeshEntity(entity:MeshEntity):void{
		var i:int;
		for(i=0;i<mAnimatedMeshEntities.length;++i){
			if(mAnimatedMeshEntities[i]==entity)break;
		}

		if(i!=mAnimatedMeshEntities.length){
			mAnimatedMeshEntities.splice(i,1);
		}
	}

	internal function registerSpriteEntity(entity:SpriteEntity):void{
		var i:int;
		for(i=0;i<mSpriteEntities.length;++i){
			if(mSpriteEntities[i]==entity)break;
		}

		if(i==mSpriteEntities.length){
			mSpriteEntities[mSpriteEntities.length]=entity;
		}
	}

	internal function unregisterSpriteEntity(entity:SpriteEntity):void{
		var i:int;
		for(i=0;i<mSpriteEntities.length;++i){
			if(mSpriteEntities[i]==entity)break;
		}

		if(i!=mSpriteEntities.length){
			mSpriteEntities.splice(i,1);
		}
	}

	internal function registerLightEntity(light:LightEntity):void{
		if(mLight!=null){
			throw new Error("Currently limited to one light!");
		}
		else{
			mLight=light;
		}
	}

	internal function unregisterLightEntity(light:LightEntity):void{
		if(mLight==light){
			mLight=null;
		}
	}

	protected var mTime:int=0;

	protected var mCamera:CameraEntity;
	protected var mRenderer:Renderer;
	protected var mRenderingScope:int;

	protected var mBackgroundNode:NodeEntity;

	protected var mLayers:Array;
	protected var mLayersSize:Array;

	protected var mAnimatedMeshEntities:Array=new Array();
	protected var mSpriteEntities:Array=new Array();

	protected var mAmbient:int;
	protected var mAmbientIntensity:int;
	protected var mLight:LightEntity;

	protected var mModelViewMatrix:Matrix4x3=new Matrix4x3();

	protected var mCacheVector:Vector3=new Vector3();
	protected var mCacheScale:Matrix4x3=new Matrix4x3();
	protected var mCacheMatrix:Matrix4x3=new Matrix4x3();
}

}
