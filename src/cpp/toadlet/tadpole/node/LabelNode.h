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

#ifndef TOADLET_TADPOLE_NODE_LABELNODE_H
#define TOADLET_TADPOLE_NODE_LABELNODE_H

#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/Font.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/node/RenderableNode.h>

namespace toadlet{
namespace tadpole{
namespace node{

class TOADLET_API LabelNode:public RenderableNode,public Renderable{
public:
	TOADLET_NODE(LabelNode,RenderableNode);

	LabelNode();
	virtual Node *create(Engine *engine);
	virtual void destroy();

	virtual void setFont(const Font::ptr &font);
	virtual void setText(const egg::String &text);
	virtual void setScaled(bool scaled);

	virtual const Material::ptr &getRenderMaterial() const{return mMaterial;}
	virtual const Matrix4x4 &getRenderTransform() const{return super::getVisualWorldTransform();}

	void queueRenderables(Scene *scene);
	void render(peeper::Renderer *renderer) const;

	peeper::VertexBufferAccessor vba;

protected:
	TOADLET_GIB_DEFINE(LabelNode);

	void rebuild();

	Font::ptr mFont;
	egg::String mText;
	bool mScaled;

	Material::ptr mMaterial;
	peeper::VertexData::ptr mVertexData;
	peeper::IndexData::ptr mIndexData;

	Matrix4x4 cache_queueRenderables_scale;
	Vector4 cache_queueRenderables_point;
};

}
}
}

#endif

