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

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/widget/AnimationControllerWidget.h>
#include <toadlet/tadpole/widget/ParentWidget.h>
#include <toadlet/tadpole/widget/Window.h>

using namespace toadlet::egg;
using namespace toadlet::tadpole::animation;

namespace toadlet{
namespace tadpole{
namespace widget{

AnimationControllerWidget::AnimationControllerWidget(Engine *engine):Widget(engine),
	//mAnimationController,
	mDestroyOnFinish(false),
	mRoot(NULL)
{
	mAnimationController=AnimationController::ptr(new AnimationController());
	mAnimationController->setAnimationControllerFinishedListener(this);

	setLayout(Layout_ABSOLUTE);
}

AnimationControllerWidget::~AnimationControllerWidget(){
}

void AnimationControllerWidget::setDestroyOnFinish(bool destroy){
	mDestroyOnFinish=destroy;
}

void AnimationControllerWidget::start(){
	if(mAnimationController->isRunning()){
		stop();
	}

	if(mRoot!=NULL){
		mRoot->registerUpdateWidget(this);
	}

	mAnimationController->start();
}

void AnimationControllerWidget::stop(){
	if(mAnimationController->isRunning()==false){
		return;
	}

	if(mRoot!=NULL){
		mRoot->unregisterUpdateWidget(this);
	}

	mAnimationController->stop();
}

void AnimationControllerWidget::logicUpdate(int dt){
	mAnimationController->logicUpdate(dt);
}

void AnimationControllerWidget::visualUpdate(int dt){
	mAnimationController->visualUpdate(dt);
}

void AnimationControllerWidget::controllerFinished(AnimationController *controller){
	if(mDestroyOnFinish){
		destroy();
	}
}

void AnimationControllerWidget::parentChanged(Widget *widget,ParentWidget *oldParent,ParentWidget *newParent){
	if(oldParent!=NULL && oldParent->instanceOf(Type_WINDOW)){
		reroot(NULL);
	}

	if(newParent!=NULL && newParent->instanceOf(Type_WINDOW)){
		reroot((Window*)newParent);
	}

	Super::parentChanged(widget,oldParent,newParent);
}

void AnimationControllerWidget::reroot(Window *root){
	if(mAnimationController->isRunning()){
		if(root==NULL && mRoot!=NULL){
			mRoot->unregisterUpdateWidget(this);
		}
		else if(root!=NULL){
			root->registerUpdateWidget(this);
		}
	}
	mRoot=root;
}

}
}
}
