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

#include "HopComponent.h"
#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{

HopComponent::HopComponent(HopManager *manager):
	//mManager,
	//mSolid,
	//mOldPosition,mNewPosition,mCurrentPosition,
	mTransformChanged(false)
	//mTraceableShape,
	//mTraceable
{
	mManager=manager;

	mSolid=new hop::Solid();
	mSolid->setUserData(this);
	mSolid->setCollisionListener(this);

	mBound=new Bound();
	mWorldBound=new Bound();
}

void HopComponent::destroy(){
	mSolid->setCollisionListener(NULL);
	mSolid->setUserData(NULL);

	BaseComponent::destroy();
}

void HopComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->physicsRemoved(this);
		mParent->spacialRemoved(this);
	}

	BaseComponent::parentChanged(node);

	if(mParent!=NULL){
		mParent->physicsAttached(this);
		mParent->spacialAttached(this);

		mOldPosition.set(mSolid->getPosition());
		mNewPosition.set(mOldPosition);
		lerpPosition(0);

		mParent->boundChanged();
	}
}

void HopComponent::rootChanged(Node *root){
	BaseComponent::rootChanged(root);

	if(mParent!=NULL && root!=NULL && mParent->getParent()==root){
		// Make sure solid has proper position before being inserted into simulation
		preSimulate();
		mManager->getSimulator()->addSolid(mSolid);
	}
	else{
		mManager->getSimulator()->removeSolid(mSolid);
	}
}

void HopComponent::addCollisionListener(PhysicsCollisionListener *listener){
	mListeners.push_back(listener);
}

void HopComponent::removeCollisionListener(PhysicsCollisionListener *listener){
	mListeners.erase(stlit::remove(mListeners.begin(),mListeners.end(),listener),mListeners.end());
}

void HopComponent::setPosition(const Vector3 &position){
	mSolid->setPosition(position);
}

void HopComponent::setBound(Bound *bound){
	mSolid->removeAllShapes();
	hop::Shape *shape=NULL;
	if(bound->getType()==Bound::Type_AABOX){
		shape=new hop::Shape(bound->getAABox());
	}
	else if(bound->getType()==Bound::Type_SPHERE){
		shape=new hop::Shape(bound->getSphere());
	}
	if(shape!=NULL){
		mSolid->addShape(shape);
	}

	mBound->set(mSolid->getLocalBound());
}

void HopComponent::setTraceable(PhysicsTraceable *traceable){
	if(mTraceable!=NULL){
		removeShape(mTraceableShape);
		mTraceableShape=NULL;
	}

	mTraceable=traceable;

	if(mTraceable!=NULL){
		mTraceableShape=new hop::Shape(this);
		addShape(mTraceableShape);
	}

	mBound->set(mSolid->getLocalBound());
}

void HopComponent::addShape(hop::Shape *shape){
	mSolid->addShape(shape);
}

void HopComponent::removeShape(hop::Shape *shape){
	mSolid->removeShape(shape);
}

// Update solids with new node positions if moved
// We dont just check active nodes, because then the solids would never deactivate, so the nodes would never deactivate
void HopComponent::preSimulate(){
	if(mSolid->getScope()!=mParent->getScope()){
		mSolid->setScope(mParent->getScope());
	}

	if(mTransformChanged){
		mTransformChanged=false;
		mSolid->setPosition(mNewPosition);
	}
}

// Update nodes with new solid positions if active
void HopComponent::postSimulate(){
	if(mSolid->active()==true){
		mParent->activate();

		if(mTransformChanged){
			mTransformChanged=false;
			mSolid->setPosition(mNewPosition);
		}
		else{
			updatePosition(mSolid->getPosition());
		}

		mBound->set(mSolid->getLocalBound());
	}
}

void HopComponent::updatePosition(const Vector3 &position){
	mOldPosition.set(mNewPosition);
	mNewPosition.set(position);
}

void HopComponent::lerpPosition(scalar fraction){
	Math::lerp(mCurrentPosition,mOldPosition,mNewPosition,mParent->getScene()->getLogicFraction());

	mParent->getTransform()->setTranslate(mCurrentPosition);
}

void HopComponent::transformChanged(Transform *transform){
	if(mParent==NULL){
		return;
	}

	const Vector3 &translate=mParent->getTransform()->getTranslate();
	if(mCurrentPosition.equals(translate)==false){
		mOldPosition.set(translate);
		mNewPosition.set(translate);
		mCurrentPosition.set(translate);

		mBound->set(mSolid->getLocalBound());

		mWorldBound->transform(mBound,mParent->getWorldTransform());

		mTransformChanged=true;
	}
}

void HopComponent::collision(const PhysicsCollision &collision){
	int i;
	for(i=0;i<mListeners.size();++i){
		mListeners[i]->collision(collision);
	}
}

void HopComponent::collision(const hop::Collision &c){
	HopComponent::ptr reference=this;

	PhysicsCollision pc;
	HopManager::set(pc,c);

	collision(pc);
}

void HopComponent::getBound(AABox &result){
	if(mTraceable!=NULL){
		result.set(mTraceable->getPhysicsBound()->getAABox());
	}
}

void HopComponent::traceSegment(hop::Collision &result,const Vector3 &position,const Segment &segment){
	if(mTraceable!=NULL){
		PhysicsCollision collision;
		mTraceable->tracePhysicsSegment(collision,position,segment,Math::ZERO_VECTOR3);
		HopManager::set(result,collision,mSolid,NULL);
	}
}

void HopComponent::traceSolid(hop::Collision &result,hop::Solid *solid,const Vector3 &position,const Segment &segment){
	if(mTraceable!=NULL){
		PhysicsCollision collision;
		const AABox &bound=solid->getLocalBound();

		Vector3 size;
		Math::sub(size,bound.maxs,bound.mins);

		Vector3 origin;
		Math::add(origin,bound.maxs,bound.mins);
		Math::mul(origin,Math::HALF);

		Segment adjustedSegment(segment);
		Math::add(adjustedSegment.origin,origin);

		mTraceable->tracePhysicsSegment(collision,position,adjustedSegment,size);

		Math::sub(collision.point,origin);

		HopManager::set(result,collision,mSolid,solid);
	}
}

}
}
