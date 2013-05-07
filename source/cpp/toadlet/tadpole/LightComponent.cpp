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
#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/LightComponent.h>

namespace toadlet{
namespace tadpole{

LightComponent::LightComponent():
	mEnabled(true)
	//mLightState
{
	mBound=new Bound();
	mWorldBound=new Bound();

	setLightState(mLightState);
}

void LightComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->spacialRemoved(this);
		mParent->lightRemoved(this);
	}

	BaseComponent::parentChanged(node);
	
	if(mParent!=NULL){
		mParent->spacialAttached(this);
		mParent->lightAttached(this);
	}
}

void LightComponent::setLightState(const LightState &state){
	mDirection.set(state.direction);
	mLightState.set(state);

	if(state.type==LightState::Type_DIRECTION){
		mBound->setInfinite();
	}
	else{
		mBound->set(state.radius);
	}
}

void LightComponent::setDirection(const Vector3 &direction){
	mDirection.set(direction);
	mLightState.direction.set(direction);
}

void LightComponent::transformChanged(Transform *transform){
	if(mParent==NULL){
		return;
	}

	mLightState.position.set(mParent->getWorldTranslate());
	Math::mul(mLightState.direction,mParent->getWorldRotate(),mDirection);

	mWorldBound->transform(mBound,mParent->getWorldTransform());
}

}
}
