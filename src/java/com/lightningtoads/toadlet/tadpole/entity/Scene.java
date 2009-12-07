/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

package com.lightningtoads.toadlet.tadpole.entity;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.egg.*;
import com.lightningtoads.toadlet.egg.Error;
import com.lightningtoads.toadlet.peeper.*;
import com.lightningtoads.toadlet.tadpole.*;
import java.util.Vector;

public class Scene extends ParentEntity{
	public Scene(){super();}

	public Entity create(Engine engine){
		super.create(engine);

		if(mManaged){
			Error.unknown("Scene must be unmanaged until I clean it up");
			return this;
		}

		mBackgroundEntity=new ParentEntity();
		mBackgroundEntity.create(engine);
		attach(mBackgroundEntity);
		
		return this;
	}
	
	public void destroy(){
		if(mBackgroundEntity!=null){
			super.remove(mBackgroundEntity);
			mBackgroundEntity.destroy();
			mBackgroundEntity=null;
		}

		super.destroy();
	}

	public ParentEntity getBackgroundEntity(){return mBackgroundEntity;}

	public void setAmbientColor(Color ambientColor){mAmbientColor=ambientColor;}
	public Color getAmbientColor(){return mAmbientColor;}

	public void setLayerClearing(int layer,boolean clear){getRenderLayer(layer).clearLayer=clear;}
	public boolean getLayerClearing(int layer){return getRenderLayer(layer).clearLayer;}

	public void setIdealParticleViewTransform(Matrix4x4 transform){mIdealParticleViewTransform.set(transform);}
	public Matrix4x4 getIdealParticleViewTransform(){return mIdealParticleViewTransform;}
	
	public void setExcessiveDT(int dt){mExcessiveDT=dt;}
	public int getExcessiveDT(){return mExcessiveDT;}
	public void setLogicDT(int dt){
		#if defined(TOADLET_DEBUG)
			if(dt<=0){
				Error.unknown(Categories.TOADLET_TADPOLE,
					"setLogicDT must be > 0");
				return;
			}
		#endif

		mLogicDT=dt;
	}
	public int getLogicDT(){return mLogicDT;}
	public void setLogicTimeAndFrame(int time,int frame){
		mLogicTime=time;
		mLogicFrame=frame;
		mAccumulatedDT=0;
		mVisualFrame=0;

		resetModifiedFrames(this);
	}
	public int getLogicTime(){return mLogicTime;}
	public int getLogicFrame(){return mLogicFrame;}
	public int getVisualTime(){return mLogicTime+mAccumulatedDT;}
	public int getVisualFrame(){return mVisualFrame;}

	public void update(int dt){
		#if defined(TOADLET_DEBUG)
			if(destroyed()){
				Error.unknown(Categories.TOADLET_TADPOLE,
					"update called on a destroyed Scene");
				return;
			}
		#endif

		if(mExcessiveDT>0 && dt>mExcessiveDT){
			Logger.alert(Categories.TOADLET_TADPOLE,
				"skipping excessive dt:"+dt);
			return;
		}

		mAccumulatedDT+=dt;

		if(mAccumulatedDT>=mLogicDT){
			preLogicUpdateLoop(dt);

			while(mAccumulatedDT>=mLogicDT){
				mAccumulatedDT-=mLogicDT;

				if(mUpdateListener!=null){
					mUpdateListener.logicUpdate(mLogicDT);
				}
				else{
					logicUpdate(mLogicDT);
				}
			}

			postLogicUpdateLoop(dt);
		}

		if(mUpdateListener!=null){
			mUpdateListener.visualUpdate(dt);
		}
		else{
			visualUpdate(dt);
		}

		/*
		AudioPlayer audioPlayer=mEngine.getAudioPlayer();
		if(audioPlayer!=null){
			audioPlayer.update(dt);
		}
		*/
	}

	public void preLogicUpdateLoop(int dt){}
	public void logicUpdate(int dt){
		mLogicTime+=dt;
		mLogicFrame++;

		logicUpdate(this,dt);
	}
	public void postLogicUpdateLoop(int dt){}
	public void visualUpdate(int dt){
		mVisualFrame++;

		mLight=null;
		visualUpdate(this,dt);
	}

