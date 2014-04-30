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

#ifndef TOADLET_PEEPER_RASTERIZERSTATE_H
#define TOADLET_PEEPER_RASTERIZERSTATE_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TOADLET_API TOADLET_ALIGNED RasterizerState{
public:
	TOADLET_ALIGNED_NEW;

	enum CullType{
		CullType_NONE,
		CullType_FRONT,
		CullType_BACK,
	};

	enum FillType{
		FillType_POINT,
		FillType_LINE,
		FillType_SOLID,
	};

	RasterizerState():
		cull(CullType_BACK),
		fill(FillType_SOLID),
		depthBiasConstant(0),
		depthBiasSlope(0),
		multisample(false),
		dither(false),
		lineSize(Math::ONE)
	{}

	RasterizerState(CullType cull1,FillType fill1=FillType_SOLID,scalar depthBiasConstant1=0,scalar depthBiasSlope1=0,bool multisample1=false,bool dither1=false,scalar lineSize1=Math::ONE):
		cull(cull1),
		fill(fill1),
		depthBiasConstant(depthBiasConstant1),
		depthBiasSlope(depthBiasSlope1),
		multisample(multisample1),
		dither(dither1),
		lineSize(lineSize1)
	{}

	RasterizerState &set(const RasterizerState &state){
		cull=state.cull;
		fill=state.fill;
		depthBiasConstant=state.depthBiasConstant;
		depthBiasSlope=state.depthBiasSlope;
		multisample=state.multisample;
		dither=state.dither;
		lineSize=state.lineSize;

		return *this;
	}

	RasterizerState &set(CullType cull1,FillType fill1=FillType_SOLID,scalar depthBiasConstant1=0,scalar depthBiasSlope1=0,bool multisample1=false,bool dither1=false,scalar lineSize1=Math::ONE){
		cull=cull1;
		fill=fill1;
		depthBiasConstant=depthBiasConstant1;
		depthBiasSlope=depthBiasSlope1;
		multisample=multisample1;
		dither=dither1;
		lineSize=lineSize1;

		return *this;
	}

	inline bool equals(const RasterizerState &state) const{
		return (cull==state.cull && fill==state.fill && 
			depthBiasConstant==state.depthBiasConstant && depthBiasSlope==state.depthBiasSlope &&
			multisample==state.multisample && dither==state.dither && lineSize==state.lineSize);
	}

	inline bool operator==(const RasterizerState &state) const{
		return equals(state);
	}

	inline bool operator!=(const RasterizerState &state) const{
		return !equals(state);
	}

	CullType cull;
	FillType fill;
	scalar depthBiasConstant;
	scalar depthBiasSlope;
	bool multisample;
	bool dither;
	scalar lineSize;
};

}
}

#endif
