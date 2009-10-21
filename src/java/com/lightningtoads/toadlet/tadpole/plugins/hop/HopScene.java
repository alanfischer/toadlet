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

package com.lightningtoads.toadlet.tadpole.plugins.hop;

#include <com/lightningtoads/toadlet/tadpole/Types.h>

import com.lightningtoads.toadlet.hop.*;
import com.lightningtoads.toadlet.peeper.*;
import com.lightningtoads.toadlet.tadpole.*;
import com.lightningtoads.toadlet.tadpole.entity.*;
import java.util.Vector;

public class HopScene extends Scene{
	public HopScene(){super();}

	public Entity create(Engine engine){
		super.create(engine);

		resetNetworkIDs();

		mShadowMaterial=new Material();
		mShadowMaterial.setBlend(new Blend(Blend.Operation.DEST_COLOR,Blend.Operation.ZERO));
		mShadowMaterial.setAlphaTest(Renderer.AlphaTest.NONE,0);
		mShadowMaterial.setLighting(false);
		mShadowMaterial.setFaceCulling(Renderer.FaceCulling.NONE);

		mCollisionVolumeMaterial=new Material();
		LightEffect lightEffect=new LightEffect();
		lightEffect.ambient=Colors.WHITE;
		lightEffect.diffuse=Colors.BLACK;
		lightEffect.emissive=Colors.BLACK;
		lightEffect.specular=Colors.BLACK;
		lightEffect.trackColor=false;
		mCollisionVolumeMaterial.setLightEffect(lightEffect);
		mCollisionVolumeMaterial.setLighting(true);
		
		return this;
	}

	public void destroy(){
		setShadows((Texture)null,false,0,0,0,0);

		super.destroy();
	}

	public void setFluidVelocity(Vector3 fluidVelocity){mSimulator.setFluidVelocity(fluidVelocity);}
	public Vector3 getFluidVelocity(){return mSimulator.getFluidVelocity();}

	public void setGravity(Vector3 gravity){mSimulator.setGravity(gravity);}
	public Vector3 getGravity(){return mSimulator.getGravity();}

	public void findHopEntitiesInAABox(AABox box,Vector<HopEntity> entities){
		if(mSolidCollection.length<mSimulator.getNumSolids()){
			mSolidCollection=new Solid[mSimulator.getNumSolids()];
		}
		int numSolids=mSimulator.findSolidsInAABox(box,mSolidCollection,mSolidCollection.length);
		findHopEntitiesInSolids(entities,mSolidCollection,numSolids);
	}
	
	public void findHopEntitiesInSphere(Sphere sphere,Vector<HopEntity> entities){
		if(mSolidCollection.length<mSimulator.getNumSolids()){
			mSolidCollection=new Solid[mSimulator.getNumSolids()];
		}
		int numSolids=mSimulator.findSolidsInSphere(sphere,mSolidCollection,mSolidCollection.length);
		findHopEntitiesInSolids(entities,mSolidCollection,numSolids);
	}
	
	public void findHopEntitiesInSolids(Vector<HopEntity> entities,Solid solids[],int numSolids){
		entities.clear();
		int i;
		for(i=numSolids-1;i>=0;--i){
			Solid solid=solids[i];
			HopEntity entity=((HopEntity)(solid.getUserData()));
			if(entity!=null){
				entities.add(entity);
			}
		}
	}

	public void traceSegment(HopCollision result,Segment segment,int collideWithBits,HopEntity ignore){
		result.reset();

		Solid ignoreSolid=null;
		if(ignore!=null){
			ignoreSolid=ignore.getSolid();
		}

		Collision collision=cache_traceSegment_collision.reset();
		mSimulator.traceSegment(collision,segment,collideWithBits,ignoreSolid);

		if(collision.time!=-Math.ONE){
			result.time=collision.time;
			result.point.set(collision.point);
			result.normal.set(collision.normal);

			if(collision.collider!=null){
				HopEntity entity=((HopEntity)(collision.collider.getUserData()));
				if(entity!=null){
					result.collider=entity;
				}
			}
		}
	}

	public int getNumHopEntities(){return mHopEntities.size();}
	public HopEntity getHopEntity(int i){return mHopEntities.get(i);}

	public HopEntity getHopEntityFromNetworkID(int id){
		if(id>=0 && id<mNetworkIDMap.size()){
			return mNetworkIDMap.get(id);
		}
		else{
			return null;
		}
	}

	public void resetNetworkIDs(){
		mNetworkIDMap.clear();
		mNetworkIDMap.add(null);
		mFreeNetworkIDs.clear();
	}
	
