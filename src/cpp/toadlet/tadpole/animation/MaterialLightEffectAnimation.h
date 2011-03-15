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

#ifndef TOADLET_TADPOLE_ANIMATION_MATERIALLIGHTEFFECTANIMATION_H
#define TOADLET_TADPOLE_ANIMATION_MATERIALLIGHTEFFECTANIMATION_H

#include <toadlet/peeper/LightEffect.h>
#include <toadlet/tadpole/animation/Animatable.h>
#include <toadlet/tadpole/Material.h>

namespace toadlet{
namespace tadpole{
namespace animation{

class MaterialLightEffectAnimation:public Animatable{
public:
	TOADLET_SHARED_POINTERS(MaterialLightEffectAnimation);

	MaterialLightEffectAnimation(Material::ptr target,const peeper::LightEffect &start,const peeper::LightEffect &end,scalar time){
		mTarget=target;
		mStart.set(start);
		mEnd.set(end);
		mEndTime=time;
	}

	virtual ~MaterialLightEffectAnimation(){}

	void set(scalar value){
		scalar t=Math::div(value,mEndTime);

		peeper::LightEffect &le=cache_set_le;
		Math::lerp(le.ambient,mStart.ambient,mEnd.ambient,t);
		Math::lerp(le.diffuse,mStart.diffuse,mEnd.diffuse,t);
		Math::lerp(le.specular,mStart.specular,mEnd.specular,t);
		Math::lerp(le.emissive,mStart.emissive,mEnd.emissive,t);
		mTarget->setLightEffect(le);
	}

	scalar getMin() const{return 0;}
	scalar getMax() const{return mEndTime;}

protected:
	Material::ptr mTarget;
	peeper::LightEffect mStart;
	peeper::LightEffect mEnd;
	scalar mEndTime;

	peeper::LightEffect cache_set_le;
};

}
}
}

#endif
