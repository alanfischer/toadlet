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

#include <toadlet/tadpole/widget/ButtonWidget.h>
#include <toadlet/tadpole/widget/ParentWidget.h>
#include <toadlet/peeper/Colors.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace widget{

ButtonWidget::ButtonWidget(Engine *engine,bool realButton):ParentWidget(engine),
	mPressedByKey(false),
	mPressedByMouse(false)

	//mLightColor,
	//mDarkColor
{
	mType|=Type_BUTTON;

	mLabel=new LabelWidget(mEngine);
	mLabel->setWordWrap(false);
	mLabel->setLayout(Layout_FLOW);
	attach(mLabel);

	if(realButton){
		mBorder=new BorderWidget(mEngine);
		mBorder->setLayout(Layout_FILL);
		attach(mBorder);
	}

	setColors(Colors::WHITE,Colors::GREY);
}

ButtonWidget::~ButtonWidget(){
}

void ButtonWidget::setColor(const Color &color){
	mLightColor.set(color);
	mDarkColor.set(color);

	mLabel->setColor(mLightColor);
	if(mBorder!=NULL){
		mBorder->setColors(mLightColor,mDarkColor);
	}
}

void ButtonWidget::setColors(const Color &lightColor,const Color &darkColor){
	mLightColor.set(lightColor);
	mDarkColor.set(darkColor);

	mLabel->setColor(mLightColor);
	if(mBorder!=NULL){
		mBorder->setColors(mLightColor,mDarkColor);
	}
}

void ButtonWidget::getDesiredSize(int size[]){
	if(mLabel->mBorder==0 && mLabel->mWindow!=NULL){
		mLabel->setBorder(mLabel->mWindow->getDefaultInternalBorder());
	}

	Super::getDesiredSize(size);
}

void ButtonWidget::setFocused(bool focused){
	Widget::setFocused(focused);

	if(focused==false){
		mPressedByKey=false;
		mPressedByMouse=false;
	}
}

bool ButtonWidget::keyPressed(int key){
//	if(event==KE_ACTION){
//		mPressedByKey=true;
//	}

	return false;
}

bool ButtonWidget::keyReleased(int key){
//	if(event==KE_ACTION){
//		if(mPressedByKey){
//			action(this);
//		}
//		mPressedByKey=false;
//	}

	return false;
}

bool ButtonWidget::mousePressed(int x,int y,int button){
	if(mBorder!=NULL && isPointInside(x,y)){
		mPressedByMouse=true;
		mBorder->setColors(mDarkColor,mLightColor);

		return true;
	}

	return false;
}

bool ButtonWidget::mouseMoved(int x,int y){
	if(mBorder!=NULL && isPointInside(x,y)==false && mPressedByMouse==true){
		action(this);
		mPressedByMouse=false;
		mBorder->setColors(mLightColor,mDarkColor);

		return true;
	}

	return false;
}

bool ButtonWidget::mouseReleased(int x,int y,int button){
	if(mBorder!=NULL && isPointInside(x,y) && mPressedByMouse==true){
		action(this);
		mPressedByMouse=false;
		mBorder->setColors(mLightColor,mDarkColor);

		return true;
	}

	return false;
}

}
}
}

