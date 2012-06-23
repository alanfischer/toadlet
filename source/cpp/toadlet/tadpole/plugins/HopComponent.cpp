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

#if 0

#include "HopComponent.h"
#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{

HopComponent::HopComponent(HopManager *manager):
	mManager(manager),
	mSolid(new Solid()),
	//mOldPosition,mNewPosition,mCurrentPosition,
	mSkipNextPreSimulate(false),
	//mTraceableShape,
	mTraceable(NULL),
	//mTraceableNode,
	//mVolumeNode,
	mNextThink(0)
	//mTriggerTarget
{

	mSolid->reset();
	mSolid->setUserData(this);
	mSolid->setCollisionListener(this);

	mTraceableShape=NULL;
	mTraceable=NULL;
	mTraceableNode=NULL;
	mVolumeNode=NULL;
	mNextThink=0;
	mTriggerTarget=HopEntity::ptr();
}

void HopComponent::setPosition(const Vector3 &position){
	mSolid->setPosition(position);
	mSkipNextPreSimulate=true;
}

void HopComponent::setPositionDirect(const Vector3 &position){
	mSolid->setPositionDirect(position);
	mSkipNextPreSimulate=true;
}

void HopComponent::setVelocity(const Vector3 &velocity){
	mSolid->setVelocity(velocity);
}

void HopComponent::setNextThink(int think){
	if(think>0){
		mNextThink=mScene->getLogicTime()+think;
	}
	else{
		mNextThink=0;
	}
	setStayActive(mNextThink>0);
}

void HopComponent::setTraceableShape(Traceable *traceable,Node *traceableNode){
	if(mTraceable!=NULL){
		removeShape(mTraceableShape);
		mTraceableShape=NULL;
	}

	mTraceable=traceable;
	mTraceableNode=traceableNode;

	if(mTraceable!=NULL){
		mTraceableShape=Shape::ptr(new Shape(this));
		addShape(mTraceableShape);
	}
}

void HopComponent::addShape(hop::Shape::ptr shape){
	mSolid->addShape(shape);

	setBound(Bound(mSolid->getLocalBound()));

	updateCollisionVolumes();
}

void HopComponent::removeShape(Shape *shape){
	mSolid->removeShape(shape);

	updateCollisionVolumes();
}

void HopComponent::removeAllShapes(){
	mSolid->removeAllShapes();

	updateCollisionVolumes();
}

void HopComponent::setCollisionVolumesVisible(bool visible){
	if(visible){
		if(mVolumeNode==NULL){
			mVolumeNode=mEngine->createNodeType(Node::type(),mScene);
			attach(mVolumeNode);
		}
	}
	else{
		if(mVolumeNode!=NULL){
			mVolumeNode->destroy();
			mVolumeNode=NULL;
		}
	}

	updateCollisionVolumes();
}

// Update solids with new node positions if moved
// We dont just check active nodes, because then the solids would never deactivate, so the nodes would never deactivate
void HopComponent::preSimulate(){
	if(mSkipNextPreSimulate){
		mSkipNextPreSimulate=false;
		return;
	}

	if(getActive()==true && mNewPosition.equals(mSolid->getPosition())==false){
		mSolid->setPosition(mNewPosition);
	}
}

// Update nodes with new solid positions if active
void HopComponent::postSimulate(){
	if(mSolid->active()==true){
		activate();
		updatePosition(mSolid->getPosition());
	}
}

void HopComponent::updatePosition(const Vector3 &position){
	mOldPosition.set(mNewPosition);
	mNewPosition.set(position);
}

void HopComponent::lerpPosition(scalar fraction){
	Math::lerp(mCurrentPosition,mOldPosition,mNewPosition,mScene->getLogicFraction());
	setTranslate(mCurrentPosition);
}

void HopComponent::spacialUpdated(){
	super::spacialUpdated();

	const Vector3 &translate=mTransform.getTranslate();
	if(mCurrentPosition.equals(translate)==false){
		mOldPosition.set(translate);
		mNewPosition.set(translate);
		mCurrentPosition.set(translate);
	}
}

bool HopComponent::parentChanged(Node *node){
	if(mHopScene!=NULL && mHopScene->getSimulator()!=NULL){
		if(node==mScene->getRoot()){
			mHopScene->getSimulator()->addSolid(getSolid());
		}
		else{
			mHopScene->getSimulator()->removeSolid(getSolid());
		}
	}

	return super::parentChanged(node);
}

void HopComponent::logicUpdate(int dt,int scope){
	if(mNextThink>0 && mNextThink<=mScene->getLogicTime()){
		setNextThink(0);
		think();
	}

	super::logicUpdate(dt,scope);
}

void HopComponent::frameUpdate(int dt,int scope){
	if(mSolid->active()){
		lerpPosition(mScene->getLogicFraction());
	}

	super::frameUpdate(dt,scope);
}

void HopComponent::getBound(AABox &result){
	if(mTraceable!=NULL){
		result.set(mTraceable->getBound().getAABox());
	}
}

void HopComponent::traceSegment(hop::Collision &result,const Vector3 &position,const Segment &segment){
	if(mTraceable!=NULL){
		tadpole::Collision collision;
		mTraceable->traceSegment(collision,position,segment,Math::ZERO_VECTOR3);
		HopScene::set(result,collision,getSolid(),NULL);
	}
}

void HopComponent::traceSolid(hop::Collision &result,hop::Solid *solid,const Vector3 &position,const Segment &segment){
	if(mTraceable!=NULL){
		tadpole::Collision collision;
		const AABox &bound=solid->getLocalBound();
		Vector3 size;
		Math::sub(size,bound.maxs,bound.mins);
		mTraceable->traceSegment(collision,position,segment,size);
		HopScene::set(result,collision,getSolid(),solid);
	}
}

void HopComponent::collision(const hop::Collision &c){
	tadpole::Collision collision;
	HopScene::set(collision,c);
	touch(collision);
}

void HopComponent::updateCollisionVolumes(){
	if(mVolumeNode!=NULL){
		mVolumeNode->destroyAllChildren();

		int i;
		for(i=0;i<mSolid->getNumShapes();++i){
			Mesh::ptr mesh;

			Shape *shape=mSolid->getShape(i);
			switch(shape->getType()){
				case Shape::Type_AABOX:
					mesh=mEngine->createAABoxMesh(shape->getAABox());
				break;
				case Shape::Type_SPHERE:
					mesh=mEngine->createSphereMesh(shape->getSphere());
				break;
				default:{
					AABox box;
					shape->getBound(box);
					mesh=mEngine->createAABoxMesh(box);
				}break;
			}

			/// @todo: Use LINES for bounding volumes
			Mesh::SubMesh::ptr subMesh=mesh->getSubMesh(0);
			IndexBuffer::ptr indexBuffer=subMesh->indexData->indexBuffer;
			subMesh->indexData->setIndexBuffer(NULL);
			subMesh->indexData->destroy();
			subMesh->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_LINES,indexBuffer));
			subMesh->material->getPass()->setMaterialState(MaterialState(false));
			MeshNode *meshNode=mEngine->createNodeType(MeshNode::type(),mScene);
			meshNode->setMesh(mesh);
			mVolumeNode->attach(meshNode);
		}
	}
}

}
}

#endif
