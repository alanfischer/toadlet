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

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Profile.h>
#include <toadlet/peeper/CapabilitySet.h> 
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/plugins/hop/HopScene.h>
#include <toadlet/tadpole/plugins/hop/HopEntity.h>
#include <toadlet/tadpole/plugins/hop/HopCollision.h>
#include <toadlet/tadpole/plugins/hop/HopParticleSimulator.h>

using namespace toadlet::egg;
using namespace toadlet::hop;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::entity;

namespace toadlet{
namespace tadpole{

HopScene::HopScene():Scene(),
	//mHopEntities,

	mSimulator(new Simulator()),

	//mFreeNetworkIDs,
	//mNetworkIDMap,
	mHopEntityFactory(NULL),

	//mSolidCollection,

	mRenderCollisionVolumes(false),
	mInterpolateCollisionVolumes(false),
	//mCollisionVolumeVertexData,
	//mCollisionVolumeIndexData,
	mCollisionVolumeLayer(0)
	//mCollisionVolumeMatrix,
	//mCollisionVolumeMaterial,
{}

HopScene::~HopScene(){
	if(mSimulator!=NULL){
		delete mSimulator;
		mSimulator=NULL;
	}
}

Entity *HopScene::create(Engine *engine){
	super::create(engine);

	resetNetworkIDs();

	mCollisionVolumeMaterial=Material::ptr(new Material());
	LightEffect lightEffect;
	lightEffect.ambient=Colors::WHITE;
	lightEffect.diffuse=Colors::BLACK;
	lightEffect.emissive=Colors::BLACK;
	lightEffect.specular=Colors::BLACK;
	lightEffect.trackColor=false;
	mCollisionVolumeMaterial->setLightEffect(lightEffect);
	mCollisionVolumeMaterial->setLighting(true);

	return this;
}

void HopScene::setFluidVelocity(const Vector3 &fluidVelocity){
	mSimulator->setFluidVelocity(fluidVelocity);
}

const Vector3 &HopScene::getFluidVelocity() const{
	return mSimulator->getFluidVelocity();
}

void HopScene::setGravity(const Vector3 &gravity){
	mSimulator->setGravity(gravity);
}

const Vector3 &HopScene::getGravity() const{
	return mSimulator->getGravity();
}

void HopScene::findHopEntitiesInAABox(const AABox &box,Collection<HopEntity::ptr> &entities){
	if(mSolidCollection.size()<mSimulator->getNumSolids()){
		mSolidCollection.resize(mSimulator->getNumSolids());
	}
	int numSolids=mSimulator->findSolidsInAABox(box,mSolidCollection.begin(),mSolidCollection.size());
	findHopEntitiesInSolids(entities,mSolidCollection.begin(),numSolids);
}

void HopScene::findHopEntitiesInSphere(const Sphere &sphere,Collection<HopEntity::ptr> &entities){
	if(mSolidCollection.size()<mSimulator->getNumSolids()){
		mSolidCollection.resize(mSimulator->getNumSolids());
	}
	int numSolids=mSimulator->findSolidsInSphere(sphere,mSolidCollection.begin(),mSolidCollection.size());
	findHopEntitiesInSolids(entities,mSolidCollection.begin(),numSolids);
}

void HopScene::findHopEntitiesInSolids(Collection<HopEntity::ptr> &entities,Solid *solids[],int numSolids){
	entities.clear();
	int i;
	for(i=numSolids-1;i>=0;--i){
		Solid *solid=solids[i];
		HopEntity *entity=static_cast<HopEntity*>(solid->getUserData());
		if(entity!=NULL){
			entities.add(entity);
		}
	}
}

void HopScene::traceSegment(HopCollision &result,const Segment &segment,int collideWithBits,HopEntity *ignore){
	result.reset();

	Solid *ignoreSolid=NULL;
	if(ignore!=NULL){
		ignoreSolid=ignore->getSolid();
	}

	Collision &collision=cache_traceSegment_collision.reset();
	mSimulator->traceSegment(collision,segment,collideWithBits,ignoreSolid);

	if(collision.time!=-Math::ONE){
		result.time=collision.time;
		result.point=collision.point;
		result.normal=collision.normal;

		if(collision.collider!=NULL){
			HopEntity *entity=static_cast<HopEntity*>(collision.collider->getUserData());
			if(entity!=NULL){
				result.collider=HopEntity::ptr(entity);
			}
		}
	}
}

HopEntity *HopScene::getHopEntityFromNetworkID(int id) const{
	if(id>=0 && id<mNetworkIDMap.size()){
		return mNetworkIDMap[id];
	}
	else{
		return NULL;
	}
}

void HopScene::setHopEntityNetworkID(HopEntity *entity,int id){
	if(entity->getNetworkID()>=0 && entity->getNetworkID()<mNetworkIDMap.size()){
		mFreeNetworkIDs.add(entity->getNetworkID());
		mNetworkIDMap[entity->getNetworkID()]=NULL;
	}
	if(id>=0){
		if(id>=mNetworkIDMap.size()){
			mNetworkIDMap.resize(id+1);
		}
		mNetworkIDMap[id]=entity;
		entity->mNetworkID=id;
	}
}

void HopScene::resetNetworkIDs(){
	mNetworkIDMap.clear();
	mNetworkIDMap.add(NULL);
	mFreeNetworkIDs.clear();
}

void HopScene::setHopEntityFactory(HopEntityFactory *factory){
	mHopEntityFactory=factory;
}

void HopScene::setRenderCollisionVolumes(bool volumes,bool interpolate,int layer){
	mRenderCollisionVolumes=volumes;
	mInterpolateCollisionVolumes=interpolate;
	mCollisionVolumeLayer=layer;

	if(volumes){
		getRenderLayer(mCollisionVolumeLayer)->forceRender=true;
		if(mCollisionVolumeVertexData==NULL){
			VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::UsageFlags_DYNAMIC,Buffer::AccessType_WRITE_ONLY,mEngine->getVertexFormats().POSITION_COLOR,16);
			scalar scale=Math::ONE;
			uint32 color=Colors::WHITE.getABGR();
			{
				VertexBufferAccessor vba;
				vba.lock(vertexBuffer,Buffer::AccessType_WRITE_ONLY);

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

				vba.set3(6,0,	scale,-scale,scale);
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
			mCollisionVolumeVertexData=VertexData::ptr(new VertexData(vertexBuffer));
		}
		if(mCollisionVolumeIndexData==NULL){
			mCollisionVolumeIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_LINESTRIP,NULL,0,16));
		}
	}
	else{
		mCollisionVolumeVertexData=NULL;
		mCollisionVolumeIndexData=NULL;
	}
}

