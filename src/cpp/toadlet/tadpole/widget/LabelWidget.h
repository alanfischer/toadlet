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

#ifndef TOADLET_TADPOLE_WIDGET_LABELWIDGET_H
#define TOADLET_TADPOLE_WIDGET_LABELWIDGET_H

#include <toadlet/peeper/TextureStage.h>
#include <toadlet/peeper/Blend.h>
#include <toadlet/tadpole/Font.h>
#include <toadlet/tadpole/widget/RenderableWidget.h>
#include <toadlet/tadpole/widget/Window.h>

namespace toadlet{
namespace tadpole{
namespace widget{

class ButtonWidget;

class TOADLET_API LabelWidget:public RenderableWidget{
public:
	TOADLET_WIDGET(LabelWidget,RenderableWidget);

	LabelWidget(Engine *engine);
	virtual ~LabelWidget();

	void setText(const egg::String &text);
	inline const egg::String &getText() const{return mText;}

	void setFont(Font::ptr font);
	inline Font::ptr getFont() const{return mFont;}

	void setColor(const peeper::Color &color){mColor.set(color);}
	const peeper::Color &getColor() const{return mColor;}

	void setWordWrap(bool wordWrap);
	inline bool getWordWrap() const{return mWordWrap;}

	virtual void parentChanged(Widget *widget,ParentWidget *oldParent,ParentWidget *newParent);

	virtual void setSize(int width,int height);

	virtual void getDesiredSize(int size[]);

	virtual void render(peeper::Renderer *renderer);

protected:
	egg::String wordWrap(Font::ptr font,int width,egg::String text);
	void rebuild();

	egg::String mText;
	Font::ptr mFont;
	peeper::Color mColor;
	bool mWordWrap;

	peeper::VertexData::ptr mVertexData;
	peeper::IndexData::ptr mIndexData;
	peeper::TextureStage::ptr mTextureStage;
	peeper::Blend mBlend;

	Window::ptr mWindow;

	friend class ButtonWidget;
};

}
}
}

#endif

