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

	BlendState():
		source(Operation_ONE),
		dest(Operation_ZERO)
	{}

	BlendState(const Combination &state){
		set(state);
	}

	BlendState(const Operation &src,const Operation &dst){
		set(src,dst);
	}

	BlendState &set(const BlendState &state){
		source=state.source;
		dest=state.dest;

		return *this;
	}

	BlendState &set(const Combination &blend){
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

		return *this;
	}

	BlendState &set(Operation src,Operation dst){
		source=src;
		dest=dst;

		return *this;
	}

	inline bool equals(const BlendState &state) const{
		return (source==state.source && dest==state.dest);
	}

	inline bool operator==(const BlendState &state) const{
		return (source==state.source && dest==state.dest);
	}

	inline bool operator!=(const BlendState &state) const{
		return !(source==state.source && dest==state.dest);
	}

	Operation source;
	Operation dest;
};

}
}

#endif

