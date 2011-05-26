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
using namespace toadlet::tadpole::animation;

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
	//mControllers,

	//mParent,
	mParentData(NULL),

	//mDependsUpon,

	mActive(false),
	mDeactivateCount(0),
	mLastLogicFrame(0),
	mLastFrame(0),
	mTransformUpdatedFrame(0),

	//mTransform,
	//mBound,
	//mWorldTransform,
	//mWorldBound,
	mScope(0)
	//mName,
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
	if(scene!=NULL){
		mScene=scene;
		mEngine=mScene->getEngine();
	}

	if(mEngine!=NULL){
		mUniqueHandle=mEngine->internal_registerNode(this);
	}

	mNodeListeners=NULL;
	mControllers=NULL;

	mParent=NULL;
	mParentData=NULL;

	mDependsUpon=NULL;

	mActive=true;
	mDeactivateCount=0;
	mTransformUpdatedFrame=-1;

	mTransform.reset();
	mBound.reset();
	mWorldTransform.reset();
	mWorldBound.reset();
	mScope=-1;
	mName="";

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

	mControllers=NULL;

	if(mEngine!=NULL){
		mEngine->internal_deregisterNode(this);
		mUniqueHandle=0;
	}

	mEngine->freeNode(this);
	mEngine=NULL;
	mScene=NULL;
}

Node *Node::set(Node *node){
	TOADLET_ASSERT(getType()==node->getType());

	setTransform(node->mTransform);
	setBound(node->mBound);
	setScope(node->mScope);
	setName(node->mName);

	return node;
}

Node *Node::clone(Scene *scene){return mEngine->createNode(getType(),scene)->set(this);}

void *Node::hasInterface(int type){
	if(type==InterfaceType_TRANSFORMABLE) return (Transformable*)this;
	return NULL;
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

void Node::addController(Controller::ptr controller){
	if(mControllers==NULL){
		mControllers=Collection<Controller::ptr>::ptr(new egg::Collection<Controller::ptr>());
	}
	mControllers->add(controller);
}

void Node::removeController(Controller::ptr controller){
	if(mControllers!=NULL){
		mControllers->remove(controller);
		if(mControllers->size()==0){
			mControllers=NULL;
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
	mTransform.setTranslate(translate);
	spacialUpdated();
}

void Node::setTranslate(scalar x,scalar y,scalar z){
	mTransform.setTranslate(x,y,z);
	spacialUpdated();
}

void Node::setRotate(const Quaternion &rotate){
	mTransform.setRotate(rotate);
	spacialUpdated();
}

void Node::setRotate(const Matrix3x3 &rotate){
	mTransform.setRotate(rotate);
	spacialUpdated();
}

void Node::setRotate(const Vector3 &axis,scalar angle){
	mTransform.setRotate(axis,angle);
	spacialUpdated();
}

void Node::setScale(const Vector3 &scale){
	mTransform.setScale(scale);
	spacialUpdated();
}

void Node::setScale(scalar x,scalar y,scalar z){
	mTransform.setScale(x,y,z);
	spacialUpdated();
}

void Node::setTransform(const Transform &transform){
	mTransform.set(transform);
	spacialUpdated();
}

void Node::setBound(const Bound &bound){
	mBound.set(bound);
	spacialUpdated();
}

void Node::logicUpdate(int dt,int scope){
	mLastLogicFrame=mScene->getLogicFrame();

	logicUpdateListeners(dt);

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
	if(mDeactivateCount>=0 && mNodeListeners==NULL && mControllers==NULL){
		mDeactivateCount++;
		if(mDeactivateCount>4){
			deactivate();
		}
	}
}

bool Node::getTransformUpdated(){return mScene->getFrame()==mTransformUpdatedFrame;}

void Node::updateWorldTransform(){
	if(mParent==NULL){
		mWorldTransform.set(mTransform);
	}
	else if(mTransformUpdatedFrame==-1){
		mWorldTransform.set(mParent->mWorldTransform);
	}
	else{
		mWorldTransform.setTransform(mParent->mWorldTransform,mTransform);
	}

	mWorldBound.transform(mBound,mWorldTransform);

	if(mParent!=NULL){
		((ParentNode*)mParent.get())->mergeWorldBound(this,false);
	}
}

void Node::spacialUpdated(){
	mTransformUpdatedFrame=mScene->getFrame();
	activate();
}

void Node::updateAllWorldTransforms(){
	updateWorldTransform();
}

void Node::logicUpdateListeners(int dt){
	if(mNodeListeners!=NULL){
		int i;
		for(i=0;i<mNodeListeners->size();++i){
			mNodeListeners->at(i)->logicUpdated(this,dt);
		}
	}
}

void Node::frameUpdateListeners(int dt){
	if(mNodeListeners!=NULL){
		int i;
		for(i=0;i<mNodeListeners->size();++i){
			mNodeListeners->at(i)->frameUpdated(this,dt);
		}
	}

	if(mControllers!=NULL){
		int i;
		for(i=0;i<mControllers->size();++i){
			mControllers->at(i)->update(dt);
		}
	}
}

}
}
}
