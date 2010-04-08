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

#ifndef TOADLET_TADPOLE_BSP_BSPNODE_H
#define TOADLET_TADPOLE_BSP_BSPNODE_H

#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/tadpole/Traceable.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/node/PartitionNode.h>
#include <toadlet/tadpole/bsp/BSP30Map.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

/// @todo: When we switch the nodes to using an AABox bound, then modify these classes so we don't go bsp(aabox)->sphere(local)->aabox(physics);

class TOADLET_API BSP30ModelNode:public node::Node,public Traceable{
public:
	TOADLET_NODE(BSP30ModelNode,Node);

	class TOADLET_API SubModel:public Renderable{
	public:
		TOADLET_SHARED_POINTERS(SubModel);

		SubModel(BSP30ModelNode *modelNode,BSP30Map *map);

		Material *getRenderMaterial() const{return material;}
		const Matrix4x4 &getRenderTransform() const{return modelNode->getWorldTransform();}
		void render(peeper::Renderer *renderer) const;

		BSP30ModelNode *modelNode;
		BSP30Map *map;
		Material::ptr material;
		BSP30Map::facedata *faces;
	};

	BSP30ModelNode();
	virtual ~BSP30ModelNode();

	void setModel(BSP30Map::ptr map,const egg::String &name);
	void setModel(BSP30Map::ptr map,int index);
	int getModel() const{return mModelIndex;}

	void setVisible(bool visible){mVisible=visible;}
	bool getVisible() const{return mVisible;}

	void setInternalScope(int scope){mInternalScope=scope;}
	int getInternalScope() const{return mInternalScope;}

	inline int getNumSubModels() const{return mSubModels.size();}
	SubModel *getSubModel(int i){return mSubModels[i];}

	void queueRenderables(node::CameraNode *camera,RenderQueue *queue);

	// Traceable items
	const Sphere &getLocalBound() const{return super::getLocalBound();}
	void traceSegment(Collision &result,const Segment &segment,const Vector3 &size);

protected:
	BSP30Map::ptr mMap;
	int mModelIndex;
	bool mVisible; // Should be moved to the overall 'mesh/visible/renderable' class perhaps
	int mInternalScope;
	egg::Collection<SubModel::ptr> mSubModels;
};

class TOADLET_API BSP30Node:public node::PartitionNode,public Traceable,public Renderable{
public:
	TOADLET_NODE(BSP30Node,node::PartitionNode);

	BSP30Node();
	virtual ~BSP30Node();

	void setMap(const egg::String &name);
	void setMap(BSP30Map::ptr map);
	BSP30Map::ptr getMap() const{return mMap;}

	// Node items
	void nodeAttached(Node *node);
	void nodeRemoved(Node *node);
	void insertNodeLeafIndexes(const egg::Collection<int> &indexes,Node *node);
	void removeNodeLeafIndexes(const egg::Collection<int> &indexes,Node *node);

	void childTransformUpdated(Node *child);

	void queueRenderables(node::CameraNode *camera,RenderQueue *queue);

	bool senseBoundingVolumes(SensorResultsListener *listener,const Sphere &volume);

	// Traceable items
	const Sphere &getLocalBound() const{return super::getLocalBound();}
	void traceSegment(Collision &result,const Segment &segment,const Vector3 &size);

	// Renderable items
	Material *getRenderMaterial() const{return NULL;}
	const Matrix4x4 &getRenderTransform() const{return Math::IDENTITY_MATRIX4X4;}
	void render(peeper::Renderer *renderer) const;

protected:
	void addLeafToVisible(bleaf *leaf,node::CameraNode *camera);

	BSP30Map::ptr mMap;

	class childdata{
	public:
		childdata():counter(0){}
		egg::Collection<int> leafs;
		int counter;
	};

	class leafdata{
	public:
		egg::Collection<Node*> occupants;
	};

	egg::Collection<leafdata> mLeafData;
	uint8 *mMarkedFaces;
	toadlet::egg::Collection<BSP30Map::facedata*> mVisibleMaterialFaces;

	int mCounter;
	egg::Collection<int> mLeafIndexes;
};

}
}
}

#endif
