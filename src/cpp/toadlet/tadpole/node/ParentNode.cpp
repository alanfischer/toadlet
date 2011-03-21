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

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(ParentNode,Categories::TOADLET_TADPOLE_NODE+".ParentNode");

ParentNode::ParentNode():super(),
	mChildrenActive(false),
	mActivateChildren(false),

	mShadowChildrenDirty(false)
{
}

Node *ParentNode::create(Scene *scene){
	super::create(scene);

	mChildren.clear();
	mChildrenActive=false;
	mActivateChildren=true;

	mShadowChildren.clear();
	mShadowChildrenDirty=false;

	return this;
}

void ParentNode::destroy(){
	while(mChildren.size()>0){
		mChildren[0]->destroy();
	}

	mShadowChildren.clear();

	super::destroy();
}

Node *ParentNode::set(Node *node){
	super::set(node);

	ParentNode *parentNode=(ParentNode*)node;
	int i;
	for(i=0;i<parentNode->getNumChildren();++i){
		attach(parentNode->getChild(i)->clone(mScene));
	}

	return this;
}

void ParentNode::destroyAllChildren(){
	while(mChildren.size()>0){
		mChildren[0]->destroy();
	}
}

void ParentNode::removeAllNodeListeners(){
	super::removeAllNodeListeners();

	int i;
	for(i=mChildren.size()-1;i>=0;--i){
		mChildren[i]->removeAllNodeListeners();
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

	mChildren.add(node);

	node->parentChanged(this);
	nodeAttached(node);

	mShadowChildrenDirty=true;

	activate();

	node->frameUpdate(0,-1);
	mergeWorldBound(node,true);

	return true;
}

bool ParentNode::remove(Node *node){
	Node::ptr reference(node); // To make sure that the object is not deleted until we can call parentChanged
	int i;
	for(i=mChildren.size()-1;i>=0;--i){
		if(mChildren[i]==node)break;
	}
	if(i>=0){
		mChildren.removeAt(i);

		node->parentChanged(NULL);
		nodeRemoved(node);

		mShadowChildrenDirty=true;

		activate();

		return true;
	}
	else{
		return false;
	}
}

void ParentNode::handleEvent(const Event::ptr &event){
	int numChildren=mChildren.size();
	int i;
	for(i=0;i<numChildren;++i){
		mChildren[i]->handleEvent(event);
	}
}

void ParentNode::logicUpdate(int dt,int scope){
	super::logicUpdate(dt,scope);

	mChildrenActive=false;
	if(mShadowChildrenDirty){
		updateShadowChildren();
	}

	int numChildren=mShadowChildren.size();
	Node *child=NULL;
	int i;
	for(i=0;i<numChildren;++i){
		child=mShadowChildren[i];
		bool dependent=false;
		if(mActivateChildren){
			child->activate();
		}
		if(child->active() && (child->getScope()&scope)!=0){
			Node *depends=child->getDependsUpon();
			if(depends!=NULL && depends->mLastLogicFrame!=mScene->getLogicFrame()){
				mScene->queueDependent(child);
			}
			else{
				child->logicUpdate(dt,scope);
				child->tryDeactivate();
				mChildrenActive=true;
			}
		}

		if(dependent==false){
			mergeWorldBound(child,false);
		}
	}

	mActivateChildren=false;
}

void ParentNode::frameUpdate(int dt,int scope){
	super::frameUpdate(dt,scope);

	if(mShadowChildrenDirty){
		updateShadowChildren();
	}

	int numChildren=mShadowChildren.size();

	Node *child=NULL;
	int i;
	for(i=0;i<numChildren;++i){
		child=mShadowChildren[i];

		bool dependent=false;
		// This may not be in logicUpdate, but it solves an issue where a deactivated Node would not get updated if 
		//  it only had frameUpdate called on it before a render
		if(mActivateChildren){
			child->activate();
		}
		if(child->active() && (child->getScope()&scope)!=0){
			Node *depends=child->getDependsUpon();
			if(depends!=NULL && depends->mLastFrame!=mScene->getFrame()){
				mScene->queueDependent(child);
				dependent=true;
			}
			else{
				child->frameUpdate(dt,scope);
			}
		}

		if(dependent==false){
			mergeWorldBound(child,false);
		}
	}

	// See the comment above on child->activate() in frameUpdate()
	mActivateChildren=false;
}

void ParentNode::gatherRenderables(CameraNode *camera,RenderableSet *set){
	int numChildren=getNumChildren();
	int i;
	for(i=0;i<numChildren;++i){
		Node *child=getChild(i);
		if(camera->culled(child)==false){
			child->gatherRenderables(camera,set);
		}
	}
}

void ParentNode::mergeWorldBound(Node *child,bool justAttached){
	mWorldBound->merge(child->getWorldBound(),mScene->getEpsilon());
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
	int i;
	for(i=0;i<mChildren.size();++i){
		mChildren[i]->deactivate();
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

void ParentNode::updateShadowChildren(){
	mShadowChildrenDirty=false;

	mShadowChildren.resize(mChildren.size());
	int i;
	for(i=mChildren.size()-1;i>=0;--i){
		mShadowChildren[i]=mChildren[i];
	}
}

}
}
}
