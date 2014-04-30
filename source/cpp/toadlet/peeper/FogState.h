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

#ifndef TOADLET_PEEPER_FOGSTATE_H
#define TOADLET_PEEPER_FOGSTATE_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TOADLET_API TOADLET_ALIGNED FogState{
public:
	TOADLET_ALIGNED_NEW;

	enum FogType{
		FogType_NONE,
		FogType_LINEAR,
		FogType_EXP,
		FogType_EXP2,
	};

	FogState():
		fog(FogType_NONE),
		density(0),
		nearDistance(0),
		farDistance(Math::ONE)
		//color
	{}

	FogState(FogType type1,scalar density1,scalar nearDistance1,scalar farDistance1,const Vector4 &color1){
		set(type1,density1,nearDistance1,farDistance1,color1);
	}

	FogState &set(const FogState &state){
		fog=state.fog;
		density=state.density;
		nearDistance=state.nearDistance;
		farDistance=state.farDistance;
		color.set(state.color);
		return *this;
	}

	FogState &set(FogType fog1,scalar density1,scalar nearDistance1,scalar farDistance1,const Vector4 &color1){
		fog=fog1;
		density=density1;
		nearDistance=nearDistance1;
		farDistance=farDistance1;
		color.set(color1);
		return *this;
	}

	inline bool equals(const FogState &state) const{
		return (fog==state.fog && density==state.density && nearDistance==state.nearDistance && farDistance==state.farDistance && color.equals(state.color));
	}

	inline bool operator==(const FogState &state) const{
		return equals(state);
	}

	inline bool operator!=(const FogState &state) const{
		return !equals(state);
	}

	FogType fog;
	scalar density;
	scalar nearDistance;
	scalar farDistance;
	Vector4 color;
};

}
}

#endif