	public void setHopEntityFactory(HopEntityFactory factory){mHopEntityFactory=factory;}

	public void setShadows(String textureName,boolean shadows,scalar scale,scalar testLength,scalar offset,int layer){
		setShadows(mEngine.loadTexture(textureName),shadows,scale,testLength,offset,layer);
	}
	
	public void setShadows(Texture texture,boolean shadows,scalar scale,scalar testLength,scalar offset,int layer){
		if(mShadowLayer!=mCollisionVolumeLayer){
			getRenderLayer(mShadowLayer).forceRender=false;
		}

		mShadowTexture=texture;
		mShadowScale=scale;
		mShadowTestLength=testLength;
		mShadowOffset=offset;
		mShadowLayer=layer;

		if(shadows){
			getRenderLayer(mShadowLayer).forceRender=true;

			mShadowMaterial.setTextureStage(0,new TextureStage(mShadowTexture));

			if(mShadowVertexData==null){
				VertexBuffer vertexBuffer=mEngine.loadVertexBuffer(new VertexBuffer(Buffer.UsageType.DYNAMIC,Buffer.AccessType.WRITE_ONLY,mEngine.getVertexFormats().POSITION_TEX_COORD,4));
				{
					VertexBufferAccessor vba=new VertexBufferAccessor();
					vba.lock(vertexBuffer,Buffer.LockType.WRITE_ONLY);

					vba.set3(0,0, -scale,scale,0);
					vba.set2(0,1, 0,0);

					vba.set3(1,0, scale,scale,0);
					vba.set2(1,1, Math.ONE,0);

					vba.set3(2,0, -scale,-scale,0);
					vba.set2(2,1, 0,Math.ONE);

					vba.set3(3,0, scale,-scale,0);
					vba.set2(3,1, Math.ONE,Math.ONE);

					vba.unlock();
				}
				mShadowVertexData=new VertexData(vertexBuffer);
			}

			if(mShadowIndexData==null){
				mShadowIndexData=new IndexData(IndexData.Primitive.TRISTRIP,null,0,4);
			}
		}
		else{
			mShadowTexture=null;
			mShadowVertexData=null;
			mShadowIndexData=null;
		}
	}

	public void setRenderCollisionVolumes(boolean volumes,boolean interpolate,int layer){
		if(mShadowLayer!=mCollisionVolumeLayer){
			getRenderLayer(mCollisionVolumeLayer).forceRender=false;
		}

		mRenderCollisionVolumes=volumes;
		mInterpolateCollisionVolumes=interpolate;
		mCollisionVolumeLayer=layer;

		if(volumes){
			getRenderLayer(mCollisionVolumeLayer).forceRender=true;
			if(mCollisionVolumeVertexData==null){
				VertexBuffer vertexBuffer=mEngine.loadVertexBuffer(new VertexBuffer(Buffer.UsageType.DYNAMIC,Buffer.AccessType.WRITE_ONLY,mEngine.getVertexFormats().POSITION_COLOR,16));
				scalar scale=Math.ONE;
				int color=Colors.WHITE.getABGR();
				{
					VertexBufferAccessor vba=new VertexBufferAccessor();
					vba.lock(vertexBuffer,Buffer.LockType.WRITE_ONLY);

					vba.set3(0,0,	-scale,-scale,-scale);
					vba.setABGR(0,1,color);

					vba.set3(1,0,	scale,-scale,-scale);
					vba.setABGR(1,1,color);

					vba.set3(2,0,	scale,scale,-scale);
					vba.setABGR(2,1,color);

					vba.set3(3,0,	-scale,scale,-scale);
					vba.setABGR(3,1,color);

					vba.set3(4,0,	-scale,-scale,-scale);
					vba.setABGR(4,1,color);

					vba.set3(5,0,	-scale,-scale,scale);
					vba.setABGR(5,1,color);

					vba.set3(6,0,	-scale,-scale,scale);
					vba.setABGR(6,1,color);

					vba.set3(7,0,	scale,-scale,-scale);
					vba.setABGR(7,1,color);

					vba.set3(8,0,	scale,-scale,scale);
					vba.setABGR(8,1,color);

					vba.set3(9,0,	scale,scale,scale);
					vba.setABGR(9,1,color);

					vba.set3(10,0,	scale,scale,-scale);
					vba.setABGR(10,1,color);

					vba.set3(11,0,	scale,scale,scale);
					vba.setABGR(11,1,color);

					vba.set3(12,0,	-scale,scale,scale);
					vba.setABGR(12,1,color);

					vba.set3(13,0,	-scale,scale,-scale);
					vba.setABGR(13,1,color);

					vba.set3(14,0,	-scale,scale,scale);
					vba.setABGR(14,1,color);

					vba.set3(15,0,	-scale,-scale,scale);
					vba.setABGR(15,1,color);

					vba.unlock();
				}
				mCollisionVolumeVertexData=new VertexData(vertexBuffer);
			}
			if(mCollisionVolumeIndexData==null){
				mCollisionVolumeIndexData=new IndexData(IndexData.Primitive.LINESTRIP,null,0,16);
			}
		}
		else{
			mCollisionVolumeVertexData=null;
			mCollisionVolumeIndexData=null;
		}
	}

