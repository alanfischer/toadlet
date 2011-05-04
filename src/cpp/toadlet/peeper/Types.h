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

#ifndef TOADLET_PEEPER_TYPES_H
#define TOADLET_PEEPER_TYPES_H

#include <toadlet/Types.h>
#include <toadlet/egg/mathfixed/Math.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/SharedPointer.h>
#include <toadlet/egg/WeakPointer.h>

namespace toadlet{
namespace peeper{

#if defined(TOADLET_FIXED_POINT)
	using namespace toadlet::egg::mathfixed;
#else
	using namespace toadlet::egg::math;
#endif

enum ClearFlag{
	ClearFlag_COLOR=1,
	ClearFlag_DEPTH=2,
	ClearFlag_STENCIL=4,
};

enum AlphaTest{
	AlphaTest_NONE,
	AlphaTest_LESS,
	AlphaTest_EQUAL,
	AlphaTest_LEQUAL,
	AlphaTest_GREATER,
	AlphaTest_NOTEQUAL,
	AlphaTest_GEQUAL,
	AlphaTest_ALWAYS,
};

enum Normalize{
	Normalize_NONE,
	Normalize_RESCALE,
	Normalize_NORMALIZE,
};

}
}

#endif
