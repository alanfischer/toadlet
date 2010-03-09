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

#include <toadlet/tadpole/node/SceneNode.h>
#include <toadlet/tadpole/node/Traceable.h>
#include <toadlet/tadpole/bsp/BSP30Map.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Texture.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class BSP30SceneNode;

class TOADLET_API BSP30ModelNode:public node::Node,public node::Renderable,public node::Traceable{
public:
	TOADLET_NODE(BSP30ModelNode,Node);

	BSP30ModelNode();
	virtual ~BSP30ModelNode();
	virtual node::Node *create(Engine *engine);
	virtual void destroy();

	void setModel(BSP30Map::ptr map,const egg::String &name);
	void setModel(BSP30Map::ptr map,int index);
	int getModel() const{return mModelIndex;}

	virtual node::Renderable *isRenderable(){return this;}

	void setVisible(bool visible){mVisible=visible;}
	bool getVisible() const{return mVisible;}

	void setInternalScope(int scope){mInternalScope=scope;}
	int getInternalScope() const{return mInternalScope;}

	void queueRenderable(node::SceneNode *queue,node::CameraNode *camera);
	Material *getRenderMaterial() const;
	const Matrix4x4 &getRenderTransform() const;
	void render(peeper::Renderer *renderer) const;

	const Sphere &getLocalBound() const{return super::getLocalBound();}
	void traceSegment(Collision &result,const Segment &segment);

protected:
	BSP30Map::ptr mMap;
	int mModelIndex;
	bool mVisible;
	int mInternalScope;
};

class TOADLET_API BSP30SceneNode:public node::SceneNode{
public:
	TOADLET_NODE(BSP30SceneNode,SceneNode);

	BSP30SceneNode();
	virtual ~BSP30SceneNode();
	virtual node::Node *create(Engine *engine);
	virtual void destroy();

	Scene *getRootScene(){return this;}

	void setMap(BSP30Map::ptr map);
	BSP30Map::ptr getMap() const{return mMap;}

	bool performAABoxQuery(SpacialQuery *query,const AABox &box,bool exact);

	bool attach(Node *node);
	bool remove(Node *node);
	void insertNodeLeafIndexes(const egg::Collection<int> &indexes,Node *node);
	void removeNodeLeafIndexes(const egg::Collection<int> &indexes,Node *node);

	void childTransformUpdated(Node *child);

//protected:
	// TODO: We need to get away from this markfaces stuff, make it more universal
	class RendererData{
	public:
		toadlet::egg::Collection<uint8> markedFaces;
		toadlet::egg::Collection<toadlet::egg::Collection<int> > textureVisibleFaces;
	};

	// TODO: Switch this to using an index into the lightmap array, and a matrix offset/scale
	class RenderFace{
	public:
		peeper::IndexData::ptr indexData;
		peeper::Texture::ptr lightmap;
	};

	// TODO: We need a better hook to start rendering the level, since in theory this wont have access to preLayerRender,
	//  cause it wont be a main scene node
	void queueRenderables();
	bool preLayerRender(peeper::Renderer *renderer,int layer);
	void processVisibleFaces(node::CameraNode *camera);
	void renderVisibleFaces(peeper::Renderer *renderer);
	void addLeafToVisible(bleaf *leaf,RendererData &data,node::CameraNode *camera) const;
	void calculateSurfaceExtents(bface *face,Vector2 &mins,Vector2 &maxs);
	egg::image::Image::ptr computeLightmap(bface *face,const Vector2 &mins,const Vector2 &maxs);

	peeper::VertexBufferAccessor vba;

	BSP30Map::ptr mMap;
	RendererData mRendererData;
	peeper::VertexData::ptr mVertexData;
	egg::Collection<RenderFace> mRenderFaces;

	int mCounter;
	egg::Collection<int> mLeafIndexes;

	class childdata{
	public:
		childdata():counter(0){}
		egg::Collection<int> leafs;
		int counter;
	};

	class bleafdata{
	public:
		egg::Collection<Node*> occupants;
	};
	egg::Collection<bleafdata> mLeafData;
};

}
}
}

#endif
