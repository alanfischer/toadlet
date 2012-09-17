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

#include <toadlet/tadpole/ActionComponent.h>
#include <toadlet/tadpole/Node.h>

namespace toadlet{
namespace tadpole{

ActionComponent::ActionComponent(const String &name,Action *action):BaseComponent(name)
	//mAction
{
	if(action!=NULL){
		attach(action);
	}
}

ActionComponent::~ActionComponent(){
}

void ActionComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->actionRemoved(this);
	}

	BaseComponent::parentChanged(node);
	
	if(mParent!=NULL){
		mParent->actionAttached(this);
	}
}

void ActionComponent::attach(Action *action){
	mActions.add(action);
	action->addActionListener(this);
}

void ActionComponent::remove(Action *action){
	mActions.remove(action);
	action->removeActionListener(this);
}

void ActionComponent::start(){
	if(mParent!=NULL){
		mParent->activate();
	}

	int i;
	for(i=0;i<mActions.size();++i){
		mActions[i]->start();
	}
}

void ActionComponent::stop(){
	int i;
	for(i=0;i<mActions.size();++i){
		mActions[i]->stop();
	}
}

void ActionComponent::frameUpdate(int dt,int scope){
	int i;
	for(i=0;i<mActions.size();++i){
		mActions[i]->update(dt);
	}
}

bool ActionComponent::getActive() const{
	int i;
	for(i=0;i<mActions.size();++i){
		if(mActions[i]->getActive()){
			return true;
		}
	}

	return false;
}

void ActionComponent::actionStarted(Action *component){
}

void ActionComponent::actionStopped(Action *component){
	int i;
	for(i=0;i<mActions.size();++i){
		if(mActions[i]!=component && mActions[i]->getActive()){
			mActions[i]->stop();
		}
	}
}

}
}
