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

#ifndef TOADLET_TADPOLE_WIDGET_WIDGET_H
#define TOADLET_TADPOLE_WIDGET_WIDGET_H

#include <toadlet/egg/IntrusivePointer.h>
#include <toadlet/egg/WeakPointer.h>
#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/widget/ActionListener.h>
#include <toadlet/tadpole/widget/WidgetDestroyedListener.h>

#ifndef TOADLET_WIDGET
#	define TOADLET_WIDGET(Class,SuperClass) \
		typedef SuperClass Super; \
		TOADLET_INTRUSIVE_POINTERS(Class,toadlet::tadpole::widget::Widget)
#endif

namespace toadlet{
namespace tadpole{

class Engine;

namespace widget{

class ParentWidget;
class Window;

class TOADLET_API Widget{
public:
	TOADLET_INTRUSIVE_POINTERS(Widget,Widget);

	// Types
	const static int Type_PARENT=		1<<0;
	const static int Type_WINDOW=		1<<1;
	const static int Type_RENDERABLE=	1<<2;
	const static int Type_LABEL=		1<<3;
	const static int Type_BORDER=		1<<4;
	const static int Type_BUTTON=		1<<5;
	const static int Type_SPRITE=		1<<6;

	enum Layout{
		Layout_ABSOLUTE=0,
		Layout_FRACTIONAL,
		Layout_FILL,
		Layout_FLOW,
	};

	Widget(Engine *engine);
	virtual ~Widget();
	virtual void destroy();

	inline bool instanceOf(uint64 type) const{return (mType&type)>0;}
	inline uint64 getType() const{return mType;}
	inline bool destroyed() const{return mDestroyed;}

	virtual void setWidgetDestroyedListener(WidgetDestroyedListener *listener,bool owns);
	inline WidgetDestroyedListener *getWidgetDestroyedListener() const{return mWidgetDestroyedListener;}
	virtual void removeAllWidgetDestroyedListeners();

	virtual void parentChanged(Widget *widget,ParentWidget *oldParent,ParentWidget *newParent);
	inline ParentWidget *getParent() const{return mParent;}

	virtual void setPosition(int x,int y);
	inline int getPositionX() const{return mPositionX;}
	inline int getPositionY() const{return mPositionY;}

	virtual void setFractionalPosition(scalar x,scalar y);
	inline scalar getFractionalPositionX() const{return mFractionalPositionX;}
	inline scalar getFractionalPositionY() const{return mFractionalPositionY;}

	virtual void setSize(int width,int height);
	inline int getWidth() const{return mWidth;}
	inline int getHeight() const{return mHeight;}

	virtual void setFractionalSize(scalar width,scalar height,bool respectToWidth=true,bool respectToHeight=true);
	inline scalar getFractionalWidth() const{return mFractionalWidth;}
	inline scalar getFractionalHeight() const{return mFractionalHeight;}

	void setRightJustified(bool right){mRightJustified=right;}
	inline bool getRightJustified() const{return mRightJustified;}

	inline bool isPointInside(int x,int y) const{return x>=mPositionX && x<mPositionX+mWidth && y>=mPositionY && y<mPositionY+mHeight;}

	virtual void setLayout(Layout layout);
	inline Layout getLayout() const{return mLayout;}

	/// Takes in a maximum possible size, and writes out its desired size
	virtual void getDesiredSize(int size[]); /// @todo  If we add in some sort of integer Vector2, return that instead

	virtual void setBorder(int border);
	inline int getBorder() const{return mBorder;}

	virtual void setFocused(bool focused){}

	/// Only called if the Widget registers itself with the Window in registerUpdateWidget.
	/// Dont forget to call unregisterUpdateWidget in its destroy.
	virtual void logicUpdate(int dt){}
	virtual void visualUpdate(int dt){}

	virtual bool keyPressed(int key){return false;}
	virtual bool keyReleased(int key){return false;}
	virtual bool mousePressed(int x,int y,int button){return false;}
	virtual bool mouseMoved(int x,int y){return false;}
	virtual bool mouseReleased(int x,int y,int button){return false;}

	virtual bool action(Widget *widget);

	virtual void setActionListener(ActionListener *listener,bool owns);
	inline ActionListener *getActionListener() const{return mActionListener;}

	inline Engine *getEngine() const{return mEngine;}

	inline egg::PointerCounter<Widget> *getCounter() const{return mCounter;}

protected:
	egg::PointerCounter<Widget> *mCounter;
	bool mDestroyed;
	Engine *mEngine;
	uint64 mType;

	WidgetDestroyedListener *mWidgetDestroyedListener;
	bool mOwnsWidgetDestroyedListener;

	egg::IntrusivePointer<ParentWidget,Widget> mParent;

	int mPositionX,mPositionY;
	scalar mFractionalPositionX,mFractionalPositionY;
	bool mRightJustified;
	int mWidth,mHeight;
	scalar mFractionalWidth,mFractionalHeight;
	bool mRespectToWidth,mRespectToHeight;
	Layout mLayout;
	int mBorder;

	ActionListener *mActionListener;
	bool mOwnsActionListener;

	int mWorldPositionX,mWorldPositionY;

	Matrix4x4 mWorldTransform;

	friend class Window;
};

}
}
}

#endif
