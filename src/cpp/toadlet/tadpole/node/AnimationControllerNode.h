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

#ifndef TOADLET_TADPOLE_NODE_ANIMATIONCONTROLLERNODE_H
#define TOADLET_TADPOLE_NODE_ANIMATIONCONTROLLERNODE_H

#include <toadlet/tadpole/node/Node.h>
#include <toadlet/tadpole/animation/AnimationController.h>

namespace toadlet{
namespace tadpole{
namespace node{

class Scene;

class TOADLET_API AnimationControllerNode:public Node,public animation::AnimationControllerFinishedListener{
public:
	TOADLET_NODE(AnimationControllerNode,Node);

	AnimationControllerNode();
	virtual Node *create(Engine *engine);
	virtual void destroy();

	inline void setTime(int time,bool setagain=true){mAnimationController->setTime(time,setagain);}
	inline int getTime() const{return mAnimationController->getTime();}

	inline void setCycling(animation::AnimationController::Cycling cycling){mAnimationController->setCycling(cycling);}
	inline animation::AnimationController::Cycling getCycling() const{return mAnimationController->getCycling();}

	inline void setInterpolation(animation::AnimationController::Interpolation interpolation){mAnimationController->setInterpolation(interpolation);}
	inline animation::AnimationController::Interpolation getInterpolation() const{return mAnimationController->getInterpolation();}

	inline void setTimeScale(scalar scale){mAnimationController->setTimeScale(scale);}
	inline scalar getTimeScale() const{return mAnimationController->getTimeScale();}

	inline void setMinMaxValue(scalar minValue,scalar maxValue){mAnimationController->setMinMaxValue(minValue,maxValue);}
	inline scalar getMinValue() const{return mAnimationController->getMinValue();}
	inline scalar getMaxValue() const{return mAnimationController->getMaxValue();}

	inline void attach(animation::Animation::ptr animation){mAnimationController->attach(animation);}
	inline void remove(animation::Animation::ptr animation){mAnimationController->remove(animation);}

	void setDestroyOnFinish(bool destroy);
	inline bool getDestroyOnFinish() const{return mDestroyOnFinish;}

	void start();
	void stop();
	inline bool isRunning() const{return mAnimationController->isRunning();}

	void logicUpdate(int dt);
	void renderUpdate(int dt);

	void controllerFinished(animation::AnimationController *controller);

protected:
	animation::AnimationController::ptr mAnimationController;
	int mStartingFrame;
	bool mDestroyOnFinish;
};

}
}
}

#endif

