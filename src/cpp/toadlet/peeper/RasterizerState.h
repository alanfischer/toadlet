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

class TOADLET_API RasterizerState{
public:
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
		dither(false)
	{}

	RasterizerState(CullType cull1,FillType fill1=FillType_SOLID,scalar depthBiasConstant1=0,scalar depthBiasSlope1=0,bool dither1=false):
		cull(cull1),
		fill(fill1),
		depthBiasConstant(depthBiasConstant1),
		depthBiasSlope(depthBiasSlope1),
		dither(dither1)
	{}

	RasterizerState &set(const RasterizerState &state){
		cull=state.cull;
		fill=state.fill;
		depthBiasConstant=state.depthBiasConstant;
		depthBiasSlope=state.depthBiasSlope;
		dither=state.dither;

		return *this;
	}

	RasterizerState &set(CullType cull,FillType fill=FillType_SOLID,scalar depthBiasConstant=0,scalar depthBiasSlope=0,bool dither=false){
		this->cull=cull;
		this->fill=fill;
		this->depthBiasConstant=depthBiasConstant;
		this->depthBiasSlope=depthBiasSlope;
		this->dither=dither;

		return *this;
	}

	CullType cull;
	FillType fill;
	scalar depthBiasConstant;
	scalar depthBiasSlope;
	bool dither;
};

}
}

#endif