	public Simulator getSimulator(){return mSimulator;}

	public boolean postLayerRender(Renderer renderer,int layer){
		boolean rendered=false;

		if(layer==mShadowLayer && mShadowTexture!=null && mShadowVertexData!=null && mShadowIndexData!=null){
			mShadowMaterial.setupRenderer(renderer,mPreviousMaterial);
			mPreviousMaterial=mShadowMaterial;

			// Run through all hop entities
			int i;
			for(i=mHopEntities.size()-1;i>=0;--i){
				if(mHopEntities.get(i).getCastsShadow() && mHopEntities.get(i).getParent()==this){
					castShadow(renderer,mHopEntities.get(i));
				}
			}

			rendered=true;
		}

		if(layer==mCollisionVolumeLayer && mRenderCollisionVolumes){
			mCollisionVolumeMaterial.setupRenderer(renderer,mPreviousMaterial);
			mPreviousMaterial=mCollisionVolumeMaterial;

			int i,j;
			for(i=mHopEntities.size()-1;i>=0;--i){
				HopEntity entity=mHopEntities.get(i);
				Solid solid=entity.getSolid();
				for(j=solid.getNumShapes()-1;j>=0;--j){
					Shape shape=solid.getShape(j);
					if(shape.getType()==Shape.Type.AABOX){
						renderer.setAmbientColor(Colors.WHITE);
					}
					else if(shape.getType()==Shape.Type.SPHERE){
						renderer.setAmbientColor(Colors.GREEN);
					}
					else if(shape.getType()==Shape.Type.CAPSULE){
						renderer.setAmbientColor(Colors.RED);
					}
					// Approximate all solids with AABoxes
					{
						AABox box=new AABox();
						shape.getBound(box);

						mCollisionVolumeMatrix.reset();

						Vector3 translate=new Vector3();
						Math.add(translate,box.maxs,box.mins);
						Math.div(translate,Math.fromInt(2));

						Vector3 scale=new Vector3();
						Math.sub(scale,box.maxs,translate);

						if(mInterpolateCollisionVolumes){
							Math.add(translate,new Vector3(entity.getVisualTransform().at(0,3),entity.getVisualTransform().at(1,3),entity.getVisualTransform().at(2,3)));
						}
						else{
							Math.add(translate,entity.getTranslate());
						}
						Math.setMatrix4x4FromTranslate(mCollisionVolumeMatrix,translate);

						Math.setMatrix4x4FromScale(mCollisionVolumeMatrix,scale);
						renderer.setModelMatrix(mCollisionVolumeMatrix);

						renderer.renderPrimitive(mCollisionVolumeVertexData,mCollisionVolumeIndexData);
					}
				}
			}

			rendered=true;
		}

		return rendered;
	}

	public ParticleEntity.ParticleSimulator newParticleSimulator(ParticleEntity particleEntity){
		return new HopParticleSimulator(this,particleEntity);
	}

	public void registerHopEntity(HopEntity entity){
		if(mHopEntities.contains(entity)==false){
			mHopEntities.add(entity);

			if(entity.getNetworkID()!=HopEntity.NETWORKID_NOT_NETWORKED){
				defaultRegisterHopEntity(entity);
			}
		}
	}

	public void unregisterHopEntity(HopEntity entity){
		if(mHopEntities.contains(entity)){
			defaultUnregisterHopEntity(entity);

			mHopEntities.remove(entity);
		}
	}

	public void preLogicUpdateLoop(int dt){
		int i;
		for(i=mHopEntities.size()-1;i>=0;--i){
			HopEntity entity=mHopEntities.get(i);
			entity.preLogicUpdateLoop(dt);
		}
	}
	
	public void logicUpdate(int dt){
		int i;

		super.logicUpdate(dt);

		TOADLET_PROFILE_BEGINSECTION("physics");
		mSimulator.update(dt);
		TOADLET_PROFILE_ENDSECTION("physics");

		for(i=mHopEntities.size()-1;i>=0;--i){
			HopEntity entity=mHopEntities.get(i);
			entity.postLogicUpdate(dt);
		}
	}

