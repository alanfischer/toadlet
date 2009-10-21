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

#ifndef TOADLET_TADPOLE_WIDGET_WINDOW_H
#define TOADLET_TADPOLE_WIDGET_WINDOW_H

#include <toadlet/tadpole/widget/ParentWidget.h>
#include <toadlet/tadpole/Font.h>
#include <toadlet/peeper/LightEffect.h>

namespace toadlet{
namespace tadpole{
namespace widget{

class TOADLET_API Window:public ParentWidget{
public:
	TOADLET_WIDGET(Window,ParentWidget);

	Window(Engine *engine);
	virtual ~Window();

	virtual void setDefaultFont(Font::ptr font);
	inline Font::ptr getDefaultFont() const{return mDefaultFont;}

	virtual void setDefaultInternalBorder(int border);
	inline int getDefaultInternalBorder() const{return mDefaultInternalBorder;}

	virtual void logicUpdate(int dt);
	virtual void visualUpdate(int dt);

	virtual void render(peeper::Renderer *renderer);

	virtual void registerUpdateWidget(Widget *widget);
	virtual void unregisterUpdateWidget(Widget *widget);

protected:
	void render(peeper::Renderer *renderer,Widget *widget);

	Font::ptr mDefaultFont;
	int mDefaultInternalBorder;

	egg::Collection<Widget::ptr> mUpdateWidgets;

	peeper::LightEffect mLightEffect;
};

}
}
}

#endif
