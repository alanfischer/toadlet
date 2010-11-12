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

#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/node/Node.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/ParentNode.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(Node,Categories::TOADLET_TADPOLE_NODE+".Node");


Node::Node():
	mPointerCounter(new PointerCounter(0)),
	mManaged(false),

	mCreated(false),
	mEngine(NULL),
	mScene(NULL),
	mUniqueHandle(0),

	//mNodeListeners,

	//mParent,
	mParentData(NULL),

	//mDependsUpon,

	mTransformUpdatedFrame(0),
	//mTranslate,
	//mRotate,
	//mScale,
	//mWorldTranslate,
	//mWorldRotate,
	//mWorldScale,
	mScope(0),
	//mName,
	
	mActive(false),
	mDeactivateCount(0)

	//mLocalBound,
	//mWorldBound,

	//mRenderTransform,
	//mWorldRenderTransform
{
//	TOADLET_PROPERTY_INIT(translate);
//	TOADLET_PROPERTY_INIT(rotate);
//	TOADLET_PROPERTY_INIT(scale);
//	TOADLET_PROPERTY_INIT(scope);
//	TOADLET_PROPERTY_INIT(name);
}

Node::~Node(){
	TOADLET_ASSERT(!mCreated);
}

Node *Node::create(Scene *scene){
	if(mCreated){
		return this;
	}

	mCreated=true;
	mEngine=scene->getEngine();
	mScene=scene;
	mUniqueHandle=mScene->nodeCreated(this);

	mNodeListeners=NULL;

	mParent=NULL;
	mParentData=NULL;

	mDependsUpon=NULL;

	mTransformUpdatedFrame=-1;
	mTranslate.reset();
	mRotate.reset();
	mScale.set(Math::ONE,Math::ONE,Math::ONE);
	mWorldTranslate.reset();
	mWorldRotate.reset();
	mWorldScale.set(Math::ONE,Math::ONE,Math::ONE);
	mWorldTransform.reset();
	mScope=-1;
	mName="";

	mActive=true;
	mDeactivateCount=0;

	mLocalBound.reset();
	mWorldBound.reset();

	return this;
}

void Node::destroy(){
	if(mCreated==false){
		return;
	}

	Node::ptr reference(this); // To make sure that the object is not deleted right away

	mCreated=false;

	if(mParent!=NULL){
		((ParentNode*)mParent.get())->remove(this);
		mParent=NULL;
	}

	mDependsUpon=NULL;

	if(mNodeListeners!=NULL){
		int i;
		for(i=0;i<mNodeListeners->size();++i){
			mNodeListeners->at(i)->nodeDestroyed(this);
		}
		mNodeListeners=NULL;
	}

	mScene->nodeDestroyed(this);

	mEngine->freeNode(this);
	mEngine=NULL;
	mScene=NULL;
	mUniqueHandle=0;
}

void Node::addNodeListener(NodeListener::ptr listener){
	if(mNodeListeners==NULL){
		mNodeListeners=Collection<NodeListener::ptr>::ptr(new egg::Collection<NodeListener::ptr>());
	}
	mNodeListeners->add(listener);
}

void Node::removeNodeListener(NodeListener::ptr listener){
	if(mNodeListeners!=NULL){
		mNodeListeners->remove(listener);
		if(mNodeListeners->size()==0){
			mNodeListeners=NULL;
		}
	}
}

void Node::parentChanged(ParentNode *parent){
	mParent=(Node*)parent;
}

ParentNode *Node::getParent() const{
	return (ParentNode*)mParent.get();
}

void Node::parentDataChanged(void *parentData){
	mParentData=parentData;
}

void Node::setTranslate(const Vector3 &translate){
	mTranslate.set(translate);
	transformUpdated();
}

void Node::setTranslate(scalar x,scalar y,scalar z){
	mTranslate.set(x,y,z);
	setTranslate(mTranslate);
}

void Node::setRotate(const Quaternion &rotate){
	mRotate.set(rotate);
	transformUpdated();
}

void Node::setRotate(const Matrix3x3 &rotate){
	Math::setQuaternionFromMatrix3x3(mRotate,rotate);
	setRotate(mRotate);
}

void Node::setRotate(scalar x,scalar y,scalar z,scalar angle){
	Math::setQuaternionFromAxisAngle(mRotate,cache_setRotate_vector.set(x,y,z),angle);
	setRotate(mRotate);
}

void Node::setScale(const Vector3 &scale){
	mScale.set(scale);
	transformUpdated();
}

void Node::setScale(scalar x,scalar y,scalar z){
	mScale.set(x,y,z);
	setScale(mScale);
}

