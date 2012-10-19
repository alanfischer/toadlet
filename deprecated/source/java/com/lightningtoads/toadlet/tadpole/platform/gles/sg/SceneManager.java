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

import com.lightningtoads.toadlet.egg.mathfixed.*;
import com.lightningtoads.toadlet.egg.mathfixed.Math;
import com.lightningtoads.toadlet.tadpole.*;
import javax.microedition.khronos.opengles.*;
//#ifdef TOADLET_DEBUG
	import com.lightningtoads.toadlet.egg.*;
//#endif

class RenderLayer{
	public RenderLayer(){}

	public void add(MeshEntity.SubMesh subMesh){
		if(subMeshes.length<numSubMeshes+1){
			MeshEntity.SubMesh[] array=new MeshEntity.SubMesh[subMeshes.length+1];
			System.arraycopy(subMeshes,0,array,0,subMeshes.length);
			subMeshes=array;
		}
		subMeshes[numSubMeshes++]=subMesh;
	}

	public void add(SpriteEntity sprite){
		if(sprites.length<numSprites+1){
			SpriteEntity[] array=new SpriteEntity[sprites.length+1];
			System.arraycopy(sprites,0,array,0,sprites.length);
			sprites=array;
		}
		sprites[numSprites++]=sprite;
	}

	public void add(BeamEntity beam){
		if(beams.length<numBeams+1){
			BeamEntity[] array=new BeamEntity[beams.length+1];
			System.arraycopy(beams,0,array,0,beams.length);
			beams=array;
		}
		beams[numBeams++]=beam;
	}

	public void add(ParticleEntity particle){
		if(particles.length<numParticles+1){
			ParticleEntity[] array=new ParticleEntity[particles.length+1];
			System.arraycopy(particles,0,array,0,particles.length);
			particles=array;
		}
		particles[numParticles++]=particle;
	}

	public MeshEntity.SubMesh[] subMeshes=new MeshEntity.SubMesh[0];
	public int numSubMeshes;
	public SpriteEntity[] sprites=new SpriteEntity[0];
	public int numSprites;
	public BeamEntity[] beams=new BeamEntity[0];
	public int numBeams;
	public ParticleEntity[] particles=new ParticleEntity[0];
	public int numParticles;
	public boolean hasElements;
	public boolean forceRender;
};

