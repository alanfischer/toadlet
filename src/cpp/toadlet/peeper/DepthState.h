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

#ifndef TOADLET_PEEPER_DEPTHSTATE_H
#define TOADLET_PEEPER_DEPTHSTATE_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TOADLET_API DepthState{
public:
	enum DepthTest{
		DepthTest_NONE,
		DepthTest_NEVER,
		DepthTest_LESS,
		DepthTest_EQUAL,
		DepthTest_LEQUAL,
		DepthTest_GREATER,
		DepthTest_NOTEQUAL,
		DepthTest_GEQUAL,
		DepthTest_ALWAYS,
	};

	DepthState():
		test(DepthTest_LEQUAL),
		write(true)
	{}

	DepthState(DepthTest test1,bool write1):
		test(test1),
		write(write1)
	{}

	DepthState &set(const DepthState &state){
		test=state.test;
		write=state.write;

		return *this;
	}

	DepthState &set(DepthTest test1,bool write1){
		test=test1;
		write=write1;

		return *this;
	}

	inline bool equals(const DepthState &state) const{
		return (test==state.test && write==state.write);
	}

	inline bool operator==(const DepthState &state) const{
		return (test==state.test && write==state.write);
	}

	inline bool operator!=(const DepthState &state) const{
		return !(test==state.test && write==state.write);
	}

	DepthTest test;
	bool write;
};

}
}

#endif
