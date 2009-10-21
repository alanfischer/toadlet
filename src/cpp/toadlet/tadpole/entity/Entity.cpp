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

#include <toadlet/tadpole/entity/Entity.h>
#include <toadlet/tadpole/entity/ParentEntity.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace entity{

Entity::Entity():
	mCounter(new PointerCounter<Entity>(0)),
	mManaged(false),

	mCreated(false),
	mEngine(NULL),

	mEntityDestroyedListener(NULL),
	mOwnsEntityDestroyedListener(false),

	//mParent,

	mIdentityTransform(false),
	//mScale,
	mScope(0),
	mBoundingRadius(0),
	mReceiveUpdates(false),

	mModifiedLogicFrame(0),
	mModifiedVisualFrame(0),
	mWorldModifiedLogicFrame(0),
	mWorldModifiedVisualFrame(0)
{
}

Entity::~Entity(){
	TOADLET_ASSERT(!mCreated);
}

Entity *Entity::create(Engine *engine){
	if(mCreated){
		return this;
	}

	mCreated=true;
	mEngine=engine;

	mEntityDestroyedListener=NULL;
	mOwnsEntityDestroyedListener=false;

	mParent=NULL;

	mIdentityTransform=true;
	mTranslate.reset();
	mRotate.reset();
	mScale.set(Math::ONE,Math::ONE,Math::ONE);
	mScope=-1;
	mBoundingRadius=0;
	mReceiveUpdates=false;

	mModifiedLogicFrame=-1;
	mModifiedVisualFrame=-1;
	mWorldModifiedLogicFrame=-1;
	mWorldModifiedVisualFrame=-1;

	mVisualTransform.reset();
	mVisualWorldBound.reset();
	mVisualWorldTransform.reset();

	return this;
}

void Entity::destroy(){
	if(mCreated==false){
		return;
	}

	Entity::ptr reference(this); // To make sure that the object is not deleted right away

	mCreated=false;

	if(mParent!=NULL){
		mParent->remove(this);
		mParent=NULL;
	}

	if(mEntityDestroyedListener!=NULL){
		mEntityDestroyedListener->entityDestroyed(this);
		setEntityDestroyedListener(NULL,false);
	}

	mReceiveUpdates=false;

	mEngine->freeEntity(this);
	mEngine=NULL;
}

void Entity::setEntityDestroyedListener(EntityDestroyedListener *listener,bool owns){
	if(mOwnsEntityDestroyedListener && mEntityDestroyedListener!=NULL){
		delete mEntityDestroyedListener;
	}
	mEntityDestroyedListener=listener;
	mOwnsEntityDestroyedListener=owns;
}

void Entity::removeAllEntityDestroyedListeners(){
	setEntityDestroyedListener(NULL,false);
}

void Entity::setTranslate(const Vector3 &translate){
	mTranslate.set(translate);

	setVisualTransformTranslate(mTranslate);

	mIdentityTransform=false;
	modified();
}

void Entity::setTranslate(scalar x,scalar y,scalar z){
	mTranslate.set(x,y,z);

	setVisualTransformTranslate(mTranslate);

	mIdentityTransform=false;
	modified();
}

void Entity::setRotate(const Matrix3x3 &rotate){
	mRotate.set(rotate);

	setVisualTransformRotateScale(mRotate,mScale);

	mIdentityTransform=false;
	modified();
}

void Entity::setRotate(scalar x,scalar y,scalar z,scalar angle){
	Math::setMatrix3x3FromAxisAngle(mRotate,cache_setRotate_vector.set(x,y,z),angle);

	setVisualTransformRotateScale(mRotate,mScale);

	mIdentityTransform=false;
	modified();
}

void Entity::setScale(const Vector3 &scale){
	mScale.set(scale);

	setVisualTransformRotateScale(mRotate,mScale);

	mIdentityTransform=false;
	modified();
}

void Entity::setScale(scalar x,scalar y,scalar z){
	mScale.set(x,y,z);

	setVisualTransformRotateScale(mRotate,mScale);

	mIdentityTransform=false;
	modified();
}

void Entity::setScope(int scope){
	mScope=scope;
}

void Entity::setBoundingRadius(scalar boundingRadius){
	mBoundingRadius=boundingRadius;
}

void Entity::setReceiveUpdates(bool receiveUpdates){
	mReceiveUpdates=receiveUpdates;
}

/// @todo  Change the modified setup so you can tell if you were modified, or if any of your parents were modified
void Entity::modified(){
	if(mEngine!=NULL){
		mModifiedLogicFrame=mEngine->getScene()->getLogicFrame();
		mModifiedVisualFrame=mEngine->getScene()->getVisualFrame();
	}
}

bool Entity::modifiedSinceLastLogicFrame() const{
	return mWorldModifiedLogicFrame+1>=mEngine->getScene()->getLogicFrame();
}

bool Entity::modifiedSinceLastVisualFrame() const{
	return mWorldModifiedVisualFrame+1>=mEngine->getScene()->getVisualFrame();
}

void Entity::setVisualTransformTranslate(const Vector3 &translate){
	Math::setMatrix4x4FromTranslate(mVisualTransform,translate);
}

void Entity::setVisualTransformRotateScale(const Matrix3x3 &rotate,const Vector3 &scale){
	Math::setMatrix4x4FromRotateScale(mVisualTransform,rotate,scale);
}

}
}
}
