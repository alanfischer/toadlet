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

TOADLET_NODE_IMPLEMENT(Node,"toadlet::tadpole::node::Node");

Node::Node():
	mCounter(new PointerCounter(0)),
	mManaged(false),

	mCreated(false),
	mEngine(NULL),

	mNodeDestroyedListener(NULL),
	mOwnsNodeDestroyedListener(false),

	//mParent,

	mIdentityTransform(false),
	//mScale,
	mScope(0),
	//mName,
	mAlignXAxis(false),mAlignYAxis(false),mAlignZAxis(false),
	mBoundingRadius(0),
	mReceiveUpdates(false),
	mAwakeCount(0)
{
}

Node::~Node(){
	TOADLET_ASSERT(!mCreated);
}

Node *Node::create(Engine *engine){
	if(mCreated){
		return this;
	}

	mCreated=true;
	mEngine=engine;

	mNodeDestroyedListener=NULL;
	mOwnsNodeDestroyedListener=false;

	mParent=NULL;

	mIdentityTransform=true;
	mTranslate.reset();
	mRotate.reset();
	mScale.set(Math::ONE,Math::ONE,Math::ONE);
	mScope=-1;
	mBoundingRadius=0;
	mReceiveUpdates=false;
	mAwakeCount=2;

	mRenderTransform.reset();
	mRenderWorldBound.reset();
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
		mParent->remove(this);
		mParent=NULL;
	}

	if(mNodeDestroyedListener!=NULL){
		mNodeDestroyedListener->nodeDestroyed(this);
		setNodeDestroyedListener(NULL,false);
	}

	mReceiveUpdates=false;

	mEngine->freeNode(this);
	mEngine=NULL;
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

void Node::setTranslate(const Vector3 &translate){
	mTranslate.set(translate);

	setRenderTransformTranslate(mTranslate);

	mIdentityTransform=false;
	modified();
}

void Node::setTranslate(scalar x,scalar y,scalar z){
	mTranslate.set(x,y,z);

	setRenderTransformTranslate(mTranslate);

	mIdentityTransform=false;
	modified();
}

void Node::setRotate(const Matrix3x3 &rotate){
	mRotate.set(rotate);

	setRenderTransformRotateScale(mRotate,mScale);

	mIdentityTransform=false;
	modified();
}

void Node::setRotate(scalar x,scalar y,scalar z,scalar angle){
	Math::setMatrix3x3FromAxisAngle(mRotate,cache_setRotate_vector.set(x,y,z),angle);

	setRenderTransformRotateScale(mRotate,mScale);

	mIdentityTransform=false;
	modified();
}

void Node::setScale(const Vector3 &scale){
	mScale.set(scale);

	setRenderTransformRotateScale(mRotate,mScale);

	mIdentityTransform=false;
	modified();
}

void Node::setScale(scalar x,scalar y,scalar z){
	mScale.set(x,y,z);

	setRenderTransformRotateScale(mRotate,mScale);

	mIdentityTransform=false;
	modified();
}

void Node::setTransform(const Matrix4x4 &transform){
	Math::setVector3FromMatrix4x4(mTranslate,transform);
	Math::setMatrix3x3FromMatrix4x4(mRotate,transform);
	mScale.set(Math::ONE_VECTOR3); // TODO: Extract scale from transform

	mRenderTransform.set(transform);

	mIdentityTransform=false;
	modified();
}

// Currently just uses the worldRenderTransform, should be changed to walk up the scene till it finds a common parent,
//  and then calculate the transform walking that path.
void Node::findTransformTo(Matrix4x4 &result,Node *node){
	Math::invert(result,node->mWorldRenderTransform);
	Math::postMul(result,mWorldRenderTransform);
}

void Node::setScope(int scope){
	mScope=scope;
}

void Node::setName(const String &name){
	mName=name;
}

Node *Node::findNodeByName(const String &name){
	return (mName!=(char*)NULL && mName.equals(name))?this:NULL;
}

void Node::setAlignmentCalculation(bool xAxis,bool yAxis,bool zAxis){
	mAlignXAxis=xAxis;mAlignYAxis=yAxis;mAlignZAxis=zAxis;
}

void Node::setBoundingRadius(scalar boundingRadius){
	mBoundingRadius=boundingRadius;
}

void Node::setReceiveUpdates(bool receiveUpdates){
	mReceiveUpdates=receiveUpdates;
}

void Node::modified(){
	if(mAwakeCount==0){
		awake();
	}
	else if(mAwakeCount==1){
		mAwakeCount=2;
	}
}

void Node::awake(){
	mAwakeCount=2;
	if(mParent!=NULL){
		mParent->awake();
	}
}

void Node::asleep(){
	mAwakeCount=0;
}

void Node::setRenderTransformTranslate(const Vector3 &translate){
	Math::setMatrix4x4FromTranslate(mRenderTransform,translate);
}

void Node::setRenderTransformRotateScale(const Matrix3x3 &rotate,const Vector3 &scale){
	Math::setMatrix4x4FromRotateScale(mRenderTransform,rotate,scale);
}

}
}
}
