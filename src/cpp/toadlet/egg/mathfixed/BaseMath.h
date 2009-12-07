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

#ifndef TOADLET_EGG_MATHFIXED_BASEMATH_H
#define TOADLET_EGG_MATHFIXED_BASEMATH_H

#include <toadlet/Types.h>
#include <toadlet/egg/mathfixed/Inline.h>

#if defined(TOADLET_REPORT_OVERFLOWS)
	#include <toadlet/egg/Error.h>

	#define TOADLET_CHECK_OVERFLOW(x,s) \
		if(x<0){ \
			Error::overflow(Categories::TOADLET_EGG,s); \
		}
#else
	#define TOADLET_CHECK_OVERFLOW(x,s)
#endif

namespace toadlet{
namespace egg{
namespace mathfixed{

typedef int fixed;

namespace Math{
	const int BITS=16;
	const fixed ONE=65536;
	const fixed TWO=ONE*2;
	const fixed THREE=ONE*3;
	const fixed FOUR=ONE*4;
	const fixed THREE_QUARTERS=ONE*3/4;
	const fixed HALF=ONE/2;
	const fixed QUARTER=ONE/4;
	const fixed ONE_EIGHTY=ONE*180;
	const fixed PI=205887;
	const fixed TWO_PI=PI*2;
	const fixed THREE_QUARTERS_PI=PI*3/4;
	const fixed HALF_PI=PI/2;
	const fixed QUARTER_PI=PI/4;
	const fixed E=178145;
	const fixed SQRT_2=92682;

	inline fixed mul(fixed x,fixed y){
		return TOADLET_MUL_XX(x,y);
	}

	inline fixed div(fixed x,fixed y){
		return TOADLET_DIV_XX(x,y);
	}

	inline fixed madd(fixed x,fixed y,fixed z){
		return TOADLET_MADD_XXX(x,y,z);
	}

	inline fixed msub(fixed x,fixed y,fixed z){
		return TOADLET_MSUB_XXX(x,y,z);
	}

	inline fixed milliToFixed(int m){
		return div(m<<BITS,1000<<BITS);
	}

	inline fixed fromMilli(int m){return milliToFixed(m);}

	inline int fixedToMilli(fixed x){
		return (x*1000)>>BITS;
	}

	inline int toMilli(fixed x){return fixedToMilli(x);}

	inline fixed intToFixed(int i){
		return i<<BITS;
	}

	inline fixed fromInt(int i){return intToFixed(i);}

	inline int fixedToInt(fixed x){
		return x>>BITS;
	}

	inline int toInt(fixed x){return fixedToInt(x);}

	inline fixed floatToFixed(float f){
		return (fixed)(f*(float)ONE);
	}

	inline fixed fromFloat(float f){return floatToFixed(f);}

	inline float fixedToFloat(fixed x){
		return (float)x/(float)ONE;
	}

	inline float toFloat(fixed x){return fixedToFloat(x);}

	inline fixed abs(fixed x){
		return (x^(x>>31))-(x>>31);
	}

	inline fixed ceil(fixed x){
		return ((x+(ONE-1))>>BITS)<<BITS;
	}

	inline fixed floor(fixed x){
		return ((x>>BITS)<<BITS);
	}

	inline int intCeil(fixed x){
		return (x+(ONE-1))>>BITS;
	}

	inline int intFloor(fixed x){
		return x>>BITS;
	}

	inline fixed square(fixed x){
		x = TOADLET_MUL_XX(x,x);
		TOADLET_CHECK_OVERFLOW(x,"overflow in square");
		return x;
	}

	inline fixed sqrt(fixed x){
		fixed s=(x + ONE) >> 1;
		for(int i=0;i<6;i++){
			s=(s + TOADLET_DIV_XX(x,s)) >> 1;
		}
		return s;
	}

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

