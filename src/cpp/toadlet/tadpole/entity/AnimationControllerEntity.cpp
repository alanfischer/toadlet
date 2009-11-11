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

#include <toadlet/tadpole/entity/AnimationControllerEntity.h>
#include <toadlet/tadpole/entity/ParentEntity.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::tadpole::animation;

namespace toadlet{
namespace tadpole{
namespace entity{

TOADLET_ENTITY_IMPLEMENT(AnimationControllerEntity,"toadlet::tadpole::entity::AnimationControllerEntity");

AnimationControllerEntity::AnimationControllerEntity():Entity(),
	//mAnimationController,
	mStartingFrame(0),
	mDestroyOnFinish(false)
{}

Entity *AnimationControllerEntity::create(Engine *engine){
	super::create(engine);

	mStartingFrame=0;
	mDestroyOnFinish=false;

	mAnimationController=AnimationController::ptr(new AnimationController());
	mAnimationController->setAnimationControllerFinishedListener(this);

	return this;
}

void AnimationControllerEntity::destroy(){
	if(mAnimationController!=NULL){
		mAnimationController->setAnimationControllerFinishedListener(NULL);
		mAnimationController=NULL;
	}

	super::destroy();
}

void AnimationControllerEntity::setDestroyOnFinish(bool destroy){
	mDestroyOnFinish=destroy;
}

void AnimationControllerEntity::start(){
	if(mAnimationController->isRunning()){
		stop();
	}

	setReceiveUpdates(true);

	mAnimationController->start();
	if(mEngine->getScene()!=NULL){
		mStartingFrame=mEngine->getScene()->getLogicFrame();
	}
}

void AnimationControllerEntity::stop(){
	if(mAnimationController->isRunning()==false){
		return;
	}

	setReceiveUpdates(false);

	mAnimationController->stop();
}

void AnimationControllerEntity::logicUpdate(int dt){
	if(mEngine->getScene()==NULL || mStartingFrame!=mEngine->getScene()->getLogicFrame()){
		mAnimationController->logicUpdate(dt);
	}
}

void AnimationControllerEntity::visualUpdate(int dt){
	mAnimationController->visualUpdate(dt);
}

void AnimationControllerEntity::controllerFinished(AnimationController *controller){
	if(mDestroyOnFinish){
		destroy();
	}
}

}
}
}
