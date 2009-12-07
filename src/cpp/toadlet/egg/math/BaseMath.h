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

#ifndef TOADLET_EGG_MATH_BASEMATH_H
#define TOADLET_EGG_MATH_BASEMATH_H

#include <toadlet/Types.h>

#if defined(TOADLET_PLATFORM_OSX)
	#include <sys/../math.h> // Force it to use the right math.h, because xcode is buggy
#else
	#include <math.h>
#endif

namespace toadlet{
namespace egg{
namespace math{

typedef float real;

namespace Math{
	const real ONE=(real)1.0;
	const real TWO=(real)2.0;
	const real THREE=(real)3.0;
	const real FOUR=(real)4.0;
	const real THREE_QUARTERS=(real)0.75;
	const real HALF=(real)0.5;
	const real QUARTER=(real)0.25;
	const real ONE_EIGHTY=(real)180.0;
	const real PI=(real)3.14159265358979323846264;
	const real TWO_PI=(real)(PI*2.0);
	const real THREE_QUARTERS_PI=(real)(PI*3.0/4.0);
	const real HALF_PI=(real)(PI/2.0);
	const real QUARTER_PI=(real)(PI/4.0);
	const real E=(real)2.71828182845904523536028;
	const real SQRT_2=(real)1.41421356237309504880168;

	inline real mul(real b,real c){
		return b*c;
	}

	inline real div(real b,real c){
		return b/c;
	}

	inline real madd(real b,real c,real a){
		return b*c+a;
	}

	inline real msub(real b,real c,real a){
		return b*c-a;
	}

	inline real milliToReal(int m){
		return (real)(m/1000.0);
	}

	inline real fromMilli(int m){return milliToReal(m);}

	inline int realToMilli(real r){
		return (int)(r*1000);
	}

	inline int toMilli(real r){return realToMilli(r);}

	inline real fromInt(int i){return (real)i;}

	inline int toInt(real r){return (int)r;}

	inline real abs(real r){return ::fabs(r);}
	inline int abs(int i){return (i^(i>>31))-(i>>31);}

	inline real ceil(real r){return ::ceil(r);}

	inline real floor(real r){return ::floor(r);}

	inline int intCeil(real r){
		return (int)ceil(r);
	}

	inline int intFloor(real x){
		return (int)floor(x);
	}

	inline real square(real r){
		return r*r;
	}

	inline real sqrt(real r){return ::sqrt(r);}

	template<class T> inline T maxVal(const T &t1,const T &t2){
		return ((t1>t2)?t1:t2);
	}

	template<class T> inline T minVal(const T &t1,const T &t2){
		return ((t1<t2)?t1:t2);
	}

	template<class T1,class T2,class T3> inline T3 clamp(const T1 low,const T2 high,const T3 value){
		if(value<low){
			return low;
		}
		if(value>high){
			return high;
		}
		return value;
	}

	inline real lerp(real t1,real t2,real time){
		return (t1 + time*(t2-t1));
	}

	inline int nextPowerOf2(int i){
		int result=1;
		while(result<i){
			result<<=1;
		}
		return result;
	}

	inline bool isPowerOf2(int i){
		return (i&(i-1))==0;
	}

	inline real degToRad(real deg){
		return (real)(deg*(real)(PI/ONE_EIGHTY));
	}

	inline real radToDeg(real rad){
		return (real)(rad*(real)(ONE_EIGHTY/PI));
	}

	inline real cos(real r){return ::cos(r);}
	inline real acos(real r){return ::acos(r);}
	inline real sin(real r){return ::sin(r);}
	inline real asin(real r){return ::asin(r);}
	inline real tan(real r){return ::tan(r);}
	inline real atan(real r){return ::atan(r);}
	inline real atan2(real r1,real r2){return ::atan2(r1,r2);}
	inline real log(real r){return ::log(r);}
}

}
}
}

#endif
