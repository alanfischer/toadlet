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

#include <toadlet/peeper/MaterialState.h>
#include <toadlet/tadpole/animation/Animatable.h>
#include <toadlet/tadpole/material/Material.h>

namespace toadlet{
namespace tadpole{
namespace animation{

/// @todo: This should just be a MaterialStateAnimation
/*
class MaterialLightEffectAnimation:public Animatable{
public:
	TOADLET_SPTR(MaterialLightEffectAnimation);

	MaterialLightEffectAnimation(Material::ptr target,const MaterialState &start,const MaterialState &end,scalar time){
		mTarget=target;
		mStart.set(start);
		mEnd.set(end);
		mEndTime=time;
	}

	virtual ~MaterialLightEffectAnimation(){}

	void set(scalar value){
		scalar t=Math::div(value,mEndTime);

		MaterialState state;
		Math::lerp(state.ambient,mStart.ambient,mEnd.ambient,t);
		Math::lerp(state.diffuse,mStart.diffuse,mEnd.diffuse,t);
		Math::lerp(state.specular,mStart.specular,mEnd.specular,t);
		Math::lerp(state.emissive,mStart.emissive,mEnd.emissive,t);
		mTarget->setMaterialState(state);
	}

	scalar getMin() const{return 0;}
	scalar getMax() const{return mEndTime;}

protected:
	Material::ptr mTarget;
	MaterialState mStart;
	MaterialState mEnd;
	scalar mEndTime;
};
*/
}
}
}

#endif
