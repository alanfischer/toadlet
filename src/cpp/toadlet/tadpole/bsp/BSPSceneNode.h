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

#include <toadlet/tadpole/node/Scene.h>
#include <toadlet/tadpole/bsp/BSPMap.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/Texture.h>

namespace toadlet{
namespace tadpole{
namespace bsp{

class TOADLET_API BSPSceneNode:public node::Scene{
public:
	TOADLET_NODE(BSPSceneNode,Scene);

	BSPSceneNode();
	virtual ~BSPSceneNode();
	virtual node::Node *create(Engine *engine);
	virtual void destroy();

	void setBSPMap(BSPMap::ptr map);
	BSPMap::ptr getBSPMap() const{return mBSPMap;}

protected:
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
	bool preLayerRender(peeper::Renderer *renderer,int layer);
	void processVisibleFaces(node::CameraNode *camera);
	void renderVisibleFaces(peeper::Renderer *renderer);
	int findLeaf(const Vector3 &point) const;
	void addLeafToVisible(const Leaf &leaf,RendererData &data,node::CameraNode *camera) const;
	void decompressVIS();

	peeper::VertexBufferAccessor vba;

	BSPMap::ptr mBSPMap;
	RendererData mRendererData;
	peeper::VertexData::ptr mVertexData;
	egg::Collection<RenderFace> mRenderFaces;

	// TODO: And clean up these hacky members.
	egg::Collection<peeper::Texture::ptr> textures; // Shouldnt be storing textures here, instead we need to go by material
	egg::Collection<egg::Collection<int> > leafVisibility;
};

}
}
}

#endif
