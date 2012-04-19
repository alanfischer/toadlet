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
#include <toadlet/tadpole/Action.h>
#include <toadlet/tadpole/node/Node.h>
#include <toadlet/tadpole/node/CameraNode.h>

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(Node,Categories::TOADLET_TADPOLE_NODE+".Node");

Node::Node():BaseComponent(),
	mCreated(false),
	mEngine(NULL),
	mScene(NULL),
	mUniqueHandle(0),

	//mParent,
	mParentData(NULL),

	mChildrenActive(false),
	mActivateChildren(false),

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
{
}

Node::~Node(){
	destroy();
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

	mParent=NULL;
	mParentData=NULL;

	mChildrenActive=false;
	mActivateChildren=true;

	mActive=true;
	mDeactivateCount=0;
	mTransformUpdatedFrame=-1;

	mTransform.reset();
	mBound.reset();
	mWorldTransform.reset();
	mWorldBound.reset();
	mScope=-1;

	return this;
}

void Node::destroy(){
	if(mCreated==false){
		return;
	}

	destroyAllChildren();

	Node::ptr reference(this); // To make sure that the object is not deleted right away

	mCreated=false;

	if(mParent!=NULL){
		mParent->remove(this);
		mParent=NULL;
	}

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

void Node::destroyAllChildren(){
	while(mFirstChild!=NULL){
		mFirstChild->destroy();
	}
}

bool Node::attach(Component *component){
	Component::ptr reference(component); // To make sure that the object is not released early

	mComponents.add(component);

	if(component->parentChanged(this)==false){
		mComponents.remove(component);

		return false;
	}

	if(getActive()==false){
		activate();
	}

	return true;
}

bool Node::remove(Component *component){
	Component::ptr reference(component); // To make sure that the object is not released early

	if(component->parentChanged(NULL)==false){
		return false;
	}

	mComponents.remove(component);

	if(getActive()==false){
		activate();
	}

	return true;
}

void Node::nodeAttached(Node *node){
	mComponents.remove(node);

	Node *previous=mLastChild;
	node->previousChanged(previous);
	if(previous!=NULL){
		previous->nextChanged(node);
	}
	else{
		mFirstChild=node;
	}
	mLastChild=node;

	node->nextChanged(NULL);
}

void Node::nodeRemoved(Node *node){
	Node *previous=node->getPrevious();
	Node *next=node->getNext();
	if(previous!=NULL){
		previous->nextChanged(next);
	}
	if(next!=NULL){
		next->previousChanged(previous);
	}

	if(mFirstChild==node){
		mFirstChild=next;
	}
	if(mLastChild==node){
		mLastChild=previous;
	}

	node->previousChanged(NULL);
	// Leave node->next so traversals can continue
}

void Node::actionAttached(Action *action){
	mActions.add(action);
}

void Node::actionRemoved(Action *action){
	mActions.remove(action);
}

void Node::playAction(const String &action){
	int i;
	for(i=0;i<mActions.size();++i){
		if(mActions[i]->getName()==action){
			mActions[i]->start();
		}
	}
}

bool Node::getActionActive(const String &action){
	int i;
	for(i=0;i<mActions.size();++i){
		if(mActions[i]->getName()==action){
			return mActions[i]->getActive();
		}
	}
	return false;
}

bool Node::parentChanged(Node *node){
	if(node!=NULL && mParent!=NULL){
		mParent->remove(this);
	}

	if(mParent!=NULL){
		mParent->nodeRemoved(this);
	}

	mParent=node;

	if(mParent!=NULL){
		mParent->nodeAttached(this);
	}

	return true;
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

void Node::setMatrix4x4(const Matrix4x4 &matrix){
	mTransform.setMatrix(matrix);
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
	bool anyActive=false;

	/// @todo: Use a linked list for the Components, and NOT check to see if they are a node here
	int i;
	for(i=0;i<mComponents.size();++i){
		mComponents[i]->logicUpdate(dt,scope);
		anyActive|=mComponents[i]->getActive();
	}
	if(anyActive){
		activate();
	}

	updateWorldTransform();

	mChildrenActive=false;

	Node::ptr node,next;
	for(node=getFirstChild();node!=NULL;node=next){
		next=node->getNext();
		if(mActivateChildren){
			node->activate();
		}
		if(node->getActive() && (node->getScope()&scope)!=0){
			node->logicUpdate(dt,scope);
			node->tryDeactivate();
			mChildrenActive=true;
		}
		else{
			mergeWorldBound(node,false);
		}
	}

	mActivateChildren=false;
}

void Node::frameUpdate(int dt,int scope){
	mLastFrame=mScene->getFrame();

	/// @todo: Use a linked list for the Components, and NOT check to see if they are a node here
	int i;
	for(i=0;i<mComponents.size();++i){
		mComponents[i]->frameUpdate(dt,scope);
	}

	updateWorldTransform();

	Node::ptr node,next;
	for(node=getFirstChild();node!=NULL;node=next){
		next=node->getNext();
		// This may not be in logicUpdate, but it solves an issue where a deactivated Node would not get updated if 
		//  it only had frameUpdate called on it before a render
		if(mActivateChildren){
			node->activate();
		}
		if(node->getActive() && (node->getScope()&scope)!=0){
			node->frameUpdate(dt,scope);
		}
		else{
			mergeWorldBound(node,false);
		}
	}

	// See the comment above on child->activate() in frameUpdate()
	mActivateChildren=false;
}

void Node::mergeWorldBound(Node *child,bool justAttached){
	mWorldBound.merge(child->getWorldBound(),mScene->getEpsilon());
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
	mActivateChildren=true;

	if(mDeactivateCount>0){
		mDeactivateCount=0;
	}
	if(mActive==false){
		mActive=true;
		if(mParent!=NULL && mParent->getActive()==false){
			mParent->activate();
		}
	}
}

void Node::deactivate(){
	if(mActive==false){
		return;
	}

	mActive=false;
	mDeactivateCount=0;

	super::deactivate();
	Node::ptr node;
	for(node=getFirstChild();node!=NULL;node=node->getNext()){
		node->deactivate();
	}
}

void Node::tryDeactivate(){
	if(mDeactivateCount>=0){
		if(mChildrenActive==false){
			mDeactivateCount++;
			if(mDeactivateCount>4){
				deactivate();
			}
		}
		else{
			mDeactivateCount=0;
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
		mParent->mergeWorldBound(this,false);
	}
}

void Node::spacialUpdated(){
	if(mScene!=NULL){
		mTransformUpdatedFrame=mScene->getFrame();
	}
	activate();
}

void Node::updateAllWorldTransforms(){
	updateWorldTransform();

	Node *node;
	for(node=getFirstChild();node!=NULL;node=node->getNext()){
		node->updateAllWorldTransforms();
	}
}

void Node::gatherRenderables(CameraNode *camera,RenderableSet *set){
	Node *node;
	for(node=getFirstChild();node!=NULL;node=node->getNext()){
		if(camera->culled(node)==false){
			node->gatherRenderables(camera,set);
		}
	}
}

}
}
}