	public void render(Renderer renderer,CameraEntity camera){
		#if defined(TOADLET_DEBUG)
			if(destroyed()){
				Error.unknown(Categories.TOADLET_TADPOLE,
					"render called on a destroyed Scene");
			}
		#endif

		int i,j;

		mCamera=camera;

		mCamera.updateFramesPerSecond();
		mCamera.updateViewTransform();

		if(mBackgroundEntity.getNumChildren()>0){
			Math.setMatrix4x4FromTranslate(mBackgroundEntity.mVisualWorldTransform,mCamera.getVisualWorldTranslate());
		}

		if(mCamera.getViewportSet()){
			renderer.setViewport(mCamera.getViewport());
		}
		else{
			RenderTarget renderTarget=renderer.getRenderTarget();
			renderer.setViewport(cache_render_viewport.set(0,0,renderTarget.getWidth(),renderTarget.getHeight()));
		}

		int clearFlags=mCamera.getClearFlags();
		if(clearFlags>0 && !mCamera.getSkipFirstClear()){
			renderer.setDepthWrite(true);
			renderer.clear(clearFlags,mCamera.getClearColor());
		}

		renderer.setProjectionMatrix(mCamera.mProjectionTransform);

		renderer.setViewMatrix(mCamera.mViewTransform);

		renderer.setModelMatrix(Math.IDENTITY_MATRIX4X4);

		if(mLight!=null){
			renderer.setLight(0,mLight.internal_getLight());
			renderer.setLightEnabled(0,true);
		}
		else{
			renderer.setLightEnabled(0,false);
		}

		renderer.setAmbientColor(mAmbientColor);

		mCamera.mNumCulledEntities=0;

		queueRenderables(this);

		boolean renderedLayer=false;
		for(i=0;i<mRenderLayers.length;++i){
			int layerNum=i+Material.MIN_LAYER;
			RenderLayer layer=mRenderLayers[i];
			if(layer==null || (layer.numRenderables==0 && layer.forceRender==false)){
				continue;
			}

			if(renderedLayer==true){
				if(layer.clearLayer && (clearFlags&Renderer.ClearFlag.DEPTH)>0){
					renderer.clear(Renderer.ClearFlag.DEPTH,Colors.BLACK);
				}
			}
			else{
				renderedLayer=true;
			}

			preLayerRender(renderer,layerNum);

			for(j=0;j<layer.numRenderables;++j){
				Renderable renderable=layer.renderables[j];
				Material material=renderable.getRenderMaterial();
				if(material!=null && mPreviousMaterial!=material){
					material.setupRenderer(renderer,mPreviousMaterial);
					mPreviousMaterial=material;
				}
				renderer.setModelMatrix(renderable.getRenderTransform());

				renderable.render(renderer);
			}
			layer.numRenderables=0;
			mPreviousMaterial=null;

			postLayerRender(renderer,layerNum);

			// This is necessary for some hardware accelerated devices to depth buffer clear properly
			renderer.setDepthWrite(true);
		}

		mCamera=null;
	}

	public void queueRenderable(Renderable renderable){
		Material material=renderable.getRenderMaterial();
		int layer=(material==null)?0:material.getLayer();
		RenderLayer renderLayer=getRenderLayer(layer);
		if(renderLayer.renderables.length<renderLayer.numRenderables+1){
			Renderable[] renderables=new Renderable[renderLayer.numRenderables+1];
			System.arraycopy(renderLayer.renderables,0,renderables,0,renderLayer.renderables.length);
			renderLayer.renderables=renderables;
		}
		renderLayer.renderables[renderLayer.numRenderables++]=renderable;
	}

	public CameraEntity getCamera(){return mCamera;} // Only valid during rendering operations

	public void setUpdateListener(UpdateListener updateListener){mUpdateListener=updateListener;}
	public UpdateListener getUpdateListener(){return mUpdateListener;}

	public ParticleEntity.ParticleSimulator newParticleSimulator(ParticleEntity particleEntity){return null;}

	public boolean remove(Entity entity){
		boolean result=super.remove(entity);

		if(entity==mBackgroundEntity){
			Error.unknown(Categories.TOADLET_TADPOLE,
				"can not remove background entity");
			return false;
		}

		return result;
	}

	protected final class RenderLayer{
		public RenderLayer(){}

		public Renderable[] renderables=new Renderable[0];
		public int numRenderables=0;
		public boolean forceRender=false;
		public boolean clearLayer=false;
	};

	protected final RenderLayer getRenderLayer(int layer){
		layer-=Material.MIN_LAYER;
		if(mRenderLayers.length<=layer){
			RenderLayer[] renderLayers=new RenderLayer[layer+1];
			System.arraycopy(mRenderLayers,0,renderLayers,0,mRenderLayers.length);
			mRenderLayers=renderLayers;
		}

		RenderLayer renderLayer=mRenderLayers[layer];
		if(renderLayer==null){
			renderLayer=new RenderLayer();
			mRenderLayers[layer]=renderLayer;
		}
		return renderLayer;
	}

	protected void resetModifiedFrames(Entity entity){
		entity.mModifiedLogicFrame=-1;
		entity.mModifiedVisualFrame=-1;
		entity.mWorldModifiedLogicFrame=-1;
		entity.mWorldModifiedVisualFrame=-1;

		if(entity.isParent()){
			ParentEntity parent=(ParentEntity)entity;
			int i;
			for(i=0;i<parent.mNumChildren;++i){
				resetModifiedFrames(parent.mChildren[i]);
			}
		}
	}

