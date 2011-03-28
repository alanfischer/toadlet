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

#ifndef TOADLET_PEEPER_LIGHT_H
#define TOADLET_PEEPER_LIGHT_H

#include <toadlet/egg/Extents.h>
#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TOADLET_API LightState{
public:
	enum Type{
		Type_DIRECTION,
		Type_POINT,
		Type_SPOT,
	};

	LightState():
		type(Type_DIRECTION),
		specularColor(Math::ZERO_VECTOR4),
		diffuseColor(Math::ONE_VECTOR4),
		constantAttenuation(Math::ONE),
		linearAttenuation(0),
		quadraticAttenuation(0),
		spotInnerRadius(0),
		spotOuterRadius(Math::PI),
		spotFalloff(Math::ONE),
		#if defined(TOADLET_FIXED_POINT)
			radius(Math::sqrt(egg::Extents::MAX_FIXED>>1)),
		#else
			radius(Math::sqrt(egg::Extents::MAX_FLOAT)),
		#endif
		//position,
		direction(0,0,-Math::ONE)
	{}

	LightState &set(const LightState &state){
		type=state.type;
		specularColor.set(state.specularColor);
		diffuseColor.set(state.diffuseColor);
		constantAttenuation=state.constantAttenuation;
		linearAttenuation=state.linearAttenuation;
		quadraticAttenuation=state.quadraticAttenuation;
		spotInnerRadius=state.spotInnerRadius;
		spotOuterRadius=state.spotOuterRadius;
		spotFalloff=state.spotFalloff;
		radius=state.radius;
		position.set(state.position);
		direction.set(state.direction);

		return *this;
	}

	Type type;
	Vector4 specularColor;
	Vector4 diffuseColor;
	scalar constantAttenuation;
	scalar linearAttenuation;
	scalar quadraticAttenuation;
	scalar spotInnerRadius;
	scalar spotOuterRadius;
	scalar spotFalloff;
	scalar radius;
	Vector3 position;
	Vector3 direction;
};

}
}

#endif
