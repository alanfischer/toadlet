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

#ifndef TOADLET_TADPOLE_WIDGET_BUTTONWIDGET_H
#define TOADLET_TADPOLE_WIDGET_BUTTONWIDGET_H

#include <toadlet/tadpole/widget/ParentWidget.h>
#include <toadlet/tadpole/widget/LabelWidget.h>
#include <toadlet/tadpole/widget/BorderWidget.h>
#include <toadlet/tadpole/Font.h>

namespace toadlet{
namespace tadpole{
namespace widget{

class TOADLET_API ButtonWidget:public ParentWidget{
public:
	TOADLET_WIDGET(ButtonWidget,ParentWidget);

	ButtonWidget(Engine *engine,bool realButton=true);
	virtual ~ButtonWidget();

	virtual bool getSelectable() const{return true;}

	void setText(const egg::String &text){mLabel->setText(text);}
	inline const egg::String &getText() const{return mLabel->getText();}

	void setFont(Font::ptr font){mLabel->setFont(font);}
	inline Font::ptr getFont() const{return mLabel->getFont();}

	void setColor(const peeper::Color &color);
	void setColors(const peeper::Color &lightColor,const peeper::Color &darkColor);
	inline peeper::Color getLightColor() const{return mLightColor;}
	inline peeper::Color getDarkColor() const{return mDarkColor;}

	virtual void setInternalBorder(int border){mLabel->setBorder(border);}
	inline int getInternalBorder() const{return mLabel->getBorder();}

	virtual void getDesiredSize(int size[]);

	virtual void setFocused(bool focused);

	virtual bool keyPressed(int key);
	virtual bool keyReleased(int key);
	virtual bool mousePressed(int x,int y,int button);
	virtual bool mouseMoved(int x,int y);
	virtual bool mouseReleased(int x,int y,int button);

protected:
	bool mPressedByKey;
	bool mPressedByMouse;

	LabelWidget::ptr mLabel;
	BorderWidget::ptr mBorder;

	peeper::Color mLightColor;
	peeper::Color mDarkColor;
};

}
}
}

#endif
