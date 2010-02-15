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
#include <toadlet/tadpole/node/SizeableParentNode.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(SizeableParentNode,Categories::TOADLET_TADPOLE_NODE+".SizeableParentNode");

SizeableParentNode::SizeableParentNode():super(),
	mLayout(Layout_NONE)
{}

Node *SizeableParentNode::create(Engine *engine){
	super::create(engine);

	mLayout=Layout_NONE;
	mSizeableChildren.clear();

	return this;
}

void SizeableParentNode::destroy(){
	mSizeableChildren.clear();

	super::destroy();
}

bool SizeableParentNode::attach(Node *node){
	if(super::attach(node)==false){
		return false;
	}
	
	if(node->isSizeable()!=NULL){
		mSizeableChildren.add(node);
	}

	return true;
}

void SizeableParentNode::setLayout(int layout){
	mLayout=layout;

	mShadowChildrenDirty=true;
}

bool SizeableParentNode::remove(Node *node){
	if(super::remove(node)==false){
		return false;
	}
	
	if(node->isSizeable()!=NULL){
		mSizeableChildren.remove(node);
	}
	
	return true;
}

void SizeableParentNode::setSize(scalar x,scalar y,scalar z){
	mSize.set(x,y,z);

	mShadowChildrenDirty=true;
}

void SizeableParentNode::setSize(const Vector3 &size){
	mSize.set(size);

	mShadowChildrenDirty=true;
}

const Vector3 &SizeableParentNode::getDesiredSize(){
	return Math::ZERO_VECTOR3;
}

// TODO: Rename this to something more general, since we are going to be using it for more than just the shadow children.
//  Maybe just a dirty flag, and this dirty flag should be use to replace the updateSprite & updateLabel functions, so they
//  would all be done by this dirty/modified update thing
// Should also be tied into the Modified_X_Frame thing in entity probably
void SizeableParentNode::updateShadowChildren(){
	if((mLayout&Layout_SHRINK)>0){
		Vector3 size;
		int i;
		for(i=0;i<mSizeableChildren.size();++i){
			const Vector3 &childSize=mSizeableChildren[i]->isSizeable()->getSize();
			if(childSize.x>size.x) size.x=childSize.x;
			if(childSize.y>size.y) size.y=childSize.y;
			if(childSize.z>size.z) size.z=childSize.z;
		}

		setSize(size);
	}

	// Layout child nodes
	if((mLayout&Layout_CHILDREN_VERTICAL)>0){
		scalar x=0;
		scalar y=0-mSize.y/2;
		int i;
		for(i=0;i<mSizeableChildren.size();++i){
			y+=Math::div(mSize.y,Math::fromInt(mSizeableChildren.size()+1));
			mSizeableChildren[i]->setTranslate(x,y,0);
		}
	}

	super::updateShadowChildren();
}

}
}
}