bool HopScene::postLayerRender(Renderer *renderer,int layer){
	bool rendered=false;

	if(layer==mCollisionVolumeLayer && mRenderCollisionVolumes){
		mCollisionVolumeMaterial->setupRenderer(renderer,mPreviousMaterial);
		mPreviousMaterial=mCollisionVolumeMaterial;

		int i,j;
		for(i=mHopEntities.size()-1;i>=0;--i){
			HopEntity *entity=mHopEntities[i];
			Solid *solid=entity->getSolid();
			for(j=solid->getNumShapes()-1;j>=0;--j){
				Shape *shape=solid->getShape(j);
				if(shape->getType()==Shape::Type_AABOX){
					renderer->setAmbientColor(Colors::WHITE);
				}
				else if(shape->getType()==Shape::Type_SPHERE){
					renderer->setAmbientColor(Colors::GREEN);
				}
				else if(shape->getType()==Shape::Type_CAPSULE){
					renderer->setAmbientColor(Colors::RED);
				}
				// Approximate all solids with AABoxes
				{
					AABox box;
					shape->getBound(box);

					mCollisionVolumeMatrix.reset();

					Vector3 translate;
					Math::add(translate,box.maxs,box.mins);
					Math::div(translate,Math::fromInt(2));

					Vector3 scale;
					Math::sub(scale,box.maxs,translate);

					if(mInterpolateCollisionVolumes){
						Math::add(translate,Vector3(entity->mVisualTransform.at(0,3),entity->mVisualTransform.at(1,3),entity->mVisualTransform.at(2,3)));
					}
					else{
						Math::add(translate,entity->getTranslate());
					}
					Math::setMatrix4x4FromTranslate(mCollisionVolumeMatrix,translate);

					Math::setMatrix4x4FromScale(mCollisionVolumeMatrix,scale);
					renderer->setModelMatrix(mCollisionVolumeMatrix);

					renderer->renderPrimitive(mCollisionVolumeVertexData,mCollisionVolumeIndexData);
				}
			}
		}

		rendered=true;
	}

	return rendered;
}

