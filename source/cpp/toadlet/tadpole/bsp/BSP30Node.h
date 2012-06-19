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

#ifndef TOADLET_TADPOLE_BSP_BSP30NODE_H
#define TOADLET_TADPOLE_BSP_BSP30NODE_H

#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/Traceable.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/node/PartitionNode.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/bsp/BSP30Map.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class TOADLET_API BSP30ModelNode:public Node,public Traceable,public Visible{
public:
	TOADLET_NODE(BSP30ModelNode,Node);

	class TOADLET_API SubModel:public Renderable{
	public:
		TOADLET_SPTR(SubModel);

		SubModel(BSP30ModelNode *modelNode,BSP30Map *map);
		void destroy(){material=NULL;}

		Material *getRenderMaterial() const{return material;}
		const Transform &getRenderTransform() const{return modelNode->getWorldTransform();}
		const Bound &getRenderBound() const{return modelNode->getWorldBound();}
		void render(RenderManager *manager) const;

		BSP30ModelNode *modelNode;
		BSP30Map *map;
		Material::ptr material;
		BSP30Map::facedata *faces;
	};

	BSP30ModelNode();
	virtual ~BSP30ModelNode();
	Node *create(Scene *scene);
	void destroy();
	virtual Node *set(Node *node);

	void *hasInterface(int type);

	void setModel(BSP30Map::ptr map,const String &name);
	void setModel(BSP30Map::ptr map,int index);
	int getModel() const{return mModelIndex;}
	BSP30Map::ptr getMap(){return mMap;}

	inline int getNumSubModels() const{return mSubModels.size();}
	SubModel *getSubModel(int i){return mSubModels[i];}

	void showPlanarFaces(int plane);

	// Visible
	bool getRendered() const{return mRendered;}
	void setRendered(bool rendered){mRendered=rendered;}
	RenderState::ptr getSharedRenderState();
	void gatherRenderables(CameraNode *camera,RenderableSet *set);

	// Traceable interface
	const Bound &getBound() const{return super::getBound();}
	void traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);

protected:
	BSP30Map::ptr mMap;
	int mModelIndex;
	Collection<SubModel::ptr> mSubModels;
	RenderState::ptr mSharedRenderState;
	bool mRendered;
};

class TOADLET_API BSP30Node:public PartitionNode,public Traceable,public Renderable{
public:
	TOADLET_NODE(BSP30Node,PartitionNode);

	BSP30Node();
	virtual ~BSP30Node();
	virtual Node *set(Node *node);

	void *hasInterface(int type){return type==InterfaceType_TRACEABLE?(Traceable*)this:NULL;}

	void setMap(const String &name);
	void setMap(BSP30Map::ptr map);
	BSP30Map::ptr getMap() const{return mMap;}

	void setSkyName(const String &skyName);
	const String &getSkyName() const{return mSkyName;}
	void setSkyTextures(const String &skyDown,const String &skyUp,const String &skyWest,const String &skyEast,const String &skySouth,const String &skyNorth);
	MeshNode *getSkyNode() const{return mSkyNode;}

	void setStyleIntensity(int style,scalar intensity){mMap->styleIntensities[style]=255*intensity;}
	scalar getStyleIntensity(int style){return (float)mMap->styleIntensities[style]/255.0;}

	// Node items
	void nodeAttached(Node *node);
	void nodeRemoved(Node *node);
	void insertNodeLeafIndexes(const Collection<int> &indexes,Node *node);
	void removeNodeLeafIndexes(const Collection<int> &indexes,Node *node);

	void mergeWorldBound(Node *child,bool justAttached);
	void gatherRenderables(CameraNode *camera,RenderableSet *set);

	bool senseBoundingVolumes(SensorResultsListener *listener,const Bound &bound);
	bool sensePotentiallyVisible(SensorResultsListener *listener,const Vector3 &point);
	bool findAmbientForPoint(Vector4 &r,const Vector3 &point);

	// Traceable items
	const Bound &getBound() const{return super::getBound();}
	void traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size);

	// Renderable items
	Material *getRenderMaterial() const{return NULL;}
	const Transform &getRenderTransform() const{return mWorldTransform;}
	const Bound &getRenderBound() const{return mWorldBound;}
	void render(RenderManager *manager) const;

protected:
	void childTransformUpdated(Node *child);
	void addLeafToVisible(bleaf *leaf,CameraNode *camera);
	void findBoundLeafs(Collection<int> &leafs,Node *node);

	BSP30Map::ptr mMap;
	String mSkyName;
	MeshNode::ptr mSkyNode;

	class childdata{
	public:
		childdata():counter(0){}
		Collection<int> leafs;
		int counter;
	};

	class leafdata{
	public:
		Collection<Node*> occupants;
	};

	Collection<leafdata> mLeafData;
	leafdata mGlobalLeafData;
	uint8 *mMarkedFaces;
	Collection<BSP30Map::facedata*> mVisibleMaterialFaces;

	int mCounter;
	Collection<int> mLeafIndexes;
};

}
}
}

#endif
