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
#include <toadlet/tadpole/node/CameraAlignedNode.h>

namespace toadlet{
namespace tadpole{
namespace node{

/// @todo: Look more into letting the nodes be used for 2D widget layouts.
//  Originally we had the Sizeable interface for this, but it was removed
//  in favor of eventually letting us set the LocalBound as a AABox, then
//  the layout of a LabelNode or possibly size of a SpriteNode would be
//  determined by the LocalBound, if it was externally set.

class TOADLET_API LabelNode:public CameraAlignedNode,public Renderable{
public:
	TOADLET_NODE(LabelNode,CameraAlignedNode);

	LabelNode();
	virtual Node *create(Scene *scene);
	virtual void destroy();

	void setFont(const Font::ptr &font);
	Font::ptr getFont() const{return mFont;}

	void setText(const egg::String &text);
	const egg::String &getText() const{return mText;}

	void setAlignment(int alignment);
	int getAlignment() const{return mAlignment;}

	void setNormalized(bool normalized);
	bool getNormalized() const{return mNormalized;}

	void setWordWrap(bool wordWrap);
	bool getWordWrap() const{return mWordWrap;}

	void setColor(const peeper::Color &color){mMaterial->setLightEffect(peeper::LightEffect(color));}
	const peeper::Color &getColor(){return mMaterial->getLightEffect().diffuse;}

	void queueRenderables(CameraNode *camera,RenderQueue *queue);
	Material *getRenderMaterial() const{return mMaterial;}
	const Matrix4x4 &getRenderTransform() const{return super::getWorldTransform();}
	void render(peeper::Renderer *renderer) const;

	peeper::VertexBufferAccessor vba;

protected:
	TOADLET_GIB_DEFINE(LabelNode);

	void updateLabel();

	static egg::String wordWrap(Font::ptr font,float width,const egg::String &text);

	Font::ptr mFont;
	egg::String mText;
	int mAlignment;
	bool mNormalized;
	bool mWordWrap;
peeper::Color mColor;

	Material::ptr mMaterial;
	peeper::VertexData::ptr mVertexData;
	peeper::IndexData::ptr mIndexData;
};

}
}
}

#endif

