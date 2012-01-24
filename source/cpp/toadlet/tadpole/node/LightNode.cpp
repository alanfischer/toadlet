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

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/node/LightNode.h>
#include <toadlet/tadpole/node/ParentNode.h>

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(LightNode,Categories::TOADLET_TADPOLE_NODE+".LightNode");

LightNode::LightNode():super(),
	mEnabled(true)
	//mLightState
{}

Node *LightNode::create(Scene *scene){
	super::create(scene);

	mBound.setInfinite();

	return this;
}

Node *LightNode::set(Node *node){
	super::set(node);

	LightNode *lightNode=(LightNode*)this;
	setLightState(lightNode->mLightState);

	return this;
}

void LightNode::frameUpdate(int dt,int scope){
	super::frameUpdate(dt,scope);

	mLightState.position.set(getWorldTranslate());
	Math::mul(mLightState.direction,getWorldRotate(),mDirection);
}

void LightNode::gatherRenderables(CameraNode *node,RenderableSet *set){
	super::gatherRenderables(node,set);

	if(mEnabled){
		set->queueLight(this);
	}
}

}
}
}
