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
#include <toadlet/tadpole/node/SceneNode.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::hop;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::node;
using namespace toadlet::tadpole::mesh;

namespace toadlet{
namespace tadpole{

TOADLET_NODE_IMPLEMENT(HopEntity,Categories::TOADLET_TADPOLE+".HopEntity");

HopEntity::HopEntity():ParentNode(),
	mNextThink(0),
	mSolid(new Solid()),
	//mTraceableShape,
	mTraceable(NULL),

	//mHopScene,
	//mLastPosition,
	mActivePrevious(false),

	mShadowTestLength(0),
	mShadowOffset(0)
	//mShadowMesh,
	//mShadowMaterial,
	//mShadowNode
{}

Node *HopEntity::create(Scene *scene){
	super::create(scene);

	// No need to worry about this
	mIdentityTransform=false;

	mNextThink=0;
	mSolid->reset();
	mSolid->setUserData(this);
	mHopScene=NULL;
	mLastPosition.reset();
	mActivePrevious=true;

	mShadowTestLength=0;
	mShadowOffset=0;
	mShadowMesh=NULL;
	mShadowMaterial=NULL;
	mShadowNode=NULL;

	mIdentityTransform=false;

	mHopScene=(HopScene*)(mScene);

	if(mHopScene==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Invalid scene");
		return this;
	}

	mHopScene->nodeCreated(this);

	return this;
}

void HopEntity::destroy(){
	if(mHopScene!=NULL){
		mHopScene->nodeDestroyed(this);
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
}

void HopEntity::setInfiniteMass(){
	mSolid->setInfiniteMass();
}

void HopEntity::setTranslate(const Vector3 &translate){
	super::setTranslate(translate);

	mSolid->setPosition(mTranslate);

	mLastPosition.set(mTranslate);
}

void HopEntity::setTranslate(scalar x,scalar y,scalar z){
	mTranslate.set(x,y,z);

	setTranslate(mTranslate);
}

void HopEntity::setTransform(const Matrix4x4 &transform){
	super::setTransform(transform);

	mSolid->setPosition(mTranslate);

	mLastPosition.set(mTranslate);
}

void HopEntity::setVelocity(const Vector3 &velocity){
	mSolid->setVelocity(velocity);
}

void HopEntity::setVelocity(scalar x,scalar y,scalar z){
	Vector3 &velocity=cache_setVelocity_velocity.set(x,y,z);
	setVelocity(velocity);
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

void HopEntity::setTraceableShape(Traceable *traceable){
	if(mTraceable!=NULL){
		removeShape(mTraceableShape);
		mTraceableShape=NULL;
	}

	mTraceable=traceable;

	if(mTraceable!=NULL){
		mTraceableShape=Shape::ptr(new Shape(this));
		addShape(mTraceableShape);
	}
}

void HopEntity::addShape(hop::Shape::ptr shape){
	mSolid->addShape(shape);

	Sphere bound;
	set(bound,mSolid->getLocalBound());
	setLocalBound(bound);

	if(mVolumeNode!=NULL){
		showCollisionVolumes(true);
	}
}

void HopEntity::removeShape(Shape *shape){
	mSolid->removeShape(shape);

	if(mVolumeNode!=NULL){
		showCollisionVolumes(true);
	}
}

void HopEntity::removeAllShapes(){
	mSolid->removeAllShapes();

	if(mVolumeNode!=NULL){
		showCollisionVolumes(true);
	}
}

void HopEntity::think(){
}

void HopEntity::touch(const tadpole::Collision &c){
}

void HopEntity::setShadowMesh(Mesh::ptr shadow,scalar scale,scalar testLength,scalar offset){
	mShadowMesh=shadow;
	mShadowTestLength=testLength;
	mShadowOffset=offset;
	if(mShadowMesh!=NULL){
		if(mShadowNode==NULL){
			mShadowNode=mEngine->createNodeType(MeshNode::type(),mScene);
			mScene->getRootNode()->attach(mShadowNode);
		}
		mShadowNode->setMesh(mShadowMesh);
		mShadowNode->setScale(scale,scale,scale);
		// We assume the shadow only has 1 subMesh
		mShadowMaterial=mShadowNode->getSubMesh(0)->material;
		mShadowMaterial->setBlend(Blend::Combination_ALPHA);
	}
	else{
		if(mShadowNode!=NULL){
			mShadowNode->destroy();
			mShadowNode=NULL;
		}
	}
}

void HopEntity::parentChanged(ParentNode *parent){
	if(mHopScene!=NULL){
		if(parent==mScene->getRootNode()){
			mHopScene->getSimulator()->addSolid(getSolid());
		}
		else{
			mHopScene->getSimulator()->removeSolid(getSolid());
		}
	}

	super::parentChanged(parent);
}

void HopEntity::getBound(AABox &result){
	if(mTraceable!=NULL){
		const Sphere &bound=mTraceable->getLocalBound();
		result.set(bound.radius);
		Math::add(result,bound.origin);
	}
}

void HopEntity::traceSegment(hop::Collision &result,const Segment &segment){
	if(mTraceable!=NULL){
		tadpole::Collision collision;
		mTraceable->traceSegment(collision,segment,Math::ZERO_VECTOR3);
		HopScene::set(result,collision,this);
	}
}

void HopEntity::traceSolid(hop::Collision &result,const Segment &segment,const hop::Solid *solid){
	if(mTraceable!=NULL){
		tadpole::Collision collision;
		const AABox &bound=solid->getLocalBound();
		Vector3 size;
		Math::sub(size,bound.maxs,bound.mins);
		mTraceable->traceSegment(collision,segment,size);
		HopScene::set(result,collision,this);
	}
}

void HopEntity::collision(const hop::Collision &c){
	tadpole::Collision collision;
	mHopScene->set(collision,c);
	touch(collision);
}

void HopEntity::preLogicUpdateLoop(int dt){
	mLastPosition.set(mSolid->getPosition());
	mActivePrevious=mSolid->active();
}

void HopEntity::postLogicUpdate(int dt){
	if(mSolid->active()){
		mTranslate.set(mSolid->getPosition());
		activate();
	}

	if(mNextThink>0){
		mNextThink-=dt;
		if(mNextThink<=0){
			mNextThink=0;
			think();
		}
	}
}

void HopEntity::renderUpdate(CameraNode *camera,RenderQueue *queue){
	bool active=mSolid->active();
	bool activePrevious=mActivePrevious;
	if(active || activePrevious){
		if(mShadowMesh!=NULL){
			castShadow();
		}
		if(mVolumeNode!=NULL){
			updateVolumes(mHopScene->mInterpolateCollisionVolumes);
		}
	}
}

void HopEntity::updateRenderTransforms(){
	scalar f=mScene->getLogicFraction();

	bool active=mSolid->active();
	bool activePrevious=mActivePrevious;
	if(active || activePrevious){
		// TODO: Add an option to either use strict interpolation, or fuzzy interpolation
		// If we are deactivating, then make sure we are at our rest point
#if 1
		if(active==false && activePrevious){
			interpolatePhysicalParameters(Math::ONE);
		}
		else{
			interpolatePhysicalParameters(f);
		}
#elif 0
		// TODO: This needs to be based on the logicFraction SOMEHOW,otherwise the result of calling this method multiple timer per frame will be bad

		Vector3 last;Math::setTranslateFromMatrix4x4(last,entity->getRenderTransform());
		Vector3 translate;Math::lerp(translate,last,entity->getTranslate(),0.3);
		setRenderTransformTranslate(translate);
#else
		setRenderTransformTranslate(getTranslate());
#endif
	}

	super::updateRenderTransforms();
}

void HopEntity::interpolatePhysicalParameters(scalar f){
	Vector3 &interpolate=cache_interpolatePhysicalParameters_interpolate;
	Math::lerp(interpolate,mLastPosition,mSolid->getPosition(),f);
	setRenderTransformTranslate(interpolate);
}

void HopEntity::castShadow(){
	Segment segment;
	Vector3 vector;
	Matrix3x3 rotate;
	Math::setTranslateFromMatrix4x4(segment.origin,mRenderTransform);
	Math::setMatrix3x3FromMatrix4x4(rotate,mRenderTransform);
	Math::normalize(vector,mSolid->getSimulator()->getGravity());
	Math::mul(segment.direction,vector,mShadowTestLength);

	hop::Collision collision;
	mSolid->getSimulator()->traceSegment(collision,segment,-1,mSolid);
	// Shadow if the object beneath us is of infinite mass (eg static)
	if(collision.collider!=NULL && collision.collider->hasInfiniteMass()){
		Math::mul(vector,collision.normal,mShadowOffset);
		Math::add(collision.point,vector);
		mShadowNode->setTranslate(collision.point);

		Vector3 zAxis;
		Math::mul(zAxis,rotate,Math::Z_UNIT_VECTOR3);
		Matrix3x3 shadowRotate;
		Math::setMatrix3x3FromVector3ToVector3(shadowRotate,collision.normal,zAxis,Math::ONE/2048);
		Math::postMul(rotate,shadowRotate);
		mShadowNode->setRotate(rotate);

		LightEffect le(Color(0,0,0,Math::ONE-collision.time));
		mShadowMaterial->setLightEffect(le);
	}
	else{
		LightEffect le(Colors::TRANSPARENT_BLACK);
		mShadowMaterial->setLightEffect(le);
	}
}

void HopEntity::showCollisionVolumes(bool show){
	if(show){
		if(mVolumeNode==NULL){
			mVolumeNode=mEngine->createNodeType(ParentNode::type(),mScene);
			mScene->getRootNode()->attach(mVolumeNode);
		}
		else{
			mVolumeNode->destroyAllChildren();
		}

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

			// TODO: Use LINES for bounding volumes
			IndexBuffer::ptr indexBuffer=mesh->subMeshes[0]->indexData->indexBuffer;
			mesh->subMeshes[0]->indexData->setIndexBuffer(NULL);
			mesh->subMeshes[0]->indexData->destroy();
			mesh->subMeshes[0]->indexData=IndexData::ptr(new IndexData(IndexData::Primitive_LINES,indexBuffer));
			mesh->subMeshes[0]->material->setLighting(false);
			MeshNode *meshNode=mEngine->createNodeType(MeshNode::type(),mScene);
			meshNode->setMesh(mesh);
			mVolumeNode->attach(meshNode);
		}
	}
	else{
		if(mVolumeNode!=NULL){
			mVolumeNode->destroy();
			mVolumeNode=NULL;
		}
	}
}

void HopEntity::updateVolumes(bool interpolate){
	if(interpolate){
		Vector3 translate;
		Math::setTranslateFromMatrix4x4(translate,mRenderTransform);
		mVolumeNode->setTranslate(translate);
	}
	else{
		mVolumeNode->setTranslate(getTranslate());
	}
}

}
}