	protected void logicUpdate(Entity entity,int dt){
		if(entity.mReceiveUpdates){
			entity.logicUpdate(dt);
		}

		entity.mWorldModifiedLogicFrame=entity.mModifiedLogicFrame;

		if(entity.isParent()){
			ParentEntity parent=(ParentEntity)entity;
			if(parent.mShadowChildrenDirty){
				parent.updateShadowChildren();
			}

			Entity child;
			int i;
			for(i=0;i<parent.mNumShadowChildren;++i){
				child=parent.mShadowChildren[i];

				logicUpdate(child,dt);

				parent.mWorldModifiedLogicFrame=parent.mWorldModifiedLogicFrame>child.mWorldModifiedLogicFrame?parent.mWorldModifiedLogicFrame:child.mWorldModifiedLogicFrame;
			}
		}
	}
	
	protected void visualUpdate(Entity entity,int dt){
		if(entity.mReceiveUpdates){
			entity.visualUpdate(dt);
		}

		if(entity.mParent==null){
			entity.mVisualWorldTransform.set(entity.mVisualTransform);
		}
		else if(entity.mIdentityTransform){
			entity.mVisualWorldTransform.set(entity.mParent.mVisualWorldTransform);
		}
		else{
			Math.mul(entity.mVisualWorldTransform,entity.mParent.mVisualWorldTransform,entity.mVisualTransform);
		}

		entity.mWorldModifiedVisualFrame=entity.mModifiedVisualFrame;

		if(entity.isParent()){
			ParentEntity parent=(ParentEntity)entity;
			if(parent.mShadowChildrenDirty){
				parent.updateShadowChildren();
			}

			Entity child;
			int i;
			for(i=0;i<parent.mNumShadowChildren;++i){
				child=parent.mShadowChildren[i];

				visualUpdate(child,dt);

				if(parent.mVisualWorldBound.radius>=0){
					if(child.mVisualWorldBound.radius>=0){
						scalar d=Math.length(parent.mVisualWorldBound.origin,child.mVisualWorldBound.origin) + child.mVisualWorldBound.radius;
						parent.mVisualWorldBound.radius=Math.maxVal(parent.mVisualWorldBound.radius,d);
					}
					else{
						parent.mVisualWorldBound.radius=-Math.ONE;
					}
				}

				parent.mWorldModifiedVisualFrame=parent.mWorldModifiedVisualFrame>child.mWorldModifiedVisualFrame?parent.mWorldModifiedVisualFrame:child.mWorldModifiedVisualFrame;
			}
		}
		else if(entity.isLight()){
			// TODO: Find the best light
			mLight=(LightEntity)entity;
		}
		else{
			Math.setVector3FromMatrix4x4(entity.mVisualWorldBound.origin,entity.mVisualWorldTransform);
			entity.mVisualWorldBound.radius=entity.mBoundingRadius;
		}
	}

	protected void queueRenderables(Entity entity){
		if((entity.mScope&mCamera.mScope)==0){
			return;
		}

		if(culled(entity)){
			mCamera.mNumCulledEntities++;
			return;
		}

		if(entity.isParent()){
			ParentEntity parent=(ParentEntity)entity;
			int i;
			for(i=0;i<parent.mNumChildren;++i){
				queueRenderables(parent.mChildren[i]);
			}
		}
		else if(entity.isRenderable()){
			RenderableEntity renderable=(RenderableEntity)entity;
			if(renderable.mVisible){
				renderable.queueRenderables(this);
			}
		}
	}

	protected boolean culled(Entity entity){
		if(entity.mVisualWorldBound.radius<0){
			return false;
		}

		return mCamera.culled(entity.mVisualWorldBound);
	}

	public boolean preLayerRender(Renderer renderer,int layer){return false;}
	
	public boolean postLayerRender(Renderer renderer,int layer){return false;}

	protected int mExcessiveDT=1000;
	protected int mLogicDT=100;
	protected int mLogicTime=0;
	protected int mLogicFrame=0;
	protected int mAccumulatedDT=0;
	protected int mVisualFrame=0;

	protected ParentEntity mBackgroundEntity=null;

	protected Vector<Entity> mUpdateEntities=new Vector<Entity>();
	protected UpdateListener mUpdateListener=null;

	protected Color mAmbientColor=new Color();
	protected LightEntity mLight=null;

	protected Matrix4x4 mIdealParticleViewTransform=new Matrix4x4();
	
	protected CameraEntity mCamera=null;
	protected Material mPreviousMaterial=null;

	protected RenderLayer[] mRenderLayers=new RenderLayer[0];

	protected Viewport cache_render_viewport=new Viewport();
}