public class SceneManager extends NodeEntity{
	public SceneManager(Engine engine){
		super(engine);

		mBackgroundNode=new NodeEntity(engine);
		attach(mBackgroundNode);

		int i;
		for(i=0;i<mRenderLayers.length;++i){
			mRenderLayers[i]=new RenderLayer();
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

	public final void setAmbientColor(int ambient){
		mAmbient=ambient;
	}

	public final int getAmbientColor(){
		return mAmbient;
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
			if(mAnimatedMeshEntities[i].getParent()!=null){
				mAnimatedMeshEntities[i].updateAnimation(dt);
			}
		}

		for(i=0;i<mNumSpriteEntities;++i){
			if(mSpriteEntities[i].getParent()!=null){
				mSpriteEntities[i].updateAnimation(dt);
			}
		}

		for(i=0;i<mNumParticleEntities;++i){
			if(mParticleEntities[i].getParent()!=null){
				mParticleEntities[i].updateAnimation(dt);
			}
		}
	}

	public final void render(GL target,CameraEntity camera){
try{
		GL10 gl=(GL10)target;

		//#ifdef TOADLET_DEBUG
			if(mDestroyed){
				throw new RuntimeException("render called on a destroyed SceneManager");
			}
		//#endif

		//#ifdef TOADLET_DEBUG
			Profile.beginSection(Profile.SECTION_RENDER);
		//#endif

		mCamera=camera;

		updateWorldTransform(mCamera);

		mCamera.updateViewTransform();

		if(mBackgroundNode.mNumEntities>0){
			mBackgroundNode.setTranslate(mCamera.mWorldTransform.at(0,3),mCamera.mWorldTransform.at(1,3),mCamera.mWorldTransform.at(2,3));
		}

		renderUpdate(this);
		
		mEngine.contextUpdate(gl);

		gl.glDisable(GL10.GL_DITHER);
		gl.glDisable(GL10.GL_MULTISAMPLE);
		gl.glEnable(GL10.GL_RESCALE_NORMAL);

		int width=mEngine.mWidth;
		int height=mEngine.mHeight;

		int viewportX=0;
		int viewportY=0;
		int viewportWidth=width;
		int viewportHeight=height;

		if(mCamera.mViewportSet){
			viewportX=mCamera.getViewportX();
			viewportY=mCamera.getViewportY();
			viewportWidth=mCamera.getViewportWidth();
			viewportHeight=mCamera.getViewportHeight();
		}

		{
			gl.glViewport(viewportX,height-(viewportY+viewportHeight),viewportWidth,viewportHeight);
			if(viewportX==0 && viewportY==0 && viewportWidth==width && viewportHeight==height){
				gl.glDisable(gl.GL_SCISSOR_TEST);
			}
			else{
				gl.glScissor(viewportX,height-(viewportY+viewportHeight),viewportWidth,viewportHeight);
				gl.glEnable(gl.GL_SCISSOR_TEST);
			}
		}

		{
			int buffers=mCamera.getClearBuffers();
			int color=mCamera.getClearColor();

			int glBuffers=0;
			if((buffers&CameraEntity.BUFFER_COLOR)>0){
				glBuffers|=gl.GL_COLOR_BUFFER_BIT;

				int[] fixedColor=mCacheInt4;
				uint32ColorToFixedColor(color,fixedColor);
				gl.glClearColorx(fixedColor[0],fixedColor[1],fixedColor[2],fixedColor[3]);
			}
			if((buffers&CameraEntity.BUFFER_DEPTH)>0){
				glBuffers|=gl.GL_DEPTH_BUFFER_BIT;
			}

			if(glBuffers!=0){
				gl.glClear(glBuffers);
			}
		}

		gl.glMatrixMode(gl.GL_TEXTURE);
		gl.glLoadIdentity();

		gl.glMatrixMode(gl.GL_PROJECTION);
		// Android Bug:
		// glLoadMatrixx does not appear to function properly, so we LoadIdentity then MultMatrix
		gl.glLoadIdentity();
		gl.glMultMatrixx(camera.mProjectionTransform.data,0);

		gl.glMatrixMode(gl.GL_MODELVIEW);
		// Android Bug:
		// glLoadMatrixx does not appear to function properly, so we LoadIdentity then MultMatrix
		gl.glLoadIdentity();
		gl.glMultMatrixx(camera.mViewTransform.data,0);

		if(mLight!=null){
			gl.glLightx(gl.GL_LIGHT0,gl.GL_CONSTANT_ATTENUATION,Math.ONE);
			gl.glLightx(gl.GL_LIGHT0,gl.GL_LINEAR_ATTENUATION,0);
			gl.glLightx(gl.GL_LIGHT0,gl.GL_SPOT_CUTOFF,Math.ONE*180);
			gl.glLightx(gl.GL_LIGHT0,gl.GL_SPOT_EXPONENT,0);

			int[] position=mCacheInt4;
			position[0]=-mLight.mDirection.x;
			position[1]=-mLight.mDirection.y;
			position[2]=-mLight.mDirection.z;
			position[3]=0;
			gl.glLightxv(gl.GL_LIGHT0,gl.GL_POSITION,position,0);

			int[] color=mCacheInt4;
			uint32ColorToFixedColor(0x00000000,color);
			gl.glLightxv(gl.GL_LIGHT0,gl.GL_AMBIENT,color,0);
			gl.glLightxv(gl.GL_LIGHT0,gl.GL_SPECULAR,color,0);
			gl.glLightxv(gl.GL_LIGHT0,gl.GL_DIFFUSE,mLight.mDiffuseColor,0);
			gl.glEnable(gl.GL_LIGHT0);
		}
		else{
			gl.glDisable(gl.GL_LIGHT0);
		}

		int[] color=mCacheInt4;
		uint32ColorToFixedColor(mAmbient,color);
		gl.glLightModelxv(gl.GL_LIGHT_MODEL_AMBIENT,color,0);

		render(this);

		boolean lighting=false;
		gl.glDisable(gl.GL_LIGHTING);

		Texture texture=null;
		gl.glDisable(gl.GL_TEXTURE_2D);

		byte faceCulling=Mesh.SubMesh.FC_CW;
		gl.glCullFace(gl.GL_BACK);
		gl.glEnable(gl.GL_CULL_FACE);

		boolean alphaTesting=false;
		gl.glAlphaFuncx(gl.GL_GEQUAL,Math.ONE>>2);
		gl.glDisable(gl.GL_ALPHA_TEST);

		gl.glDepthFunc(gl.GL_LEQUAL);
		gl.glEnable(gl.GL_DEPTH_TEST);
		gl.glDepthMask(true);

		gl.glColor4x(Math.ONE,Math.ONE,Math.ONE,Math.ONE);

		VertexData vertexData=null;
		boolean renderedLayer=false;

		int i,j,k,l;
		for(i=0;i<RenderableEntity.MAX_LAYERS;++i){
			RenderLayer layer=mRenderLayers[i];

			if(layer.hasElements==false && layer.forceRender==false){
				continue;
			}
			layer.hasElements=false;

			if(renderedLayer==true){
				gl.glClear(gl.GL_DEPTH_BUFFER_BIT);
			}
			else{
				renderedLayer=true;
			}

			preLayerRender(i);

			for(j=0;j<layer.numSubMeshes;++j){
				MeshEntity.SubMesh subMesh=layer.subMeshes[j];

				setMaterial(gl,subMesh.material);

				gl.glPushMatrix();
				gl.glMultMatrixx(subMesh.meshEntity.mWorldTransform.data,0);

				if(vertexData!=subMesh.vertexData){
					if(vertexData!=null){
						vertexData.unbind(gl);
					}
					vertexData=subMesh.vertexData;
					vertexData.bind(gl);
				}
				subMesh.indexData.bind(gl);
				subMesh.indexData.draw(gl);

				gl.glPopMatrix();
			}
			layer.numSubMeshes=0;

			for(j=0;j<layer.numSprites;++j){
				SpriteEntity sprite=layer.sprites[j];

				setMaterial(gl,sprite.mMaterial);

				gl.glPushMatrix();
				gl.glMultMatrixx(sprite.mWorldTransform.data,0);

				if(vertexData!=sprite.mVertexData){
					if(vertexData!=null){
						vertexData.unbind(gl);
					}
					vertexData=sprite.mVertexData;
					vertexData.bind(gl);
				}
				sprite.mIndexData.bind(gl);
				sprite.mIndexData.draw(gl);

				gl.glPopMatrix();
			}
			layer.numSprites=0;

			for(j=0;j<layer.numBeams;++j){
				BeamEntity beam=layer.beams[j];

				setMaterial(gl,beam.mMaterial);

				gl.glPushMatrix();
				gl.glMultMatrixx(beam.mWorldTransform.data,0);

				if(vertexData!=beam.mVertexData){
					if(vertexData!=null){
						vertexData.unbind(gl);
					}
					vertexData=beam.mVertexData;
					vertexData.bind(gl);
				}
				beam.mIndexData.bind(gl);
				beam.mIndexData.draw(gl);

				gl.glPopMatrix();
			}
			layer.numBeams=0;

			for(j=0;j<layer.numParticles;++j){
				ParticleEntity particle=layer.particles[j];

				setMaterial(gl,particle.mMaterial);

				gl.glPushMatrix();
				gl.glMultMatrixx(particle.mWorldTransform.data,0);
				
				if(vertexData!=particle.mVertexData){
					if(vertexData!=null){
						vertexData.unbind(gl);
					}
					vertexData=particle.mVertexData;
					vertexData.bind(gl);
				}
				particle.mIndexData.bind(gl);
				particle.mIndexData.draw(gl);

				gl.glPopMatrix();
			}
			layer.numParticles=0;

			layer.hasElements=false;

			postLayerRender(i);
		}

		mCamera=null;

		//#ifdef TOADLET_DEBUG
			Profile.endSection(Profile.SECTION_RENDER);
		//#endif
}catch(Throwable t){t.printStackTrace();}
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
				Math.mul(entity.mWorldTransform,entity.mParent.mWorldTransform,entity.mTransform);
			}
		}
	}

	void renderUpdate(Entity entity){
		if((entity.mType&TYPE_NODE)>0){
			NodeEntity node=(NodeEntity)entity;

			if(node.mParent==null){
				node.mWorldTransform.set(node.mTransform);
			}
			else if(node.mIdentityTransform){
				node.mWorldTransform.set(node.mParent.mWorldTransform);
			}
			else{
				Math.mul(node.mWorldTransform,node.mParent.mWorldTransform,node.mTransform);
			}

			node.mWorldBound.origin.set(node.mTransform.at(0,3),node.mTransform.at(1,3),node.mTransform.at(2,3));
			node.mWorldBound.radius=node.mBoundingRadius;

			int i;
			for(i=node.getNumChildren()-1;i>=0;--i){
				Entity child=node.getChild(i);

				renderUpdate(child);

				if(node.mWorldBound.radius>=0){
					if(child.mWorldBound.radius>=0){
						int d=Math.length(node.mWorldBound.origin,child.mWorldBound.origin) + child.mWorldBound.radius;
						node.mWorldBound.radius=java.lang.Math.max(node.mWorldBound.radius,d);
					}
					else{
						node.mWorldBound.radius=-Math.ONE;
					}
				}
			}
		}
		else if((entity.mType&TYPE_RENDERABLE)>0){
			RenderableEntity renderable=(RenderableEntity)entity;

			if(renderable.mVisible==true){
				if((renderable.mType&TYPE_MESH)>0){
					MeshEntity mesh=(MeshEntity)renderable;

					if(mesh.mMesh!=null){
						mCacheScale.setAt(0,0,mesh.mMesh.worldScale);
						mCacheScale.setAt(1,1,mesh.mMesh.worldScale);
						mCacheScale.setAt(2,2,mesh.mMesh.worldScale);
						Math.mul(mCacheMatrix,mesh.mTransform,mCacheScale);
						Math.mul(mesh.mWorldTransform,mesh.mParent.mWorldTransform,mCacheMatrix);
					}
				}
				else if((renderable.mType&TYPE_SPRITE)>0){
					SpriteEntity sprite=(SpriteEntity)renderable;

					if(sprite.mIdentityTransform){
						mCacheMatrix.set(sprite.mParent.mWorldTransform);
					}
					else{
						Math.mul(mCacheMatrix,sprite.mParent.mWorldTransform,sprite.mTransform);
					}

					mCacheMatrix.setAt(0,0,mCamera.mViewTransform.at(0,0));
					mCacheMatrix.setAt(1,0,mCamera.mViewTransform.at(0,1));
					mCacheMatrix.setAt(2,0,mCamera.mViewTransform.at(0,2));
					mCacheMatrix.setAt(0,1,mCamera.mViewTransform.at(1,0));
					mCacheMatrix.setAt(1,1,mCamera.mViewTransform.at(1,1));
					mCacheMatrix.setAt(2,1,mCamera.mViewTransform.at(1,2));
					mCacheMatrix.setAt(0,2,mCamera.mViewTransform.at(2,0));
					mCacheMatrix.setAt(1,2,mCamera.mViewTransform.at(2,1));
					mCacheMatrix.setAt(2,2,mCamera.mViewTransform.at(2,2));

					if(sprite.mScaled){
						mCacheScale.setAt(0,0,sprite.mWidth);
						mCacheScale.setAt(1,1,sprite.mHeight);
						mCacheScale.setAt(2,2,Math.ONE);
					}
					else{
						mCacheVector.set(mCacheMatrix.at(0,3),mCacheMatrix.at(1,3),mCacheMatrix.at(2,3));
						int scale=Math.length(mCacheVector,mCamera.getTranslate());
						mCacheScale.setAt(0,0,Math.mul(scale,sprite.mWidth));
						mCacheScale.setAt(1,1,Math.mul(scale,sprite.mHeight));
						mCacheScale.setAt(2,2,Math.ONE);
					}
					Math.mul(sprite.mWorldTransform,mCacheMatrix,mCacheScale);
				}
				else if((renderable.mType&TYPE_BEAM)>0){
					BeamEntity beam=(BeamEntity)renderable;

					beam.mWorldTransform.set(beam.mTransform);
				}
				else if((renderable.mType&TYPE_PARTICLE)>0){
					ParticleEntity particle=(ParticleEntity)renderable;

					particle.updateVertexBuffer(mCamera);

					// No need to touch the particles mWorldTranslate
				}
				else{
					Math.mul(renderable.mWorldTransform,renderable.mParent.mWorldTransform,renderable.mTransform);
				}

				renderable.mWorldBound.origin.set(renderable.mWorldTransform.at(0,3),renderable.mWorldTransform.at(1,3),renderable.mWorldTransform.at(2,3));
				renderable.mWorldBound.radius=renderable.mBoundingRadius;
			}
			else{
				renderable.mWorldBound.origin.set(renderable.mParent.mWorldTransform.at(0,3),renderable.mParent.mWorldTransform.at(1,3),renderable.mParent.mWorldTransform.at(2,3));
			}
		}
	}

	void render(Entity entity){
		if((entity.mScope&mCamera.mScope)==0){
			return;
		}

		if(culled(entity)){
			return;
		}

		if((entity.mType&TYPE_NODE)>0){
			NodeEntity node=(NodeEntity)entity;

			int i;
			for(i=node.getNumChildren()-1;i>=0;--i){
				render(node.getChild(i));
			}
		}
		else if((entity.mType&TYPE_MESH)>0){
			MeshEntity mesh=(MeshEntity)entity;

			if(mesh.mVisible==true){
				RenderLayer layer=mRenderLayers[mesh.mLayer];
				int i;
				for(i=0;i<mesh.mSubMeshes.length;++i){
					layer.add(mesh.mSubMeshes[i]);
				}
				layer.hasElements=true;
			}
		}
		else if((entity.mType&TYPE_SPRITE)>0){
			SpriteEntity sprite=(SpriteEntity)entity;

			if(sprite.mVisible==true){
				RenderLayer layer=mRenderLayers[sprite.mLayer];
				layer.add(sprite);
				layer.hasElements=true;
			}
		}
		else if((entity.mType&TYPE_BEAM)>0){
			BeamEntity beam=(BeamEntity)entity;

			if(beam.mVisible==true){
				RenderLayer layer=mRenderLayers[beam.mLayer];
				layer.add(beam);
				layer.hasElements=true;
			}
		}
		else if((entity.mType&TYPE_PARTICLE)>0){
			ParticleEntity particle=(ParticleEntity)entity;

			if(particle.mVisible==true){
				RenderLayer layer=mRenderLayers[particle.mLayer];
				layer.add(particle);
				layer.hasElements=true;
			}
		}
	}

	boolean culled(Entity entity){
		if(entity.mWorldBound.radius<0){
			return false;
		}

		Math.sub(mCacheVector,mCamera.mBoundingOrigin,entity.mWorldBound.origin);
		int sum=mCamera.mBoundingRadius+entity.mWorldBound.radius;
		return Math.dot(mCacheVector,mCacheVector)>Math.square(sum);
	}

	void setMaterial(GL10 gl,Material material){
		switch(material.mFaceCulling){
			case(Material.FC_NONE):
				gl.glDisable(gl.GL_CULL_FACE);
			break;
			case(Material.FC_CCW):
				gl.glCullFace(gl.GL_FRONT);
				gl.glEnable(gl.GL_CULL_FACE);
			break;
			case(Material.FC_CW):
				gl.glCullFace(gl.GL_BACK);
				gl.glEnable(gl.GL_CULL_FACE);
			break;
		}

		boolean lighting=material.mLighting;
		if(lighting){
			gl.glEnable(GL10.GL_LIGHTING);
			gl.glMaterialxv(gl.GL_FRONT_AND_BACK,gl.GL_AMBIENT,material.mLightEffect.ambient,0);
			gl.glMaterialxv(gl.GL_FRONT_AND_BACK,gl.GL_DIFFUSE,material.mLightEffect.diffuse,0);
			gl.glMaterialxv(gl.GL_FRONT_AND_BACK,gl.GL_SPECULAR,material.mLightEffect.specular,0);
			gl.glMaterialx(gl.GL_FRONT_AND_BACK,gl.GL_SHININESS,material.mLightEffect.shininess);
			gl.glMaterialxv(gl.GL_FRONT_AND_BACK,gl.GL_EMISSION,material.mLightEffect.emissive,0);
		}
		else{
			gl.glDisable(GL10.GL_LIGHTING);
		}

		if(material.mAlphaTest==Material.AT_NONE){
			gl.glDisable(GL10.GL_ALPHA_TEST);
		}
		else{
			float cutoff=Math.fixedToFloat(material.mAlphaTestCutoff);
			switch(material.mAlphaTest){
				case(Material.AT_LESS):
					gl.glAlphaFunc(GL10.GL_LESS,cutoff);
				break;
				case(Material.AT_EQUAL):
					gl.glAlphaFunc(GL10.GL_EQUAL,cutoff);
				break;
				case(Material.AT_LEQUAL):
					gl.glAlphaFunc(GL10.GL_LEQUAL,cutoff);
				break;
				case(Material.AT_GREATER):
					gl.glAlphaFunc(GL10.GL_GREATER,cutoff);
				break;
				case(Material.AT_NOTEQUAL):
					gl.glAlphaFunc(GL10.GL_NOTEQUAL,cutoff);
				break;
				case(Material.AT_GEQUAL):
					gl.glAlphaFunc(GL10.GL_GEQUAL,cutoff);
				break;
				case(Material.AT_ALWAYS):
					gl.glAlphaFunc(GL10.GL_ALWAYS,cutoff);
				break;
			}

			gl.glEnable(GL10.GL_ALPHA_TEST);
		}

		if(material.mBlend==Material.DISABLED_BLEND){
			gl.glDisable(GL10.GL_BLEND);
		}
		else{
			switch(material.mBlend){
				case(Material.COMBINATION_COLOR):
					gl.glBlendFunc(GL10.GL_ONE,GL10.GL_ONE_MINUS_SRC_COLOR);
				break;
				case(Material.COMBINATION_COLOR_ADDITIVE):
					gl.glBlendFunc(GL10.GL_ONE,GL10.GL_ONE);
				break;
				case(Material.COMBINATION_ALPHA):
					gl.glBlendFunc(GL10.GL_SRC_ALPHA,GL10.GL_ONE_MINUS_SRC_ALPHA);
				break;
				case(Material.COMBINATION_ALPHA_ADDITIVE):
					gl.glBlendFunc(GL10.GL_SRC_ALPHA,GL10.GL_ONE);
				break;
			}

			gl.glEnable(GL10.GL_BLEND);
		}

		if(material.mDepthWrite){
			gl.glDepthMask(true);
		}
		else{
			gl.glDepthMask(false);
		}

		if(material.mTexture!=null){
			material.mTexture.bind(gl);
			gl.glEnable(GL10.GL_TEXTURE_2D);
		}
		else{
			gl.glDisable(GL10.GL_TEXTURE_2D);
		}
	}

	public boolean preLayerRender(int layer){
		return false;
	}

	public boolean postLayerRender(int layer){
		return false;
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

	final void registerParticleEntity(ParticleEntity entity){
		int i;
		for(i=0;i<mNumParticleEntities;++i){
			if(mParticleEntities[i]==entity)break;
		}

		if(i==mNumParticleEntities){
			if(mParticleEntities.length<mNumParticleEntities+1){
				ParticleEntity[] entities=new ParticleEntity[mNumParticleEntities+1];
				System.arraycopy(mParticleEntities,0,entities,0,mParticleEntities.length);
				mParticleEntities=entities;
			}

			mParticleEntities[mNumParticleEntities]=entity;
			mNumParticleEntities++;
		}
	}

	final void unregisterParticleEntity(ParticleEntity entity){
		int i;
		for(i=0;i<mNumParticleEntities;++i){
			if(mParticleEntities[i]==entity)break;
		}

		if(i!=mNumParticleEntities){
			mParticleEntities[i]=null;
			System.arraycopy(mParticleEntities,i+1,mParticleEntities,i,mNumParticleEntities-(i+1));
			mNumParticleEntities--;
			mParticleEntities[mNumParticleEntities]=null;
		}
	}

	final void registerLightEntity(LightEntity entity){
		if(mLight!=null){
			throw new RuntimeException("Currently limited to one light!");
		}

		mLight=entity;
	}

	final void unregisterLightEntity(LightEntity entity){
		mLight=null;
	}

	int mTime=0;

	CameraEntity mCamera;

	NodeEntity mBackgroundNode;

	MeshEntity[] mAnimatedMeshEntities=new MeshEntity[0];
	int mNumAnimatedMeshEntities;
	SpriteEntity[] mSpriteEntities=new SpriteEntity[0];
	int mNumSpriteEntities;
	ParticleEntity[] mParticleEntities=new ParticleEntity[0];
	int mNumParticleEntities;

	RenderLayer[] mRenderLayers=new RenderLayer[RenderableEntity.MAX_LAYERS];

	int mAmbient;
	LightEntity mLight;

	Vector3 mCacheVector=new Vector3();
	Matrix4x4 mCacheMatrix=new Matrix4x4();
	Matrix4x4 mCacheScale=new Matrix4x4();
	int[] mCacheInt4=new int[4];
}
