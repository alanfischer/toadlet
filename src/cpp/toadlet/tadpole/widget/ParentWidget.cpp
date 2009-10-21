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

#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/widget/ParentWidget.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace widget{

ParentWidget::ParentWidget(Engine *engine):Widget(engine),
	//mWidgets,
	mLayoutManager(true)
{
	mType|=Type_PARENT;
}

ParentWidget::~ParentWidget(){
}

void ParentWidget::destroy(){
	while(mChildren.size()>0){
		mChildren[0]->destroy();
	}

	Super::destroy();
}

void ParentWidget::removeAllWidgetDestroyedListeners(){
	Super::removeAllWidgetDestroyedListeners();

	int i;
	for(i=mChildren.size()-1;i>=0;--i){
		mChildren[i]->removeAllWidgetDestroyedListeners();
	}
}

void ParentWidget::parentChanged(Widget *widget,ParentWidget *oldParent,ParentWidget *newParent){
	Super::parentChanged(widget,oldParent,newParent);

	int i;
	for(i=mChildren.size()-1;i>=0;--i){
		mChildren[i]->parentChanged(widget,oldParent,newParent);
	}
}

bool ParentWidget::attach(Widget *widget){
	if(widget->destroyed()){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"cannot attach a destroyed widget");
		return false;
	}

	if(widget->getParent()!=NULL){
		widget->getParent()->remove(widget);
	}

	mChildren.add(widget);

	widget->parentChanged(widget,NULL,this);
	ParentWidget *parent=this;
	while(parent->mParent!=NULL){
		widget->parentChanged(parent,NULL,parent->mParent);
		parent=parent->mParent;
	}

	layout(); /// @todo Set a layoutNeeded flag instead

	return true;
}

bool ParentWidget::remove(Widget *widget){
	Widget::ptr reference(widget); // To make sure that the object is not deleted until we can call parentChanged
	if(mChildren.remove(reference)){
		ParentWidget *parent=this;
		while(parent->mParent!=NULL){
			widget->parentChanged(parent,parent->mParent,NULL);
			parent=parent->mParent;
		}
		widget->parentChanged(widget,this,NULL);

		layout(); /// @todo Set a layoutNeeded flag instead

		return true;
	}
	else{
		return false;
	}
}

void ParentWidget::setSize(int width,int height){
	Super::setSize(width,height);

	layout();
}

void ParentWidget::getDesiredSize(int size[]){
	if(mLayout==Layout_FRACTIONAL){
		size[0]=Math::toInt(Math::mul(Math::fromInt(size[0]),mFractionalWidth));
		size[1]=Math::toInt(Math::mul(Math::fromInt(size[1]),mFractionalHeight));
	}
	else{
		layout(false,size);
	}
}

void ParentWidget::layout(){
	int size[2];
	size[0]=getWidth();
	size[1]=getHeight();
	layout(true,size);
}

/// By default its just a vertical flow layout
void ParentWidget::layout(bool doLayout,int size[]){
	if(mLayoutManager==false){
		return;
	}

	int maxWidth=size[0];
	int maxHeight=size[1];

	int maxWidgetWidth=0;
	int y=0;
	int numFlowWidgets=0;
	int flowWidget=0;
	int i;

	for(i=0;i<mChildren.size();++i){
		Widget *widget=mChildren[i];
		if(widget->getLayout()==Layout_FLOW){
			numFlowWidgets++;
		}
	}

	for(i=0;i<mChildren.size();++i){
		Widget *widget=mChildren[i];
		int border=widget->getBorder();

		if(widget->getLayout()==Layout_ABSOLUTE){
			// Just reset the size, as a notification that it has been layed out
			// No other work is necessary
			widget->setSize(widget->getWidth(),widget->getHeight());
		}
		else if(widget->getLayout()==Layout_FRACTIONAL){
			int wsize[2];
			wsize[0]=maxWidth-border*2;
			wsize[1]=maxHeight-border*2;

			if(doLayout){
				int wsize2[2];wsize2[0]=wsize[0];wsize2[1]=wsize[1];
				widget->getDesiredSize(wsize2);
				widget->setSize(wsize2[0],wsize2[1]);

				if(widget->getRightJustified()){
					widget->setPosition(
						Math::toInt(Math::mul(widget->getFractionalPositionX(),Math::fromInt(wsize[0])))-wsize2[0],
						Math::toInt(Math::mul(widget->getFractionalPositionY(),Math::fromInt(wsize[1]))));
				}
				else{
					widget->setPosition(
						Math::toInt(Math::mul(widget->getFractionalPositionX(),Math::fromInt(wsize[0]))),
						Math::toInt(Math::mul(widget->getFractionalPositionY(),Math::fromInt(wsize[1]))));
				}
			}
		}
		else if(widget->getLayout()==Layout_FILL){
			int width=getWidth()-border*2;
			int height=getHeight()-border*2;
			if(width<0) width=0;
			if(height<0) height=0;

			if(doLayout){
				widget->setPosition(border,border);
				widget->setSize(width,height);
			}
		}
		else if(widget->getLayout()==Layout_FLOW){
			int wsize[2];
			wsize[0]=maxWidth-border*2;
			wsize[1]=maxHeight-border*2;
			widget->getDesiredSize(wsize);
			int width=wsize[0];
			int height=wsize[1];
//			if(width+border*2>maxWidth) width=maxWidth-border*2;
//			if(height+border*2>maxHeight-y) height=maxHeight-y-border*2;

			if(doLayout){
			// Flowing
//				widget->setPosition(
//					(getWidth()-width)/2,
//					y+border);

			// Centering
				widget->setPosition(
					(getWidth()-width)/2,
					flowWidget*getHeight()/(numFlowWidgets+1) + getHeight()/(numFlowWidgets+1) - height/2);

				widget->setSize(width,height);
			}

			if(width+border*2>maxWidgetWidth){
				maxWidgetWidth=width+border*2;
			}
			y+=height+border*2;

			flowWidget++;
		}
	}

	if(size!=NULL){
		size[0]=maxWidgetWidth;
		size[1]=y;
	}
}

bool ParentWidget::keyPressed(int key){
	return false;
}

bool ParentWidget::keyReleased(int key){
	return false;
}

bool ParentWidget::mousePressed(int x,int y,int button){
	bool handled=false;

	if(isPointInside(x,y)){
		x-=mPositionX;
		y-=mPositionY;

		int i;
		for(i=0;i<mChildren.size();++i){
			Widget::ptr widget=mChildren[i];
			if(widget->isPointInside(x,y)){
				handled|=widget->mousePressed(x,y,button);
			}
		}

		x+=mPositionX;
		y+=mPositionY;
	}

	return handled;
}

bool ParentWidget::mouseMoved(int x,int y){
	bool handled=false;

	x-=mPositionX;
	y-=mPositionY;

	int i;
	for(i=0;i<mChildren.size();++i){
		Widget::ptr widget=mChildren[i];
		handled|=widget->mouseMoved(x,y);
	}

	x+=mPositionX;
	y+=mPositionY;

	return handled;
}

bool ParentWidget::mouseReleased(int x,int y,int button){
	bool handled=false;

	if(isPointInside(x,y)){
		x-=mPositionX;
		y-=mPositionY;

		int i;
		for(i=0;i<mChildren.size();++i){
			Widget::ptr widget=mChildren[i];
			if(widget->isPointInside(x,y)){
				handled|=widget->mouseReleased(x,y,button);
			}
		}

		x+=mPositionX;
		y+=mPositionY;
	}

	return handled;
}

}
}
}
