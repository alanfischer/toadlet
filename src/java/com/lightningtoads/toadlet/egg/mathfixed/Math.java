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

package com.lightningtoads.toadlet.egg.mathfixed;

#include <com/lightningtoads/toadlet/Types.h>
#include <com/lightningtoads/toadlet/egg/mathfixed/Inline.h>

#if defined(TOADLET_REPORT_OVERFLOWS)
	import com.lightningtoads.toadlet.egg.Categories;
	import com.lightningtoads.toadlet.egg.Error;

	#define TOADLET_CHECK_OVERFLOW(x,s) \
		if(x<0){ \
			Error.overflow(Categories.TOADLET_EGG,s); \
		}
#else
	#define TOADLET_CHECK_OVERFLOW(x,s)
#endif

public final class Math{
	public static final fixed BITS=16;
	public static final fixed ONE=65536;
	public static final fixed TWO=ONE*2;
	public static final fixed THREE=ONE*3;
	public static final fixed FOUR=ONE*4;
	public static final fixed THREE_QUARTERS=ONE*3/4;
	public static final fixed HALF=ONE/2;
	public static final fixed QUARTER=ONE/4;
	public static final fixed ONE_EIGHTY=ONE*180;
	public static final fixed PI=205887;
	public static final fixed TWO_PI=PI*2;
	public static final fixed THREE_QUARTERS_PI=PI*3/4;
	public static final fixed HALF_PI=PI/2;
	public static final fixed QUARTER_PI=PI/4;
	public static final fixed E=178145;
	public static final fixed SQRT_2=92682;

	public static final Vector2 ZERO_VECTOR2=new Vector2();
	public static final Vector2 ONE_VECTOR2=new Vector2(ONE,ONE);
	public static final Vector2 X_UNIT_VECTOR2=new Vector2(ONE,0);
	public static final Vector2 NEG_X_UNIT_VECTOR2=new Vector2(-ONE,0);
	public static final Vector2 Y_UNIT_VECTOR2=new Vector2(0,ONE);
	public static final Vector2 NEG_Y_UNIT_VECTOR2=new Vector2(0,-ONE);
	public static final Vector3 ZERO_VECTOR3=new Vector3();
	public static final Vector3 ONE_VECTOR3=new Vector3(ONE,ONE,ONE);
	public static final Vector3 X_UNIT_VECTOR3=new Vector3(ONE,0,0);
	public static final Vector3 NEG_X_UNIT_VECTOR3=new Vector3(-ONE,0,0);
	public static final Vector3 Y_UNIT_VECTOR3=new Vector3(0,ONE,0);
	public static final Vector3 NEG_Y_UNIT_VECTOR3=new Vector3(0,-ONE,0);
	public static final Vector3 Z_UNIT_VECTOR3=new Vector3(0,0,ONE);
	public static final Vector3 NEG_Z_UNIT_VECTOR3=new Vector3(0,0,-ONE);
	public static final Vector4 ZERO_VECTOR4=new Vector4();
	public static final Vector4 ONE_VECTOR4=new Vector4(ONE,ONE,ONE,ONE);
	public static final Vector4 X_UNIT_VECTOR4=new Vector4(ONE,0,0,0);
	public static final Vector4 NEG_X_UNIT_VECTOR4=new Vector4(-ONE,0,0,0);
	public static final Vector4 Y_UNIT_VECTOR4=new Vector4(0,ONE,0,0);
	public static final Vector4 NEG_Y_UNIT_VECTOR4=new Vector4(0,-ONE,0,0);
	public static final Vector4 Z_UNIT_VECTOR4=new Vector4(0,0,ONE,0);
	public static final Vector4 NEG_Z_UNIT_VECTOR4=new Vector4(0,0,-ONE,0);
	public static final Vector4 W_UNIT_VECTOR4=new Vector4(0,0,0,ONE);
	public static final Vector4 NEG_W_UNIT_VECTOR4=new Vector4(0,0,0,-ONE);
	public static final Matrix3x3 IDENTITY_MATRIX3X3=new Matrix3x3();
	public static final Matrix4x4 IDENTITY_MATRIX4X4=new Matrix4x4();
	public static final Quaternion IDENTITY_QUATERNION=new Quaternion();
	public static final EulerAngle IDENTITY_EULERANGLE=new EulerAngle();
	public static final AABox ZERO_AABOX=new AABox();

	public static fixed mul(fixed b,fixed c){
		return TOADLET_MUL_XX(b,c);
	}

	public static fixed div(fixed b,fixed c){
		return TOADLET_DIV_XX(b,c);
	}

	public static fixed madd(fixed b,fixed c,fixed a){
		return TOADLET_MADD_XXX(b,c,a);
	}

	public static fixed msub(fixed b,fixed c,fixed a){
		return TOADLET_MSUB_XXX(b,c,a);
	}

	public static fixed milliToFixed(int m){
		return div(m<<BITS,1000<<BITS);
	}

	public static fixed fromMilli(int m){return milliToFixed(m);}

	public static int fixedToMilli(fixed x){
		return (x*1000)>>BITS;
	}

	public static int toMilli(fixed x){return fixedToMilli(x);}

	public static fixed intToFixed(int i){
		return i<<BITS;
	}

	public static fixed fromInt(int i){return intToFixed(i);}

	public static int fixedToInt(fixed x){
		return x>>BITS;
	}

	public static int toInt(fixed x){return fixedToInt(x);}

	public static fixed floatToFixed(float f){
		return (fixed)(f*(float)ONE);
	}

	public static fixed fromFloat(float f){return floatToFixed(f);}

	public static float fixedToFloat(fixed x){
		return (float)x/(float)ONE;
	}

	public static float toFloat(fixed x){return fixedToFloat(x);}

	public static fixed abs(fixed x){
		return (x^(x>>31))-(x>>31);
	}

	public static fixed ceil(fixed x){
		return ((x+(ONE-1))>>BITS)<<BITS;
	}

	public static fixed floor(fixed x){
		return ((x>>BITS)<<BITS);
	}

	public static int intCeil(fixed x){
		return (x+(ONE-1))>>BITS;
	}

	public static int intFloor(fixed x){
		return x>>BITS;
	}

	public static fixed square(fixed x){
		x = TOADLET_MUL_XX(x,x);
		TOADLET_CHECK_OVERFLOW(x,"overflow in square");
		return x;
	}

	public static fixed sqrt(fixed x){
		fixed s = (x + ONE) >> 1;
		for(int i=0;i<6;i++){
			s = (s + TOADLET_DIV_XX(x,s)) >> 1;
		}
		return s;
	}

	public static fixed minVal(fixed x,fixed y){
		return TOADLET_MIN_XX(x,y);
	}

	public static fixed maxVal(fixed x,fixed y){
		return TOADLET_MAX_XX(x,y);
	}

	public static fixed clamp(fixed low,fixed high,fixed value){
		low=TOADLET_MAX_XX(low,value);
		return TOADLET_MIN_XX(high,low);
	}

	public static fixed lerp(fixed t1,fixed t2,fixed time){
		return (t1 + TOADLET_MUL_XX(time,t2-t1));
	}

	public static int nextPowerOf2(int i){
		int result=1;
		while(result<i){
			result<<=1;
		}
		return result;
	}

	public static boolean isPowerOf2(int i){
		return (i&(i-1))==0;
	}

	public static fixed degToRad(fixed d){
		return TOADLET_DIV_XX(TOADLET_MUL_XX(d,PI),ONE_EIGHTY);
	}

	public static fixed radToDeg(fixed r){
		return TOADLET_DIV_XX(TOADLET_MUL_XX(r,ONE_EIGHTY),PI);
	}

