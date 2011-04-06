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

#ifndef TOADLET_PEEPER_BLENDSTATE_H
#define TOADLET_PEEPER_BLENDSTATE_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TOADLET_API BlendState{
public:
	enum Operation{
		Operation_ONE,
		Operation_ZERO,
		Operation_DEST_COLOR,
		Operation_SOURCE_COLOR,
		Operation_ONE_MINUS_DEST_COLOR,
		Operation_ONE_MINUS_SOURCE_COLOR,
		Operation_DEST_ALPHA,
		Operation_SOURCE_ALPHA,
		Operation_ONE_MINUS_DEST_ALPHA,
		Operation_ONE_MINUS_SOURCE_ALPHA
	};

	enum Combination{
		Combination_DISABLED,
		Combination_COLOR,
		Combination_COLOR_ADDITIVE,
		Combination_ALPHA,
		Combination_ALPHA_ADDITIVE,
	};

	enum ColorWrite{
		ColorWrite_BIT_R=1<<0,
		ColorWrite_BIT_G=1<<1,
		ColorWrite_BIT_B=1<<2,
		ColorWrite_BIT_A=1<<3,
		ColorWrite_ALL=0xF,
	};

	BlendState():
		source(Operation_ONE),
		dest(Operation_ZERO),
		colorWrite(ColorWrite_ALL)
	{}

	BlendState(const Combination &state,int colorWrite=ColorWrite_ALL){
		set(state,colorWrite);
	}

	BlendState(const Operation &source,const Operation &dest,int colorWrite=ColorWrite_ALL){
		set(source,dest,colorWrite);
	}

	BlendState &set(const BlendState &state){
		source=state.source;
		dest=state.dest;
		colorWrite=state.colorWrite;

		return *this;
	}

	BlendState &set(const Combination &blend,int colorWrite=ColorWrite_ALL){
		switch(blend){
			case Combination_COLOR:
				source=Operation_ONE;
				dest=Operation_ONE_MINUS_SOURCE_COLOR;
			break;
			case Combination_COLOR_ADDITIVE:
				source=Operation_ONE;
				dest=Operation_ONE;
			break;
			case Combination_ALPHA:
				source=Operation_SOURCE_ALPHA;
				dest=Operation_ONE_MINUS_SOURCE_ALPHA;
			break;
			case Combination_ALPHA_ADDITIVE:
				source=Operation_SOURCE_ALPHA;
				dest=Operation_ONE;
			break;
			default:
				source=Operation_ONE;
				dest=Operation_ZERO;
			break;
		}
		
		this->colorWrite=colorWrite;

		return *this;
	}

	BlendState &set(Operation source1,Operation dest1,int colorWrite1=ColorWrite_ALL){
		source=source1;
		dest=dest1;
		colorWrite=colorWrite1;

		return *this;
	}

	inline bool equals(const BlendState &state) const{
		return (source==state.source && dest==state.dest && colorWrite==state.colorWrite);
	}

	inline bool operator==(const BlendState &state) const{
		return equals(state);
	}

	inline bool operator!=(const BlendState &state) const{
		return !equals(state);
	}

	Operation source;
	Operation dest;
	int colorWrite;
};

}
}

#endif

