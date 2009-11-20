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
	mBoundingRadius(0),
	mReceiveUpdates(false),

	mModifiedLogicFrame(0),
	mModifiedRenderFrame(0),
	mWorldModifiedLogicFrame(0),
	mWorldModifiedRenderFrame(0)
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

	mModifiedLogicFrame=-1;
	mModifiedRenderFrame=-1;
	mWorldModifiedLogicFrame=-1;
	mWorldModifiedRenderFrame=-1;

	mRenderTransform.reset();
	mRenderWorldBound.reset();
	mRenderWorldTransform.reset();

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

void Node::setScope(int scope){
	mScope=scope;
}

void Node::setBoundingRadius(scalar boundingRadius){
	mBoundingRadius=boundingRadius;
}

void Node::setReceiveUpdates(bool receiveUpdates){
	mReceiveUpdates=receiveUpdates;
}

/// @todo  Change the modified setup so you can tell if you were modified, or if any of your parents were modified
void Node::modified(){
	if(mEngine!=NULL){
		mModifiedLogicFrame=mEngine->getScene()->getLogicFrame();
		mModifiedRenderFrame=mEngine->getScene()->getRenderFrame();
	}
}

bool Node::modifiedSinceLastLogicFrame() const{
	return mWorldModifiedLogicFrame+1>=mEngine->getScene()->getLogicFrame();
}

bool Node::modifiedSinceLastRenderFrame() const{
	return mWorldModifiedRenderFrame+1>=mEngine->getScene()->getRenderFrame();
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
