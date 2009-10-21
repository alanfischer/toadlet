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

#include <toadlet/tadpole/plugins/hop/HopEntity.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::hop;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::entity;

namespace toadlet{
namespace tadpole{

HopEntity::HopEntity():ParentEntity(),
	mNextThink(0),
	mSolid(new Solid()),
	//mScene,
	mListener(NULL),
	//mHopCollision,
	//mLastPosition,
	mActivePrevious(false),

	mNetworkID(0),
	mModifiedFields(0),

	mShadowTestLength(0),
	mShadowOffset(0)
	//mShadowMesh,
	//mShadowMaterial,
	//mShadowEntity
{}

Entity *HopEntity::create(Engine *engine,bool networked){
	super::create(engine);

	mNextThink=0;
	mSolid->reset();
	mSolid->setUserData(this);
	mScene=NULL;
	mListener=NULL;
	mHopCollision.reset();
	mLastPosition.reset();
	mActivePrevious=true;

	mNetworkID=0;
	mModifiedFields=0;

	mShadowTestLength=0;
	mShadowOffset=0;
	mShadowMesh=NULL;
	mShadowMaterial=NULL;
	mShadowEntity=NULL;

	mIdentityTransform=false;

	if(networked==false){
		mNetworkID=NETWORKID_NOT_NETWORKED;
	}

	mScene=shared_static_cast<HopScene>(mEngine->getScene());

	if(mScene==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Invalid scene");
		return this;
	}

	mScene->registerHopEntity(this);

	return this;
}

void HopEntity::destroy(){
	if(mScene!=NULL){
		mScene->unregisterHopEntity(this);
	}

	// We handle this here, because destroy() must be last, but we would normally need mScene when parentChanged is called
	if(mScene!=NULL && mScene->getSimulator()!=NULL){
		mScene->getSimulator()->removeSolid(getSolid());
		mScene=NULL;
	}

	super::destroy();
}

void HopEntity::setCollisionBits(int bits){
	mSolid->setCollisionBits(bits);
}

void HopEntity::setCollideWithBits(int bits){
	mSolid->setCollideWithBits(bits);
}

void HopEntity::setMass(scalar mass){
	mSolid->setMass(mass);

	mModifiedFields|=ENTITY_BIT_MASS;
}

void HopEntity::setInfiniteMass(){
	mSolid->setInfiniteMass();

	mModifiedFields|=ENTITY_BIT_MASS;
}

void HopEntity::setTranslate(const Vector3 &translate){
	mTranslate.set(translate);

	mSolid->setPosition(translate);

	mLastPosition.set(translate);

	mModifiedFields|=ENTITY_BIT_POSITION;

	setVisualTransformTranslate(mTranslate);
}

void HopEntity::setTranslate(scalar x,scalar y,scalar z){
	mTranslate.set(x,y,z);

	setTranslate(mTranslate);
}

void HopEntity::setVelocity(const Vector3 &velocity){
	mSolid->setVelocity(velocity);

	mModifiedFields|=ENTITY_BIT_VELOCITY;
}

void HopEntity::setVelocity(scalar x,scalar y,scalar z){
	Vector3 &velocity=cache_setVelocity_velocity.set(x,y,z);
	setVelocity(velocity);
}

void HopEntity::addForce(const Vector3 &force){
	mSolid->addForce(force);

	mModifiedFields|=ENTITY_BIT_FORCE;
}

void HopEntity::clearForce(){
	mSolid->clearForce();

	mModifiedFields|=ENTITY_BIT_VELOCITY;
}

void HopEntity::setLocalGravity(const Vector3 &gravity){
	mSolid->setLocalGravity(gravity);

	mModifiedFields|=ENTITY_BIT_GRAVITY;
}

void HopEntity::setWorldGravity(){
	mSolid->setWorldGravity();

	mModifiedFields|=ENTITY_BIT_GRAVITY;
}

void HopEntity::setCoefficientOfRestitution(scalar coeff){
	mSolid->setCoefficientOfRestitution(coeff);

	mModifiedFields|=ENTITY_BIT_CO_RESTITUTION;
}

void HopEntity::setCoefficientOfRestitutionOverride(bool over){
	mSolid->setCoefficientOfRestitutionOverride(over);

	mModifiedFields|=ENTITY_BIT_CO_RESTITUTIONOVERRIDE;
}

void HopEntity::setCoefficientOfStaticFriction(scalar coeff){
	mSolid->setCoefficientOfStaticFriction(coeff);

	mModifiedFields|=ENTITY_BIT_CO_STATICFRICTION;
}

void HopEntity::setCoefficientOfDynamicFriction(scalar coeff){
	mSolid->setCoefficientOfDynamicFriction(coeff);

	mModifiedFields|=ENTITY_BIT_CO_DYNAMICFRICTION;
}

void HopEntity::setCoefficientOfEffectiveDrag(scalar coeff){
	mSolid->setCoefficientOfEffectiveDrag(coeff);

	mModifiedFields|=ENTITY_BIT_CO_EFFECTIVEDRAG;
}

HopEntity::ptr HopEntity::getTouching() const{
	HopEntity::ptr ent;
	Solid *solid=mSolid->getTouching();
	if(solid!=NULL){
		ent=HopEntity::ptr(static_cast<HopEntity*>(solid->getUserData()));
	}
	return ent;
}

void HopEntity::setCollisionListener(HopCollisionListener *listener){
	if(listener!=NULL){
		mSolid->setCollisionListener(this);
	}
	else{
		mSolid->setCollisionListener(NULL);
	}

	mListener=listener;
}

void HopEntity::think(){
}

void HopEntity::setShadowMesh(Mesh::ptr shadow,scalar scale,scalar testLength,scalar offset){
	mShadowMesh=shadow;
	mShadowTestLength=testLength;
	mShadowOffset=offset;
	if(mShadowMesh!=NULL){
		if(mShadowEntity==NULL){
			mShadowEntity=(MeshEntity*)((new MeshEntity())->create(mEngine));
			mScene->attach(mShadowEntity);
		}
		mShadowEntity->load(mShadowMesh);
		mShadowEntity->setScale(scale,scale,scale);
		// We assume the shadow only has 1 subMesh
		mShadowMaterial=mShadowEntity->getSubMesh(0)->material;
		mShadowMaterial->setBlend(Blend::Combination_ALPHA);
	}
	else{
		if(mShadowEntity!=NULL){
			mShadowEntity->destroy();
			mShadowEntity=NULL;
		}
	}
}

void HopEntity::parentChanged(ParentEntity *newParent){
	if(mScene!=NULL){
		if(newParent==mScene){
			mScene->getSimulator()->addSolid(getSolid());
		}
		else{
			mScene->getSimulator()->removeSolid(getSolid());
		}
	}

	super::parentChanged(newParent);
}

void HopEntity::collision(const Collision &c){
	if(mListener!=NULL){
		Entity::ptr reference(this); // To make sure that the object is not deleted by the collision callback until we exit this function
		mHopCollision.time=c.time;
		mHopCollision.point.set(c.point);
		mHopCollision.normal.set(c.normal);
		mHopCollision.velocity.set(c.velocity);
		if(c.collider!=NULL){
			mHopCollision.collider=static_cast<HopEntity*>(c.collider->getUserData());
		}
		else{
			mHopCollision.collider=NULL;
		}
		if(c.collidee!=NULL){
			mHopCollision.collidee=static_cast<HopEntity*>(c.collidee->getUserData());
		}
		else{
			mHopCollision.collidee=NULL;
		}
		mListener->collision(mHopCollision);
	}
}

void HopEntity::preLogicUpdateLoop(int dt){
	mLastPosition.set(mSolid->getPosition());
	mActivePrevious=mSolid->getActive();
}

void HopEntity::postLogicUpdate(int dt){
	if(mSolid->getActive()){
		mTranslate.set(mSolid->getPosition());
	}

	if(mNextThink>0){
		mNextThink-=dt;
		if(mNextThink<=0){
			mNextThink=0;
			think();
		}
	}
}

void HopEntity::interpolatePhysicalParameters(scalar f){
	Vector3 &interpolate=cache_interpolatePhysicalParameters_interpolate;
	Math::lerp(interpolate,mLastPosition,mSolid->getPosition(),f);
	setVisualTransformTranslate(interpolate);
}

void HopEntity::castShadow(){
	Segment segment;
	Vector3 vector;
	Matrix3x3 rotate;
	Math::setVector3FromMatrix4x4(segment.origin,mVisualTransform);
	Math::setMatrix3x3FromMatrix4x4(rotate,mVisualTransform);
	if(mSolid->hasLocalGravity()){
		Math::normalize(vector,mSolid->getLocalGravity());
	}
	else{
		Math::normalize(vector,mScene->getSimulator()->getGravity());
	}
	Math::mul(segment.direction,vector,mShadowTestLength);

	Collision collision;
	mScene->getSimulator()->traceSegment(collision,segment,-1,mSolid);
	// Shadow if the object beneath us is of infinite mass (eg static)
	if(collision.collider!=NULL && collision.collider->hasInfiniteMass()){
		Math::mul(vector,collision.normal,mShadowOffset);
		Math::add(collision.point,vector);
		mShadowEntity->setTranslate(collision.point);

		Vector3 zAxis;
		Math::mul(zAxis,rotate,Math::Z_UNIT_VECTOR3);
		Matrix3x3 shadowRotate;
		Math::setMatrix3x3FromVector3ToVector3(shadowRotate,collision.normal,zAxis,Math::ONE/2048);
		Math::postMul(rotate,shadowRotate);
		mShadowEntity->setRotate(rotate);

		LightEffect le(Color(0,0,0,Math::ONE-collision.time));
		mShadowMaterial->setLightEffect(le);
	}
	else{
		LightEffect le(Colors::TRANSPARENT_BLACK);
		mShadowMaterial->setLightEffect(le);
	}
}

}
}
