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

#ifndef TOADLET_TADPOLE_NODE_SPRITENODE_H
#define TOADLET_TADPOLE_NODE_SPRITENODE_H

#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/node/CameraAlignedNode.h>

namespace toadlet{
namespace tadpole{
namespace node{

class TOADLET_API SpriteNode:public CameraAlignedNode,public Renderable{
public:
	TOADLET_NODE(SpriteNode,CameraAlignedNode);
	
	SpriteNode();
	virtual Node *create(Scene *scene);
	virtual void destroy();

	void setMaterial(const egg::String &name);
	void setMaterial(Material::ptr material);
	Material::ptr getMaterial() const{return mMaterial;}

	void setAlignment(int alignment);
	int getAlignment() const{return mAlignment;}

	void queueRenderables(CameraNode *camera,RenderQueue *queue);
	Material *getRenderMaterial() const{return mMaterial;}
	const Matrix4x4 &getRenderTransform() const{return super::getWorldTransform();}
	void render(peeper::Renderer *renderer) const;

	peeper::VertexBufferAccessor vba;

protected:
	TOADLET_GIB_DEFINE(SpriteNode);

	void updateSprite();

	int mAlignment;

	Material::ptr mMaterial;
	peeper::VertexData::ptr mVertexData;
	peeper::IndexData::ptr mIndexData;
};

}
}
}

#endif

