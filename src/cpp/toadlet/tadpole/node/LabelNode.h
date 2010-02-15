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
#include <toadlet/tadpole/node/Node.h>
#include <toadlet/tadpole/node/Renderable.h>
#include <toadlet/tadpole/node/Sizeable.h>

namespace toadlet{
namespace tadpole{
namespace node{

class TOADLET_API LabelNode:public Node,public Renderable,public Sizeable{
public:
	TOADLET_NODE(LabelNode,Node);

	LabelNode();
	virtual Node *create(Engine *engine);
	virtual void destroy();

	Renderable *isRenderable(){return this;}
	Sizeable *isSizeable(){return this;}

	void setFont(const Font::ptr &font);
	Font::ptr getFont() const{return mFont;}

	void setText(const egg::String &text);
	const egg::String &getText() const{return mText;}

	void setPerspective(bool perspective);
	bool getPerspective() const{return mPerspective;}

	void setAlignment(int alignment);
	int getAlignment() const{return mAlignment;}

	void setPixelSpace(bool pixelSpace);
	bool getPixelSpace() const{return mPixelSpace;}

	void setWordWrap(bool wordWrap);
	bool getWordWrap() const{return mWordWrap;}

	void setSize(scalar x,scalar y,scalar z);
	void setSize(const Vector3 &size);
	const Vector3 &getSize() const{return mSize;}
	const Vector3 &getDesiredSize();

	void queueRenderable(SceneNode *scene,CameraNode *camera);
	Material *getRenderMaterial() const{return mMaterial;}
	const Matrix4x4 &getRenderTransform() const{return super::getWorldRenderTransform();}
	void render(peeper::Renderer *renderer) const;

	peeper::VertexBufferAccessor vba;

protected:
	TOADLET_GIB_DEFINE(LabelNode);

	void updateLabel();
	void updateBound();

	static egg::String wordWrap(Font::ptr font,float width,const egg::String &text);

	Font::ptr mFont;
	egg::String mText;
	bool mPerspective;
	int mAlignment;
	bool mPixelSpace;
	bool mWordWrap;
	Vector3 mSize;

	Material::ptr mMaterial;
	peeper::VertexData::ptr mVertexData;
	peeper::IndexData::ptr mIndexData;

	Matrix4x4 cache_queueRenderable_scale;
	Vector4 cache_queueRenderable_point;
};

}
}
}

#endif

