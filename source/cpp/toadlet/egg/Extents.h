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

#ifndef TOADLET_EGG_EXTENTS_H
#define TOADLET_EGG_EXTENTS_H

#include <toadlet/egg/math/BaseMath.h>
#include <toadlet/egg/mathfixed/BaseMath.h>
#include <limits.h>
#include <float.h>

namespace toadlet{
namespace egg{

namespace Extents{
	using namespace std; // Bring isinf/isnan into Extents if its there, otherwise if its global no harm done

	const char MAX_CHAR=SCHAR_MAX;
	const char MIN_CHAR=SCHAR_MIN;
	const unsigned char MAX_UNSIGNED_CHAR=UCHAR_MAX;
	const short MIN_SHORT=SHRT_MIN;
	const short MAX_SHORT=SHRT_MAX;
	const unsigned short MAX_UNSIGNED_SHORT=USHRT_MAX;
	const int MAX_INT=INT_MAX;
	const int MIN_INT=INT_MIN;
	const unsigned int MAX_UNSIGNED_INT=UINT_MAX;
	const float MIN_FLOAT=FLT_MIN;
	const float MAX_FLOAT=FLT_MAX;
	const double MIN_DOUBLE=DBL_MIN;
	const double MAX_DOUBLE=DBL_MAX;

	const mathfixed::fixed MIN_FIXED=MIN_INT;
	const mathfixed::fixed MAX_FIXED=MAX_INT;

	#if defined(TOADLET_EGG_MATH_DOUBLE)
		const math::real MIN_REAL=MIN_DOUBLE;
		const math::real MAX_REAL=MAX_DOUBLE;
	#else
		const math::real MIN_REAL=MIN_FLOAT;
		const math::real MAX_REAL=MAX_FLOAT;
	#endif

	#if defined(TOADLET_FIXED_POINT)  
		const mathfixed::fixed MIN_SCALAR=MIN_FIXED;
		const mathfixed::fixed MAX_SCALAR=MAX_FIXED;
	#else
		const math::real MIN_SCALAR=MIN_REAL;
		const math::real MAX_SCALAR=MAX_REAL;
	#endif

	const int8 MAX_INT8=127;
	const int8 MIN_INT8=-128;
	const uint8 MAX_UINT8=255;
	const int16 MAX_INT16=32767;
	const int16 MIN_INT16=-32768;
	const uint16 MAX_UINT16=65535;
	const int32 MAX_INT32=0x7FFFFFFF;
	const int32 MIN_INT32=-MAX_INT32-1;
	const uint32 MAX_UINT32=0xFFFFFFFF;
	const int64 MAX_INT64=TOADLET_MAKE_INT64(0x7FFFFFFFFFFFFFFF);
	const int64 MIN_INT64=-MAX_INT64-1;
	const uint64 MAX_UINT64=TOADLET_MAKE_UINT64(0xFFFFFFFFFFFFFFFF);

	inline bool isInfinite(double v){
		#if defined(TOADLET_PLATFORM_WIN32)
			return !_finite(v);
		#else
			return isinf(v);
		#endif
	}

	inline bool isNAN(double v){
		#if defined(TOADLET_PLATFORM_WIN32)
			return _isnan(v)>0;
		#else
			return isnan(v)>0;
		#endif
	}

	inline bool isReal(double v){
		return isInfinite(v)==false && isNAN(v)==false;
	}
}

}
}

#endif
