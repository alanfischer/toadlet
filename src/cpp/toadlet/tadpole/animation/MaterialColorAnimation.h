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

#ifndef TOADLET_TADPOLE_ANIMATION_MATERIALCOLORANIMATION_H
#define TOADLET_TADPOLE_ANIMATION_MATERIALCOLORANIMATION_H

#include <toadlet/peeper/LightEffect.h>
#include <toadlet/tadpole/animation/Animation.h>
#include <toadlet/tadpole/Material.h>

namespace toadlet{
namespace tadpole{
namespace animation{

class MaterialColorAnimation:public Animation{
public:
	TOADLET_SHARED_POINTERS(MaterialColorAnimation);

	MaterialColorAnimation(Material::ptr target){
		mTarget=target;
		mStart.set(target->getLightEffect().diffuse);
	}

	MaterialColorAnimation(Material::ptr target,const peeper::Color &end,scalar time){
		mTarget=target;
		mStart.set(target->getLightEffect().diffuse);
		mEnd.set(end);
		mEndTime=time;
	}

	virtual ~MaterialColorAnimation(){}

	void setTarget(Material::ptr target){mTarget=target;}
	inline Material::ptr getTarget() const{return mTarget;}

	void setStart(const peeper::Color &start){
		mStart.set(start);
	}

	void setEnd(const peeper::Color &end,scalar time){
		mEnd.set(end);
		mEndTime=time;
	}

	void set(scalar value){
		scalar t=Math::div(value,mEndTime);

		peeper::LightEffect &le=cache_set_le;
		peeper::Color::lerp(le.diffuse,mStart,mEnd,t);
		le.ambient.set(le.diffuse);
		le.specular.set(le.diffuse);
		mTarget->setLightEffect(le);
	}

	scalar getMin() const{return 0;}
	scalar getMax() const{return mEndTime;}

protected:
	Material::ptr mTarget;
	peeper::Color mStart;
	peeper::Color mEnd;
	scalar mEndTime;

	peeper::LightEffect cache_set_le;
};

}
}
}

#endif
