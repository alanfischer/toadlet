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
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/node/AnimationControllerNode.h>
#include <toadlet/tadpole/node/ParentNode.h>

using namespace toadlet::egg;
using namespace toadlet::tadpole::animation;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(AnimationControllerNode,Categories::TOADLET_TADPOLE_NODE+".AnimationControllerNode");

AnimationControllerNode::AnimationControllerNode():super(),
	//mAnimationController,
	mStartingFrame(0),
	mDestroyOnFinish(false)
{}

Node *AnimationControllerNode::create(Scene *scene){
	super::create(scene);

	mStartingFrame=0;
	mDestroyOnFinish=false;
	mDeactivateCount=-1; /// @todo: HACK

	mAnimationController=AnimationController::ptr(new AnimationController());
	mAnimationController->setAnimationControllerFinishedListener(this,false);

	return this;
}

void AnimationControllerNode::destroy(){
	if(mAnimationController!=NULL){
		mAnimationController->setAnimationControllerFinishedListener(NULL,false);
		mAnimationController=NULL;
	}

	super::destroy();
}

void AnimationControllerNode::setDestroyOnFinish(bool destroy){
	mDestroyOnFinish=destroy;
}

void AnimationControllerNode::start(){
	if(mAnimationController->isRunning()){
		stop();
	}

	mAnimationController->start();
	mStartingFrame=mScene->getLogicFrame();
}

void AnimationControllerNode::stop(){
	if(mAnimationController->isRunning()==false){
		return;
	}

	mAnimationController->stop();
}

void AnimationControllerNode::frameUpdate(int dt,int scope){
	super::frameUpdate(dt,scope);
	
	if(mStartingFrame!=mScene->getLogicFrame()){
		mAnimationController->update(dt);
	}
}

void AnimationControllerNode::controllerFinished(AnimationController *controller){
	if(mDestroyOnFinish){
		destroy();
	}
}

}
}
}
