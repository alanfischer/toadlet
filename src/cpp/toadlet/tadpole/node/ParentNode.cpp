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
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/node/SceneNode.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(ParentNode,Categories::TOADLET_TADPOLE_NODE+".ParentNode");

ParentNode::ParentNode():super(),
	mActivateChildren(false),

	mShadowChildrenDirty(false)
{
}

Node *ParentNode::create(Scene *scene){
	super::create(scene);

	mChildren.clear();
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

void ParentNode::destroyAllChildren(){
	while(mChildren.size()>0){
		mChildren[0]->destroy();
	}
}

void ParentNode::removeAllNodeDestroyedListeners(){
	super::removeAllNodeDestroyedListeners();

	int i;
	for(i=mChildren.size()-1;i>=0;--i){
		mChildren[i]->removeAllNodeDestroyedListeners();
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

	/// @todo: Maybe all of the frameUpdateness should be moved just to the parentNode, so I can just update & remerge bounds, without calling into mScene
	/// @todo: This should actually call to the parent, and update the bounds as far as needed.  If at some point, worldBound didnt change, then don't call further
	mScene->getRootNode()->frameUpdate(node,0,-1);
	merge(mWorldBound,node->getWorldBound());

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

void ParentNode::activate(){
	mActivateChildren=true;
	super::activate();
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