ParticleEntity::ParticleSimulator::ptr HopScene::newParticleSimulator(ParticleEntity *particleEntity){
	return ParticleEntity::ParticleSimulator::ptr(new HopParticleSimulator(this,particleEntity));
}

void HopScene::registerHopEntity(HopEntity *entity){
	if(mHopEntities.contains(entity)==false){
		mHopEntities.add(entity);

		if(entity->getNetworkID()!=HopEntity::NETWORKID_NOT_NETWORKED){
			defaultRegisterHopEntity(entity);
		}
	}
}

void HopScene::unregisterHopEntity(HopEntity *entity){
	if(mHopEntities.contains(entity)){
		defaultUnregisterHopEntity(entity);

		mHopEntities.remove(entity);
	}
}

void HopScene::preLogicUpdateLoop(int dt){
	int i;
	for(i=mHopEntities.size()-1;i>=0;--i){
		HopEntity *entity=mHopEntities[i];
		entity->preLogicUpdateLoop(dt);
	}
}

void HopScene::logicUpdate(int dt){
	super::logicUpdate(dt);

	TOADLET_PROFILE_BEGINSECTION(Simulator::update);

	mSimulator->update(dt);

	TOADLET_PROFILE_ENDSECTION(Simulator::update);

	int i;
	for(i=mHopEntities.size()-1;i>=0;--i){
		HopEntity *entity=mHopEntities[i];
		entity->postLogicUpdate(dt);
	}
}

void HopScene::postLogicUpdateLoop(int dt){
}

void HopScene::visualUpdate(int dt){
	int i;
	bool active,activePrevious;

	scalar f=Math::div(Math::fromInt(mAccumulatedDT),Math::fromInt(mLogicDT));

	for(i=mHopEntities.size()-1;i>=0;--i){
		HopEntity *entity=mHopEntities[i];
		active=entity->mSolid->getActive();
		activePrevious=entity->mActivePrevious;
		if(active || activePrevious){
			// If we are deactivating, then make sure we are at our rest point
			if(active==false && activePrevious){
				entity->interpolatePhysicalParameters(Math::ONE);
			}
			else{
				entity->interpolatePhysicalParameters(f);
			}
			if(entity->mShadowMesh!=NULL){
				entity->castShadow();
			}
		}
	}

	super::visualUpdate(dt);
}

void HopScene::defaultRegisterHopEntity(HopEntity *entity){
	int networkID=0;
	if(mFreeNetworkIDs.size()>0){
		networkID=mFreeNetworkIDs[0];
		mFreeNetworkIDs.removeAt(0);
	}
	else{
		networkID=mNetworkIDMap.size();
	}

	entity->mNetworkID=networkID;
	if(networkID>=mNetworkIDMap.size()){
		mNetworkIDMap.resize(networkID+1);
	}
	mNetworkIDMap[networkID]=entity;
}

void HopScene::defaultUnregisterHopEntity(HopEntity *entity){
	if(entity->getNetworkID()>0){
		mFreeNetworkIDs.add(entity->getNetworkID());
		mNetworkIDMap[entity->getNetworkID()]=NULL;
	}
}

}
}
