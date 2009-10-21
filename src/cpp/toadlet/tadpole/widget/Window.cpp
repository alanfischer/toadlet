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

#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/Blend.h>
#include <toadlet/peeper/LightEffect.h>
#include <toadlet/tadpole/widget/Window.h>
#include <toadlet/tadpole/widget/LabelWidget.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace widget{

Window::Window(Engine *engine):ParentWidget(engine),
	//mDefaultFont,
	mDefaultInternalBorder(0)
{
	mType|=Type_WINDOW;
}

Window::~Window(){
}

void Window::setDefaultFont(Font::ptr font){
	mDefaultFont=font;

	layout();
}

void Window::setDefaultInternalBorder(int border){
	mDefaultInternalBorder=border;

	layout();
}

void Window::logicUpdate(int dt){
	int i;
	for(i=0;i<mUpdateWidgets.size();++i){
		Widget::ptr widget=mUpdateWidgets[i];
		widget->logicUpdate(dt);
		if(mUpdateWidgets.size()>i && widget!=mUpdateWidgets[i]){
			i--;
		}
	}
}

void Window::visualUpdate(int dt){
	int i;
	for(i=0;i<mUpdateWidgets.size();++i){
		Widget::ptr widget=mUpdateWidgets[i];
		widget->visualUpdate(dt);
		if(mUpdateWidgets.size()>i && widget!=mUpdateWidgets[i]){
			i--;
		}
	}
}

void Window::render(Renderer *renderer){
	if(mWidth==0 || mHeight==0){
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_WARNING,
			"rendering a zero sized window");
	}

	render(renderer,this);
}

void Window::registerUpdateWidget(Widget *widget){
	mUpdateWidgets.add(widget);
}

void Window::unregisterUpdateWidget(Widget *widget){
	mUpdateWidgets.remove(widget);
}

void Window::render(Renderer *renderer,Widget *widget){
	if(widget->mParent==NULL){
		widget->mWorldPositionX=widget->mPositionX;
		widget->mWorldPositionY=widget->mPositionY;
	}
	else{
		widget->mWorldPositionX=widget->mParent->mWorldPositionX+widget->mPositionX;
		widget->mWorldPositionY=widget->mParent->mWorldPositionY+widget->mPositionY;
	}

	if((widget->mType&Type_PARENT)>0){
		ParentWidget *parent=(ParentWidget*)widget;

		int numChildren=parent->getNumChildren();
		int i;
		for(i=0;i<numChildren;++i){
			render(renderer,parent->getChild(i));
		}
	}
	else if((widget->mType&Type_RENDERABLE)>0){
		RenderableWidget *renderable=(RenderableWidget*)widget;

		// Set up default widget states
		renderer->setBlend(Blend::Combination_ALPHA);
		renderer->setDepthTest(Renderer::DepthTest_NONE);
		renderer->setFaceCulling(Renderer::FaceCulling_NONE);
		renderer->setLighting(true);
		mLightEffect.set(renderable->getColor());
		mLightEffect.trackColor=renderable->getTrackColor();
		renderer->setLightEffect(mLightEffect);
		renderer->setTextureStage(0,NULL);

		Math::setMatrix4x4FromTranslate(widget->mWorldTransform,Math::fromInt(widget->mWorldPositionX),Math::fromInt(widget->mWorldPositionY),0);
		if(renderable->applyScale()){
			Math::setMatrix4x4FromScale(widget->mWorldTransform,Math::fromInt(widget->mWidth),Math::fromInt(widget->mHeight),Math::ONE);
		}
		renderer->setModelMatrix(widget->mWorldTransform);

		renderable->render(renderer);
	}
}

}
}
}

