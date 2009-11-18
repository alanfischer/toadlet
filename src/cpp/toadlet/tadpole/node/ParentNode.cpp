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
#include <toadlet/tadpole/node/ParentNode.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(ParentNode,"toadlet::tadpole::node::ParentNode");

ParentNode::ParentNode():super(),
	mShadowChildrenDirty(false)
{
}

Node *ParentNode::create(Engine *engine){
	super::create(engine);

	mChildren.clear();
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

void ParentNode::removeAllNodeDestroyedListeners(){
	super::removeAllNodeDestroyedListeners();

	int i;
	for(i=mChildren.size()-1;i>=0;--i){
		mChildren[i]->removeAllNodeDestroyedListeners();
	}
}

bool ParentNode::attach(Node *node){
	if(node->destroyed()){
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

	mShadowChildrenDirty=true;

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

		mShadowChildrenDirty=true;

		return true;
	}
	else{
		return false;
	}
}

void ParentNode::handleEvent(const Event::ptr &event){
	int numChildren=mShadowChildren.size();
	int i;
	for(i=0;i<numChildren;++i){
		mShadowChildren[i]->handleEvent(event);
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
