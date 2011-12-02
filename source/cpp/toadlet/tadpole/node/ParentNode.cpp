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
#include <toadlet/egg/Logger.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/node/ParentNode.h>

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(ParentNode,Categories::TOADLET_TADPOLE_NODE+".ParentNode");

ParentNode::ParentNode():super(),
	mChildrenActive(false),
	mActivateChildren(false)
{
}

Node *ParentNode::create(Scene *scene){
	super::create(scene);

	mChildrenActive=false;
	mActivateChildren=true;

	return this;
}

void ParentNode::destroy(){
	destroyAllChildren();

	super::destroy();
}

Node *ParentNode::set(Node *node){
	super::set(node);

	ParentNode *parentNode=(ParentNode*)node;
	for(node=parentNode->getFirstChild();node!=NULL;node=node->getNext()){
		attach(node->clone(mScene));
	}

	return this;
}

void ParentNode::destroyAllChildren(){
	while(mFirstChild!=NULL){
		mFirstChild->destroy();
	}
}

void ParentNode::removeAllNodeListeners(){
	super::removeAllNodeListeners();

	Node::ptr node;
	for(node=getFirstChild();node!=NULL;node=node->getNext()){
		node->removeAllNodeListeners();
	}
}

bool ParentNode::attach(Node *node){
	if(!node->created()){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"cannot attach a destroyed node");
		return false;
	}

	ParentNode *parent=node->getParent();
	if(parent!=NULL){
		parent->remove(node);
	}

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
	node->parentChanged(this);
	nodeAttached(node);

	if(active()==false){
		activate();
	}

	node->frameUpdate(0,-1);

	return true;
}

bool ParentNode::remove(Node *node){
	Node::ptr reference(node); // To make sure that the object is not deleted until we can call parentChanged
	if(node->getParent()==this){
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

		node->parentChanged(NULL);
		node->previousChanged(NULL);
		// Leave node->next so traversals can continue

		nodeRemoved(node);

		if(active()==false){
			activate();
		}

		return true;
	}
	else{
		return false;
	}
}

void ParentNode::handleEvent(const Event::ptr &event){
	Node::ptr node;
	for(node=getFirstChild();node!=NULL;node=node->getNext()){
		node->handleEvent(event);
	}
}

void ParentNode::logicUpdate(int dt,int scope){
	super::logicUpdate(dt,scope);

	mChildrenActive=false;

	Node::ptr node,next;
	for(node=getFirstChild();node!=NULL;node=next){
		next=node->getNext();
		if(mActivateChildren){
			node->activate();
		}
		if(node->active() && (node->getScope()&scope)!=0){
			Node *depends=node->getDependsUpon();
			if(depends!=NULL && depends->mLastLogicFrame!=mScene->getLogicFrame()){
				mScene->queueDependent(node);
			}
			else{
				node->logicUpdate(dt,scope);
				node->tryDeactivate();
				mChildrenActive=true;
			}
		}
		else{
			mergeWorldBound(node,false);
		}
	}

	mActivateChildren=false;
}

void ParentNode::frameUpdate(int dt,int scope){
	super::frameUpdate(dt,scope);

	Node::ptr node,next;
	for(node=getFirstChild();node!=NULL;node=next){
		next=node->getNext();
		bool dependent=false;
		// This may not be in logicUpdate, but it solves an issue where a deactivated Node would not get updated if 
		//  it only had frameUpdate called on it before a render
		if(mActivateChildren){
			node->activate();
		}
		if(node->active() && (node->getScope()&scope)!=0){
			Node *depends=node->getDependsUpon();
			if(depends!=NULL && depends->mLastFrame!=mScene->getFrame()){
				mScene->queueDependent(node);
				dependent=true;
			}
			else{
				node->frameUpdate(dt,scope);
			}
		}
		else{
			mergeWorldBound(node,false);
		}
	}

	// See the comment above on child->activate() in frameUpdate()
	mActivateChildren=false;
}

void ParentNode::gatherRenderables(CameraNode *camera,RenderableSet *set){
	Node *node;
	for(node=getFirstChild();node!=NULL;node=node->getNext()){
		if(camera->culled(node)==false){
			node->gatherRenderables(camera,set);
		}
	}
}

void ParentNode::mergeWorldBound(Node *child,bool justAttached){
	mWorldBound.merge(child->getWorldBound(),mScene->getEpsilon());
}

void ParentNode::activate(){
	mActivateChildren=true;
	super::activate();
}

void ParentNode::deactivate(){
	if(mActive==false){
		return;
	}

	super::deactivate();
	Node::ptr node;
	for(node=getFirstChild();node!=NULL;node=node->getNext()){
		node->deactivate();
	}
}

void ParentNode::tryDeactivate(){
	if(mDeactivateCount>=0 && mNodeListeners==NULL){
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

void ParentNode::updateAllWorldTransforms(){
	super::updateAllWorldTransforms();

	Node *node;
	for(node=getFirstChild();node!=NULL;node=node->getNext()){
		node->updateAllWorldTransforms();
	}
}

}
}
}
