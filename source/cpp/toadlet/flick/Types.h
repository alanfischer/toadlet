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

#ifndef TOADLET_FLICK_TYPES_H
#define TOADLET_FLICK_TYPES_H

#include <toadlet/egg/Types.h>
#if defined(TOADLET_FIXED_POINT)
	#include <toadlet/egg/mathfixed/Math.h>
#else
	#include <toadlet/egg/math/Math.h>
#endif
#include <toadlet/egg/SharedPointer.h>
#include <toadlet/egg/IntrusivePointer.h>
#include <toadlet/egg/Object.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/Log.h>
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace flick{

#if defined(TOADLET_FIXED_POINT)
	using namespace toadlet::egg::mathfixed;
#else
	using namespace toadlet::egg::math;
#endif

using namespace toadlet::egg;
using namespace toadlet::peeper;

}
}

#endif
