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
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/material/Material.h>
#include <toadlet/tadpole/node/CameraAlignedNode.h>

namespace toadlet{
namespace tadpole{
namespace node{

class TOADLET_API LabelNode:public CameraAlignedNode,public Visible,public Renderable{
public:
	TOADLET_NODE(LabelNode,CameraAlignedNode);

	LabelNode();
	virtual Node *create(Scene *scene);
	virtual void destroy();
	virtual Node *set(Node *node);

	void setFont(const Font::ptr &font);
	Font::ptr getFont() const{return mFont;}

	void setText(const String &text);
	const String &getText() const{return mText;}

	void setAlignment(int alignment);
	int getAlignment() const{return mAlignment;}

	void setNormalized(bool normalized);
	bool getNormalized() const{return mNormalized;}

	void setWordWrap(bool wordWrap);
	bool getWordWrap() const{return mWordWrap;}

	void setWidth(scalar width){mWidth=width;}
	inline scalar getWidth() const{return mWidth;}

	// Visible
	bool getRendered() const{return mRendered;}
	void setRendered(bool rendered){mRendered=rendered;}
	RenderState::ptr getSharedRenderState();
	void gatherRenderables(CameraNode *camera,RenderableSet *set);

	// Renderable
	Material *getRenderMaterial() const{return mMaterial;}
	const Transform &getRenderTransform() const{return getWorldTransform();}
	const Bound &getRenderBound() const{return getWorldBound();}
	void render(RenderManager *manager) const;

	VertexBufferAccessor vba;

protected:
	TOADLET_GIB_DEFINE(LabelNode);

	void updateLabel();

	/// @todo: Word wrapping and width need to be fixed so it makes sense.  Right now wordWrap only works in font space, not local space
	static String wordWrap(Font::ptr font,float width,const String &text);

	Font::ptr mFont;
	String mText;
	int mAlignment;
	bool mNormalized;
	bool mWordWrap;
	scalar mWidth;
	bool mRendered;

	Material::ptr mMaterial;
	RenderState::ptr mSharedRenderState;
	VertexData::ptr mVertexData;
	IndexData::ptr mIndexData;
};

}
}
}

#endif

