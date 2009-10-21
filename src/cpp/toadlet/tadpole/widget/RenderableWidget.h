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

#ifndef TOADLET_TADPOLE_WIDGET_RENDERABLEWIDGET_H
#define TOADLET_TADPOLE_WIDGET_RENDERABLEWIDGET_H

#include <toadlet/tadpole/widget/Widget.h>
#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/Colors.h>

namespace toadlet{
namespace tadpole{
namespace widget{

class TOADLET_API RenderableWidget:public Widget{
public:
	TOADLET_WIDGET(RenderableWidget,Widget);

	RenderableWidget(Engine *engine);
	virtual ~RenderableWidget();

	virtual void render(peeper::Renderer *renderer)=0;
	virtual bool applyScale() const{return false;}
	virtual const peeper::Color &getColor() const{return peeper::Colors::WHITE;}
	virtual bool getTrackColor() const{return false;}
};

}
}
}

#endif