	public static fixed sin(fixed f){
		if(f<0){
			f=((f%TWO_PI)+TWO_PI);
		}
		else if(f>=TWO_PI){
			f=f%TWO_PI;
		}

		int sign=1;
		if((f>HALF_PI)&&(f<=PI)){
			f=PI-f;
		}
		else if((f>PI)&&(f<=(PI+HALF_PI))){
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

	public static fixed cos(fixed f){
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

	public static fixed tan(fixed f){
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
	
	public static fixed atan(fixed f){
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

	public static fixed asin(fixed f){
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

	public static fixed acos(fixed f){
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

	public static fixed atan2(fixed y,fixed x){
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

	// Vector2 operations
	public static void neg(Vector2 r,Vector2 v){
		r.x=-v.x;
		r.y=-v.y;
	}

	public static void neg(Vector2 v){
		v.x=-v.x;
		v.y=-v.y;
	}

	public static void add(Vector2 r,Vector2 v1,Vector2 v2){
		r.x=v1.x+v2.x;
		r.y=v1.y+v2.y;
	}

	public static void add(Vector2 r,Vector2 v){
		r.x+=v.x;
		r.y+=v.y;
	}

	public static void sub(Vector2 r,Vector2 v1,Vector2 v2){
		r.x=v1.x-v2.x;
		r.y=v1.y-v2.y;
	}

	public static void sub(Vector2 r,Vector2 v){
		r.x-=v.x;
		r.y-=v.y;
	}

	public static void mul(Vector2 r,Vector2 v,fixed f){
		r.x=TOADLET_MUL_XX(v.x,f);
		r.y=TOADLET_MUL_XX(v.y,f);
	}

	public static void mul(Vector2 r,fixed f){
		r.x=TOADLET_MUL_XX(r.x,f);
		r.y=TOADLET_MUL_XX(r.y,f);
	}
	
	public static void div(Vector2 r,Vector2 v,fixed f){
		fixed i=TOADLET_DIV_XX(ONE,f);
		r.x=TOADLET_MUL_XX(v.x,i);
		r.y=TOADLET_MUL_XX(v.y,i);
	}

	public static void div(Vector2 r,fixed f){
		fixed i=TOADLET_DIV_XX(ONE,f);
		r.x=TOADLET_MUL_XX(r.x,i);
		r.y=TOADLET_MUL_XX(r.y,i);
	}

	public static void madd(Vector2 r,Vector2 b,fixed c,Vector2 a){
		r.x=TOADLET_MADD_XXX(b.x,c,a.x);
		r.y=TOADLET_MADD_XXX(b.y,c,a.y);
	}

	public static void madd(Vector2 r,fixed c,Vector2 a){
		r.x=TOADLET_MADD_XXX(r.x,c,a.x);
		r.y=TOADLET_MADD_XXX(r.y,c,a.y);
	}

	public static void msub(Vector2 r,Vector2 b,fixed c,Vector2 a){
		r.x=TOADLET_MSUB_XXX(b.x,c,a.x);
		r.y=TOADLET_MSUB_XXX(b.y,c,a.y);
	}

	public static void msub(Vector2 r,fixed c,Vector2 a){
		r.x=TOADLET_MSUB_XXX(r.x,c,a.x);
		r.y=TOADLET_MSUB_XXX(r.y,c,a.y);
	}

	public static fixed lengthSquared(Vector2 v){
		fixed r=TOADLET_MUL_XX(v.x,v.x) + TOADLET_MUL_XX(v.y,v.y);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	public static fixed lengthSquared(Vector2 v1,Vector2 v2){
		fixed x=v1.x-v2.x;
		fixed y=v1.y-v2.y;
		fixed r=TOADLET_MUL_XX(x,x) + TOADLET_MUL_XX(y,y);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	public static fixed length(Vector2 v){
		return sqrt(lengthSquared(v));
	}

	public static fixed length(Vector2 v1,Vector2 v2){
		return sqrt(lengthSquared(v1,v2));
	}

	public static void normalize(Vector2 v){
		fixed l=TOADLET_DIV_XX(ONE,length(v));
		v.x=TOADLET_MUL_XX(v.x,l);
		v.y=TOADLET_MUL_XX(v.x,l);
	}

	public static void normalize(Vector2 r,Vector2 v){
		fixed l=TOADLET_DIV_XX(ONE,length(v));
		r.x=TOADLET_MUL_XX(v.x,l);
		r.y=TOADLET_MUL_XX(v.y,l);
	}

	public static boolean normalizeCarefully(Vector2 v,fixed epsilon){
		fixed l=length(v);
		if(l>epsilon){
			l=TOADLET_DIV_XX(ONE,l);
			v.x=TOADLET_MUL_XX(v.x,l);
			v.y=TOADLET_MUL_XX(v.y,l);
			return true;
		}
		return false;
	}

	public static boolean normalizeCarefully(Vector2 r,Vector2 v,fixed epsilon){
		fixed l=length(v);
		if(l>epsilon){
			l=TOADLET_DIV_XX(ONE,l);
			r.x=TOADLET_MUL_XX(v.x,l);
			r.y=TOADLET_MUL_XX(v.y,l);
			return true;
		}
		return false;
	}

	public static fixed dot(Vector2 v1,Vector2 v2){
		return TOADLET_MUL_XX(v1.x,v2.x) + TOADLET_MUL_XX(v1.y,v2.y);
	}

	public static void lerp(Vector2 r,Vector2 v1,Vector2 v2,fixed t){
		sub(r,v2,v1);
		mul(r,t);
		add(r,v1);
	}

	// Vector3 operations
	public static void neg(Vector3 r,Vector3 v){
		r.x=-v.x;
		r.y=-v.y;
		r.z=-v.z;
	}

	public static void neg(Vector3 v){
		v.x=-v.x;
		v.y=-v.y;
		v.z=-v.z;
	}

	public static void add(Vector3 r,Vector3 v1,Vector3 v2){
		r.x=v1.x+v2.x;
		r.y=v1.y+v2.y;
		r.z=v1.z+v2.z;
	}

	public static void add(Vector3 r,Vector3 v){
		r.x+=v.x;
		r.y+=v.y;
		r.z+=v.z;
	}
	
	public static void sub(Vector3 r,Vector3 v1,Vector3 v2){
		r.x=v1.x-v2.x;
		r.y=v1.y-v2.y;
		r.z=v1.z-v2.z;
	}
	
	public static void sub(Vector3 r,Vector3 v){
		r.x-=v.x;
		r.y-=v.y;
		r.z-=v.z;
	}

	public static void mul(Vector3 r,Vector3 v,fixed f){
		r.x=TOADLET_MUL_XX(v.x,f);
		r.y=TOADLET_MUL_XX(v.y,f);
		r.z=TOADLET_MUL_XX(v.z,f);
	}
	
	public static void mul(Vector3 r,fixed f){
		r.x=TOADLET_MUL_XX(r.x,f);
		r.y=TOADLET_MUL_XX(r.y,f);
		r.z=TOADLET_MUL_XX(r.z,f);
	}
	
	public static void div(Vector3 r,Vector3 v,fixed f){
		fixed i=TOADLET_DIV_XX(ONE,f);
		r.x=TOADLET_MUL_XX(v.x,i);
		r.y=TOADLET_MUL_XX(v.y,i);
		r.z=TOADLET_MUL_XX(v.z,i);
	}
	
	public static void div(Vector3 r,fixed f){
		fixed i=TOADLET_DIV_XX(ONE,f);
		r.x=TOADLET_MUL_XX(r.x,i);
		r.y=TOADLET_MUL_XX(r.y,i);
		r.z=TOADLET_MUL_XX(r.z,i);
	}
	
	public static void madd(Vector3 r,Vector3 b,fixed c,Vector3 a){
		r.x=TOADLET_MADD_XXX(b.x,c,a.x);
		r.y=TOADLET_MADD_XXX(b.y,c,a.y);
		r.z=TOADLET_MADD_XXX(b.z,c,a.z);
	}

	public static void madd(Vector3 r,fixed c,Vector3 a){
		r.x=TOADLET_MADD_XXX(r.x,c,a.x);
		r.y=TOADLET_MADD_XXX(r.y,c,a.y);
		r.z=TOADLET_MADD_XXX(r.z,c,a.z);
	}

	public static void msub(Vector3 r,Vector3 b,fixed c,Vector3 a){
		r.x=TOADLET_MSUB_XXX(b.x,c,a.x);
		r.y=TOADLET_MSUB_XXX(b.y,c,a.y);
		r.z=TOADLET_MSUB_XXX(b.z,c,a.z);
	}

	public static void msub(Vector3 r,fixed c,Vector3 a){
		r.x=TOADLET_MSUB_XXX(r.x,c,a.x);
		r.y=TOADLET_MSUB_XXX(r.y,c,a.y);
		r.z=TOADLET_MSUB_XXX(r.z,c,a.z);
	}

	public static fixed lengthSquared(Vector3 v){
		fixed r=TOADLET_MUL_XX(v.x,v.x) + TOADLET_MUL_XX(v.y,v.y) + TOADLET_MUL_XX(v.z,v.z);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	public static fixed lengthSquared(Vector3 v1,Vector3 v2){
		fixed x=v1.x-v2.x;
		fixed y=v1.y-v2.y;
		fixed z=v1.z-v2.z;
		fixed r=TOADLET_MUL_XX(x,x) + TOADLET_MUL_XX(y,y) + TOADLET_MUL_XX(z,z);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}
	
	public static fixed length(Vector3 v){
		return sqrt(lengthSquared(v));
	}
	
	public static fixed length(Vector3 v1,Vector3 v2){
		return sqrt(lengthSquared(v1,v2));
	}

	public static void normalize(Vector3 v){
		fixed l=TOADLET_DIV_XX(ONE,length(v));
		v.x=TOADLET_MUL_XX(v.x,l);
		v.y=TOADLET_MUL_XX(v.y,l);
		v.z=TOADLET_MUL_XX(v.z,l);
	}

	public static void normalize(Vector3 r,Vector3 v){
		fixed l=TOADLET_DIV_XX(ONE,length(v));
		r.x=TOADLET_MUL_XX(v.x,l);
		r.y=TOADLET_MUL_XX(v.y,l);
		r.z=TOADLET_MUL_XX(v.z,l);
	}

	public static boolean normalizeCarefully(Vector3 v,fixed epsilon){
		fixed l=length(v);
		if(l>epsilon){
			l=TOADLET_DIV_XX(ONE,l);
			v.x=TOADLET_MUL_XX(v.x,l);
			v.y=TOADLET_MUL_XX(v.y,l);
			v.z=TOADLET_MUL_XX(v.z,l);
			return true;
		}
		return false;
	}
	
	public static boolean normalizeCarefully(Vector3 r,Vector3 v,fixed epsilon){
		fixed l=length(v);
		if(l>epsilon){
			l=TOADLET_DIV_XX(ONE,l);
			r.x=TOADLET_MUL_XX(v.x,l);
			r.y=TOADLET_MUL_XX(v.y,l);
			r.z=TOADLET_MUL_XX(v.z,l);
			return true;
		}
		return false;
	}

	public static fixed dot(Vector3 v1,Vector3 v2){
		return TOADLET_MUL_XX(v1.x,v2.x) + TOADLET_MUL_XX(v1.y,v2.y) + TOADLET_MUL_XX(v1.z,v2.z);
	}

	public static void lerp(Vector3 r,Vector3 v1,Vector3 v2,fixed t){
		sub(r,v2,v1);
		mul(r,t);
		add(r,v1);
	}

	public static void cross(Vector3 r,Vector3 v1,Vector3 v2){
		r.x=TOADLET_MUL_XX(v1.y,v2.z) - TOADLET_MUL_XX(v1.z,v2.y);
		r.y=TOADLET_MUL_XX(v1.z,v2.x) - TOADLET_MUL_XX(v1.x,v2.z);
		r.z=TOADLET_MUL_XX(v1.x,v2.y) - TOADLET_MUL_XX(v1.y,v2.x);
	}

	public static void setVector3FromMatrix4x4(Vector3 r,Matrix4x4 m){
		r.x=m.data[0+3*4];
		r.y=m.data[1+3*4];
		r.z=m.data[2+3*4];
	}

	// Vector4 operations
	public static void neg(Vector4 r,Vector4 v){
		r.x=-v.x;
		r.y=-v.y;
		r.z=-v.z;
		r.w=-v.w;
	}

	public static void neg(Vector4 v){
		v.x=-v.x;
		v.y=-v.y;
		v.z=-v.z;
		v.w=-v.w;
	}

	public static void add(Vector4 r,Vector4 v1,Vector4 v2){
		r.x=v1.x+v2.x;
		r.y=v1.y+v2.y;
		r.z=v1.z+v2.z;
		r.w=v1.w+v2.w;
	}

	public static void add(Vector4 r,Vector4 v){
		r.x+=v.x;
		r.y+=v.y;
		r.z+=v.z;
		r.w+=v.w;
	}
	
	public static void sub(Vector4 r,Vector4 v1,Vector4 v2){
		r.x=v1.x-v2.x;
		r.y=v1.y-v2.y;
		r.z=v1.z-v2.z;
		r.w=v1.w-v2.w;
	}
	
	public static void sub(Vector4 r,Vector4 v){
		r.x-=v.x;
		r.y-=v.y;
		r.z-=v.z;
		r.w-=v.w;
	}

	public static void mul(Vector4 r,Vector4 v,fixed f){
		r.x=TOADLET_MUL_XX(v.x,f);
		r.y=TOADLET_MUL_XX(v.y,f);
		r.z=TOADLET_MUL_XX(v.z,f);
		r.w=TOADLET_MUL_XX(v.w,f);
	}
	
	public static void mul(Vector4 r,fixed f){
		r.x=TOADLET_MUL_XX(r.x,f);
		r.y=TOADLET_MUL_XX(r.y,f);
		r.z=TOADLET_MUL_XX(r.z,f);
		r.w=TOADLET_MUL_XX(r.w,f);
	}
	
	public static void div(Vector4 r,Vector4 v,fixed f){
		fixed i=TOADLET_DIV_XX(ONE,f);
		r.x=TOADLET_MUL_XX(v.x,i);
		r.y=TOADLET_MUL_XX(v.y,i);
		r.z=TOADLET_MUL_XX(v.z,i);
		r.w=TOADLET_MUL_XX(v.w,i);
	}
	
	public static void div(Vector4 r,fixed f){
		fixed i=TOADLET_DIV_XX(ONE,f);
		r.x=TOADLET_MUL_XX(r.x,i);
		r.y=TOADLET_MUL_XX(r.y,i);
		r.z=TOADLET_MUL_XX(r.z,i);
		r.w=TOADLET_MUL_XX(r.w,i);
	}

	public static void madd(Vector4 r,Vector4 b,fixed c,Vector4 a){
		r.x=TOADLET_MADD_XXX(b.x,c,a.x);
		r.y=TOADLET_MADD_XXX(b.y,c,a.y);
		r.z=TOADLET_MADD_XXX(b.z,c,a.z);
		r.w=TOADLET_MADD_XXX(b.w,c,a.w);
	}

	public static void madd(Vector4 r,fixed c,Vector4 a){
		r.x=TOADLET_MADD_XXX(r.x,c,a.x);
		r.y=TOADLET_MADD_XXX(r.y,c,a.y);
		r.z=TOADLET_MADD_XXX(r.z,c,a.z);
		r.w=TOADLET_MADD_XXX(r.w,c,a.w);
	}

	public static void msub(Vector4 r,Vector4 b,fixed c,Vector4 a){
		r.x=TOADLET_MSUB_XXX(b.x,c,a.x);
		r.y=TOADLET_MSUB_XXX(b.y,c,a.y);
		r.z=TOADLET_MSUB_XXX(b.z,c,a.z);
		r.w=TOADLET_MSUB_XXX(b.w,c,a.w);
	}

	public static void msub(Vector4 r,fixed c,Vector4 a){
		r.x=TOADLET_MSUB_XXX(r.x,c,a.x);
		r.y=TOADLET_MSUB_XXX(r.y,c,a.y);
		r.z=TOADLET_MSUB_XXX(r.z,c,a.z);
		r.w=TOADLET_MSUB_XXX(r.w,c,a.w);
	}

	public static fixed lengthSquared(Vector4 v){
		fixed r=TOADLET_MUL_XX(v.x,v.x) + TOADLET_MUL_XX(v.y,v.y) + TOADLET_MUL_XX(v.z,v.z) + TOADLET_MUL_XX(v.w,v.w);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	public static fixed lengthSquared(Vector4 v1,Vector4 v2){
		fixed x=v1.x-v2.x;
		fixed y=v1.y-v2.y;
		fixed z=v1.z-v2.z;
		fixed w=v1.w-v2.w;
		fixed r=TOADLET_MUL_XX(x,x) + TOADLET_MUL_XX(y,y) + TOADLET_MUL_XX(z,z) + TOADLET_MUL_XX(w,w);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	public static fixed length(Vector4 v){
		return sqrt(lengthSquared(v));
	}

	public static fixed length(Vector4 v1,Vector4 v2){
		return sqrt(lengthSquared(v1,v2));
	}

	public static void normalize(Vector4 v){
		fixed l=TOADLET_DIV_XX(ONE,length(v));
		v.x=TOADLET_MUL_XX(v.x,l);
		v.y=TOADLET_MUL_XX(v.y,l);
		v.z=TOADLET_MUL_XX(v.z,l);
		v.w=TOADLET_MUL_XX(v.w,l);
	}

	public static void normalize(Vector4 r,Vector4 v){
		fixed l=TOADLET_DIV_XX(ONE,length(v));
		r.x=TOADLET_MUL_XX(v.x,l);
		r.y=TOADLET_MUL_XX(v.y,l);
		r.z=TOADLET_MUL_XX(v.z,l);
		r.w=TOADLET_MUL_XX(v.w,l);
	}

	public static boolean normalizeCarefully(Vector4 v,fixed epsilon){
		fixed l=length(v);
		if(l>epsilon){
			l=TOADLET_DIV_XX(ONE,l);
			v.x=TOADLET_MUL_XX(v.x,l);
			v.y=TOADLET_MUL_XX(v.y,l);
			v.z=TOADLET_MUL_XX(v.w,l);
			v.w=TOADLET_MUL_XX(v.z,l);
			return true;
		}
		return false;
	}

	public static boolean normalizeCarefully(Vector4 r,Vector4 v,fixed epsilon){
		fixed l=length(v);
		if(l>epsilon){
			l=TOADLET_DIV_XX(ONE,l);
			r.x=TOADLET_MUL_XX(v.x,l);
			r.y=TOADLET_MUL_XX(v.y,l);
			r.z=TOADLET_MUL_XX(v.w,l);
			r.w=TOADLET_MUL_XX(v.z,l);
			return true;
		}
		return false;
	}

	public static fixed dot(Vector4 v1,Vector4 v2){
		return TOADLET_MUL_XX(v1.x,v2.x) + TOADLET_MUL_XX(v1.y,v2.y) + TOADLET_MUL_XX(v1.z,v2.z) + TOADLET_MUL_XX(v1.w,v2.w);
	}

	public static void lerp(Vector4 r,Vector4 v1,Vector4 v2,fixed t){
		sub(r,v2,v1);
		mul(r,t);
		add(r,v1);
	}

	// EulerAngle operations
	public static boolean setEulerAngleXYZFromMatrix3x3(EulerAngle r,Matrix3x3 m){
		r.y=asin(m.at(0,2));
		if(r.y<HALF_PI){
			if(r.y>-HALF_PI){
				r.x=atan2(-m.at(1,2),m.at(2,2));
				r.z=atan2(-m.at(0,1),m.at(0,0));
				return true;
			}
			else{
				// WARNING: Not a unique solution.
				fixed my=atan2(m.at(1,0),m.at(1,1));
				r.z=0; // any angle works
				r.x=r.z-my;
				return false;
			}
		}
		else{
			// WARNING: Not a unique solution.
			fixed py=atan2(m.at(1,0),m.at(1,1));
			r.z=0; // any angle works
			r.x=py-r.z;
			return false;
		}
	}

	// Matrix3x3 basic operations
	public static void mul(Matrix3x3 r,Matrix3x3 m1,Matrix3x3 m2){
		r.data[0+0*3]=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+0*3]);
		r.data[0+1*3]=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+1*3]);
		r.data[0+2*3]=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+2*3]);

		r.data[1+0*3]=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+0*3]);
		r.data[1+1*3]=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+1*3]);
		r.data[1+2*3]=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+2*3]);

		r.data[2+0*3]=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+0*3]);
		r.data[2+1*3]=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+1*3]);
		r.data[2+2*3]=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+2*3]);
	}

	public static void postMul(Matrix3x3 m1,Matrix3x3 m2){
		fixed d00=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+0*3]);
		fixed d01=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+1*3]);
		fixed d02=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+2*3]);

		fixed d10=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+0*3]);
		fixed d11=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+1*3]);
		fixed d12=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+2*3]);

		fixed d20=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+0*3]);
		fixed d21=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+1*3]);
		fixed d22=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+2*3]);

		m1.data[0+0*3]=d00; m1.data[0+1*3]=d01; m1.data[0+2*3]=d02;
		m1.data[1+0*3]=d10; m1.data[1+1*3]=d11; m1.data[1+2*3]=d12;
		m1.data[2+0*3]=d20; m1.data[2+1*3]=d21; m1.data[2+2*3]=d22;
	}

	public static void preMul(Matrix3x3 m2,Matrix3x3 m1){
		fixed d00=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+0*3]);
		fixed d01=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+1*3]);
		fixed d02=TOADLET_MUL_XX(m1.data[0+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[0+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[0+2*3],m2.data[2+2*3]);

		fixed d10=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+0*3]);
		fixed d11=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+1*3]);
		fixed d12=TOADLET_MUL_XX(m1.data[1+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[1+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[1+2*3],m2.data[2+2*3]);

		fixed d20=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+0*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+0*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+0*3]);
		fixed d21=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+1*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+1*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+1*3]);
		fixed d22=TOADLET_MUL_XX(m1.data[2+0*3],m2.data[0+2*3]) + TOADLET_MUL_XX(m1.data[2+1*3],m2.data[1+2*3]) + TOADLET_MUL_XX(m1.data[2+2*3],m2.data[2+2*3]);

		m2.data[0+0*3]=d00; m2.data[0+1*3]=d01; m2.data[0+2*3]=d02;
		m2.data[1+0*3]=d10; m2.data[1+1*3]=d11; m2.data[1+2*3]=d12;
		m2.data[2+0*3]=d20; m2.data[2+1*3]=d21; m2.data[2+2*3]=d22;
	}

	public static void mul(Vector3 r,Matrix3x3 m,Vector3 v){
		r.x=TOADLET_MUL_XX(m.data[0+0*3],v.x) + TOADLET_MUL_XX(m.data[0+1*3],v.y) + TOADLET_MUL_XX(m.data[0+2*3],v.z);
		r.y=TOADLET_MUL_XX(m.data[1+0*3],v.x) + TOADLET_MUL_XX(m.data[1+1*3],v.y) + TOADLET_MUL_XX(m.data[1+2*3],v.z);
		r.z=TOADLET_MUL_XX(m.data[2+0*3],v.x) + TOADLET_MUL_XX(m.data[2+1*3],v.y) + TOADLET_MUL_XX(m.data[2+2*3],v.z);
	}

	public static void mul(Vector3 r,Matrix3x3 m){
		fixed tx=TOADLET_MUL_XX(m.data[0+0*3],r.x) + TOADLET_MUL_XX(m.data[0+1*3],r.y) + TOADLET_MUL_XX(m.data[0+2*3],r.z);
		fixed ty=TOADLET_MUL_XX(m.data[1+0*3],r.x) + TOADLET_MUL_XX(m.data[1+1*3],r.y) + TOADLET_MUL_XX(m.data[1+2*3],r.z);
		fixed tz=TOADLET_MUL_XX(m.data[2+0*3],r.x) + TOADLET_MUL_XX(m.data[2+1*3],r.y) + TOADLET_MUL_XX(m.data[2+2*3],r.z);
		r.x=tx;
		r.y=ty;
		r.z=tz;
	}

	public static void transpose(Matrix3x3 r,Matrix3x3 m){
		r.data[0+0*3]=m.data[0+0*3]; r.data[0+1*3]=m.data[1+0*3]; r.data[0+2*3]=m.data[2+0*3];
		r.data[1+0*3]=m.data[0+1*3]; r.data[1+1*3]=m.data[1+1*3]; r.data[1+2*3]=m.data[2+1*3];
		r.data[2+0*3]=m.data[0+2*3]; r.data[2+1*3]=m.data[1+2*3]; r.data[2+2*3]=m.data[2+2*3];
	}

	public static fixed determinant(Matrix3x3 m){
		return	-TOADLET_MUL_XX(TOADLET_MUL_XX(m.data[0+0*3],m.data[1+1*3]),m.data[2+2*3]) + TOADLET_MUL_XX(TOADLET_MUL_XX(m.data[0+0*3],m.data[2+1*3]),m.data[1+2*3]) +
				 TOADLET_MUL_XX(TOADLET_MUL_XX(m.data[0+1*3],m.data[1+0*3]),m.data[2+2*3]) - TOADLET_MUL_XX(TOADLET_MUL_XX(m.data[0+1*3],m.data[2+0*3]),m.data[1+2*3]) -
				 TOADLET_MUL_XX(TOADLET_MUL_XX(m.data[0+2*3],m.data[1+0*3]),m.data[2+1*3]) + TOADLET_MUL_XX(TOADLET_MUL_XX(m.data[0+2*3],m.data[2+0*3]),m.data[1+1*3]);
	}

	public static boolean invert(Matrix3x3 r,Matrix3x3 m){
		fixed det=TOADLET_DIV_XX(ONE,determinant(m));
		if(det==0){
			return false;
		}
		else{
			r.data[0+0*3]=-TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+1*3],m.data[2+2*3]) - TOADLET_MUL_XX(m.data[2+1*3],m.data[1+2*3]));
			r.data[1+0*3]= TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+0*3],m.data[2+2*3]) - TOADLET_MUL_XX(m.data[2+0*3],m.data[1+2*3]));
			r.data[2+0*3]=-TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+0*3],m.data[2+1*3]) - TOADLET_MUL_XX(m.data[2+0*3],m.data[1+1*3]));

			r.data[0+1*3]= TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+1*3],m.data[2+2*3]) - TOADLET_MUL_XX(m.data[2+1*3],m.data[0+2*3]));
			r.data[1+1*3]=-TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+0*3],m.data[2+2*3]) - TOADLET_MUL_XX(m.data[2+0*3],m.data[0+2*3]));
			r.data[2+1*3]= TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+0*3],m.data[2+1*3]) - TOADLET_MUL_XX(m.data[2+0*3],m.data[0+1*3]));

			r.data[0+2*3]=-TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+1*3],m.data[1+2*3]) - TOADLET_MUL_XX(m.data[1+1*3],m.data[0+2*3]));
			r.data[1+2*3]= TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+0*3],m.data[1+2*3]) - TOADLET_MUL_XX(m.data[1+0*3],m.data[0+2*3]));
			r.data[2+2*3]=-TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+0*3],m.data[1+1*3]) - TOADLET_MUL_XX(m.data[1+0*3],m.data[0+1*3]));

			return true;
		}
	}
	
	// Matrix4x4 basic operations
	public static void mul(Matrix4x4 r,Matrix4x4 m1,Matrix4x4 m2){
		r.data[0+0*4]=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+0*4]);
		r.data[0+1*4]=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+1*4]);
		r.data[0+2*4]=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+2*4]);
		r.data[0+3*4]=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+3*4]);

		r.data[1+0*4]=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+0*4]);
		r.data[1+1*4]=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+1*4]);
		r.data[1+2*4]=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+2*4]);
		r.data[1+3*4]=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+3*4]);

		r.data[2+0*4]=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+0*4]);
		r.data[2+1*4]=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+1*4]);
		r.data[2+2*4]=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+2*4]);
		r.data[2+3*4]=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+3*4]);

		r.data[3+0*4]=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+0*4]);
		r.data[3+1*4]=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+1*4]);
		r.data[3+2*4]=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+2*4]);
		r.data[3+3*4]=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+3*4]);
	}

	public static void postMul(Matrix4x4 m1,Matrix4x4 m2){
		fixed d00=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+0*4]);
		fixed d01=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+1*4]);
		fixed d02=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+2*4]);
		fixed d03=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+3*4]);

		fixed d10=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+0*4]);
		fixed d11=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+1*4]);
		fixed d12=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+2*4]);
		fixed d13=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+3*4]);

		fixed d20=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+0*4]);
		fixed d21=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+1*4]);
		fixed d22=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+2*4]);
		fixed d23=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+3*4]);

		fixed d30=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+0*4]);
		fixed d31=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+1*4]);
		fixed d32=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+2*4]);
		fixed d33=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+3*4]);

		m1.data[0+0*4]=d00; m1.data[0+1*4]=d01; m1.data[0+2*4]=d02; m1.data[0+3*4]=d03;
		m1.data[1+0*4]=d10; m1.data[1+1*4]=d11; m1.data[1+2*4]=d12; m1.data[1+3*4]=d13;
		m1.data[2+0*4]=d20; m1.data[2+1*4]=d21; m1.data[2+2*4]=d22; m1.data[2+3*4]=d23;
		m1.data[3+0*4]=d30; m1.data[3+1*4]=d31; m1.data[3+2*4]=d32; m1.data[3+3*4]=d33;
	}

	public static void preMul(Matrix4x4 m2,Matrix4x4 m1){
		fixed d00=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+0*4]);
		fixed d01=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+1*4]);
		fixed d02=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+2*4]);
		fixed d03=TOADLET_MUL_XX(m1.data[0+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[0+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[0+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[0+3*4],m2.data[3+3*4]);

		fixed d10=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+0*4]);
		fixed d11=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+1*4]);
		fixed d12=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+2*4]);
		fixed d13=TOADLET_MUL_XX(m1.data[1+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[1+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[1+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[1+3*4],m2.data[3+3*4]);

		fixed d20=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+0*4]);
		fixed d21=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+1*4]);
		fixed d22=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+2*4]);
		fixed d23=TOADLET_MUL_XX(m1.data[2+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[2+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[2+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[2+3*4],m2.data[3+3*4]);

		fixed d30=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+0*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+0*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+0*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+0*4]);
		fixed d31=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+1*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+1*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+1*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+1*4]);
		fixed d32=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+2*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+2*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+2*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+2*4]);
		fixed d33=TOADLET_MUL_XX(m1.data[3+0*4],m2.data[0+3*4]) + TOADLET_MUL_XX(m1.data[3+1*4],m2.data[1+3*4]) + TOADLET_MUL_XX(m1.data[3+2*4],m2.data[2+3*4]) + TOADLET_MUL_XX(m1.data[3+3*4],m2.data[3+3*4]);

		m2.data[0+0*4]=d00; m2.data[0+1*4]=d01; m2.data[0+2*4]=d02; m2.data[0+3*4]=d03;
		m2.data[1+0*4]=d10; m2.data[1+1*4]=d11; m2.data[1+2*4]=d12; m2.data[1+3*4]=d13;
		m2.data[2+0*4]=d20; m2.data[2+1*4]=d21; m2.data[2+2*4]=d22; m2.data[2+3*4]=d23;
		m2.data[3+0*4]=d30; m2.data[3+1*4]=d31; m2.data[3+2*4]=d32; m2.data[3+3*4]=d33;
	}	

	public static void mul(Vector4 r,Matrix4x4 m,Vector4 v){
		r.x=TOADLET_MUL_XX(m.data[0+0*4],v.x) + TOADLET_MUL_XX(m.data[0+1*4],v.y) + TOADLET_MUL_XX(m.data[0+2*4],v.z) + TOADLET_MUL_XX(m.data[0+3*4],v.w);
		r.y=TOADLET_MUL_XX(m.data[1+0*4],v.x) + TOADLET_MUL_XX(m.data[1+1*4],v.y) + TOADLET_MUL_XX(m.data[1+2*4],v.z) + TOADLET_MUL_XX(m.data[1+3*4],v.w);
		r.z=TOADLET_MUL_XX(m.data[2+0*4],v.x) + TOADLET_MUL_XX(m.data[2+1*4],v.y) + TOADLET_MUL_XX(m.data[2+2*4],v.z) + TOADLET_MUL_XX(m.data[2+3*4],v.w);
		r.w=TOADLET_MUL_XX(m.data[3+0*4],v.x) + TOADLET_MUL_XX(m.data[3+1*4],v.y) + TOADLET_MUL_XX(m.data[3+2*4],v.z) + TOADLET_MUL_XX(m.data[3+3*4],v.w);
	}

	public static void mul(Vector4 r,Matrix4x4 m){
		fixed tx=TOADLET_MUL_XX(m.data[0+0*4],r.x) + TOADLET_MUL_XX(m.data[0+1*4],r.y) + TOADLET_MUL_XX(m.data[0+2*4],r.z) + TOADLET_MUL_XX(m.data[0+3*4],r.w);
		fixed ty=TOADLET_MUL_XX(m.data[1+0*4],r.x) + TOADLET_MUL_XX(m.data[1+1*4],r.y) + TOADLET_MUL_XX(m.data[1+2*4],r.z) + TOADLET_MUL_XX(m.data[1+3*4],r.w);
		fixed tz=TOADLET_MUL_XX(m.data[2+0*4],r.x) + TOADLET_MUL_XX(m.data[2+1*4],r.y) + TOADLET_MUL_XX(m.data[2+2*4],r.z) + TOADLET_MUL_XX(m.data[2+3*4],r.w);
		fixed tw=TOADLET_MUL_XX(m.data[3+0*4],r.x) + TOADLET_MUL_XX(m.data[3+1*4],r.y) + TOADLET_MUL_XX(m.data[3+2*4],r.z) + TOADLET_MUL_XX(m.data[3+3*4],r.w);
		r.x=tx;
		r.y=ty;
		r.z=tz;
		r.w=tw;
	}

	public static void mul(Vector3 r,Matrix4x4 m,Vector3 v){
		r.x=TOADLET_MUL_XX(m.data[0+0*4],v.x) + TOADLET_MUL_XX(m.data[0+1*4],v.y) + TOADLET_MUL_XX(m.data[0+2*4],v.z);
		r.y=TOADLET_MUL_XX(m.data[1+0*4],v.x) + TOADLET_MUL_XX(m.data[1+1*4],v.y) + TOADLET_MUL_XX(m.data[1+2*4],v.z);
		r.z=TOADLET_MUL_XX(m.data[2+0*4],v.x) + TOADLET_MUL_XX(m.data[2+1*4],v.y) + TOADLET_MUL_XX(m.data[2+2*4],v.z);
	}

	public static void mul(Vector3 r,Matrix4x4 m){
		fixed tx=TOADLET_MUL_XX(m.data[0+0*4],r.x) + TOADLET_MUL_XX(m.data[0+1*4],r.y) + TOADLET_MUL_XX(m.data[0+2*4],r.z);
		fixed ty=TOADLET_MUL_XX(m.data[1+0*4],r.x) + TOADLET_MUL_XX(m.data[1+1*4],r.y) + TOADLET_MUL_XX(m.data[1+2*4],r.z);
		fixed tz=TOADLET_MUL_XX(m.data[2+0*4],r.x) + TOADLET_MUL_XX(m.data[2+1*4],r.y) + TOADLET_MUL_XX(m.data[2+2*4],r.z);
		r.x=tx;
		r.y=ty;
		r.z=tz;
	}

	public static void mulPoint3Fast(Vector3 r,Matrix4x4 m,Vector3 p){
		r.x=TOADLET_MUL_XX(m.data[0+0*4],p.x) + TOADLET_MUL_XX(m.data[0+1*4],p.y) + TOADLET_MUL_XX(m.data[0+2*4],p.z) + m.data[0+3*4];
		r.y=TOADLET_MUL_XX(m.data[1+0*4],p.x) + TOADLET_MUL_XX(m.data[1+1*4],p.y) + TOADLET_MUL_XX(m.data[1+2*4],p.z) + m.data[1+3*4];
		r.z=TOADLET_MUL_XX(m.data[2+0*4],p.x) + TOADLET_MUL_XX(m.data[2+1*4],p.y) + TOADLET_MUL_XX(m.data[2+2*4],p.z) + m.data[2+3*4];
	}

	public static void mulPoint3Fast(Vector3 r,Matrix4x4 m){
		fixed tx=TOADLET_MUL_XX(m.data[0+0*4],r.x) + TOADLET_MUL_XX(m.data[0+1*4],r.y) + TOADLET_MUL_XX(m.data[0+2*4],r.z) + m.data[0+3*4];
		fixed ty=TOADLET_MUL_XX(m.data[1+0*4],r.x) + TOADLET_MUL_XX(m.data[1+1*4],r.y) + TOADLET_MUL_XX(m.data[1+2*4],r.z) + m.data[1+3*4];
		fixed tz=TOADLET_MUL_XX(m.data[2+0*4],r.x) + TOADLET_MUL_XX(m.data[2+1*4],r.y) + TOADLET_MUL_XX(m.data[2+2*4],r.z) + m.data[2+3*4];
		r.x=tx;
		r.y=ty;
		r.z=tz;
	}

	public static void mulPoint3Full(Vector3 r,Matrix4x4 m,Vector3 p){
		r.x=TOADLET_MUL_XX(m.data[0+0*4],p.x) + TOADLET_MUL_XX(m.data[0+1*4],p.y) + TOADLET_MUL_XX(m.data[0+2*4],p.z) + m.data[0+3*4];
		r.y=TOADLET_MUL_XX(m.data[1+0*4],p.x) + TOADLET_MUL_XX(m.data[1+1*4],p.y) + TOADLET_MUL_XX(m.data[1+2*4],p.z) + m.data[1+3*4];
		r.z=TOADLET_MUL_XX(m.data[2+0*4],p.x) + TOADLET_MUL_XX(m.data[2+1*4],p.y) + TOADLET_MUL_XX(m.data[2+2*4],p.z) + m.data[2+3*4];
		fixed iw=TOADLET_DIV_XX(ONE,TOADLET_MUL_XX(m.data[3+0*4],p.x) + TOADLET_MUL_XX(m.data[3+1*4],p.y) + TOADLET_MUL_XX(m.data[3+2*4],p.z) + m.data[3+3*4]);
		r.x=TOADLET_MUL_XX(r.x,iw);
		r.y=TOADLET_MUL_XX(r.y,iw);
		r.z=TOADLET_MUL_XX(r.z,iw);
	}

	public static void mulPoint3Full(Vector3 r,Matrix4x4 m){
		fixed tx=TOADLET_MUL_XX(m.data[0+0*4],r.x) + TOADLET_MUL_XX(m.data[0+1*4],r.y) + TOADLET_MUL_XX(m.data[0+2*4],r.z) + m.data[0+3*4];
		fixed ty=TOADLET_MUL_XX(m.data[1+0*4],r.x) + TOADLET_MUL_XX(m.data[1+1*4],r.y) + TOADLET_MUL_XX(m.data[1+2*4],r.z) + m.data[1+3*4];
		fixed tz=TOADLET_MUL_XX(m.data[2+0*4],r.x) + TOADLET_MUL_XX(m.data[2+1*4],r.y) + TOADLET_MUL_XX(m.data[2+2*4],r.z) + m.data[2+3*4];
		fixed iw=TOADLET_DIV_XX(ONE,TOADLET_MUL_XX(m.data[3+0*4],r.x) + TOADLET_MUL_XX(m.data[3+1*4],r.y) + TOADLET_MUL_XX(m.data[3+2*4],r.z) + m.data[3+3*4]);
		r.x=TOADLET_MUL_XX(tx,iw);
		r.y=TOADLET_MUL_XX(ty,iw);
		r.z=TOADLET_MUL_XX(tz,iw);
	}

	public static void transpose(Matrix4x4 r,Matrix4x4 m){
		r.data[0+0*4]=m.data[0+0*4]; r.data[0+1*4]=m.data[1+0*4]; r.data[0+2*4]=m.data[2+0*4]; r.data[0+3*4]=m.data[3+0*4];
		r.data[1+0*4]=m.data[0+1*4]; r.data[1+1*4]=m.data[1+1*4]; r.data[1+2*4]=m.data[2+1*4]; r.data[1+3*4]=m.data[3+1*4];
		r.data[2+0*4]=m.data[0+2*4]; r.data[2+1*4]=m.data[1+2*4]; r.data[2+2*4]=m.data[2+2*4]; r.data[2+3*4]=m.data[3+2*4];
		r.data[3+0*4]=m.data[0+3*4]; r.data[3+1*4]=m.data[1+3*4]; r.data[3+2*4]=m.data[2+3*4]; r.data[3+3*4]=m.data[3+3*4];
	}

	public static fixed determinant(Matrix4x4 m){
		fixed det1 = TOADLET_MUL_XX(m.data[1+2*4],m.data[2+3*4]) - TOADLET_MUL_XX(m.data[2+2*4],m.data[1+3*4]);
		fixed det2 = TOADLET_MUL_XX(m.data[1+1*4],m.data[2+3*4]) - TOADLET_MUL_XX(m.data[2+1*4],m.data[1+3*4]);
		fixed det3 = TOADLET_MUL_XX(m.data[1+1*4],m.data[2+2*4]) - TOADLET_MUL_XX(m.data[2+1*4],m.data[1+2*4]);
		fixed det4 = TOADLET_MUL_XX(m.data[1+0*4],m.data[2+3*4]) - TOADLET_MUL_XX(m.data[2+0*4],m.data[1+3*4]);
		fixed det5 = TOADLET_MUL_XX(m.data[1+0*4],m.data[2+2*4]) - TOADLET_MUL_XX(m.data[2+0*4],m.data[1+2*4]);
		fixed det6 = TOADLET_MUL_XX(m.data[1+0*4],m.data[2+1*4]) - TOADLET_MUL_XX(m.data[2+0*4],m.data[1+1*4]);

		return -TOADLET_MUL_XX(m.data[3+0*4],TOADLET_MUL_XX(m.data[0+1*4],det1) - TOADLET_MUL_XX(m.data[0+2*4],det2) + TOADLET_MUL_XX(m.data[0+3*4],det3)) +
				TOADLET_MUL_XX(m.data[3+1*4],TOADLET_MUL_XX(m.data[0+0*4],det1) - TOADLET_MUL_XX(m.data[0+2*4],det4) + TOADLET_MUL_XX(m.data[0+3*4],det5)) -
				TOADLET_MUL_XX(m.data[3+2*4],TOADLET_MUL_XX(m.data[0+0*4],det2) - TOADLET_MUL_XX(m.data[0+1*4],det4) + TOADLET_MUL_XX(m.data[0+3*4],det6)) +
				TOADLET_MUL_XX(m.data[3+3*4],TOADLET_MUL_XX(m.data[0+0*4],det3) - TOADLET_MUL_XX(m.data[0+1*4],det5) + TOADLET_MUL_XX(m.data[0+2*4],det6));
	}

	public static boolean invert(Matrix4x4 r,Matrix4x4 m){
		fixed det=determinant(m);

		if(abs(det)==0){
			return false;
		}
		else{
			det=TOADLET_DIV_XX(ONE,det);
			r.data[0+0*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+1*4],(TOADLET_MUL_XX(m.data[2+2*4],m.data[3+3*4]) - TOADLET_MUL_XX(m.data[2+3*4],m.data[3+2*4]))) + TOADLET_MUL_XX(m.data[1+2*4],(TOADLET_MUL_XX(m.data[2+3*4],m.data[3+1*4]) - TOADLET_MUL_XX(m.data[2+1*4],m.data[3+3*4]))) + TOADLET_MUL_XX(m.data[1+3*4],(TOADLET_MUL_XX(m.data[2+1*4],m.data[3+2*4]) - TOADLET_MUL_XX(m.data[2+2*4],m.data[3+1*4]))));
			r.data[0+1*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[2+1*4],(TOADLET_MUL_XX(m.data[0+2*4],m.data[3+3*4]) - TOADLET_MUL_XX(m.data[0+3*4],m.data[3+2*4]))) + TOADLET_MUL_XX(m.data[2+2*4],(TOADLET_MUL_XX(m.data[0+3*4],m.data[3+1*4]) - TOADLET_MUL_XX(m.data[0+1*4],m.data[3+3*4]))) + TOADLET_MUL_XX(m.data[2+3*4],(TOADLET_MUL_XX(m.data[0+1*4],m.data[3+2*4]) - TOADLET_MUL_XX(m.data[0+2*4],m.data[3+1*4]))));
			r.data[0+2*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[3+1*4],(TOADLET_MUL_XX(m.data[0+2*4],m.data[1+3*4]) - TOADLET_MUL_XX(m.data[0+3*4],m.data[1+2*4]))) + TOADLET_MUL_XX(m.data[3+2*4],(TOADLET_MUL_XX(m.data[0+3*4],m.data[1+1*4]) - TOADLET_MUL_XX(m.data[0+1*4],m.data[1+3*4]))) + TOADLET_MUL_XX(m.data[3+3*4],(TOADLET_MUL_XX(m.data[0+1*4],m.data[1+2*4]) - TOADLET_MUL_XX(m.data[0+2*4],m.data[1+1*4]))));
			r.data[0+3*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+1*4],(TOADLET_MUL_XX(m.data[1+3*4],m.data[2+2*4]) - TOADLET_MUL_XX(m.data[1+2*4],m.data[2+3*4]))) + TOADLET_MUL_XX(m.data[0+2*4],(TOADLET_MUL_XX(m.data[1+1*4],m.data[2+3*4]) - TOADLET_MUL_XX(m.data[1+3*4],m.data[2+1*4]))) + TOADLET_MUL_XX(m.data[0+3*4],(TOADLET_MUL_XX(m.data[1+2*4],m.data[2+1*4]) - TOADLET_MUL_XX(m.data[1+1*4],m.data[2+2*4]))));

			r.data[1+0*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+2*4],(TOADLET_MUL_XX(m.data[2+0*4],m.data[3+3*4]) - TOADLET_MUL_XX(m.data[2+3*4],m.data[3+0*4]))) + TOADLET_MUL_XX(m.data[1+3*4],(TOADLET_MUL_XX(m.data[2+2*4],m.data[3+0*4]) - TOADLET_MUL_XX(m.data[2+0*4],m.data[3+2*4]))) + TOADLET_MUL_XX(m.data[1+0*4],(TOADLET_MUL_XX(m.data[2+3*4],m.data[3+2*4]) - TOADLET_MUL_XX(m.data[2+2*4],m.data[3+3*4]))));
			r.data[1+1*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[2+2*4],(TOADLET_MUL_XX(m.data[0+0*4],m.data[3+3*4]) - TOADLET_MUL_XX(m.data[0+3*4],m.data[3+0*4]))) + TOADLET_MUL_XX(m.data[2+3*4],(TOADLET_MUL_XX(m.data[0+2*4],m.data[3+0*4]) - TOADLET_MUL_XX(m.data[0+0*4],m.data[3+2*4]))) + TOADLET_MUL_XX(m.data[2+0*4],(TOADLET_MUL_XX(m.data[0+3*4],m.data[3+2*4]) - TOADLET_MUL_XX(m.data[0+2*4],m.data[3+3*4]))));
			r.data[1+2*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[3+2*4],(TOADLET_MUL_XX(m.data[0+0*4],m.data[1+3*4]) - TOADLET_MUL_XX(m.data[0+3*4],m.data[1+0*4]))) + TOADLET_MUL_XX(m.data[3+3*4],(TOADLET_MUL_XX(m.data[0+2*4],m.data[1+0*4]) - TOADLET_MUL_XX(m.data[0+0*4],m.data[1+2*4]))) + TOADLET_MUL_XX(m.data[3+0*4],(TOADLET_MUL_XX(m.data[0+3*4],m.data[1+2*4]) - TOADLET_MUL_XX(m.data[0+2*4],m.data[1+3*4]))));
			r.data[1+3*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+2*4],(TOADLET_MUL_XX(m.data[1+3*4],m.data[2+0*4]) - TOADLET_MUL_XX(m.data[1+0*4],m.data[2+3*4]))) + TOADLET_MUL_XX(m.data[0+3*4],(TOADLET_MUL_XX(m.data[1+0*4],m.data[2+2*4]) - TOADLET_MUL_XX(m.data[1+2*4],m.data[2+0*4]))) + TOADLET_MUL_XX(m.data[0+0*4],(TOADLET_MUL_XX(m.data[1+2*4],m.data[2+3*4]) - TOADLET_MUL_XX(m.data[1+3*4],m.data[2+2*4]))));

			r.data[2+0*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+3*4],(TOADLET_MUL_XX(m.data[2+0*4],m.data[3+1*4]) - TOADLET_MUL_XX(m.data[2+1*4],m.data[3+0*4]))) + TOADLET_MUL_XX(m.data[1+0*4],(TOADLET_MUL_XX(m.data[2+1*4],m.data[3+3*4]) - TOADLET_MUL_XX(m.data[2+3*4],m.data[3+1*4]))) + TOADLET_MUL_XX(m.data[1+1*4],(TOADLET_MUL_XX(m.data[2+3*4],m.data[3+0*4]) - TOADLET_MUL_XX(m.data[2+0*4],m.data[3+3*4]))));
			r.data[2+1*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[2+3*4],(TOADLET_MUL_XX(m.data[0+0*4],m.data[3+1*4]) - TOADLET_MUL_XX(m.data[0+1*4],m.data[3+0*4]))) + TOADLET_MUL_XX(m.data[2+0*4],(TOADLET_MUL_XX(m.data[0+1*4],m.data[3+3*4]) - TOADLET_MUL_XX(m.data[0+3*4],m.data[3+1*4]))) + TOADLET_MUL_XX(m.data[2+1*4],(TOADLET_MUL_XX(m.data[0+3*4],m.data[3+0*4]) - TOADLET_MUL_XX(m.data[0+0*4],m.data[3+3*4]))));
			r.data[2+2*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[3+3*4],(TOADLET_MUL_XX(m.data[0+0*4],m.data[1+1*4]) - TOADLET_MUL_XX(m.data[0+1*4],m.data[1+0*4]))) + TOADLET_MUL_XX(m.data[3+0*4],(TOADLET_MUL_XX(m.data[0+1*4],m.data[1+3*4]) - TOADLET_MUL_XX(m.data[0+3*4],m.data[1+1*4]))) + TOADLET_MUL_XX(m.data[3+1*4],(TOADLET_MUL_XX(m.data[0+3*4],m.data[1+0*4]) - TOADLET_MUL_XX(m.data[0+0*4],m.data[1+3*4]))));
			r.data[2+3*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+3*4],(TOADLET_MUL_XX(m.data[1+1*4],m.data[2+0*4]) - TOADLET_MUL_XX(m.data[1+0*4],m.data[2+1*4]))) + TOADLET_MUL_XX(m.data[0+0*4],(TOADLET_MUL_XX(m.data[1+3*4],m.data[2+1*4]) - TOADLET_MUL_XX(m.data[1+1*4],m.data[2+3*4]))) + TOADLET_MUL_XX(m.data[0+1*4],(TOADLET_MUL_XX(m.data[1+0*4],m.data[2+3*4]) - TOADLET_MUL_XX(m.data[1+3*4],m.data[2+0*4]))));

			r.data[3+0*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[1+0*4],(TOADLET_MUL_XX(m.data[2+2*4],m.data[3+1*4]) - TOADLET_MUL_XX(m.data[2+1*4],m.data[3+2*4]))) + TOADLET_MUL_XX(m.data[1+1*4],(TOADLET_MUL_XX(m.data[2+0*4],m.data[3+2*4]) - TOADLET_MUL_XX(m.data[2+2*4],m.data[3+0*4]))) + TOADLET_MUL_XX(m.data[1+2*4],(TOADLET_MUL_XX(m.data[2+1*4],m.data[3+0*4]) - TOADLET_MUL_XX(m.data[2+0*4],m.data[3+1*4]))));
			r.data[3+1*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[2+0*4],(TOADLET_MUL_XX(m.data[0+2*4],m.data[3+1*4]) - TOADLET_MUL_XX(m.data[0+1*4],m.data[3+2*4]))) + TOADLET_MUL_XX(m.data[2+1*4],(TOADLET_MUL_XX(m.data[0+0*4],m.data[3+2*4]) - TOADLET_MUL_XX(m.data[0+2*4],m.data[3+0*4]))) + TOADLET_MUL_XX(m.data[2+2*4],(TOADLET_MUL_XX(m.data[0+1*4],m.data[3+0*4]) - TOADLET_MUL_XX(m.data[0+0*4],m.data[3+1*4]))));
			r.data[3+2*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[3+0*4],(TOADLET_MUL_XX(m.data[0+2*4],m.data[1+1*4]) - TOADLET_MUL_XX(m.data[0+1*4],m.data[1+2*4]))) + TOADLET_MUL_XX(m.data[3+1*4],(TOADLET_MUL_XX(m.data[0+0*4],m.data[1+2*4]) - TOADLET_MUL_XX(m.data[0+2*4],m.data[1+0*4]))) + TOADLET_MUL_XX(m.data[3+2*4],(TOADLET_MUL_XX(m.data[0+1*4],m.data[1+0*4]) - TOADLET_MUL_XX(m.data[0+0*4],m.data[1+1*4]))));
			r.data[3+3*4] = TOADLET_MUL_XX(det,TOADLET_MUL_XX(m.data[0+0*4],(TOADLET_MUL_XX(m.data[1+1*4],m.data[2+2*4]) - TOADLET_MUL_XX(m.data[1+2*4],m.data[2+1*4]))) + TOADLET_MUL_XX(m.data[0+1*4],(TOADLET_MUL_XX(m.data[1+2*4],m.data[2+0*4]) - TOADLET_MUL_XX(m.data[1+0*4],m.data[2+2*4]))) + TOADLET_MUL_XX(m.data[0+2*4],(TOADLET_MUL_XX(m.data[1+0*4],m.data[2+1*4]) - TOADLET_MUL_XX(m.data[1+1*4],m.data[2+0*4]))));

			return true;
		}
	}

	// Generic Matrix operations
	#define setAxesFromMatrix(m,xAxis,yAxis,zAxis) \
		xAxis.x=m.at(0,0); yAxis.x=m.at(0,1); zAxis.x=m.at(0,2); \
		xAxis.y=m.at(1,0); yAxis.y=m.at(1,1); zAxis.y=m.at(1,2); \
		xAxis.z=m.at(2,0); yAxis.z=m.at(2,1); zAxis.z=m.at(2,2);

	#define setMatrixFromAxes(m,xAxis,yAxis,zAxis) \
		m.setAt(0,0,xAxis.x); m.setAt(0,1,yAxis.x); m.setAt(0,2,zAxis.x); \
		m.setAt(1,0,xAxis.y); m.setAt(1,1,yAxis.y); m.setAt(1,2,zAxis.y); \
		m.setAt(2,0,xAxis.z); m.setAt(2,1,yAxis.z); m.setAt(2,2,zAxis.z);

	#define setMatrixFromX(r,x) \
		fixed cx=cos(x); \
		fixed sx=sin(x); \
		\
		r.setAt(0,0,ONE);	r.setAt(0,1,0);		r.setAt(0,2,0); \
		r.setAt(1,0,0);		r.setAt(1,1,cx);	r.setAt(1,2,-sx); \
		r.setAt(2,0,0);		r.setAt(2,1,sx);	r.setAt(2,2,cx);

	#define setMatrixFromY(r,y) \
		fixed cy=cos(y); \
		fixed sy=sin(y); \
		\
		r.setAt(0,0,cy);	r.setAt(0,1,0);		r.setAt(0,2,sy); \
		r.setAt(1,0,0);		r.setAt(1,1,ONE);	r.setAt(1,2,0); \
		r.setAt(2,0,-sy);	r.setAt(2,1,0);		r.setAt(2,2,cy);

	#define setMatrixFromZ(r,z) \
		fixed cz=cos(z); \
		fixed sz=sin(z); \
		\
		r.setAt(0,0,cz);	r.setAt(0,1,-sz);	r.setAt(0,2,0); \
		r.setAt(1,0,sz);	r.setAt(1,1,cz);	r.setAt(1,2,0); \
		r.setAt(2,0,0);		r.setAt(2,1,0);		r.setAt(2,2,ONE);

	#define setMatrixFromAxisAngle(r,axis,angle) \
		fixed c=cos(angle); \
		fixed s=sin(angle); \
		fixed t=ONE-c; \
		r.setAt(0,0,c + TOADLET_MUL_XX(axis.x,TOADLET_MUL_XX(axis.x,t))); \
		r.setAt(1,1,c + TOADLET_MUL_XX(axis.y,TOADLET_MUL_XX(axis.y,t))); \
		r.setAt(2,2,c + TOADLET_MUL_XX(axis.z,TOADLET_MUL_XX(axis.z,t))); \
		\
		fixed tmp1=TOADLET_MUL_XX(axis.x,TOADLET_MUL_XX(axis.y,t)); \
		fixed tmp2=TOADLET_MUL_XX(axis.z,s); \
		r.setAt(1,0,tmp1+tmp2); \
		r.setAt(0,1,tmp1-tmp2); \
		\
		tmp1=TOADLET_MUL_XX(axis.x,TOADLET_MUL_XX(axis.z,t)); \
		tmp2=TOADLET_MUL_XX(axis.y,s); \
		r.setAt(2,0,tmp1-tmp2); \
		r.setAt(0,2,tmp1+tmp2); \
		\
		tmp1=TOADLET_MUL_XX(axis.y,TOADLET_MUL_XX(axis.z,t)); \
		tmp2=TOADLET_MUL_XX(axis.x,s); \
		r.setAt(2,1,tmp1+tmp2); \
		r.setAt(1,2,tmp1-tmp2);

	#define setMatrixFromQuaternion(r,q) \
		fixed tx  = q.x<<1; \
		fixed ty  = q.y<<1; \
		fixed tz  = q.z<<1; \
		fixed twx = TOADLET_MUL_XX(tx,q.w); \
		fixed twy = TOADLET_MUL_XX(ty,q.w); \
		fixed twz = TOADLET_MUL_XX(tz,q.w); \
		fixed txx = TOADLET_MUL_XX(tx,q.x); \
		fixed txy = TOADLET_MUL_XX(ty,q.x); \
		fixed txz = TOADLET_MUL_XX(tz,q.x); \
		fixed tyy = TOADLET_MUL_XX(ty,q.y); \
		fixed tyz = TOADLET_MUL_XX(tz,q.y); \
		fixed tzz = TOADLET_MUL_XX(tz,q.z); \
		\
		r.setAt(0,0,ONE-(tyy+tzz));	r.setAt(0,1,txy-twz);		r.setAt(0,2,txz+twy); \
		r.setAt(1,0,txy+twz);		r.setAt(1,1,ONE-(txx+tzz));	r.setAt(1,2,tyz-twx); \
		r.setAt(2,0,txz-twy);		r.setAt(2,1,tyz+twx);		r.setAt(2,2,ONE-(txx+tyy));

	// Algorithm from Tomas Moller, 1999
	#define setMatrixFromVector3ToVector3(r,from,to,epsilon) \
		Vector3 v=new Vector3(); \
		cross(v,from,to); \
		fixed e=dot(from,to); \
		\
		if(e>ONE-epsilon){ \
			r.setAt(0,0,ONE);	r.setAt(0,1,0);		r.setAt(0,2,0); \
			r.setAt(1,0,0);		r.setAt(1,1,ONE);	r.setAt(1,2,0); \
			r.setAt(2,0,0);		r.setAt(2,1,0);		r.setAt(2,2,ONE); \
		} \
		else if(e<-ONE+epsilon){ \
			Vector3 up=new Vector3(),left=new Vector3(); \
			fixed fxx,fyy,fzz,fxy,fxz,fyz; \
			fixed uxx,uyy,uzz,uxy,uxz,uyz; \
			fixed lxx,lyy,lzz,lxy,lxz,lyz; \
			\
			left.x=0;left.y=from.z;left.z=-from.y; \
			if(dot(left,left)<epsilon){ \
				left.x=-from.z;left.y=0;left.z=from.x; \
			} \
			\
			normalizeCarefully(left,0); \
			cross(up,left,from); \
			\
			fxx=mul(-from.x,from.x); fyy=mul(-from.y,from.y); fzz=mul(-from.z,from.z); \
			fxy=mul(-from.x,from.y); fxz=mul(-from.x,from.z); fyz=mul(-from.y,from.z); \
			\
			uxx=mul(up.x,up.x); uyy=mul(up.y,up.y); uzz=mul(up.z,up.z); \
			uxy=mul(up.x,up.y); uxz=mul(up.x,up.z); uyz=mul(up.y,up.z); \
			\
			lxx=mul(-left.y,left.x); lyy=mul(-left.y,left.y); lzz=mul(-left.z,left.z); \
			lxy=mul(-left.x,left.y); lxz=mul(-left.x,left.z); lyz=mul(-left.y,left.z); \
			\
			r.setAt(0,0,fxx+uxx+lxx);	r.setAt(0,1,r.at(1,0));		r.setAt(0,2,r.at(2,0)); \
			r.setAt(1,0,fxy+uxy+lxy);	r.setAt(1,1,fyy+uyy+lyy);	r.setAt(1,2,r.at(2,1)); \
			r.setAt(2,0,fxz+uxz+lxz);	r.setAt(2,1,fyz+uyz+lyz);	r.setAt(2,2,fzz+uzz+lzz); \
		} \
		else{ \
			fixed hvx,hvz,hvxy,hvxz,hvyz; \
			fixed h=div(ONE-e,dot(v,v)); \
			hvx=mul(h,v.x); \
			hvz=mul(h,v.z); \
			hvxy=mul(hvx,v.y); \
			hvxz=mul(hvx,v.z); \
			hvyz=mul(hvz,v.y); \
			r.setAt(0,0,e+mul(hvx,v.x));	r.setAt(0,1,hvxy+v.z);				 	r.setAt(0,2,hvxz-v.y); \
			r.setAt(1,0,hvxy-v.z);			r.setAt(1,1,e+mul(h,mul(v.y,v.y)));		r.setAt(1,2,hvyz+v.x); \
			r.setAt(2,0,hvxz+v.y);			r.setAt(2,1,hvyz-v.x);					r.setAt(2,2,e+mul(hvz,v.z)); \
		}

	#define setMatrixFromEulerAngleXYZ(Type,r,euler) \
		Type xmat=new Type(),ymat=new Type(),zmat=new Type(); \
		setMatrixFromX(xmat,euler.x); \
		setMatrixFromY(ymat,euler.y); \
		setMatrixFromZ(zmat,euler.z); \
		postMul(ymat,zmat); \
		mul(r,xmat,ymat);

	// Matrix3x3 advanced operations
	public static void setAxesFromMatrix3x3(Matrix3x3 m,Vector3 xAxis,Vector3 yAxis,Vector3 zAxis){ setAxesFromMatrix(m,xAxis,yAxis,zAxis); }

	public static void setMatrix3x3FromAxes(Matrix3x3 m,Vector3 xAxis,Vector3 yAxis,Vector3 zAxis){ setMatrixFromAxes(m,xAxis,yAxis,zAxis); }

	public static void setMatrix3x3FromX(Matrix3x3 r,fixed x){ setMatrixFromX(r,x); }

	public static void setMatrix3x3FromY(Matrix3x3 r,fixed y){ setMatrixFromY(r,y); }

	public static void setMatrix3x3FromZ(Matrix3x3 r,fixed z){ setMatrixFromZ(r,z); }

	public static void setMatrix3x3FromAxisAngle(Matrix3x3 r,Vector3 axis,fixed angle){ setMatrixFromAxisAngle(r,axis,angle); }

	public static void setMatrix3x3FromQuaternion(Matrix3x3 r,Quaternion q){ setMatrixFromQuaternion(r,q); }

	public static void setMatrix3x3FromEulerAngleXYZ(Matrix3x3 r,EulerAngle euler){ setMatrixFromEulerAngleXYZ(Matrix3x3,r,euler); }

	public static void setMatrix3x3FromVector3ToVector3(Matrix3x3 r,Vector3 from,Vector3 to,fixed epsilon){ setMatrixFromVector3ToVector3(r,from,to,epsilon); }

	public static void setMatrix3x3FromMatrix4x4(Matrix3x3 r,Matrix4x4 m){
		r.data[0+0*3]=m.data[0+0*4]; r.data[1+0*3]=m.data[1+0*4]; r.data[2+0*3]=m.data[2+0*4];
		r.data[0+1*3]=m.data[0+1*4]; r.data[1+1*3]=m.data[1+1*4]; r.data[2+1*3]=m.data[2+1*4];
		r.data[0+2*3]=m.data[0+2*4]; r.data[1+2*3]=m.data[1+2*4]; r.data[2+2*3]=m.data[2+2*4];
	}

	// Matrix4x4 advanced operations
	public static void setAxesFromMatrix4x4(Matrix4x4 m,Vector3 xAxis,Vector3 yAxis,Vector3 zAxis){ setAxesFromMatrix(m,xAxis,yAxis,zAxis); }

	public static void setMatrix4x4FromAxes(Matrix4x4 m,Vector3 xAxis,Vector3 yAxis,Vector3 zAxis){ setMatrixFromAxes(m,xAxis,yAxis,zAxis); }

	public static void setMatrix4x4FromX(Matrix4x4 r,fixed x){ setMatrixFromX(r,x); }

	public static void setMatrix4x4FromY(Matrix4x4 r,fixed y){ setMatrixFromY(r,y); }

	public static void setMatrix4x4FromZ(Matrix4x4 r,fixed z){ setMatrixFromZ(r,z); }

	public static void setMatrix4x4FromAxisAngle(Matrix4x4 r,Vector3 axis,fixed angle){ setMatrixFromAxisAngle(r,axis,angle); }

	public static void setMatrix4x4FromQuaternion(Matrix4x4 r,Quaternion q){ setMatrixFromQuaternion(r,q); }

	public static void setMatrix4x4FromEulerAngleXYZ(Matrix4x4 r,EulerAngle euler){ setMatrixFromEulerAngleXYZ(Matrix4x4,r,euler); }

	public static void setMatrix4x4FromVector3ToVector3(Matrix4x4 r,Vector3 from,Vector3 to,fixed epsilon){ setMatrixFromVector3ToVector3(r,from,to,epsilon); }

	public static void setMatrix4x4FromMatrix3x3(Matrix4x4 r,Matrix3x3 m){
		r.data[0+0*4]=m.data[0+0*3]; r.data[1+0*4]=m.data[1+0*3]; r.data[2+0*4]=m.data[2+0*3];
		r.data[0+1*4]=m.data[0+1*3]; r.data[1+1*4]=m.data[1+1*3]; r.data[2+1*4]=m.data[2+1*3];
		r.data[0+2*4]=m.data[0+2*3]; r.data[1+2*4]=m.data[1+2*3]; r.data[2+2*4]=m.data[2+2*3];
	}

	public static void setMatrix4x4FromTranslate(Matrix4x4 r,fixed x,fixed y,fixed z){
		r.data[0+3*4]=x;
		r.data[1+3*4]=y;
		r.data[2+3*4]=z;
	}

	public static void setMatrix4x4FromTranslate(Matrix4x4 r,Vector3 translate){
		r.data[0+3*4]=translate.x;
		r.data[1+3*4]=translate.y;
		r.data[2+3*4]=translate.z;
	}

	public static void setMatrix4x4FromScale(Matrix4x4 r,fixed x,fixed y,fixed z){
		r.data[0+0*4]=x;
		r.data[1+1*4]=y;
		r.data[2+2*4]=z;
	}

	public static void setMatrix4x4FromScale(Matrix4x4 r,Vector3 scale){
		r.data[0+0*4]=scale.x;
		r.data[1+1*4]=scale.y;
		r.data[2+2*4]=scale.z;
	}

	public static void setMatrix4x4FromRotateScale(Matrix4x4 r,Matrix3x3 rotate,Vector3 scale){
		r.data[0+0*4]=TOADLET_MUL_XX(rotate.data[0+0*3],scale.x);
		r.data[1+0*4]=TOADLET_MUL_XX(rotate.data[1+0*3],scale.x);
		r.data[2+0*4]=TOADLET_MUL_XX(rotate.data[2+0*3],scale.x);
		r.data[0+1*4]=TOADLET_MUL_XX(rotate.data[0+1*3],scale.y);
		r.data[1+1*4]=TOADLET_MUL_XX(rotate.data[1+1*3],scale.y);
		r.data[2+1*4]=TOADLET_MUL_XX(rotate.data[2+1*3],scale.y);
		r.data[0+2*4]=TOADLET_MUL_XX(rotate.data[0+2*3],scale.z);
		r.data[1+2*4]=TOADLET_MUL_XX(rotate.data[1+2*3],scale.z);
		r.data[2+2*4]=TOADLET_MUL_XX(rotate.data[2+2*3],scale.z);
	}

	public static void setMatrix4x4FromTranslateRotateScale(Matrix4x4 r,Vector3 translate,Matrix3x3 rotate,Vector3 scale){
		r.data[0+3*4]=translate.x;
		r.data[1+3*4]=translate.y;
		r.data[2+3*4]=translate.z;
		r.data[0+0*4]=TOADLET_MUL_XX(rotate.data[0+0*3],scale.x);
		r.data[1+0*4]=TOADLET_MUL_XX(rotate.data[1+0*3],scale.x);
		r.data[2+0*4]=TOADLET_MUL_XX(rotate.data[2+0*3],scale.x);
		r.data[0+1*4]=TOADLET_MUL_XX(rotate.data[0+1*3],scale.y);
		r.data[1+1*4]=TOADLET_MUL_XX(rotate.data[1+1*3],scale.y);
		r.data[2+1*4]=TOADLET_MUL_XX(rotate.data[2+1*3],scale.y);
		r.data[0+2*4]=TOADLET_MUL_XX(rotate.data[0+2*3],scale.z);
		r.data[1+2*4]=TOADLET_MUL_XX(rotate.data[1+2*3],scale.z);
		r.data[2+2*4]=TOADLET_MUL_XX(rotate.data[2+2*3],scale.z);
	}

	public static void setMatrix4x4AsTextureRotation(Matrix4x4 r){
		r.data[0+3*4]=TOADLET_MUL_XX(r.data[0+0*4],-HALF) + TOADLET_MUL_XX(r.data[0+1*4],-HALF) + HALF;
		r.data[1+3*4]=TOADLET_MUL_XX(r.data[1+0*4],-HALF) + TOADLET_MUL_XX(r.data[1+1*4],-HALF) + HALF;
		r.data[2+3*4]=0;
	}

	public static void setMatrix4x4FromLookAt(Matrix4x4 r,Vector3 eye,Vector3 point,Vector3 up,boolean camera){
		Vector3 cacheDir=new Vector3();

		sub(cacheDir,point,eye);
		setMatrix4x4FromLookDir(r,eye,cacheDir,up,camera);
	}

	public static void setMatrix4x4FromLookDir(Matrix4x4 r,Vector3 eye,Vector3 dir,Vector3 up,boolean camera){
		Vector3 cacheDir=new Vector3();
		Vector3 cacheUp=new Vector3();
		Vector3 cacheRight=new Vector3();

		normalizeCarefully(cacheDir,dir,0);
		normalizeCarefully(cacheUp,up,0);
		cross(cacheRight,cacheDir,cacheUp);
		normalizeCarefully(cacheRight,0);
		cross(cacheUp,cacheRight,cacheDir);

		if(camera){
			r.setAt(0,0,cacheRight.x);
			r.setAt(1,0,cacheRight.y);
			r.setAt(2,0,cacheRight.z);
			r.setAt(0,1,cacheUp.x);
			r.setAt(1,1,cacheUp.y);
			r.setAt(2,1,cacheUp.z);
			r.setAt(0,2,-cacheDir.x);
			r.setAt(1,2,-cacheDir.y);
			r.setAt(2,2,-cacheDir.z);
			r.setAt(0,3,eye.x);
			r.setAt(1,3,eye.y);
			r.setAt(2,3,eye.z);
		}
		else{
			Matrix3x3 rotate=new Matrix3x3();
			rotate.setAt(0,0,cacheRight.x);
			rotate.setAt(0,1,cacheRight.y);
			rotate.setAt(0,2,cacheRight.z);
			rotate.setAt(1,0,cacheUp.x);
			rotate.setAt(1,1,cacheUp.y);
			rotate.setAt(1,2,cacheUp.z);
			rotate.setAt(2,0,-cacheDir.x);
			rotate.setAt(2,1,-cacheDir.y);
			rotate.setAt(2,2,-cacheDir.z);

			Vector3 reye=new Vector3();
			mul(reye,rotate,eye);

			r.setAt(0,0,cacheRight.x);
			r.setAt(0,1,cacheRight.y);
			r.setAt(0,2,cacheRight.z);
			r.setAt(1,0,cacheUp.x);
			r.setAt(1,1,cacheUp.y);
			r.setAt(1,2,cacheUp.z);
			r.setAt(2,0,-cacheDir.x);
			r.setAt(2,1,-cacheDir.y);
			r.setAt(2,2,-cacheDir.z);
			r.setAt(0,3,-reye.x);
			r.setAt(1,3,-reye.y);
			r.setAt(2,3,-reye.z);
		}
	}

	public static void setMatrix4x4FromOrtho(Matrix4x4 r,fixed leftDist,fixed rightDist,fixed bottomDist,fixed topDist,fixed nearDist,fixed farDist){
		r.set(
			div(TWO,rightDist-leftDist),0,0,-div(rightDist+leftDist,rightDist-leftDist),
			0,div(TWO,topDist-bottomDist),0,-div(topDist+bottomDist,topDist-bottomDist),
			0,0,-div(TWO,farDist-nearDist),-div(farDist+nearDist,farDist-nearDist),
			0,0,0,ONE
		);
	}

	public static void setMatrix4x4FromFrustum(Matrix4x4 r,fixed leftDist,fixed rightDist,fixed bottomDist,fixed topDist,fixed nearDist,fixed farDist){
		r.set(
			div(nearDist<<1,rightDist-leftDist),0,div(rightDist+leftDist,rightDist-leftDist),0,
			0,div(nearDist<<1,topDist-bottomDist),div(topDist+bottomDist,topDist-bottomDist),0,
			0,0,-div(farDist+nearDist,farDist-nearDist),-div(mul(farDist,nearDist)<<1,farDist-nearDist),
			0,0,-ONE,0
		);
	}

	public static void setMatrix4x4FromPerspectiveX(Matrix4x4 r,fixed fovx,fixed aspect,fixed nearDist,fixed farDist){
		fixed h=tan(fovx>>1);

		r.set(
			div(ONE,h),0,0,0,
			0,div(ONE,mul(h,aspect)),0,0,
			0,0,div(farDist+nearDist,nearDist-farDist),div(mul(farDist,nearDist)<<1,nearDist-farDist),
			0,0,-ONE,0
		);
	}

	public static void setMatrix4x4FromPerspectiveY(Matrix4x4 r,fixed fovy,fixed aspect,fixed nearDist,fixed farDist){
		fixed h=tan(fovy>>1);

		r.set(
			div(ONE,mul(h,aspect)),0,0,0,
			0,div(ONE,h),0,0,
			0,0,div(farDist+nearDist,nearDist-farDist),div(mul(farDist,nearDist)<<1,nearDist-farDist),
			0,0,-ONE,0
		);
	}

	// Axis Angle operations
	public static fixed setAxisAngleFromQuaternion(Vector3 axis,Quaternion q,fixed epsilon){
		fixed angle=acos(q.w)<<1;

		fixed sinHalfAngle=sin(angle>>1);
		if(sinHalfAngle>epsilon){
			fixed invSinHalfAngle=div(ONE,sinHalfAngle);
			axis.x=mul(q.x,invSinHalfAngle);
			axis.y=mul(q.y,invSinHalfAngle);
			axis.z=mul(q.z,invSinHalfAngle);

			fixed l=sqrt((mul(axis.x,axis.x)+mul(axis.y,axis.y)+mul(axis.z,axis.z)));
			axis.x=div(axis.x,l);
			axis.y=div(axis.y,l);
			axis.z=div(axis.z,l);
		}
		else{
			axis.x=ONE;
			axis.y=0;
			axis.z=0;
		}
		
		return angle;
	}

	// Quaternion operations
	public static void mul(Quaternion r,Quaternion q1,Quaternion q2){
		r.x=TOADLET_MUL_XX(+q1.x,q2.w)+TOADLET_MUL_XX(q1.y,q2.z)-TOADLET_MUL_XX(q1.z,q2.y)+TOADLET_MUL_XX(q1.w,q2.x);
		r.y=TOADLET_MUL_XX(-q1.x,q2.z)+TOADLET_MUL_XX(q1.y,q2.w)+TOADLET_MUL_XX(q1.z,q2.x)+TOADLET_MUL_XX(q1.w,q2.y);
		r.z=TOADLET_MUL_XX(+q1.x,q2.y)-TOADLET_MUL_XX(q1.y,q2.x)+TOADLET_MUL_XX(q1.z,q2.w)+TOADLET_MUL_XX(q1.w,q2.z);
		r.w=TOADLET_MUL_XX(-q1.x,q2.x)-TOADLET_MUL_XX(q1.y,q2.y)-TOADLET_MUL_XX(q1.z,q2.z)+TOADLET_MUL_XX(q1.w,q2.w);
	}

	public static void postMul(Quaternion q1,Quaternion q2){
		fixed x=TOADLET_MUL_XX(+q1.x,q2.w)+TOADLET_MUL_XX(q1.y,q2.z)-TOADLET_MUL_XX(q1.z,q2.y)+TOADLET_MUL_XX(q1.w,q2.x);
		fixed y=TOADLET_MUL_XX(-q1.x,q2.z)+TOADLET_MUL_XX(q1.y,q2.w)+TOADLET_MUL_XX(q1.z,q2.x)+TOADLET_MUL_XX(q1.w,q2.y);
		fixed z=TOADLET_MUL_XX(+q1.x,q2.y)-TOADLET_MUL_XX(q1.y,q2.x)+TOADLET_MUL_XX(q1.z,q2.w)+TOADLET_MUL_XX(q1.w,q2.z);
		fixed w=TOADLET_MUL_XX(-q1.x,q2.x)-TOADLET_MUL_XX(q1.y,q2.y)-TOADLET_MUL_XX(q1.z,q2.z)+TOADLET_MUL_XX(q1.w,q2.w);
		q1.x=x;
		q1.y=y;
		q1.z=z;
		q1.w=w;
	}

	public static void preMul(Quaternion q2,Quaternion q1){
		fixed x=TOADLET_MUL_XX(+q1.x,q2.w)+TOADLET_MUL_XX(q1.y,q2.z)-TOADLET_MUL_XX(q1.z,q2.y)+TOADLET_MUL_XX(q1.w,q2.x);
		fixed y=TOADLET_MUL_XX(-q1.x,q2.z)+TOADLET_MUL_XX(q1.y,q2.w)+TOADLET_MUL_XX(q1.z,q2.x)+TOADLET_MUL_XX(q1.w,q2.y);
		fixed z=TOADLET_MUL_XX(+q1.x,q2.y)-TOADLET_MUL_XX(q1.y,q2.x)+TOADLET_MUL_XX(q1.z,q2.w)+TOADLET_MUL_XX(q1.w,q2.z);
		fixed w=TOADLET_MUL_XX(-q1.x,q2.x)-TOADLET_MUL_XX(q1.y,q2.y)-TOADLET_MUL_XX(q1.z,q2.z)+TOADLET_MUL_XX(q1.w,q2.w);
		q2.x=x;
		q2.y=y;
		q2.z=z;
		q2.w=w;
	}

	public static fixed lengthSquared(Quaternion q){
		fixed r=TOADLET_MUL_XX(q.x,q.x) + TOADLET_MUL_XX(q.y,q.y) + TOADLET_MUL_XX(q.z,q.z) + TOADLET_MUL_XX(q.w,q.w);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	public static fixed lengthSquared(Quaternion q1,Quaternion q2){
		fixed x=q1.x-q2.x;
		fixed y=q1.y-q2.y;
		fixed z=q1.z-q2.z;
		fixed w=q1.w-q2.w;
		fixed r=TOADLET_MUL_XX(x,x) + TOADLET_MUL_XX(y,y) + TOADLET_MUL_XX(z,z) + TOADLET_MUL_XX(w,w);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	public static fixed length(Quaternion q){
		return sqrt(lengthSquared(q));
	}

	public static fixed length(Quaternion q1,Quaternion q2){
		return sqrt(lengthSquared(q1,q2));
	}

	public static void normalize(Quaternion q){
		fixed l=TOADLET_DIV_XX(ONE,length(q));
		q.x=TOADLET_MUL_XX(q.x,l);
		q.y=TOADLET_MUL_XX(q.y,l);
		q.z=TOADLET_MUL_XX(q.z,l);
		q.w=TOADLET_MUL_XX(q.w,l);
	}

	public static void normalize(Quaternion r,Quaternion q){
		fixed l=TOADLET_DIV_XX(ONE,length(q));
		r.x=TOADLET_MUL_XX(q.x,l);
		r.y=TOADLET_MUL_XX(q.y,l);
		r.z=TOADLET_MUL_XX(q.z,l);
		r.w=TOADLET_MUL_XX(q.w,l);
	}

	public static boolean normalizeCarefully(Quaternion q,fixed epsilon){
		fixed l=length(q);
		if(l>epsilon){
			l=TOADLET_DIV_XX(ONE,l);
			q.x=TOADLET_MUL_XX(q.x,l);
			q.y=TOADLET_MUL_XX(q.y,l);
			q.z=TOADLET_MUL_XX(q.z,l);
			q.w=TOADLET_MUL_XX(q.w,l);
			return true;
		}
		return false;
	}

	public static boolean normalizeCarefully(Quaternion r,Quaternion q,fixed epsilon){
		fixed l=length(q);
		if(l>epsilon){
			l=TOADLET_DIV_XX(ONE,l);
			r.x=TOADLET_MUL_XX(q.x,l);
			r.y=TOADLET_MUL_XX(q.y,l);
			r.z=TOADLET_MUL_XX(q.z,l);
			r.w=TOADLET_MUL_XX(q.w,l);
			return true;
		}
		return false;
	}

	public static fixed dot(Quaternion q1,Quaternion q2){
		return TOADLET_MUL_XX(q1.x,q2.x) + TOADLET_MUL_XX(q1.y,q2.y) + TOADLET_MUL_XX(q1.z,q2.z) + TOADLET_MUL_XX(q1.w,q2.w);
	}

	public static void conjugate(Quaternion q){
		q.x=-q.x;
		q.y=-q.y;
		q.z=-q.z;
	}

	public static void conjugate(Quaternion r,Quaternion q){
		r.x=-q.x;
		r.y=-q.y;
		r.z=-q.z;
		r.w=q.w;
	}

	public static void invert(Quaternion q){
		fixed i=TOADLET_DIV_XX(ONE,TOADLET_MUL_XX(q.x,q.x)+TOADLET_MUL_XX(q.y,q.y)+TOADLET_MUL_XX(q.z,q.z)+TOADLET_MUL_XX(q.w,q.w));
		q.x=TOADLET_MUL_XX(q.x,-i);
		q.y=TOADLET_MUL_XX(q.y,-i);
		q.z=TOADLET_MUL_XX(q.z,-i);
		q.w=TOADLET_MUL_XX(q.w,i);
	}

	public static void invert(Quaternion r,Quaternion q){
		fixed i=TOADLET_DIV_XX(ONE,TOADLET_MUL_XX(q.x,q.x)+TOADLET_MUL_XX(q.y,q.y)+TOADLET_MUL_XX(q.z,q.z)+TOADLET_MUL_XX(q.w,q.w));
		r.x=TOADLET_MUL_XX(q.x,-i);
		r.y=TOADLET_MUL_XX(q.y,-i);
		r.z=TOADLET_MUL_XX(q.z,-i);
		r.w=TOADLET_MUL_XX(q.w,i);
	}

	private static int[] quaternionFromMatrix3x3_next={1,2,0};
	public static void setQuaternionFromMatrix3x3(Quaternion r,Matrix3x3 mat){
		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "Quaternion Calculus and Fast Animation".

		fixed trace=mat.at(0,0)+mat.at(1,1)+mat.at(2,2);
		fixed root;

		if(trace>0){
			// |w| > 1/2, may as well choose w > 1/2
			root=sqrt(trace+ONE); // 2w
			r.w=root>>1;
			root=div(HALF,root); // 1/(4w)
			r.x=mul(mat.at(2,1)-mat.at(1,2),root);
			r.y=mul(mat.at(0,2)-mat.at(2,0),root);
			r.z=mul(mat.at(1,0)-mat.at(0,1),root);
		}
		else{
			// |w| <= 1/2
			int i=0;
			if(mat.at(1,1)>mat.at(0,0)){
				i=1;
			}
			if(mat.at(2,2)>mat.at(i,i)){
				i=2;
			}
			int j=quaternionFromMatrix3x3_next[i];
			int k=quaternionFromMatrix3x3_next[j];

			root=sqrt(mat.at(i,i)-mat.at(j,j)-mat.at(k,k)+ONE);
			trace=root>>1; // trace used as temp in the below code, and as a dummy assignment
			trace=(i==0?(r.x=trace):(i==1?(r.y=trace):(r.z=trace)));
			root=div(HALF,root);
			r.w=mul(mat.at(k,j)-mat.at(j,k),root);
			trace=mul(mat.at(j,i)+mat.at(i,j),root);
			trace=(j==0?(r.x=trace):(j==1?(r.y=trace):(r.z=trace)));
			trace=mul(mat.at(k,i)+mat.at(i,k),root);
			trace=(k==0?(r.x=trace):(k==1?(r.y=trace):(r.z=trace)));
		}
	}

	public static void setQuaternionFromAxisAngle(Quaternion r,Vector3 axis,fixed angle){
		fixed halfAngle=angle>>1;
		fixed sinHalfAngle=sin(halfAngle);
		r.x=TOADLET_MUL_XX(axis.x,sinHalfAngle);
		r.y=TOADLET_MUL_XX(axis.y,sinHalfAngle);
		r.z=TOADLET_MUL_XX(axis.z,sinHalfAngle);
		r.w=cos(halfAngle);
	}

	public static void lerp(Quaternion r,Quaternion q1,Quaternion q2,fixed t){
		fixed cosom=dot(q1,q2);

		fixed scl1=(ONE-t);
		fixed scl2=0;
		if(cosom<0){
			scl2=-t;
		}
		else{
			scl2=t;
		}

		r.x=mul(scl1,q1.x) + mul(scl2,q2.x);
		r.y=mul(scl1,q1.y) + mul(scl2,q2.y);
		r.z=mul(scl1,q1.z) + mul(scl2,q2.z);
		r.w=mul(scl1,q1.w) + mul(scl2,q2.w);
	}

	// The logic behind this slerp function is from the GGT (http://ggt.sourceforge.net)
	public static void slerp(Quaternion r,Quaternion q1,Quaternion q2,fixed t){
		fixed cosom=dot(q1,q2);

		fixed scl2sign=ONE;
		if(cosom<0){
			cosom=-cosom;
			scl2sign=-ONE;
		}

		fixed scl1=0;
		fixed scl2=0;
		if((ONE-cosom) > 6){
			// Standard slerp
			fixed omega=acos(cosom);
			fixed sinom=sin(omega);
			scl1=div(sin(mul(ONE-t,omega)),sinom);
			scl2=div(sin(mul(t,omega)),sinom);
		}
		else{
			// Very close, just lerp
			scl1=ONE-t;
			scl2=t;
		}
		scl2=mul(scl2,scl2sign);

		r.x=mul(scl1,q1.x) + mul(scl2,q2.x);
		r.y=mul(scl1,q1.y) + mul(scl2,q2.y);
		r.z=mul(scl1,q1.z) + mul(scl2,q2.z);
		r.w=mul(scl1,q1.w) + mul(scl2,q2.w);
	}

	// Segment operations
	public static void getClosestPointOnSegment(Vector3 result,Segment segment,Vector3 point){
		Vector3 o=segment.origin;
		Vector3 d=segment.direction;

		if(d.x==0 && d.y==0 && d.z==0){
			result=o;
		}
		else{
			fixed u=div(mul(d.x,point.x-o.x)+mul(d.y,point.y-o.y)+mul(d.z,point.z-o.z),mul(d.x,d.x)+mul(d.y,d.y)+mul(d.z,d.z));
			if(u<0) u=0;
			else if(u>ONE) u=ONE;
			result.x=o.x+mul(u,d.x);
			result.y=o.y+mul(u,d.y);
			result.z=o.z+mul(u,d.z);
		}
	}

	public static void getClosestPointsOnSegments(Vector3 point1,Vector3 point2,Segment seg1,Segment seg2,fixed epsilon){
		// For a full discussion of this method see Dan Sunday's Geometry Algorithms web site: http://www.geometryalgorithms.com/
		fixed a=dot(seg1.direction,seg1.direction);
		fixed b=dot(seg1.direction,seg2.direction);
		fixed c=dot(seg2.direction,seg2.direction);

		// Make sure we don't have a very small segment somewhere, if so we treat it as a point
		if(a<=epsilon){
			point1=seg1.origin;
			getClosestPointOnSegment(point2,seg2,point1);
			return;
		}
		else if(c<epsilon){
			point2=seg2.origin;
			getClosestPointOnSegment(point1,seg1,point2);
			return;
		}

		// Use point1 as a temporary
		sub(point1,seg1.origin,seg2.origin);
		fixed d=dot(seg1.direction,point1);
		fixed e=dot(seg2.direction,point1);
		fixed denom=mul(a,c)-mul(b,b);
		
		fixed u1=0;
		fixed u2=0;
		fixed u1N=0;
		fixed u2N=0;
		fixed u1D=denom;
		fixed u2D=denom;
		
		if(denom<100/* a small fixed point value */){
			// Segments are practically parallel
			u1N=0;
			u1D=ONE;
			u2N=e;
			u2D=c;
		}
		else{
			// Find the closest point on 2 infinite lines
			u1N=mul(b,e)-mul(c,d);
			u2N=mul(a,e)-mul(b,d);
			
			// Check for closest point outside seg1 bounds
			if(u1N<0){
				u1N=0;
				u2N=e;
				u2D=c;
			}
			else if(u1N>u1D){
				u1N=u1D;
				u2N=e+b;
				u2D=c;
			}
		}
		
		// Check for closest point outside seg2 bounds
		if(u2N<0){
			u2N=0;
			// Recalculate u1N
			if(-d<0){
				u1N=0;
			}
			else if(-d>a){
				u1N=u1D;
			}
			else{
				u1N=-d;
				u1D=a;
			}
		}
		else if(u2N>u2D){
			u2N=u2D;
			// Recalculate u1N
			if((-d+b)<0){
				u1N=0;
			}
			else if((-d+b)>a){
				u1N=u1D;
			}
			else{
				u1N=(-d+b);
				u1D=a;
			}
		}
		
		u1=div(u1N,u1D);
		u2=div(u2N,u2D);

		mul(point1,seg1.direction,u1);
		add(point1,seg1.origin);
		mul(point2,seg2.direction,u2);
		add(point2,seg2.origin);
	}

	public static boolean unprojectSegment(Segment result,Matrix4x4 projViewMatrix,int x,int y,int viewportX,int viewportY,int viewportWidth,int viewportHeight){return unprojectSegment(result,projViewMatrix,x,y,viewportX,viewportY,viewportWidth,viewportHeight,new Matrix4x4());}
	public static boolean unprojectSegment(Segment result,Matrix4x4 projViewMatrix,int x,int y,int viewportX,int viewportY,int viewportWidth,int viewportHeight,Matrix4x4 scratchMatrix){
		if(viewportWidth==0 || viewportHeight==0){
			return false;
		}

		invert(scratchMatrix,projViewMatrix);

		fixed fx=div((fixed)(x-(int)viewportX)<<1,(fixed)viewportWidth)-ONE;
		fixed fy=div((fixed)((int)viewportHeight-(y-(int)viewportY))<<1,(fixed)viewportHeight)-ONE;

		result.origin.set(fx,fy,-ONE);
		mulPoint3Full(result.origin,scratchMatrix);

		result.direction.set(fx,fy,ONE);
		mulPoint3Full(result.direction,scratchMatrix);

		sub(result.direction,result.origin);

		return true;
	}

	// Plane operations
	public static void project(Plane plane,Vector3 result,Vector3 point){
		result.x=point.x - TOADLET_MUL_XX(plane.normal.x,plane.distance);
		result.y=point.y - TOADLET_MUL_XX(plane.normal.y,plane.distance);
		result.z=point.z - TOADLET_MUL_XX(plane.normal.z,plane.distance);

		fixed f=TOADLET_MUL_XX(plane.normal.x,result.x) + TOADLET_MUL_XX(plane.normal.y,result.y) + TOADLET_MUL_XX(plane.normal.z,result.z);

		result.x=point.x - TOADLET_MUL_XX(plane.normal.x,f);
		result.y=point.y - TOADLET_MUL_XX(plane.normal.y,f);
		result.z=point.z - TOADLET_MUL_XX(plane.normal.z,f);
	}

	public static void project(Plane plane,Vector3 point){
		fixed tx=point.x - TOADLET_MUL_XX(plane.normal.x,plane.distance);
		fixed ty=point.y - TOADLET_MUL_XX(plane.normal.y,plane.distance);
		fixed tz=point.z - TOADLET_MUL_XX(plane.normal.z,plane.distance);

		fixed f=TOADLET_MUL_XX(plane.normal.x,tx) + TOADLET_MUL_XX(plane.normal.y,ty) + TOADLET_MUL_XX(plane.normal.z,tz);

		point.x=point.x - TOADLET_MUL_XX(plane.normal.x,f);
		point.y=point.y - TOADLET_MUL_XX(plane.normal.y,f);
		point.z=point.z - TOADLET_MUL_XX(plane.normal.z,f);
	}

	public static void normalize(Plane plane){
		fixed l=TOADLET_DIV_XX(ONE,length(plane.normal));
		plane.normal.x=TOADLET_MUL_XX(plane.normal.x,l);
		plane.normal.y=TOADLET_MUL_XX(plane.normal.y,l);
		plane.normal.z=TOADLET_MUL_XX(plane.normal.z,l);
		plane.distance=TOADLET_MUL_XX(plane.distance,l);
	}

	public static void normalize(Plane r,Plane plane){
		fixed l=TOADLET_DIV_XX(ONE,length(plane.normal));
		r.normal.x=TOADLET_MUL_XX(plane.normal.x,l);
		r.normal.y=TOADLET_MUL_XX(plane.normal.y,l);
		r.normal.z=TOADLET_MUL_XX(plane.normal.z,l);
		r.distance=TOADLET_MUL_XX(plane.distance,l);
	}

	public static fixed length(Plane p,Vector3 v){
		return TOADLET_MUL_XX(p.normal.x,v.x) + TOADLET_MUL_XX(p.normal.y,v.y) + TOADLET_MUL_XX(p.normal.z,v.z) - p.distance;
	}

	public static boolean getIntersectionOfThreePlanes(Vector3 result,Plane p1,Plane p2,Plane p3,fixed epsilon){
		Vector3 p2xp3=new Vector3();
		cross(p2xp3,p2.normal,p3.normal);
		fixed den=dot(p1.normal,p2xp3);
		if(den<epsilon && den>-epsilon){
			return false;
		}
		else{
			Vector3 p3xp1=new Vector3(),p1xp2=new Vector3();
			cross(p3xp1,p3.normal,p1.normal);
			cross(p1xp2,p1.normal,p2.normal);
			mul(p1xp2,p3.distance);
			mul(p2xp3,p1.distance);
			mul(p3xp1,p2.distance);
			add(result,p1xp2,p2xp3);
			add(result,p3xp1);
			div(result,den);
			return true;
		}
	}

	public static void getLineOfIntersection(Segment result,Plane plane1,Plane plane2){
		fixed determinant=mul(lengthSquared(plane1.normal),lengthSquared(plane2.normal)) - mul(dot(plane1.normal,plane2.normal),dot(plane1.normal,plane2.normal));

		fixed c1=div(mul(plane1.distance,lengthSquared(plane2.normal)) - mul(plane2.distance,dot(plane1.normal,plane2.normal)),determinant);
		fixed c2=div(mul(plane2.distance,lengthSquared(plane1.normal)) - mul(plane1.distance,dot(plane1.normal,plane2.normal)),determinant);

		cross(result.direction,plane1.normal,plane2.normal);
		Vector3 n1=new Vector3(),n2=new Vector3();
		mul(n1,plane1.normal,c1);
		mul(n2,plane2.normal,c2);
		add(result.origin,n1,n2);
	}

	// AABox operations
	public static void add(AABox r,AABox b,Vector3 p){
		r.mins.x=b.mins.x+p.x;
		r.mins.y=b.mins.y+p.y;
		r.mins.z=b.mins.z+p.z;

		r.maxs.x=b.maxs.x+p.x;
		r.maxs.y=b.maxs.y+p.y;
		r.maxs.z=b.maxs.z+p.z;
	}

	public static void add(AABox b,Vector3 p){
		b.mins.x+=p.x;
		b.mins.y+=p.y;
		b.mins.z+=p.z;

		b.maxs.x+=p.x;
		b.maxs.y+=p.y;
		b.maxs.z+=p.z;
	}

	public static void sub(AABox r,AABox b,Vector3 p){
		r.mins.x=b.mins.x-p.x;
		r.mins.y=b.mins.y-p.y;
		r.mins.z=b.mins.z-p.z;

		r.maxs.x=b.maxs.x-p.x;
		r.maxs.y=b.maxs.y-p.y;
		r.maxs.z=b.maxs.z-p.z;
	}

	public static void sub(AABox b,Vector3 p){
		b.mins.x-=p.x;
		b.mins.y-=p.y;
		b.mins.z-=p.z;

		b.maxs.x-=p.x;
		b.maxs.y-=p.y;
		b.maxs.z-=p.z;
	}

	public static void findBoundingBox(AABox r,Vector3[] vertexes,int amount){
		int i;

		fixed f=0;

		for(i=0;i<amount;++i){
			Vector3 v=vertexes[i];

			if(f<abs(v.x))
				f=abs(v.x);
			if(f<abs(v.y))
				f=abs(v.y);
			if(f<abs(v.z))
				f=abs(v.z);
		}

		r.setMins(f,f,f);
		r.setMaxs(-f,-f,-f);
	}

	public static void findFitBox(AABox r,Vector3[] vertexes,int amount){
		if(amount>0){
			r.maxs.set(vertexes[0]);
			r.mins.set(vertexes[0]);
		}
		
		int i;
		for(i=1;i<amount;++i){
			Vector3 v=vertexes[i];

			if(r.maxs.x<v.x)
				r.maxs.x=v.x;
			if(r.maxs.y<v.y)
				r.maxs.y=v.y;
			if(r.maxs.z<v.z)
				r.maxs.z=v.z;
			
			if(r.mins.x>v.x)
				r.mins.x=v.x;
			if(r.mins.y>v.y)
				r.mins.y=v.y;
			if(r.mins.z>v.z)
				r.mins.z=v.z;
		}
	}

	// Sphere operations
	public static void add(Sphere r,Sphere s,Vector3 p){
		r.origin.x=s.origin.x+p.x;
		r.origin.y=s.origin.y+p.y;
		r.origin.z=s.origin.z+p.z;
	}

	public static void add(Sphere s,Vector3 p){
		s.origin.x+=p.x;
		s.origin.y+=p.y;
		s.origin.z+=p.z;
	}

	public static void sub(Sphere r,Sphere s,Vector3 p){
		r.origin.x=s.origin.x-p.x;
		r.origin.y=s.origin.y-p.y;
		r.origin.z=s.origin.z-p.z;
	}

	public static void sub(Sphere s,Vector3 p){
		s.origin.x-=p.x;
		s.origin.y-=p.y;
		s.origin.z-=p.z;
	}

	public static void findBoundingSphere(Sphere r,AABox box){
		Vector3 mins=box.mins;
		Vector3 maxs=box.maxs;

		r.origin.x=(mins.x+maxs.x)>>1;
		r.origin.y=(mins.y+maxs.y)>>1;
		r.origin.z=(mins.z+maxs.z)>>1;

		r.setRadius(sqrt(mul(maxs.x-r.origin.x,maxs.x-r.origin.x)+mul(maxs.y-r.origin.y,maxs.y-r.origin.y)+mul(maxs.z-r.origin.z,maxs.z-r.origin.z)));
	}

	public static void findFitSphere(Sphere r,AABox box){
		Vector3 mins=box.mins;
		Vector3 maxs=box.maxs;

		r.origin.x=(mins.x+maxs.x)>>1;
		r.origin.y=(mins.y+maxs.y)>>1;
		r.origin.z=(mins.z+maxs.z)>>1;

		r.setRadius(div(sqrt(mul(maxs.x-r.origin.x,maxs.x-r.origin.x)+mul(maxs.y-r.origin.y,maxs.y-r.origin.y)+mul(maxs.z-r.origin.z,maxs.z-r.origin.z)),intToFixed(3)));
	}

	// Capsule operations
	public static void add(Capsule r,Capsule c,Vector3 p){
		r.origin.x=c.origin.x+p.x;
		r.origin.y=c.origin.y+p.y;
		r.origin.z=c.origin.z+p.z;
	}

	public static void add(Capsule c,Vector3 p){
		c.origin.x+=p.x;
		c.origin.y+=p.y;
		c.origin.z+=p.z;
	}

	public static void sub(Capsule r,Capsule c,Vector3 p){
		r.origin.x=c.origin.x-p.x;
		r.origin.y=c.origin.y-p.y;
		r.origin.z=c.origin.z-p.z;
	}

	public static void sub(Capsule c,Vector3 p){
		c.origin.x-=p.x;
		c.origin.y-=p.y;
		c.origin.z-=p.z;
	}

	public static void findBoundingCapsule(Capsule r,AABox box){
		Vector3 mins=box.mins;
		Vector3 maxs=box.maxs;

		r.origin.x=(mins.x+maxs.x)>>1;
		r.origin.y=(mins.y+maxs.y)>>1;
		r.origin.z=mins.z;

		r.direction.z=(maxs.z-mins.z);

		r.setRadius(sqrt(mul(maxs.x-r.origin.x,maxs.x-r.origin.x)+mul(maxs.y-r.origin.y,maxs.y-r.origin.y)));
	}

	void findFitCapsule(Capsule r,AABox box){
		Vector3 mins=box.mins;
		Vector3 maxs=box.maxs;

		r.origin.x=(mins.x+maxs.x)>>1;
		r.origin.y=(mins.y+maxs.y)>>1;

		r.setRadius(div(sqrt(mul(maxs.x-r.origin.x,maxs.x-r.origin.x)+mul(maxs.y-r.origin.y,maxs.y-r.origin.y)),SQRT_2));

		r.origin.z=mins.z+r.radius;

		r.direction.z=(maxs.z-mins.z-r.getRadius());

		if(r.direction.z<0){
			r.direction.z=0;
		}
	}

	// Interesection operations
	public static boolean testInside(Vector3 point,Plane plane){
		return dot(point,plane.normal)<plane.distance;
	}

	public static boolean testInside(Vector3 point,Sphere sphere){
		return lengthSquared(point,sphere.origin) <= square(sphere.radius);
	}

	public static boolean testInside(Vector3 point,AABox box){
		return	point.x>box.mins.x && point.y>box.mins.y && point.z>box.mins.z &&
				point.x<box.maxs.x && point.y<box.maxs.y && point.z<box.maxs.z;
	}

	public static boolean testIntersection(AABox box1,AABox box2){
		return	!(box1.mins.x>box2.maxs.x || box1.mins.y>box2.maxs.y || box1.mins.z>box2.maxs.z ||
				box2.mins.x>box1.maxs.x || box2.mins.y>box1.maxs.y || box2.mins.z>box1.maxs.z);
	}

	public static boolean testIntersection(Sphere sphere,AABox box){
		fixed s=0;
		fixed d=0;

		if(sphere.origin.x<box.mins.x){
			s=sphere.origin.x-box.mins.x;
			d+=mul(s,s);
		}
		else if(sphere.origin.x>box.maxs.x){
			s=sphere.origin.x-box.maxs.x;
			d+=mul(s,s);
		}

		if(sphere.origin.y<box.mins.y){
			s=sphere.origin.y-box.mins.y;
			d+=mul(s,s);
		}
		else if(sphere.origin.y>box.maxs.y){
			s=sphere.origin.y-box.maxs.y;
			d+=mul(s,s);
		}

		if(sphere.origin.z<box.mins.z){
			s=sphere.origin.z-box.mins.z;
			d+=mul(s,s);
		}
		else if(sphere.origin.z>box.maxs.z){
			s=sphere.origin.z-box.maxs.z;
			d+=mul(s,s);
		}

		return d<=square(sphere.radius);
	}	

	public static fixed findIntersection(Segment segment,Plane plane,Vector3 point,Vector3 normal){
		fixed d=dot(plane.normal,segment.direction);
		if(d!=0){
			fixed t=div(plane.distance-dot(plane.normal,segment.origin),d);
			mul(point,segment.direction,t);
			add(point,segment.origin);
			normal.set(plane.normal);
			return t;
		}
		else{
			return -ONE;
		}
	}

	public static fixed findIntersection(Segment segment,Sphere sphere,Vector3 point,Vector3 normal){
		Vector3 segOrigin=segment.origin;
		Vector3 segDirection=segment.direction;
		Vector3 sphOrigin=sphere.origin;
		fixed sphRadius=sphere.radius;

		// Quadratic Q(t) = a*t^2 + 2*b*t + c
		Vector3 diff=new Vector3();
		sub(diff,segOrigin,sphOrigin);
		fixed a=lengthSquared(segDirection);
		if(a<=0){
			return -ONE;
		}
		fixed b=dot(diff,segDirection);
		fixed c=lengthSquared(diff)-square(sphRadius);
		fixed time1=-ONE;
		fixed time2=-ONE;

		// If no real roots, then no intersection
		fixed discr=square(b) - mul(a,c);
		if(discr<0){
			time1=-ONE;
		}
		else if(discr>0){
			fixed root=sqrt(discr);
			fixed invA=div(ONE,a);
			time1=mul((-b)-root,invA);
			time2=mul((-b)+root,invA);

			if(time1>ONE || time2<0){
				time1=-ONE;
			}
			else if(time1>=0){
				mul(point,segDirection,time1);
				add(point,segOrigin);
			}
			else{
				mul(point,segDirection,time2);
				add(point,segOrigin);
			}
		}
		else{
			time1=div(-b,a);
			if(0<=time1 && time1<=ONE){
				mul(point,segDirection,time1);
				add(point,segOrigin);
			}
			else{
				time1=-ONE;
			}
		}

		if(time1!=-ONE){
			sub(normal,point,sphOrigin);
			normalizeCarefully(normal,0);
		}
		return time1;
	}

	public static fixed findIntersection(Segment segment,AABox box,Vector3 point,Vector3 normal){
		Vector3 segOrigin=segment.origin;
		Vector3 segDirection=segment.direction;
		Vector3 boxMins=box.mins;
		Vector3 boxMaxs=box.maxs;

		boolean inside=true;
		byte quadX,quadY,quadZ;
		int whichPlane;
		fixed maxTX=0,maxTY=0,maxTZ=0;
		fixed candPlaneX=0,candPlaneY=0,candPlaneZ=0;
		fixed candNormX=-ONE;
		fixed candNormY=-ONE;
		fixed candNormZ=-ONE;
		fixed time=-ONE;

		// The below tests were originally < or >, but are now <= or >=
		// Without this change finds that start on the edge of a box count as inside which conflicts with how testInside works

		// X
		if(segOrigin.x<=boxMins.x){
			quadX=0; // Left
			candPlaneX=boxMins.x;
			inside=false;
		}
		else if(segOrigin.x>=boxMaxs.x){
			quadX=1; // Right
			candPlaneX=boxMaxs.x;
			candNormX=ONE;
			inside=false;
		}
		else{
			quadX=2; // Middle
		}
		// Y
		if(segOrigin.y<=boxMins.y){
			quadY=0; // Left
			candPlaneY=boxMins.y;
			inside=false;
		}
		else if(segOrigin.y>=boxMaxs.y){
			quadY=1; // Right
			candPlaneY=boxMaxs.y;
			candNormY=ONE;
			inside=false;
		}
		else{
			quadY=2; // Middle
		}
		// Z
		if(segOrigin.z<=boxMins.z){
			quadZ=0; // Left
			candPlaneZ=boxMins.z;
			inside=false;
		}
		else if(segOrigin.z>=boxMaxs.z){
			quadZ=1; // Right
			candPlaneZ=boxMaxs.z;
			candNormZ=ONE;
			inside=false;
		}
		else{
			quadZ=2; // Middle
		}

		// Inside
		if(inside){
			return -TWO;
		}

		// Calculate t distances to candidate planes
		// X
		if(quadX!=2 /*Middle*/ && segDirection.x!=0){
			maxTX=TOADLET_DIV_XX(candPlaneX-segOrigin.x,segDirection.x);
		}
		else{
			maxTX=-ONE;
		}
		// Y
		if(quadY!=2 /*Middle*/ && segDirection.y!=0){
			maxTY=TOADLET_DIV_XX(candPlaneY-segOrigin.y,segDirection.y);
		}
		else{
			maxTY=-ONE;
		}
		// Z
		if(quadZ!=2 /*Middle*/ && segDirection.z!=0){
			maxTZ=TOADLET_DIV_XX(candPlaneZ-segOrigin.z,segDirection.z);
		}
		else{
			maxTZ=-ONE;
		}

		// Find largets of maxT's
		// X
		if(maxTX>maxTY && maxTX>maxTZ){
			whichPlane=0;
			time=maxTX;
			normal.x=candNormX;normal.y=0;normal.z=0;
		}
		// Y
		else if(maxTY>maxTZ){
			whichPlane=1;
			time=maxTY;
			normal.x=0;normal.y=candNormY;normal.z=0;
		}
		// Z
		else{
			whichPlane=2;
			time=maxTZ;
			normal.x=0;normal.y=0;normal.z=candNormZ;
		}

		// Check final candidate actually inside box and calculate final point
		if(time<0 || time>ONE){
			return -ONE;
		}
		// X
		if(whichPlane!=0){
			point.x=segOrigin.x+TOADLET_MUL_XX(time,segDirection.x);
			if(point.x<boxMins.x || point.x>boxMaxs.x){
				return -ONE;
			}
		}
		else{
			point.x=candPlaneX;
		}
		// Y
		if(whichPlane!=1){
			point.y=segOrigin.y+TOADLET_MUL_XX(time,segDirection.y);
			if(point.y<boxMins.y || point.y>boxMaxs.y){
				return -ONE;
			}
		}
		else{
			point.y=candPlaneY;
		}
		// Z
		if(whichPlane!=2){
			point.z=segOrigin.z+TOADLET_MUL_XX(time,segDirection.z);
			if(point.z<boxMins.z || point.z>boxMaxs.z){
				return -ONE;
			}
		}
		else{
			point.z=candPlaneZ;
		}

		return time;
	}
}
