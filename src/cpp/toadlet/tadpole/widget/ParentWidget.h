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

#ifndef TOADLET_TADPOLE_WIDGET_PARENTWIDGET_H
#define TOADLET_TADPOLE_WIDGET_PARENTWIDGET_H

#include <toadlet/tadpole/widget/Widget.h>
#include <toadlet/egg/Collection.h>

namespace toadlet{
namespace tadpole{
namespace widget{

class TOADLET_API ParentWidget:public Widget{
public:
	TOADLET_WIDGET(ParentWidget,Widget);

	ParentWidget(Engine *engine);
	virtual ~ParentWidget();
	virtual void destroy();

	virtual void removeAllWidgetDestroyedListeners();

	virtual void parentChanged(Widget *widget,ParentWidget *oldParent,ParentWidget *newParent);

	virtual bool attach(Widget *widget);
	virtual bool remove(Widget *widget);

	inline int getNumChildren() const{return mChildren.size();}
	inline const Widget::ptr &getChild(int i) const{return mChildren[i];}

	virtual void setLayoutManager(bool layoutManager){mLayoutManager=layoutManager;}
	inline bool getLayoutManager() const{return mLayoutManager;}

	virtual void setSize(int width,int height);

	virtual void getDesiredSize(int size[]);

	virtual void layout();
	virtual void layout(bool doLayout,int size[]);

	virtual bool keyPressed(int key);
	virtual bool keyReleased(int key);
	virtual bool mousePressed(int x,int y,int button);
	virtual bool mouseMoved(int x,int y);
	virtual bool mouseReleased(int x,int y,int button);

protected:
	egg::Collection<Widget::ptr> mChildren;

	bool mLayoutManager;

	friend class Window;
};

}
}
}

#endif
