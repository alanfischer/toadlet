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

#ifndef TOADLET_TADPOLE_ANIMATION_ENTITYSCALEANIMATION_H
#define TOADLET_TADPOLE_ANIMATION_ENTITYSCALEANIMATION_H

#include <toadlet/tadpole/animation/Animation.h>
#include <toadlet/tadpole/entity/Entity.h>

namespace toadlet{
namespace tadpole{
namespace animation{

class EntityScaleAnimation:public Animation{
public:
	TOADLET_SHARED_POINTERS(EntityScaleAnimation,Animation);

	EntityScaleAnimation(entity::Entity::ptr target){
		mTarget=target;
		mStart.set(target->getScale());
	}

	EntityScaleAnimation(entity::Entity::ptr target,scalar endScale,scalar time){
		mTarget=target;
		mStart.set(target->getScale());
		mEnd.set(endScale,endScale,endScale);
		mEndTime=time;
	}

	EntityScaleAnimation(entity::Entity::ptr target,const Vector3 &end,scalar time){
		mTarget=target;
		mStart.set(target->getScale());
		mEnd.set(end);
		mEndTime=time;
	}

	virtual ~EntityScaleAnimation(){}

	void setTarget(entity::Entity::ptr target){mTarget=target;}
	inline entity::Entity::ptr getTarget() const{return mTarget;}

	void setStart(const Vector3 &start){
		mStart.set(start);
	}

	void setEnd(const Vector3 &end,scalar time){
		mEnd.set(end);
		mEndTime=time;
	}

	void set(scalar value){
		scalar t=Math::div(value,mEndTime);

		Vector3 &scale=cache_set_scale;
		Math::lerp(scale,mStart,mEnd,t);
		mTarget->setScale(scale);
	}

	scalar getMin() const{return 0;}
	scalar getMax() const{return mEndTime;}

	void attached(AnimationController *){}
	void removed(AnimationController *){}

protected:
	entity::Entity::ptr mTarget;
	Vector3 mStart;
	Vector3 mEnd;
	scalar mEndTime;

	Vector3 cache_set_scale;
};

}
}
}

#endif
