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

#ifndef TOADLET_TADPOLE_STUDIO_SPRITEMODELNODE_H
#define TOADLET_TADPOLE_STUDIO_SPRITEMODELNODE_H

#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/node/CameraAlignedNode.h>
#include <toadlet/tadpole/studio/SpriteModel.h>

namespace toadlet{
namespace tadpole{
namespace studio{

class TOADLET_API SpriteModelNode:public CameraAlignedNode,public Renderable,public Visible{
public:
	TOADLET_NODE(SpriteModelNode,CameraAlignedNode);

	SpriteModelNode();
	Node *create(Scene *scene);
	void destroy();
	Node *set(Node *node);

	void *hasInterface(int type);

	void setModel(const String &name);
	void setModel(SpriteModel::ptr model);
	SpriteModel::ptr getModel() const{return mModel;}

	void setSequenceTime(scalar time){mSequenceTime=time;}
	scalar getSequenceTime() const{return mSequenceTime;}

	// Visible
	bool getRendered() const{return mRendered;}
	void setRendered(bool rendered){mRendered=rendered;}
	RenderState::ptr getSharedRenderState();
	void gatherRenderables(CameraNode *camera,RenderableSet *set);

	// Renderable
	Material *getRenderMaterial() const{return mSequenceTime<mMaterials.size()?mMaterials[Math::toInt(mSequenceTime)]:NULL;}
	const Transform &getRenderTransform() const{return getWorldTransform();}
	const Bound &getRenderBound() const{return getWorldBound();}
	void render(RenderDevice *renderDevice) const;

protected:
	TOADLET_GIB_DEFINE(SpriteModelNode);

	void createBuffers();
	void updateBuffers();
	void destroyBuffers();

	SpriteModel::ptr mModel;
	bool mRendered;
	scalar mSequenceTime;
	Collection<Material::ptr> mMaterials;
	VertexData::ptr mVertexData;
	IndexData::ptr mIndexData;
	Material::ptr mOwnedMaterial;
};

}
}
}

#endif
