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

class FogState{
public:
	enum FogType{
		FogType_NONE,
		FogType_LINEAR,
	};

	FogState():
		type(FogType_NONE),
		nearDistance(0),
		farDistance(0)
		//color
	{}

	FogState(FogType type1,scalar nearDistance1,scalar farDistance1,const Vector4 &color1){
		set(type1,nearDistance1,farDistance1,color1);
	}

	FogState &set(const FogState &state){
		type=state.type;
		nearDistance=state.nearDistance;
		farDistance=state.farDistance;
		color.set(state.color);
		return *this;
	}

	FogState &set(FogType type1,scalar nearDistance1,scalar farDistance1,const Vector4 &color1){
		type=type1;
		nearDistance=nearDistance1;
		farDistance=farDistance1;
		color.set(color1);
		return *this;
	}

	FogType type;
	scalar nearDistance;
	scalar farDistance;
	Vector4 color;
};

}
}

#endif
