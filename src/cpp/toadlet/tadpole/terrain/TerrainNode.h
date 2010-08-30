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

#ifndef TOADLET_TADPOLE_TERRAIN_TERRAINNODE_H
#define TOADLET_TADPOLE_TERRAIN_TERRAINNODE_H

#include <toadlet/tadpole/node/PartitionNode.h>
#include <toadlet/tadpole/terrain/TerrainPatchNode.h>

namespace toadlet{
namespace tadpole{
namespace terrain{

class TOADLET_API TerrainNode:public node::PartitionNode,public Traceable{
public:
	TOADLET_NODE(TerrainNode,node::PartitionNode);

	TerrainNode();
	virtual ~TerrainNode();

	node::Node *create(Scene *scene);

	void queueRenderables(node::CameraNode *camera,RenderQueue *queue);

	const Sphere &getLocalBound() const{return super::getWorldBound();} // We're always at the origin, and we need the bound of our children included
	void traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);

	// Node items
/*	void nodeAttached(Node *node);
	void nodeRemoved(Node *node);

	void mergeWorldBound(Node *child,bool justAttached);
	void queueRenderables(node::CameraNode *camera,RenderQueue *queue);

	bool senseBoundingVolumes(SensorResultsListener *listener,const Sphere &volume);
	bool sensePotentiallyVisible(SensorResultsListener *listener,const Vector3 &point);

protected:
	void childTransformUpdated(Node *child);
*/

	const static int ts=3;
	TerrainPatchNode::ptr patches[ts*ts];
};

}
}
}

#endif
