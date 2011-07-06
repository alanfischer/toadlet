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

namespace toadlet{
namespace tadpole{

TOADLET_NODE_IMPLEMENT(HopEntity,Categories::TOADLET_TADPOLE+".HopEntity");

HopEntity::HopEntity():ParentNode(),
	mSolid(new Solid()),
	//mOldPosition,mNewPosition,mCurrentPosition,
	mSkipNextPreSimulate(false),
	//mTraceableShape,
	mTraceable(NULL),
	//mTraceableNode,
	//mVolumeNode,
	mNextThink(0)
	//mTriggerTarget
{}

Node *HopEntity::create(Scene *scene){
	super::create(scene);

	mSolid->reset();
	mSolid->setUserData(this);
	mSolid->setCollisionListener(this);

	mTraceableShape=NULL;
	mTraceable=NULL;
	mTraceableNode=NULL;
	mVolumeNode=NULL;
	mNextThink=0;
	mTriggerTarget=HopEntity::ptr();

	/// @todo: I need to remove the mScene casting here, its pretty dirty
	mHopScene=(HopScene*)(mScene);
	if(mHopScene==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Invalid scene");
		return this;
	}

	return this;
}

void HopEntity::setCollisionBits(int bits){
	mSolid->setCollisionBits(bits);
}

void HopEntity::setCollideWithBits(int bits){
	mSolid->setCollideWithBits(bits);
}

void HopEntity::setMass(scalar mass){
	mSolid->setMass(mass);
}

void HopEntity::setInfiniteMass(){
	mSolid->setInfiniteMass();
}

void HopEntity::setPosition(const Vector3 &position){
	mSolid->setPosition(position);
	mSkipNextPreSimulate=true;
}

void HopEntity::setPositionDirect(const Vector3 &position){
	mSolid->setPositionDirect(position);
	mSkipNextPreSimulate=true;
}

void HopEntity::setVelocity(const Vector3 &velocity){
	mSolid->setVelocity(velocity);
}

void HopEntity::addForce(const Vector3 &force){
	mSolid->addForce(force);
}

void HopEntity::clearForce(){
	mSolid->clearForce();
}

void HopEntity::setCoefficientOfGravity(scalar coeff){
	mSolid->setCoefficientOfGravity(coeff);
}

void HopEntity::setCoefficientOfRestitution(scalar coeff){
	mSolid->setCoefficientOfRestitution(coeff);
}

void HopEntity::setCoefficientOfRestitutionOverride(bool over){
	mSolid->setCoefficientOfRestitutionOverride(over);
}

void HopEntity::setCoefficientOfStaticFriction(scalar coeff){
	mSolid->setCoefficientOfStaticFriction(coeff);
}

void HopEntity::setCoefficientOfDynamicFriction(scalar coeff){
	mSolid->setCoefficientOfDynamicFriction(coeff);
}

void HopEntity::setCoefficientOfEffectiveDrag(scalar coeff){
	mSolid->setCoefficientOfEffectiveDrag(coeff);
}

void HopEntity::setNextThink(int think){
	if(think>0){
		mNextThink=mScene->getLogicTime()+think;
	}
	else{
		mNextThink=0;
	}
	setStayActive(mNextThink>0);
}

void HopEntity::setTraceableShape(Traceable *traceable,Node *traceableNode){
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

void HopEntity::addShape(hop::Shape::ptr shape){
	mSolid->addShape(shape);

	setBound(Bound(mSolid->getLocalBound()));

	updateCollisionVolumes();
}

void HopEntity::removeShape(Shape *shape){
	mSolid->removeShape(shape);

	updateCollisionVolumes();
}

void HopEntity::removeAllShapes(){
	mSolid->removeAllShapes();

	updateCollisionVolumes();
}

void HopEntity::setCollisionVolumesVisible(bool visible){
	if(visible){
		if(mVolumeNode==NULL){
			mVolumeNode=mEngine->createNodeType(ParentNode::type(),mScene);
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
void HopEntity::preSimulate(){
	if(mSkipNextPreSimulate){
		mSkipNextPreSimulate=false;
		return;
	}

	if(active()==true && mNewPosition.equals(mSolid->getPosition())==false){
		mSolid->setPosition(mNewPosition);
	}
}

// Update nodes with new solid positions if active
void HopEntity::postSimulate(){
	if(mSolid->active()==true){
		activate();
		updatePosition(mSolid->getPosition());
	}
}

void HopEntity::updatePosition(const Vector3 &position){
	mOldPosition.set(mNewPosition);
	mNewPosition.set(position);
}

void HopEntity::lerpPosition(scalar fraction){
	Math::lerp(mCurrentPosition,mOldPosition,mNewPosition,mScene->getLogicFraction());
	setTranslate(mCurrentPosition);
}

void HopEntity::spacialUpdated(){
	super::spacialUpdated();

	const Vector3 &translate=mTransform.getTranslate();
	if(mCurrentPosition.equals(translate)==false){
		mOldPosition.set(translate);
		mNewPosition.set(translate);
		mCurrentPosition.set(translate);
	}
}

void HopEntity::parentChanged(ParentNode *parent){
	if(mHopScene!=NULL && mHopScene->getSimulator()!=NULL){
		if(parent==mScene->getRoot()){
			mHopScene->getSimulator()->addSolid(getSolid());
		}
		else{
			mHopScene->getSimulator()->removeSolid(getSolid());
		}
	}

	super::parentChanged(parent);
}

void HopEntity::logicUpdate(int dt,int scope){
	if(mNextThink>0 && mNextThink<=mScene->getLogicTime()){
		setNextThink(0);
		think();
	}

	super::logicUpdate(dt,scope);
}

void HopEntity::frameUpdate(int dt,int scope){
	if(mSolid->active()){
		lerpPosition(mScene->getLogicFraction());
	}

	super::frameUpdate(dt,scope);
}

void HopEntity::getBound(AABox &result){
	if(mTraceable!=NULL){
		result.set(mTraceable->getBound().getAABox());
	}
}

void HopEntity::traceSegment(hop::Collision &result,const Vector3 &position,const Segment &segment){
	if(mTraceable!=NULL){
		tadpole::Collision collision;
		mTraceable->traceSegment(collision,position,segment,Math::ZERO_VECTOR3);
		HopScene::set(result,collision,getSolid(),NULL);
	}
}

void HopEntity::traceSolid(hop::Collision &result,hop::Solid *solid,const Vector3 &position,const Segment &segment){
	if(mTraceable!=NULL){
		tadpole::Collision collision;
		const AABox &bound=solid->getLocalBound();
		Vector3 size;
		Math::sub(size,bound.maxs,bound.mins);
		mTraceable->traceSegment(collision,position,segment,size);
		HopScene::set(result,collision,getSolid(),solid);
	}
}

void HopEntity::collision(const hop::Collision &c){
	tadpole::Collision collision;
	HopScene::set(collision,c);
	touch(collision);
}

void HopEntity::updateCollisionVolumes(){
	if(mVolumeNode!=NULL){
		mVolumeNode->destroyAllChildren();

		int i;
		for(i=0;i<mSolid->getNumShapes();++i){
			Mesh::ptr mesh;

			Shape *shape=mSolid->getShape(i);
			switch(shape->getType()){
				case Shape::Type_AABOX:
					mesh=mEngine->getMeshManager()->createBox(shape->getAABox());
				break;
				case Shape::Type_SPHERE:
					mesh=mEngine->getMeshManager()->createSphere(shape->getSphere());
				break;
				default:{
					AABox box;
					shape->getBound(box);
					mesh=mEngine->getMeshManager()->createBox(box);
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
