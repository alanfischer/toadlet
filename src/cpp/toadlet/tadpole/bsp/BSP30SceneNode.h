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

#ifndef TOADLET_TADPOLE_BSP_BSPSCENENODE_H
#define TOADLET_TADPOLE_BSP_BSPSCENENODE_H

#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/tadpole/Traceable.h>
#include <toadlet/tadpole/node/SceneNode.h>
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
		const Matrix4x4 &getRenderTransform() const{return modelNode->getWorldRenderTransform();}
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

	void renderUpdate(node::CameraNode *camera,RenderQueue *queue);

	const Sphere &getLocalBound() const{return super::getLocalBound();}
	void traceSegment(Collision &result,const Segment &segment,const Vector3 &size);

protected:
	BSP30Map::ptr mMap;
	int mModelIndex;
	bool mVisible; // Should be moved to the overall 'mesh/visible/renderable' class perhaps
	int mInternalScope;
	egg::Collection<SubModel::ptr> mSubModels;
};

class TOADLET_API BSP30SceneNode:public node::SceneNode,public Traceable{
public:
	TOADLET_NONINSTANCIABLENODE(BSP30SceneNode,SceneNode);

	BSP30SceneNode(Engine *engine);
	virtual ~BSP30SceneNode();

	Scene *getRootScene(){return this;}

	void setMap(const egg::String &name);
	void setMap(BSP30Map::ptr map);
	BSP30Map::ptr getMap() const{return mMap;}

	bool performAABoxQuery(SpacialQuery *query,const AABox &box,bool exact);

	const Sphere &getLocalBound() const{return super::getLocalBound();}
	void traceSegment(Collision &result,const Segment &segment,const Vector3 &size);

	bool attach(Node *node);
	bool remove(Node *node);
	void insertNodeLeafIndexes(const egg::Collection<int> &indexes,Node *node);
	void removeNodeLeafIndexes(const egg::Collection<int> &indexes,Node *node);

	void childTransformUpdated(Node *child);

	void renderUpdate(node::CameraNode *camera,RenderQueue *queue);

protected:
	bool preLayerRender(peeper::Renderer *renderer,node::CameraNode *camera,int layer);
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