	inline fixed lerp(fixed t1,fixed t2,fixed time){
		return (t1 + TOADLET_MUL_XX(time,t2-t1));
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

	inline fixed degToRad(fixed d){
		return TOADLET_DIV_XX(TOADLET_MUL_XX(d,PI),ONE_EIGHTY);
	}

	inline fixed radToDeg(fixed r){
		return TOADLET_DIV_XX(TOADLET_MUL_XX(r,ONE_EIGHTY),PI);
	}

	inline fixed sin(fixed f){
		if(f<0){
			f=((f%TWO_PI)+TWO_PI);
		}
		else if(f>=TWO_PI){
			f=f%TWO_PI;
		}

		int sign=1;
		if((f>HALF_PI) && (f<=PI)){
			f=PI-f;
		}
		else if((f>PI) && (f<=(PI+HALF_PI))){
			f=f-PI;
			sign=-1;
		}
		else if(f>(PI+HALF_PI)){
			f=TWO_PI-f;
			sign=-1;
		}

		fixed sqr=TOADLET_MUL_XX(f,f);
		fixed result=498;
		result=TOADLET_MUL_XX(result,sqr);
		result-=10882;
		result=TOADLET_MUL_XX(result,sqr);
		result+=ONE;
		result=TOADLET_MUL_XX(result,f);
		return sign*result;
	}

	inline fixed cos(fixed f){
		if(f<0){
			f=((f%TWO_PI)+TWO_PI);
		}
		else if(f>=TWO_PI){
			f=f%TWO_PI;
		}

		int sign=1;
		if((f>HALF_PI) && (f<=PI)){
			f=PI-f;
			sign=-1;
		}
		else if((f>HALF_PI) && (f<=(PI+HALF_PI))){
			f=f-PI;
			sign=-1;
		}
		else if(f>(PI+HALF_PI)){
			f=TWO_PI-f;
		}

		fixed sqr=TOADLET_MUL_XX(f,f);
		fixed result=2328;
		result=TOADLET_MUL_XX(result,sqr);
		result-=32551;
		result=TOADLET_MUL_XX(result,sqr);
		result+=ONE;
		return result*sign;
	}

	inline fixed tan(fixed f){
		if(f<0){
			f=((f%TWO_PI)+TWO_PI);
		}
		else if(f>=TWO_PI){
			f=f%TWO_PI;
		}

		fixed sqr=TOADLET_MUL_XX(f,f);
		fixed result=13323;
		result=TOADLET_MUL_XX(result,sqr);
		result+=20810;
		result=TOADLET_MUL_XX(result,sqr);
		result+=ONE;
		result=TOADLET_MUL_XX(result,f);
		return result;
	}
	
	inline fixed atan(fixed f){
		fixed sqr=TOADLET_MUL_XX(f,f);
		fixed result=1365;
		result=TOADLET_MUL_XX(result,sqr);
		result-=5579;
		result=TOADLET_MUL_XX(result,sqr);
		result+=11805;
		result=TOADLET_MUL_XX(result,sqr);
		result-=21646;
		result=TOADLET_MUL_XX(result,sqr);
		result+=65527;
		result=TOADLET_MUL_XX(result,f);
		return result;
	}

	inline fixed asin(fixed f){
		fixed fRoot=sqrt(ONE-f);
		fixed result=-1228;
		result=TOADLET_MUL_XX(result,f);
		result+=4866;
		result=TOADLET_MUL_XX(result,f);
		result-=13901;
		result=TOADLET_MUL_XX(result,f);
		result+=102939;
		result=HALF_PI-TOADLET_MUL_XX(fRoot,result);
		return result;
	}

    inline fixed acos(fixed f){
		fixed fRoot=sqrt(ONE-f);
		fixed result=-1228;
		result=TOADLET_MUL_XX(result,f);
		result+=4866;
		result=TOADLET_MUL_XX(result,f);
		result-=13901;
		result=TOADLET_MUL_XX(result,f);
		result+=102939;
		result=TOADLET_MUL_XX(fRoot,result);
		return result;
	}

	inline fixed atan2(fixed y,fixed x){
		fixed absy=(y^(y>>31))-(y>>31)+1; // Add an epsilon to avoid div zero
		fixed angle;
		if(x>=0){
			fixed r=TOADLET_DIV_XX(x-absy,x+absy);
			angle=QUARTER_PI-TOADLET_MUL_XX(QUARTER_PI,r);
		}
		else{
			fixed r=TOADLET_DIV_XX(x+absy,y-absy);
			angle=THREE_QUARTERS_PI-TOADLET_MUL_XX(QUARTER_PI,r);
		}
		return y<0?-angle:angle;
	}
}

}
}
}

#endif
