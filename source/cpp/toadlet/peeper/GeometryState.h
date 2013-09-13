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

#ifndef TOADLET_PEEPER_GEOMETRYSTATE_H
#define TOADLET_PEEPER_GEOMETRYSTATE_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class GeometryState{
public:
	enum MatrixFlags{
		MatrixFlag_NO_PERSPECTIVE=1<<0,
		MatrixFlag_ASPECT_CORRECT=1<<1,
		MatrixFlag_CAMERA_ALIGNED=1<<2,
	};

	GeometryState():
		sprite(false),
		size(Math::ONE),
		attenuated(false),
		constant(0),
		linear(0),
		quadratic(0),
		minSize(Math::ONE),
		maxSize(Math::ONE),
		matrixFlags(0)
	{}

	GeometryState(bool sprite1,scalar size1,bool attenuated1,scalar constant1=Math::ONE,scalar linear1=Math::ONE,scalar quadratic1=Math::ONE,scalar minSize1=Math::ONE,scalar maxSize1=Math::ONE*128):
		matrixFlags(0)
	{
		set(sprite1,size1,attenuated1,constant1,linear1,quadratic1,minSize1,maxSize1);
	}

	GeometryState(int matrixFlags,const Vector3 &matrixAlignAxis=Math::ZERO_VECTOR3):
		sprite(false),
		size(Math::ONE),
		attenuated(false),
		constant(0),
		linear(0),
		quadratic(0),
		minSize(Math::ONE),
		maxSize(Math::ONE)
	{
		set(matrixFlags,matrixAlignAxis);
	}

	GeometryState &set(const GeometryState &state){
		sprite=state.sprite;
		size=state.size;
		attenuated=state.attenuated;
		constant=state.constant;
		linear=state.linear;
		quadratic=state.quadratic;
		minSize=state.minSize;
		maxSize=state.maxSize;
		matrixFlags=state.matrixFlags;
		matrixAlignAxis=state.matrixAlignAxis;
		return *this;
	}

	GeometryState &set(bool sprite1,scalar size1,bool attenuated1,scalar constant1=Math::ONE,scalar linear1=Math::ONE,scalar quadratic1=Math::ONE,scalar minSize1=Math::ONE,scalar maxSize1=Math::ONE){
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

	GeometryState &set(int matrixFlags1,const Vector3 &matrixAlignAxis1=Math::ZERO_VECTOR3){
		matrixFlags=matrixFlags1;
		matrixAlignAxis.set(matrixAlignAxis1);
		return *this;
	}

	inline bool equals(const GeometryState &state) const{
		return (sprite==state.sprite && size==state.size && attenuated==state.attenuated &&
			constant==state.constant && linear==state.linear && quadratic==state.quadratic &&
			minSize==state.minSize && maxSize==state.maxSize &&
			matrixFlags==state.matrixFlags && matrixAlignAxis==state.matrixAlignAxis);
	}

	inline bool operator==(const GeometryState &state) const{
		return equals(state);
	}

	inline bool operator!=(const GeometryState &state) const{
		return !equals(state);
	}

	bool sprite;
	scalar size;
	bool attenuated;
	scalar constant;
	scalar linear;
	scalar quadratic;
	scalar minSize;
	scalar maxSize;
	int matrixFlags;
	Vector3 matrixAlignAxis;
};

}
}

#endif
