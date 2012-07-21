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

#include <toadlet/tadpole/node/PartitionNode.h>
#include <toadlet/tadpole/Scene.h>

namespace toadlet{
namespace tadpole{
namespace node{

PartitionNode::PartitionNode(Scene *scene):Node(scene){
}

bool PartitionNode::senseBoundingVolumes(SensorResultsListener *listener,Bound *bound){
	bool result=false;

	int i;
	for(i=0;i<mNodes.size();++i){
		Node *node=mNodes[i];
		if(bound->testIntersection(node->getWorldBound())){
			result|=true;
			if(listener->resultFound(node,Math::lengthSquared(bound->getSphere().origin,node->getWorldTranslate()))==false){
				return true;
			}
		}
	}
	
	return result;
}

bool PartitionNode::sensePotentiallyVisible(SensorResultsListener *listener,const Vector3 &point){
	bool result=false;

	int i;
	for(i=0;i<mNodes.size();++i){
		Node *node=mNodes[i];
		result|=true;
		if(listener->resultFound(node,Math::lengthSquared(point,node->getWorldTranslate()))==false){
			return true;
		}
	}
	
	return result;
}

bool PartitionNode::findAmbientForPoint(Vector4 &r,const Vector3 &point){
	return false;
}

}
}
}
