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

#include <toadlet/tadpole/PartitionNode.h>
#include <toadlet/tadpole/Scene.h>

namespace toadlet{
namespace tadpole{

PartitionNode::PartitionNode(Scene *scene):Node(scene){
}

bool PartitionNode::senseBoundingVolumes(SensorResultsListener *listener,Bound *bound){
	bool result=false;

	for(NodeCollection::iterator node=getNodes().begin(),end=getNodes().end();node!=end;++node){
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

	for(NodeCollection::iterator node=getNodes().begin(),end=getNodes().end();node!=end;++node){
		result|=true;
		if(listener->resultFound(node,Math::lengthSquared(point,node->getWorldTranslate()))==false){
			return true;
		}
	}
	
	return result;
}

bool PartitionNode::findAmbientForBound(Vector4 &r,Bound *bound){
	return false;
}

}
}