void Node::setScale(scalar s){
	mScale.set(s,s,s);
	setScale(mScale);
}

void Node::setTransform(const Matrix4x4 &transform){
	Math::setScaleFromMatrix4x4(mScale,transform);
	setScale(mScale);
	Math::setRotateFromMatrix4x4(cache_setTransform_matrix,transform,mScale);
	Math::setQuaternionFromMatrix3x3(mRotate,cache_setTransform_matrix);
	setRotate(mRotate);
	Math::setTranslateFromMatrix4x4(mTranslate,transform);
	setTranslate(mTranslate);
}

void Node::findTransformTo(Matrix4x4 &result,Node *node){
	Matrix3x3 rotate;
	Matrix4x4 transform;
	Math::setMatrix3x3FromQuaternion(rotate,getWorldRotate());
	Math::setMatrix4x4FromTranslateRotateScale(transform,getWorldTranslate(),rotate,getWorldScale());
	Matrix4x4 nodeTransform;
	Math::setMatrix3x3FromQuaternion(rotate,node->getWorldRotate());
	Math::setMatrix4x4FromTranslateRotateScale(nodeTransform,node->getWorldTranslate(),rotate,node->getWorldScale());
	Math::invert(result,nodeTransform);
	Math::postMul(result,transform);
}

void Node::setLocalBound(const Bound &bound){
	mLocalBound.set(bound);
	transformUpdated();
}

void Node::logicUpdate(int dt,int scope){
	mLastLogicFrame=mScene->getLogicFrame();

	logicUpdateListeners(dt);

	// If we're updating the world transforms in Node, then we also need to keep the
	// world bound merging updated in ParentNode.
	updateWorldTransform();
}

void Node::frameUpdate(int dt,int scope){
	mLastFrame=mScene->getFrame();

	frameUpdateListeners(dt);

	updateWorldTransform();
}

void Node::setStayActive(bool active){
	if(active){
		activate();
		mDeactivateCount=-1;
	}
	else{
		mDeactivateCount=0;
	}
}

void Node::activate(){
	if(mDeactivateCount>0){
		mDeactivateCount=0;
	}
	if(mActive==false){
		mActive=true;
		if(mParent!=NULL){
			mParent->activate();
		}
	}
}

void Node::deactivate(){
	mActive=false;
	mDeactivateCount=0;
}

void Node::tryDeactivate(){
	if(mDeactivateCount>=0 && mNodeListeners==NULL){
		mDeactivateCount++;
		if(mDeactivateCount>4){
			deactivate();
		}
	}
}

bool Node::getTransformUpdated(){return mScene->getFrame()==mTransformUpdatedFrame;}

void Node::updateWorldTransform(){
	if(mParent==NULL){
		mWorldScale.set(mScale);
		mWorldRotate.set(mRotate);
		mWorldTranslate.set(mTranslate);
	}
	else if(mTransformUpdatedFrame==-1){
		mWorldScale.set(mParent->mScale);
		mWorldRotate.set(mParent->mRotate);
		mWorldTranslate.set(mParent->mTranslate);
	}
	else{
		Math::mul(mWorldScale,mParent->mWorldScale,mScale);
		Math::mul(mWorldRotate,mParent->mWorldRotate,mRotate);
		Math::mul(mWorldTranslate,mParent->mWorldRotate,mTranslate);
		Math::mul(mWorldTranslate,mParent->mWorldScale);
		Math::add(mWorldTranslate,mParent->mWorldTranslate);
	}

	Bound::transform(mWorldBound,mLocalBound,mWorldTranslate,mWorldRotate,mWorldScale);

	/// @todo: add a setMatrix4x4FromTRS that can take a quat?
	Math::setMatrix3x3FromQuaternion(cache_setTransform_matrix,mWorldRotate);
	Math::setMatrix4x4FromTranslateRotateScale(mWorldTransform,mWorldTranslate,cache_setTransform_matrix,mWorldScale);
}

void Node::transformUpdated(){
	mTransformUpdatedFrame=mScene->getFrame();
	activate();
}

void Node::logicUpdateListeners(int dt){
	if(mNodeListeners!=NULL){
		int i;
		for(i=0;i<mNodeListeners->size();++i){
			mNodeListeners->at(i)->logicUpdate(this,dt);
		}
	}
}

void Node::frameUpdateListeners(int dt){
	if(mNodeListeners!=NULL){
		int i;
		for(i=0;i<mNodeListeners->size();++i){
			mNodeListeners->at(i)->frameUpdate(this,dt);
		}
	}
}

}
}
}
