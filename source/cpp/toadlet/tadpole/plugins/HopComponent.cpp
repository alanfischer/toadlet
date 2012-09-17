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
//	mTraceable(NULL)
{
	mManager=manager;

	mSolid=new hop::Solid();
	mSolid->setUserData(this);
	mSolid->setCollisionListener(this);

	mBound=new Bound();

	setName("physics");
}

void HopComponent::destroy(){
	BaseComponent::destroy();
}

void HopComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->physicsRemoved(this);
	}

	BaseComponent::parentChanged(node);

	if(mParent!=NULL){
		mParent->physicsAttached(this);
	}
}

void HopComponent::rootChanged(Node *root){
	if(root!=NULL){
		mManager->getSimulator()->addSolid(mSolid);
	}
	else{
		mManager->getSimulator()->removeSolid(mSolid);
	}
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
}

void HopComponent::setTraceable(Traceable *traceable){
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
	/// @todo: Should this be elsewhere, or at least optional
	mSolid->setScope(mParent->getScope());

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
	mParent->setTranslate(mCurrentPosition);
}

void HopComponent::transformChanged(){
	const Vector3 &translate=mParent->getTranslate();
	if(mCurrentPosition.equals(translate)==false){
		mOldPosition.set(translate);
		mNewPosition.set(translate);
		mCurrentPosition.set(translate);

		mBound->set(mSolid->getLocalBound());

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
		result.set(mTraceable->getTraceableBound()->getAABox());
	}
}

void HopComponent::traceSegment(hop::Collision &result,const Vector3 &position,const Segment &segment){
	if(mTraceable!=NULL){
		PhysicsCollision collision;
		mTraceable->traceSegment(collision,position,segment,Math::ZERO_VECTOR3);
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

		mTraceable->traceSegment(collision,position,adjustedSegment,size);

		Math::sub(collision.point,origin);

		HopManager::set(result,collision,mSolid,solid);
	}
}

}
}
