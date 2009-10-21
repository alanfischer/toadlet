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

#include <toadlet/tadpole/widget/Widget.h>
#include <toadlet/tadpole/widget/ParentWidget.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace widget{

Widget::Widget(Engine *engine):
	mCounter(new PointerCounter<Widget>(0)),
	mDestroyed(false),
	mEngine(engine),
	mType(0),

	mWidgetDestroyedListener(NULL),
	mOwnsWidgetDestroyedListener(false),

	mPositionX(0),mPositionY(0),
	mFractionalPositionX(0),mFractionalPositionY(0),
	mRightJustified(false),
	mWidth(0),mHeight(0),
	mFractionalWidth(0),mFractionalHeight(0),
	mRespectToWidth(false),mRespectToHeight(false),
	mLayout(Layout_FLOW),
	mBorder(0),

	mActionListener(NULL),
	mOwnsActionListener(false),

	mWorldPositionX(0),mWorldPositionY(0)
{
}

Widget::~Widget(){
	if(mDestroyed==false){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Widget: deleted without being destroyed");
		TOADLET_ASSERT(false);
	}

	setWidgetDestroyedListener(NULL,false);
	setActionListener(NULL,false);
}

void Widget::destroy(){
	if(mDestroyed){
		return;
	}

	Widget::ptr reference(this); // To make sure that the object is not deleted right away

	mDestroyed=true;

	if(mParent!=NULL){
		mParent->remove(this);
	}

	if(mWidgetDestroyedListener!=NULL){
		mWidgetDestroyedListener->widgetDestroyed(this);
	}
}

void Widget::setWidgetDestroyedListener(WidgetDestroyedListener *listener,bool owns){
	if(mOwnsWidgetDestroyedListener && mWidgetDestroyedListener!=NULL){
		delete mWidgetDestroyedListener;
	}
	mWidgetDestroyedListener=listener;
	mOwnsWidgetDestroyedListener=owns;
}

void Widget::removeAllWidgetDestroyedListeners(){
	setWidgetDestroyedListener(NULL,false);
}

void Widget::parentChanged(Widget *widget,ParentWidget *oldParent,ParentWidget *newParent){
	if(widget==this){
		mParent=ParentWidget::ptr(newParent);
	}
}

void Widget::setPosition(int x,int y){
	mPositionX=x;
	mPositionY=y;
}

void Widget::setFractionalPosition(scalar x,scalar y){
	mFractionalPositionX=x;
	mFractionalPositionY=y;
}

void Widget::setSize(int width,int height){
	mWidth=width;
	mHeight=height;
}

void Widget::setFractionalSize(scalar fractionalWidth,scalar fractionalHeight,bool respectToWidth,bool respectToHeight){
	mFractionalWidth=fractionalWidth;
	mFractionalHeight=fractionalHeight;
	mRespectToWidth=respectToWidth;
	mRespectToHeight=respectToHeight;
}

void Widget::setLayout(Layout layout){
	mLayout=layout;
}

void Widget::getDesiredSize(int size[]){
	if(mLayout==Layout_ABSOLUTE){
		size[0]=mWidth;
		size[1]=mHeight;
	}
	else if(mLayout==Layout_FRACTIONAL){
		if(mRespectToWidth && !mRespectToHeight){
			size[0]=Math::toInt(Math::mul(mFractionalWidth,Math::fromInt(size[0])));
			size[1]=Math::toInt(Math::mul(mFractionalHeight,Math::fromInt(size[0])));
		}
		else if(!mRespectToWidth && mRespectToHeight){
			size[0]=Math::toInt(Math::mul(mFractionalWidth,Math::fromInt(size[1])));
			size[1]=Math::toInt(Math::mul(mFractionalHeight,Math::fromInt(size[1])));
		}
		else{
			size[0]=Math::toInt(Math::mul(mFractionalWidth,Math::fromInt(size[0])));
			size[1]=Math::toInt(Math::mul(mFractionalHeight,Math::fromInt(size[1])));
		}
	}
	else{
		size[0]=0;
		size[1]=0;
	}
}

void Widget::setBorder(int border){
	mBorder=border;
}

bool Widget::action(Widget *widget){
	if(mActionListener!=NULL){
		mActionListener->action(widget);
	}
	else if(mParent!=NULL){
		mParent->action(widget);
	}
	return true;
}

void Widget::setActionListener(ActionListener *listener,bool owns){
	if(mOwnsActionListener && mActionListener!=NULL){
		delete mActionListener;
	}
	mActionListener=listener;
	mOwnsActionListener=owns;
}

}
}
}
