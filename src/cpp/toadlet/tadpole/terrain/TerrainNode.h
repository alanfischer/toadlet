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
#include <toadlet/tadpole/terrain/TerrainDataSource.h>

namespace toadlet{
namespace tadpole{
namespace terrain{

/// @todo: Use the quadtree to store child nodes and implement the commented out methods
class TOADLET_API TerrainNode:public node::PartitionNode,public Traceable{
public:
	TOADLET_NODE(TerrainNode,node::PartitionNode);

	TerrainNode();
	virtual ~TerrainNode();

	node::Node *create(Scene *scene);
	void destroy();

	void setTarget(node::Node *target);
	node::Node *getTarget(){return mTarget;}

	void setDataSource(TerrainDataSource *dataSource);
	TerrainDataSource *getDataSource(){return mDataSource;}

	void setMaterial(Material::ptr material);
	Material::ptr getMaterial() const{return mPatchMaterial;}

	TerrainPatchNode::ptr patchAt(int x,int y){return mTerrainPatches[y*mSize+x];}
	int fromWorldX(scalar x){x=Math::div(x,mPatchSize*mPatchScale.x);return Math::toInt(x>=0?x+Math::HALF:x-Math::HALF);}
	int fromWorldY(scalar y){y=Math::div(y,mPatchSize*mPatchScale.y);return Math::toInt(y>=0?y+Math::HALF:y-Math::HALF);}
	scalar toWorldX(int x){return Math::mul(Math::fromInt(x),mPatchSize*mPatchScale.x);}
	scalar toWorldY(int y){return Math::mul(Math::fromInt(y),mPatchSize*mPatchScale.y);}

	void queueRenderables(node::CameraNode *camera,RenderQueue *queue);

	void logicUpdate(int dt,int scope);

	const Sphere &getLocalBound() const{return super::getLocalBound();}
	void traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);

	void updateTarget();
	void updatePatch(int x,int y);
	void updateLocalBound();

	// Node items
/*	void nodeAttached(Node *node);
	void nodeRemoved(Node *node);

	void mergeWorldBound(Node *child,bool justAttached);
	void queueRenderables(node::CameraNode *camera,RenderQueue *queue);

	bool senseBoundingVolumes(SensorResultsListener *listener,const Sphere &volume);
	bool sensePotentiallyVisible(SensorResultsListener *listener,const Vector3 &point);
*/
protected:
//	void childTransformUpdated(Node *child);

	int mTerrainX,mTerrainY;
	node::Node::ptr mTarget;
	TerrainDataSource *mDataSource;
	int mSize;
	egg::Collection<scalar> mPatchData;
	egg::Collection<TerrainPatchNode::ptr> mTerrainPatches;
	int mPatchSize;
	Material::ptr mPatchMaterial;
	Vector3 mPatchScale;
};

}
}
}

#endif
