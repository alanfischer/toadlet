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

#ifndef TOADLET_TADPOLE_WIDGET_SPRITEWIDGET_H
#define TOADLET_TADPOLE_WIDGET_SPRITEWIDGET_H

#include <toadlet/peeper/AnimatedTexture.h>
#include <toadlet/peeper/VertexData.h>
#include <toadlet/peeper/IndexData.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/widget/RenderableWidget.h>

namespace toadlet{
namespace tadpole{
namespace widget{

class TOADLET_API SpriteWidget:public RenderableWidget{
public:
	TOADLET_WIDGET(SpriteWidget,RenderableWidget);

	SpriteWidget(Engine *engine);
	virtual ~SpriteWidget();

	void load(scalar width,scalar height,const egg::String &name);
	void load(scalar width,scalar height,const peeper::Texture::ptr &texture);

	void setFrame(int frame);
	int getNumFrames() const;

	void render(peeper::Renderer *renderer);
	virtual bool applyScale() const{return true;}

	inline peeper::TextureStage::ptr getTextureStage() const{return mTextureStage;}

protected:
	void updateFrame();

	peeper::Texture::ptr mTexture;
	peeper::AnimatedTexture::ptr mAnimatedTexture;
	Matrix4x4 mTextureMatrix;

	int mFrame;

	peeper::VertexData::ptr mVertexData;
	peeper::IndexData::ptr mIndexData;
	peeper::TextureStage::ptr mTextureStage;
};

}
}
}

#endif

