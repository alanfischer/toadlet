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

#ifndef TOADLET_PEEPER_POINTSTATE_H
#define TOADLET_PEEPER_POINTSTATE_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class PointState{
public:
	PointState():
		sprite(false),
		size(Math::ONE),
		attenuated(false),
		constant(0),
		linear(0),
		quadratic(0),
		minSize(Math::ONE),
		maxSize(Math::ONE)
	{}

	PointState(bool sprite1,scalar size1,bool attenuated1,scalar constant1,scalar linear1,scalar quadratic1,scalar minSize1,scalar maxSize1){
		set(sprite1,size1,attenuated1,constant1,linear1,quadratic1,minSize1,maxSize1);
	}

	PointState &set(const PointState &state){
		sprite=state.sprite;
		size=state.size;
		attenuated=state.attenuated;
		constant=state.constant;
		linear=state.linear;
		quadratic=state.quadratic;
		minSize=state.minSize;
		maxSize=state.maxSize;
		return *this;
	}

	PointState &set(bool sprite1,scalar size1,bool attenuated1,scalar constant1,scalar linear1,scalar quadratic1,scalar minSize1,scalar maxSize1){
		sprite=sprite1;
		size=size1;
		attenuated=attenuated1;
		constant=constant1;
		linear=linear1;
		quadratic=quadratic1;
		minSize=minSize1;
		maxSize=maxSize1;
		return *this;
	}

	bool sprite;
	scalar size;
	bool attenuated;
	scalar constant;
	scalar linear;
	scalar quadratic;
	scalar minSize;
	scalar maxSize;
};

}
}

#endif