	public void postLogicUpdateLoop(int dt){}

	public void visualUpdate(int dt){
		int i;
		boolean active,activePrevious;

		scalar f=Math.div(Math.fromInt(mAccumulatedDT),Math.fromInt(mLogicDT));

		for(i=mHopEntities.size()-1;i>=0;--i){
			HopEntity entity=mHopEntities.get(i);
			active=entity.mSolid.getActive();
			activePrevious=entity.mActivePrevious;
			if(active || activePrevious){
				// If we are deactivating, then make sure we are at our rest point
				if(active==false && activePrevious){
					entity.interpolatePhysicalParameters(Math.ONE);
				}
				else{
					entity.interpolatePhysicalParameters(f);
				}
			}
		}

		super.visualUpdate(dt);
	}

	protected void defaultRegisterHopEntity(HopEntity entity){
		int networkID=0;
		if(mFreeNetworkIDs.size()>0){
			networkID=mFreeNetworkIDs.get(0);
			mFreeNetworkIDs.remove(0);
		}
		else{
			networkID=mNetworkIDMap.size();
		}

		entity.mNetworkID=networkID;
		if(networkID>=mNetworkIDMap.size()){
			mNetworkIDMap.setSize(networkID+1);
		}
		mNetworkIDMap.set(networkID,entity);
	}
	
	protected void defaultUnregisterHopEntity(HopEntity entity){
		if(entity.getNetworkID()>0){
			mFreeNetworkIDs.add(entity.getNetworkID());
			mNetworkIDMap.set(entity.getNetworkID(),null);
		}
	}

	protected void castShadow(Renderer renderer,HopEntity entity){
		if(entity.getSolid().getActive()){
			Segment segment=cache_castShadow_segment.reset();
			segment.setOrigin(entity.getTranslate());
			segment.direction.set(0,0,-mShadowTestLength);

			Collision collision=cache_castShadow_collision.reset();
			mSimulator.traceSegment(collision,segment,-1,entity.getSolid());
			// Shadow if the object beneath us is of infinite mass (eg static)
			if(collision.collider!=null && collision.collider.hasInfiniteMass()){
				collision.point.z+=mShadowOffset;

				Matrix3x3 matrix=cache_castShadow_matrix;
				Math.setMatrix3x3FromVector3ToVector3(matrix,Math.Z_UNIT_VECTOR3,collision.normal,Math.ONE/128);

				Math.setMatrix4x4FromMatrix3x3(entity.mShadowMatrix,matrix);
				Math.setMatrix4x4FromTranslate(entity.mShadowMatrix,collision.point);
				entity.mShadowStrength=Math.ONE-collision.time;
			}
			else{
				entity.mShadowStrength=0;
			}
		}

		if(entity.mShadowStrength>0){
			LightEffect le=new LightEffect();
			le.ambient.a=le.diffuse.a=le.specular.a=entity.mShadowStrength;
			renderer.setLightEffect(le);
			renderer.setModelMatrix(entity.mShadowMatrix);
			renderer.renderPrimitive(mShadowVertexData,mShadowIndexData);
		}
	}

	protected Vector<HopEntity> mHopEntities=new Vector<HopEntity>();

	protected Simulator mSimulator=new Simulator();

	protected Vector<Integer> mFreeNetworkIDs=new Vector<Integer>();
	protected Vector<HopEntity> mNetworkIDMap=new Vector<HopEntity>();
	protected HopEntityFactory mHopEntityFactory=null;

	protected Solid[] mSolidCollection=new Solid[0];

	protected Texture mShadowTexture=null;
	protected VertexData mShadowVertexData=null;
	protected IndexData mShadowIndexData=null;
	protected scalar mShadowScale=0;
	protected scalar mShadowTestLength=0;
	protected scalar mShadowOffset=0;
	protected int mShadowLayer=0;
	protected Material mShadowMaterial=null;

	protected boolean mRenderCollisionVolumes=false;
	protected boolean mInterpolateCollisionVolumes=false;
	protected VertexData mCollisionVolumeVertexData=null;
	protected IndexData mCollisionVolumeIndexData=null;
	protected int mCollisionVolumeLayer=0;
	protected Matrix4x4 mCollisionVolumeMatrix=new Matrix4x4();
	protected Material mCollisionVolumeMaterial=null;

	protected Collision cache_traceSegment_collision=new Collision();
	protected Segment cache_castShadow_segment=new Segment();
	protected Collision cache_castShadow_collision=new Collision();
	protected Matrix3x3 cache_castShadow_matrix=new Matrix3x3();
}
