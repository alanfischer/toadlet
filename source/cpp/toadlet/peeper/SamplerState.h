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

#ifndef TOADLET_PEEPER_SAMPLERSTATE_H
#define TOADLET_PEEPER_SAMPLERSTATE_H

#include <toadlet/egg/Extents.h>
#include <toadlet/peeper/Texture.h>

namespace toadlet{
namespace peeper{

class TOADLET_API TOADLET_ALIGNED SamplerState{
public:
	TOADLET_ALIGNED_NEW;

	enum FilterType{
		FilterType_NONE,
		FilterType_NEAREST,
		FilterType_LINEAR,
	};

	enum AddressType{
		AddressType_REPEAT,
		AddressType_CLAMP_TO_EDGE,
		AddressType_CLAMP_TO_BORDER,
		AddressType_MIRRORED_REPEAT,
	};

	SamplerState(FilterType minFilter1=FilterType_LINEAR,FilterType mipFilter1=FilterType_LINEAR,FilterType magFilter1=FilterType_LINEAR,AddressType uAddress1=AddressType_REPEAT,AddressType vAddress1=AddressType_REPEAT,AddressType wAddress1=AddressType_REPEAT):
		minFilter(minFilter1),
		mipFilter(mipFilter1),
		magFilter(magFilter1),

		uAddress(uAddress1),
		vAddress(vAddress1),
		wAddress(wAddress1),

		//borderColor,

		minLOD(0),
		maxLOD(1000),

		perspective(true)
	{}

	SamplerState &set(const SamplerState &state){
		minFilter=state.minFilter;
		mipFilter=state.mipFilter;
		magFilter=state.magFilter;

		uAddress=state.uAddress;
		vAddress=state.vAddress;
		wAddress=state.wAddress;

		borderColor.set(state.borderColor);

		minLOD=state.minLOD;
		maxLOD=state.maxLOD;

		perspective=state.perspective;

		return *this;
	}

	inline bool equals(const SamplerState &state) const{
		return (minFilter==state.minFilter && mipFilter==state.mipFilter && magFilter==state.magFilter &&
			uAddress==state.uAddress && vAddress==state.vAddress && wAddress==state.wAddress &&
			borderColor.equals(state.borderColor) &&
			minLOD==state.minLOD && maxLOD==state.maxLOD &&
			perspective==state.perspective);
	}

	inline bool operator==(const SamplerState &state) const{
		return equals(state);
	}

	inline bool operator!=(const SamplerState &state) const{
		return !equals(state);
	}

	FilterType minFilter;
	FilterType mipFilter;
	FilterType magFilter;

	AddressType uAddress;
	AddressType vAddress;
	AddressType wAddress;

	Vector4 borderColor;

	int minLOD;
	int maxLOD;

	// Only supported on GLES1 & D3DM, and the first SamplerState affects all
	bool perspective;
};

}
}

#endif
