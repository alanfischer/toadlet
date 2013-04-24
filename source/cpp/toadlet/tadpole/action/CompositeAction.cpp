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

#include <toadlet/tadpole/action/CompositeAction.h>

namespace toadlet{
namespace tadpole{
namespace action{

CompositeAction::CompositeAction():
	//mListeners,
	mStopStyle(StopStyle_ON_FIRST),
	mActive(false)
{
}

CompositeAction::~CompositeAction(){
	int i;
	for(i=0;i<mActions.size();++i){
		mActions[i]->removeActionListener(this);
	}
	mActions.clear();
}

void CompositeAction::attach(Action *action){
	mActions.add(action);

	action->addActionListener(this);
}

void CompositeAction::remove(Action *action){
	action->removeActionListener(this);

	mActions.remove(action);
}

void CompositeAction::start(){
	mActive=true;

	int i;
	for(i=0;i<mActions.size();++i){
		mActions[i]->start();
	}

	for(i=0;i<mListeners.size();++i){
		mListeners[i]->actionStarted(this);
	}
}

void CompositeAction::stop(){
	mActive=false;

	int i;
	for(i=0;i<mActions.size();++i){
		mActions[i]->stop();
	}

	for(i=0;i<mListeners.size();++i){
		mListeners[i]->actionStopped(this);
	}
}

void CompositeAction::update(int dt){
	int i;
	for(i=0;i<mActions.size();++i){
		mActions[i]->update(dt);
	}
}

bool CompositeAction::getActive() const{
	bool active=false;
	int i;
	for(i=0;i<mActions.size();++i){
		active|=mActions[i]->getActive();
	}
	return active;
}

void CompositeAction::actionStarted(Action *action){
}

void CompositeAction::actionStopped(Action *action){
	if(mActive){
		if(mStopStyle==StopStyle_ON_FIRST || (mStopStyle==StopStyle_ON_LAST && getActive()==false)){
			stop();

			if(mNextAction!=NULL){
				mNextAction->start();
			}
		}
	}
}

}
}
}
