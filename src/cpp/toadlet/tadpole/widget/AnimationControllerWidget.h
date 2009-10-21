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

#ifndef TOADLET_TADPOLE_WIDGET_ANIMATIONCONTROLLERWIDGET_H
#define TOADLET_TADPOLE_WIDGET_ANIMATIONCONTROLLERWIDGET_H

#include <toadlet/tadpole/widget/Widget.h>
#include <toadlet/tadpole/animation/AnimationController.h>

namespace toadlet{
namespace tadpole{
namespace widget{

class TOADLET_API AnimationControllerWidget:public Widget,public animation::AnimationControllerFinishedListener{
public:
	TOADLET_WIDGET(AnimationControllerWidget,Widget);

	AnimationControllerWidget(Engine *engine);
	virtual ~AnimationControllerWidget();

	inline void setTime(int time,bool setagain=true){mAnimationController->setTime(time,setagain);}
	inline int getTime() const{return mAnimationController->getTime();}

	inline void setInterpolation(animation::AnimationController::Interpolation interpolation){mAnimationController->setInterpolation(interpolation);}
	inline animation::AnimationController::Interpolation getInterpolation() const{return mAnimationController->getInterpolation();}

	inline void setTimeScale(scalar scale){mAnimationController->setTimeScale(scale);}
	inline scalar getTimeScale() const{return mAnimationController->getTimeScale();}

	inline void attachAnimation(animation::Animation::ptr animation){mAnimationController->attachAnimation(animation);}
	inline void removeAnimation(animation::Animation::ptr animation){mAnimationController->removeAnimation(animation);}

	void setDestroyOnFinish(bool destroy);
	inline bool getDestroyOnFinish() const{return mDestroyOnFinish;}

	void start();
	void stop();
	inline bool isRunning() const{return mAnimationController->isRunning();}

	void logicUpdate(int dt);
	void visualUpdate(int dt);

	void controllerFinished(animation::AnimationController *controller);

	void parentChanged(Widget *widget,ParentWidget *oldParent,ParentWidget *newParent);

protected:
	void reroot(Window *window);

	animation::AnimationController::ptr mAnimationController;
	bool mDestroyOnFinish;
	Window *mRoot;
};

}
}
}

#endif

