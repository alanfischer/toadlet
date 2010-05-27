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

#ifndef TOADLET_TADPOLE_NODE_PARTITIONNODE_H
#define TOADLET_TADPOLE_NODE_PARTITIONNODE_H

#include <toadlet/tadpole/SensorResultsListener.h>
#include <toadlet/tadpole/node/ParentNode.h>

namespace toadlet{
namespace tadpole{

namespace node{

class TOADLET_API PartitionNode:public ParentNode{
public:
	TOADLET_NODE(PartitionNode,ParentNode);

	PartitionNode();
	virtual Node *create(Scene *scene);
	virtual void destroy();

	virtual bool senseBoundingVolumes(SensorResultsListener *listener,const Sphere &volume);
	virtual bool sensePotentiallyVisible(SensorResultsListener *listener,const Vector3 &point);
	
	virtual void queueRenderables(CameraNode *camera,RenderQueue *queue);
	virtual void queueRenderables(Node *node,CameraNode *camera,RenderQueue *queue);

protected:
	virtual bool culled(Node *node,CameraNode *camera);
};

}
}
}

#endif
