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

#ifndef TOADLET_TADPOLE_LABELNODE_H
#define TOADLET_TADPOLE_LABELNODE_H

#include <toadlet/peeper/IndexBufferAccessor.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/peeper/VertexBufferAccessor.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/tadpole/BaseComponent.h>
#include <toadlet/tadpole/Font.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/Visible.h>
#include <toadlet/tadpole/material/Material.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API LabelComponent:public BaseComponent,public Visible,public Renderable{
public:
	TOADLET_OBJECT(LabelComponent);

	LabelComponent(Engine *engine);
	void destroy();

	void parentChanged(Node *node);

	void setFont(Font *font);
	Font *getFont() const{return mFont;}

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

	Bound *getBound() const{return mBound;}
	
	// Visible
	bool getRendered() const{return mRendered;}
	void setRendered(bool rendered){mRendered=rendered;}
	RenderState::ptr getSharedRenderState();
	void gatherRenderables(Camera *camera,RenderableSet *set);

	// Renderable
	Material *getRenderMaterial() const{return mMaterial;}
	Transform *getRenderTransform() const{return mParent->getWorldTransform();}
	Bound *getRenderBound() const{return mParent->getWorldBound();}
	void render(RenderManager *manager) const;

protected:
	void updateLabel();

	/// @todo: Word wrapping and width need to be fixed so it makes sense.  Right now wordWrap only works in font space, not local space
	static String wordWrap(Font *font,float width,const String &text);

	Engine *mEngine;
	Font::ptr mFont;
	String mText;
	int mAlignment;
	bool mNormalized;
	bool mWordWrap;
	scalar mWidth;
	bool mRendered;
	Bound::ptr mBound;

	Material::ptr mMaterial;
	RenderState::ptr mSharedRenderState;
	VertexData::ptr mVertexData;
	IndexData::ptr mIndexData;
};

}
}

#endif

