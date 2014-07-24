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

#include <toadlet/tadpole/BaseComponent.h>
#include <toadlet/tadpole/Node.h>

namespace toadlet{
namespace tadpole{

BaseComponent::BaseComponent():
	mParent(NULL),
	mRoot(NULL)
{}

void BaseComponent::destroy(){
	if(mParent!=NULL){
		mParent->remove(componentThis());
	}
}

void BaseComponent::parentChanged(Node *node){
	baseParentChanged(node);
}

void BaseComponent::rootChanged(Node *root){
	baseRootChanged(root);
}

void BaseComponent::notifyParentAttached(){
	mParent->componentAttached(this);
}

void BaseComponent::notifyParentRemoved(){
	mParent->componentRemoved(this);
}

void BaseComponent::baseParentChanged(Node *node){
	if(mParent!=NULL){
		notifyParentRemoved();
	}

	mParent=node;

	if(mParent!=NULL){
		notifyParentAttached();
	}

	Node *root=NULL;
	if(mParent!=NULL){
		root=mParent->getRoot();
	}

	if(root!=mRoot){
		rootChanged(root);
	}
}

void BaseComponent::baseRootChanged(Node *root){
	mRoot=root;
}

}
}
