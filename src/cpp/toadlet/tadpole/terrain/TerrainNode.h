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
#include <toadlet/tadpole/terrain/TerrainNodeDataSource.h>
#include <toadlet/tadpole/terrain/TerrainNodeListener.h>
#include <toadlet/tadpole/terrain/TerrainPatchNode.h>

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

	void setListener(TerrainNodeListener *listener){mListener=listener;}
	TerrainNodeListener *getListener(){return mListener;}

	void setTarget(node::Node *target);
	node::Node *getTarget(){return mTarget;}

	void setUpdateTargetBias(scalar bias){mUpdateTargetBias=bias;}
	scalar getUpdateTargetBias(){return mUpdateTargetBias;}

	void setDataSource(TerrainNodeDataSource *dataSource);
	TerrainNodeDataSource *getDataSource(){return mDataSource;}

	void setMaterial(Material::ptr material);
	Material::ptr getMaterial() const{return mPatchMaterial;}

	void setWaterMaterial(Material::ptr material);
	Material::ptr getWaterMaterial() const{return mPatchWaterMaterial;}

	void setTolerance(scalar tolerance);

	int localPatchIndex(int x,int y){return (y+mHalfSize)*mSize+(x+mHalfSize);}
	TerrainPatchNode::ptr patchAt(int x,int y){
		int index=localPatchIndex(x-mTerrainX,y-mTerrainY);
		if(index>=0 && index<mPatchGrid.size()) return mPatchGrid[index];
		else return NULL;
	}
	void setPatchAt(int x,int y,TerrainPatchNode::ptr patch){
		int index=localPatchIndex(x-mTerrainX,y-mTerrainY);
		if(index>=0 && index<mPatchGrid.size()) mPatchGrid[index]=patch;
	}

	int getTerrainX(){return mTerrainX;}
	int getTerrainY(){return mTerrainY;}

	int fromWorldXi(scalar x){x=Math::div(x,mPatchSize*mPatchScale.x);return Math::toInt(x>=0?x+Math::HALF:x-Math::HALF);}
	int fromWorldYi(scalar y){y=Math::div(y,mPatchSize*mPatchScale.y);return Math::toInt(y>=0?y+Math::HALF:y-Math::HALF);}
	scalar fromWorldXf(scalar x){return Math::div(x,mPatchSize*mPatchScale.x);}
	scalar fromWorldYf(scalar y){return Math::div(y,mPatchSize*mPatchScale.y);}
	scalar toWorldXi(int x){return Math::mul(Math::fromInt(x),mPatchSize*mPatchScale.x);}
	scalar toWorldYi(int y){return Math::mul(Math::fromInt(y),mPatchSize*mPatchScale.y);}
	scalar toWorldXf(scalar x){return Math::mul(x,mPatchSize*mPatchScale.x);}
	scalar toWorldYf(scalar y){return Math::mul(y,mPatchSize*mPatchScale.y);}

	void queueRenderables(node::CameraNode *camera,RenderQueue *queue);

	void logicUpdate(int dt,int scope);

	const Bound &getLocalBound() const{return super::getLocalBound();}
	void traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);

	void updateTarget();
	void createPatch(int x,int y);
	void destroyPatch(int x,int y);
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

	TerrainNodeListener *mListener;
	node::Node::ptr mTarget;
	TerrainNodeDataSource *mDataSource;
	int mSize,mHalfSize;
	int mTerrainX,mTerrainY;
	egg::Collection<TerrainPatchNode::ptr> mUnactivePatches;
	egg::Collection<TerrainPatchNode::ptr> mPatchGrid;
	int mPatchSize;
	Material::ptr mPatchMaterial;
	Material::ptr mPatchWaterMaterial;
	scalar mPatchTolerance;
	Vector3 mPatchScale;
	egg::Collection<scalar> mPatchData;
	scalar mUpdateTargetBias;
};

}
}
}

#endif
