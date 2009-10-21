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

#ifndef TOADLET_TADPOLE_WIDGET_BORDERWIDGET_H
#define TOADLET_TADPOLE_WIDGET_BORDERWIDGET_H

#include <toadlet/tadpole/widget/RenderableWidget.h>

namespace toadlet{
namespace tadpole{
namespace widget{

class TOADLET_API BorderWidget:public RenderableWidget{
public:
	TOADLET_WIDGET(BorderWidget,RenderableWidget);

	BorderWidget(Engine *engine);
	virtual ~BorderWidget();

	void setColors(const peeper::Color &lightColor,const peeper::Color &darkColor);
	inline const peeper::Color getLightColor() const{return mLightColor;}
	inline const peeper::Color getDarkColor() const{return mDarkColor;}
	virtual bool getTrackColor() const{return true;}

	virtual void setSize(int width,int height);

	virtual void render(peeper::Renderer *renderer);

	peeper::VertexBufferAccessor vba;

protected:
	virtual void rebuild();

	peeper::Color mLightColor;
	peeper::Color mDarkColor;
	peeper::VertexData::ptr mVertexData;
	peeper::IndexData::ptr mIndexData;
};

}
}
}

#endif
