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

#include <toadlet/tadpole/node/Node.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/egg/Error.h>

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
	mHandle(0),

	mNodeDestroyedListener(NULL),
	mOwnsNodeDestroyedListener(false),

	//mParent,
	mParentData(NULL),
	mIdentityTransform(false),
	//mTranslate,
	//mRotate,
	//mScale,
	//mWorldTranslate,
	//mWorldRotate,
	//mWorldScale,
	mScope(0),
	//mName,
	mCameraAligned(false),
	mPerspective(true),
	
	mActive(false),
	mDeactivateCount(0)

	//mLocalBound,
	//mWorldBound,

	//mRenderTransform,
	//mWorldRenderTransform
{
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
	mHandle=mScene->nodeCreated(this);

	mNodeDestroyedListener=NULL;
	mOwnsNodeDestroyedListener=false;

	mParent=NULL;
	mParentData=NULL;
	mIdentityTransform=true;
	mTranslate.reset();
	mRotate.reset();
	mScale.set(Math::ONE,Math::ONE,Math::ONE);
	mWorldTranslate.reset();
	mWorldRotate.reset();
	mWorldScale.set(Math::ONE,Math::ONE,Math::ONE);
	mScope=-1;
	mName="";
	mCameraAligned=false;
	mPerspective=true;

	mActive=true;
	mDeactivateCount=0;

	mLocalBound.reset();
	mWorldBound.reset();

	mRenderTransform.reset();
	mWorldRenderTransform.reset();

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

	if(mNodeDestroyedListener!=NULL){
		mNodeDestroyedListener->nodeDestroyed(this);
		setNodeDestroyedListener(NULL,false);
	}

	mScene->nodeDestroyed(this);

	mEngine->freeNode(this);
	mEngine=NULL;
	mScene=NULL;
}

void Node::setNodeDestroyedListener(NodeDestroyedListener *listener,bool owns){
	if(mOwnsNodeDestroyedListener && mNodeDestroyedListener!=NULL){
		delete mNodeDestroyedListener;
	}
	mNodeDestroyedListener=listener;
	mOwnsNodeDestroyedListener=owns;
}

void Node::removeAllNodeDestroyedListeners(){
	setNodeDestroyedListener(NULL,false);
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
	setRenderTransformTranslate(mTranslate);
	transformUpdated();
}

void Node::setTranslate(scalar x,scalar y,scalar z){
	mTranslate.set(x,y,z);
	setRenderTransformTranslate(mTranslate);
	transformUpdated();
}

void Node::setRotate(const Matrix3x3 &rotate){
	Math::setQuaternionFromMatrix3x3(mRotate,rotate);
	setRenderTransformRotateScale(mRotate,mScale);
	transformUpdated();
}

void Node::setRotate(const Quaternion &rotate){
	mRotate.set(rotate);
	setRenderTransformRotateScale(mRotate,mScale);
	transformUpdated();
}

void Node::setRotate(scalar x,scalar y,scalar z,scalar angle){
	Math::setQuaternionFromAxisAngle(mRotate,cache_setRotate_vector.set(x,y,z),angle);
	setRenderTransformRotateScale(mRotate,mScale);
	transformUpdated();
}

void Node::setScale(const Vector3 &scale){
	mScale.set(scale);
	setRenderTransformRotateScale(mRotate,mScale);
	transformUpdated();
}

void Node::setScale(scalar x,scalar y,scalar z){
	mScale.set(x,y,z);
	setRenderTransformRotateScale(mRotate,mScale);
	transformUpdated();
}

void Node::setScale(scalar s){
	mScale.set(s,s,s);
	setRenderTransformRotateScale(mRotate,mScale);
	transformUpdated();
}

void Node::setTransform(const Matrix4x4 &transform){
	Math::setScaleFromMatrix4x4(mScale,transform);
	Math::setRotateFromMatrix4x4(cache_setTransform_matrix,transform,mScale);
	Math::setQuaternionFromMatrix3x3(mRotate,cache_setTransform_matrix);
	Math::setTranslateFromMatrix4x4(mTranslate,transform);
	mRenderTransform.set(transform);
	transformUpdated();
}

// Currently just uses the worldRenderTransform, should be changed to walk up the scene till it finds a common parent,
//  and then calculate the transform walking that path.
void Node::findTransformTo(Matrix4x4 &result,Node *node){
	Math::invert(result,node->mWorldRenderTransform);
	Math::postMul(result,mWorldRenderTransform);
}

void Node::setLocalBound(const Sphere &bound){
	mLocalBound.set(bound);
	transformUpdated();
}

void Node::logicUpdate(int dt){
}

void Node::renderUpdate(CameraNode *camera,RenderQueue *queue){
}

void Node::updateLogicTransforms(){
	if(mParent==NULL){
		mWorldScale.set(mScale);
		mWorldRotate.set(mRotate);
		mWorldTranslate.set(mTranslate);
	}
	else if(mIdentityTransform){
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

	mul(mWorldBound,mWorldTranslate,mWorldRotate,mWorldScale,mLocalBound);
	if(mLocalBound.radius<0 || mPerspective==false) mWorldBound.radius=-Math::ONE;
}

void Node::updateRenderTransforms(){
	if(mParent==NULL){
		mWorldRenderTransform.set(mRenderTransform);
	}
	else if(mIdentityTransform){
		mWorldRenderTransform.set(mParent->mWorldRenderTransform);
	}
	else{
		Math::mul(mWorldRenderTransform,mParent->mWorldRenderTransform,mRenderTransform);
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

void Node::setRenderTransformTranslate(const Vector3 &translate){
	Math::setMatrix4x4FromTranslate(mRenderTransform,translate);
}

void Node::setRenderTransformRotateScale(const Quaternion &rotate,const Vector3 &scale){
	Math::setMatrix3x3FromQuaternion(cache_setTransform_matrix,rotate);
	Math::setMatrix4x4FromRotateScale(mRenderTransform,cache_setTransform_matrix,scale);
}

void Node::transformUpdated(){
	mIdentityTransform=false;
	activate();
	if(mParent!=NULL){
		shared_static_cast<ParentNode>(mParent)->childTransformUpdated(this);
	}
}

}
}
}
