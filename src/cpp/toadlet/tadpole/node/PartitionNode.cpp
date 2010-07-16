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

#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/node/PartitionNode.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(PartitionNode,Categories::TOADLET_TADPOLE_NODE+".PartitionNode");

PartitionNode::PartitionNode():super(){
}

Node *PartitionNode::create(Scene *scene){
	super::create(scene);

	return this;
}

void PartitionNode::destroy(){
	super::destroy();
}

bool PartitionNode::senseBoundingVolumes(SensorResultsListener *listener,const Sphere &volume){
	bool result=false;
	int i;
	for(i=0;i<mChildren.size();++i){
		Node *child=mChildren[i];
		if(child->testWorldBound(volume)){
			result|=true;
			if(listener->resultFound(child)==false){
				return true;
			}
		}
	}
	
	return result;
}

bool PartitionNode::sensePotentiallyVisible(SensorResultsListener *listener,const Vector3 &point){
	bool result=false;
	int i;
	for(i=0;i<mChildren.size();++i){
		Node *child=mChildren[i];
		result|=true;
		if(listener->resultFound(child)==false){
			return true;
		}
	}
	
	return result;
}

void PartitionNode::queueRenderables(CameraNode *camera,RenderQueue *queue){
	queueRenderables(this,camera,queue);
}

void PartitionNode::queueRenderables(Node *node,CameraNode *camera,RenderQueue *queue){
	if(culled(node,camera)){
		return;
	}

	if(node!=this){
		node->queueRenderables(camera,queue);
	}

	ParentNode *parent=node->isParent();
	if(parent!=NULL){
		int numChildren=parent->getNumChildren();
		int i;
		for(i=0;i<numChildren;++i){
			queueRenderables(parent->getChild(i),camera,queue);
		}
	}
}

bool PartitionNode::culled(Node *node,CameraNode *camera){
//	return (node->getScope()&camera->getScope())==0 || camera->culled(node->getWorldBound());
return false;
}

}
}
}
