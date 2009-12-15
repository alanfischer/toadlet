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

package com.lightningtoads.toadlet.egg.math;

#include <com/lightningtoads/toadlet/Types.h>
#include <com/lightningtoads/toadlet/egg/math/Inline.h>

public final class Math{
	public static final real ONE=(real)1.0;
	public static final real TWO=(real)2.0;
	public static final real THREE=(real)3.0;
	public static final real FOUR=(real)4.0;
	public static final real THREE_QUARTERS=(real)0.75;
	public static final real HALF=(real)0.5;
	public static final real QUARTER=(real)0.25;
	public static final real ONE_EIGHTY=(real)180.0;
	public static final real PI=(real)3.14159265358979323846264;
	public static final real TWO_PI=(real)(PI*2.0);
	public static final real THREE_QUARTERS_PI=(real)(PI*3.0/4.0);
	public static final real HALF_PI=(real)(PI/2.0);
	public static final real QUARTER_PI=(real)(PI/4.0);
	public static final real E=(real)2.71828182845904523536028;
	public static final real SQRT_2=(real)1.41421356237309504880168;

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

	public static real mul(real b,real c){
		return b*c;
	}

	public static real div(real b,real c){
		return b/c;
	}

	public static real madd(real b,real c,real a){
		return b*c+a;
	}

	public static real msub(real b,real c,real a){
		return b*c-a;
	}

	public static real milliToReal(int m){
		return (real)(m/1000.0);
	}

	public static real fromMilli(int m){return milliToReal(m);}

	public static int realToMilli(real r){
		return (int)(r*1000);
	}

	public static int toMilli(real r){return realToMilli(r);}

	public static real fromInt(int i){return (real)i;}

	public static int toInt(real r){return (int)r;}

	public static real abs(real r){return (real)java.lang.Math.abs(r);}

	public static real ceil(real r){return (real)java.lang.Math.ceil(r);}
	public static real floor(real r){return (real)java.lang.Math.floor(r);}

	public static int intCeil(real r){return (int)java.lang.Math.ceil(r);}
	public static int intFloor(real r){return (int)java.lang.Math.floor(r);}

	public static real square(real r){
		return r*r;
	}

	public static real sqrt(real r){
		return (real)java.lang.Math.sqrt(r);
	}

	public static real minVal(real x,real y){
		return TOADLET_MIN_RR(x,y);
	}

	public static real maxVal(real x,real y){
		return TOADLET_MAX_RR(x,y);
	}

	public static real clamp(real low,real high,real value){
		low=TOADLET_MAX_RR(low,value);
		return TOADLET_MIN_RR(high,low);
	}


	public static real lerp(real t1,real t2,real time){
		return (t1 + time*(t2-t1));
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

	public static real degToRad(real d){
		return (real)(d*(real)(PI/ONE_EIGHTY));
	}

	public static real radToDeg(real r){
		return (real)(r*(real)(ONE_EIGHTY/PI));
	}

	public static real cos(real r){return (real)java.lang.Math.cos(r);}
	public static real acos(real r){return (real)java.lang.Math.acos(r);}
	public static real sin(real r){return (real)java.lang.Math.sin(r);}
	public static real asin(real r){return (real)java.lang.Math.asin(r);}
	public static real tan(real r){return (real)java.lang.Math.tan(r);}
	public static real atan(real r){return (real)java.lang.Math.atan(r);}
	public static real atan2(real r1,real r2){return (real)java.lang.Math.atan2(r1,r2);}
	public static real log(real r){return (real)java.lang.Math.log(r);}

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

	public static void mul(Vector2 r,Vector2 v,real f){
		r.x=v.x*f;
		r.y=v.y*f;
	}

	public static void mul(Vector2 r,real f){
		r.x*=f;
		r.y*=f;
	}
	
	public static void div(Vector2 r,Vector2 v,real f){
		real i=1.0f/f;
		r.x=v.x*f;
		r.y=v.y*f;
	}

	public static void div(Vector2 r,real f){
		real i=1.0f/f;
		r.x*=i;
		r.y*=i;
	}

	public static void madd(Vector2 r,Vector2 b,real c,Vector2 a){
		r.x=madd(b.x,c,a.x);
		r.y=madd(b.y,c,a.y);
	}

	public static void madd(Vector2 r,real c,Vector2 a){
		r.x=madd(r.x,c,a.x);
		r.y=madd(r.y,c,a.y);
	}

	public static void msub(Vector2 r,Vector2 b,real c,Vector2 a){
		r.x=msub(b.x,c,a.x);
		r.y=msub(b.y,c,a.y);
	}

	public static void msub(Vector2 r,real c,Vector2 a){
		r.x=msub(r.x,c,a.x);
		r.y=msub(r.y,c,a.y);
	}

	public static real lengthSquared(Vector2 v){
		return v.x*v.x + v.y*v.y;
	}

	public static real lengthSquared(Vector2 v1,Vector2 v2){
		real x=v1.x-v2.x;
		real y=v1.y-v2.y;
		return x*x + y*y;
	}

	public static real length(Vector2 v){
		return sqrt(lengthSquared(v));
	}

	public static real length(Vector2 v1,Vector2 v2){
		return sqrt(lengthSquared(v1,v2));
	}

	public static void normalize(Vector2 v){
		real l=1.0f/length(v);
		v.x*=l;
		v.y*=l;
	}

	public static void normalize(Vector2 r,Vector2 v){
		real l=1.0f/length(v);
		r.x=v.x*l;
		r.y=v.y*l;
	}

	public static boolean normalizeCarefully(Vector2 v,real epsilon){
		real l=length(v);
		if(l>epsilon){
			l=1.0f/l;
			v.x*=l;
			v.y*=l;
			return true;
		}
		return false;
	}

	public static boolean normalizeCarefully(Vector2 r,Vector2 v,real epsilon){
		real l=length(v);
		if(l>epsilon){
			l=1.0f/l;
			r.x=v.x*l;
			r.y=v.y*l;
			return true;
		}
		return false;
	}

	public static real dot(Vector2 v1,Vector2 v2){
		return v1.x*v2.x + v1.y*v2.y;
	}

	public static void lerp(Vector2 r,Vector2 v1,Vector2 v2,real t){
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

	public static void mul(Vector3 r,Vector3 v,real f){
		r.x=v.x*f;
		r.y=v.y*f;
		r.z=v.z*f;
	}
	
	public static void mul(Vector3 r,real f){
		r.x*=f;
		r.y*=f;
		r.z*=f;
	}
	
	public static void div(Vector3 r,Vector3 v,real f){
		real i=1.0f/f;
		r.x=v.x*i;
		r.y=v.y*i;
		r.z=v.z*i;
	}
	
	public static void div(Vector3 r,real f){
		real i=1.0f/f;
		r.x*=i;
		r.y*=i;
		r.z*=i;
	}

	public static void madd(Vector3 r,Vector3 b,real c,Vector3 a){
		r.x=madd(b.x,c,a.x);
		r.y=madd(b.y,c,a.y);
		r.z=madd(b.z,c,a.z);
	}

	public static void madd(Vector3 r,real c,Vector3 a){
		r.x=madd(r.x,c,a.x);
		r.y=madd(r.y,c,a.y);
		r.z=madd(r.z,c,a.z);
	}

	public static void msub(Vector3 r,Vector3 b,real c,Vector3 a){
		r.x=msub(b.x,c,a.x);
		r.y=msub(b.y,c,a.y);
		r.z=msub(b.z,c,a.z);
	}

	public static void msub(Vector3 r,real c,Vector3 a){
		r.x=msub(r.x,c,a.x);
		r.y=msub(r.y,c,a.y);
		r.z=msub(r.z,c,a.z);
	}

	public static real lengthSquared(Vector3 v){
		return v.x*v.x + v.y*v.y + v.z*v.z;
	}
	
	public static real lengthSquared(Vector3 v1,Vector3 v2){
		real x=v1.x-v2.x;
		real y=v1.y-v2.y;
		real z=v1.z-v2.z;
		return x*x + y*y + z*z;
	}
	
	public static real length(Vector3 v){
		return sqrt(lengthSquared(v));
	}
	
	public static real length(Vector3 v1,Vector3 v2){
		return sqrt(lengthSquared(v1,v2));
	}

	public static void normalize(Vector3 v){
		real l=1.0f/length(v);
		v.x*=l;
		v.y*=l;
		v.z*=l;
	}

	public static void normalize(Vector3 r,Vector3 v){
		real l=1.0f/length(v);
		r.x=v.x*l;
		r.y=v.y*l;
		r.z=v.z*l;
	}

	public static boolean normalizeCarefully(Vector3 v,real epsilon){
		real l=length(v);
		if(l>epsilon){
			l=1.0f/l;
			v.x*=l;
			v.y*=l;
			v.z*=l;
			return true;
		}
		return false;
	}
	
	public static boolean normalizeCarefully(Vector3 r,Vector3 v,real epsilon){
		real l=length(v);
		if(l>epsilon){
			l=1.0f/l;
			r.x=v.x*l;
			r.y=v.y*l;
			r.z=v.z*l;
			return true;
		}
		return false;
	}

	public static real dot(Vector3 v1,Vector3 v2){
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}

	public static void lerp(Vector3 r,Vector3 v1,Vector3 v2,real t){
		sub(r,v2,v1);
		mul(r,t);
		add(r,v1);
	}

	public static void cross(Vector3 r,Vector3 v1,Vector3 v2){
		r.x=v1.y*v2.z-v1.z*v2.y;
		r.y=v1.z*v2.x-v1.x*v2.z;
		r.z=v1.x*v2.y-v1.y*v2.x;
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

	public static void mul(Vector4 r,Vector4 v,real f){
		r.x=v.x*f;
		r.y=v.y*f;
		r.z=v.z*f;
		r.w=v.w*f;
	}
	
	public static void mul(Vector4 r,real f){
		r.x*=f;
		r.y*=f;
		r.z*=f;
		r.w*=f;
	}
	
	public static void div(Vector4 r,Vector4 v,real f){
		real i=1.0f/f;
		r.x=v.x*i;
		r.y=v.y*i;
		r.z=v.z*i;
		r.w=v.w*i;
	}
	
	public static void div(Vector4 r,real f){
		real i=1.0f/f;
		r.x*=i;
		r.y*=i;
		r.z*=i;
		r.w*=i;
	}

	public static void madd(Vector4 r,Vector4 b,real c,Vector4 a){
		r.x=madd(b.x,c,a.x);
		r.y=madd(b.y,c,a.y);
		r.z=madd(b.z,c,a.z);
		r.w=madd(b.w,c,a.w);
	}

	public static void madd(Vector4 r,real c,Vector4 a){
		r.x=madd(r.x,c,a.x);
		r.y=madd(r.y,c,a.y);
		r.z=madd(r.z,c,a.z);
		r.w=madd(r.w,c,a.w);
	}

	public static void msub(Vector4 r,Vector4 b,real c,Vector4 a){
		r.x=msub(b.x,c,a.x);
		r.y=msub(b.y,c,a.y);
		r.z=msub(b.z,c,a.z);
		r.w=msub(b.w,c,a.w);
	}

	public static void msub(Vector4 r,real c,Vector4 a){
		r.x=msub(r.x,c,a.x);
		r.y=msub(r.y,c,a.y);
		r.z=msub(r.z,c,a.z);
		r.w=msub(r.w,c,a.w);
	}

	public static real lengthSquared(Vector4 v){
		return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
	}

	public static real lengthSquared(Vector4 v1,Vector4 v2){
		real x=v1.x-v2.x;
		real y=v1.y-v2.y;
		real z=v1.z-v2.z;
		real w=v1.w-v2.w;
		return x*x + y*y + z*z + w*w;
	}

	public static real length(Vector4 v){
		return sqrt(lengthSquared(v));
	}

	public static real length(Vector4 v1,Vector4 v2){
		return sqrt(lengthSquared(v1,v2));
	}

	public static void normalize(Vector4 v){
		real l=1.0f/length(v);
		v.x*=l;
		v.y*=l;
		v.z*=l;
		v.w*=l;
	}

	public static void normalize(Vector4 r,Vector4 v){
		real l=1.0f/length(v);
		r.x=v.x*l;
		r.y=v.y*l;
		r.z=v.z*l;
		r.w=v.w*l;
	}

	public static boolean normalizeCarefully(Vector4 v,real epsilon){
		real l=length(v);
		if(l>epsilon){
			l=1.0f/l;
			v.x*=l;
			v.y*=l;
			v.z*=l;
			v.w*=l;
			return true;
		}
		return false;
	}

	public static boolean normalizeCarefully(Vector4 r,Vector4 v,real epsilon){
		real l=length(v);
		if(l>epsilon){
			l=1.0f/l;
			v.x*=l;
			v.y*=l;
			v.z*=l;
			v.w*=l;
			return true;
		}
		return false;
	}

	public static real dot(Vector4 v1,Vector4 v2){
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
	}

	public static void lerp(Vector4 r,Vector4 v1,Vector4 v2,real t){
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
				real my=atan2(m.at(1,0),m.at(1,1));
				r.z=0.0f; // any angle works
				r.x=r.z-my;
				return false;
			}
		}
		else{
			// WARNING: Not a unique solution.
			real py=atan2(m.at(1,0),m.at(1,1));
			r.z=0.0f; // any angle works
			r.x=py-r.z;
			return false;
		}
	}

	// Matrix3x3 basic operations
	public static void mul(Matrix3x3 r,Matrix3x3 m1,Matrix3x3 m2){
		r.data[0+0*3]=m1.data[0+0*3] * m2.data[0+0*3] + m1.data[0+1*3] * m2.data[1+0*3] + m1.data[0+2*3] * m2.data[2+0*3];
		r.data[0+1*3]=m1.data[0+0*3] * m2.data[0+1*3] + m1.data[0+1*3] * m2.data[1+1*3] + m1.data[0+2*3] * m2.data[2+1*3];
		r.data[0+2*3]=m1.data[0+0*3] * m2.data[0+2*3] + m1.data[0+1*3] * m2.data[1+2*3] + m1.data[0+2*3] * m2.data[2+2*3];

		r.data[1+0*3]=m1.data[1+0*3] * m2.data[0+0*3] + m1.data[1+1*3] * m2.data[1+0*3] + m1.data[1+2*3] * m2.data[2+0*3];
		r.data[1+1*3]=m1.data[1+0*3] * m2.data[0+1*3] + m1.data[1+1*3] * m2.data[1+1*3] + m1.data[1+2*3] * m2.data[2+1*3];
		r.data[1+2*3]=m1.data[1+0*3] * m2.data[0+2*3] + m1.data[1+1*3] * m2.data[1+2*3] + m1.data[1+2*3] * m2.data[2+2*3];

		r.data[2+0*3]=m1.data[2+0*3] * m2.data[0+0*3] + m1.data[2+1*3] * m2.data[1+0*3] + m1.data[2+2*3] * m2.data[2+0*3];
		r.data[2+1*3]=m1.data[2+0*3] * m2.data[0+1*3] + m1.data[2+1*3] * m2.data[1+1*3] + m1.data[2+2*3] * m2.data[2+1*3];
		r.data[2+2*3]=m1.data[2+0*3] * m2.data[0+2*3] + m1.data[2+1*3] * m2.data[1+2*3] + m1.data[2+2*3] * m2.data[2+2*3];
	}

	public static void postMul(Matrix3x3 m1,Matrix3x3 m2){
		real d00=m1.data[0+0*3] * m2.data[0+0*3] + m1.data[0+1*3] * m2.data[1+0*3] + m1.data[0+2*3] * m2.data[2+0*3];
		real d01=m1.data[0+0*3] * m2.data[0+1*3] + m1.data[0+1*3] * m2.data[1+1*3] + m1.data[0+2*3] * m2.data[2+1*3];
		real d02=m1.data[0+0*3] * m2.data[0+2*3] + m1.data[0+1*3] * m2.data[1+2*3] + m1.data[0+2*3] * m2.data[2+2*3];

		real d10=m1.data[1+0*3] * m2.data[0+0*3] + m1.data[1+1*3] * m2.data[1+0*3] + m1.data[1+2*3] * m2.data[2+0*3];
		real d11=m1.data[1+0*3] * m2.data[0+1*3] + m1.data[1+1*3] * m2.data[1+1*3] + m1.data[1+2*3] * m2.data[2+1*3];
		real d12=m1.data[1+0*3] * m2.data[0+2*3] + m1.data[1+1*3] * m2.data[1+2*3] + m1.data[1+2*3] * m2.data[2+2*3];

		real d20=m1.data[2+0*3] * m2.data[0+0*3] + m1.data[2+1*3] * m2.data[1+0*3] + m1.data[2+2*3] * m2.data[2+0*3];
		real d21=m1.data[2+0*3] * m2.data[0+1*3] + m1.data[2+1*3] * m2.data[1+1*3] + m1.data[2+2*3] * m2.data[2+1*3];
		real d22=m1.data[2+0*3] * m2.data[0+2*3] + m1.data[2+1*3] * m2.data[1+2*3] + m1.data[2+2*3] * m2.data[2+2*3];

		m1.data[0+0*3]=d00; m1.data[0+1*3]=d01; m1.data[0+2*3]=d02;
		m1.data[1+0*3]=d10; m1.data[1+1*3]=d11; m1.data[1+2*3]=d12;
		m1.data[2+0*3]=d20; m1.data[2+1*3]=d21; m1.data[2+2*3]=d22;
	}

	public static void preMul(Matrix3x3 m2,Matrix3x3 m1){
		real d00=m1.data[0+0*3] * m2.data[0+0*3] + m1.data[0+1*3] * m2.data[1+0*3] + m1.data[0+2*3] * m2.data[2+0*3];
		real d01=m1.data[0+0*3] * m2.data[0+1*3] + m1.data[0+1*3] * m2.data[1+1*3] + m1.data[0+2*3] * m2.data[2+1*3];
		real d02=m1.data[0+0*3] * m2.data[0+2*3] + m1.data[0+1*3] * m2.data[1+2*3] + m1.data[0+2*3] * m2.data[2+2*3];

		real d10=m1.data[1+0*3] * m2.data[0+0*3] + m1.data[1+1*3] * m2.data[1+0*3] + m1.data[1+2*3] * m2.data[2+0*3];
		real d11=m1.data[1+0*3] * m2.data[0+1*3] + m1.data[1+1*3] * m2.data[1+1*3] + m1.data[1+2*3] * m2.data[2+1*3];
		real d12=m1.data[1+0*3] * m2.data[0+2*3] + m1.data[1+1*3] * m2.data[1+2*3] + m1.data[1+2*3] * m2.data[2+2*3];

		real d20=m1.data[2+0*3] * m2.data[0+0*3] + m1.data[2+1*3] * m2.data[1+0*3] + m1.data[2+2*3] * m2.data[2+0*3];
		real d21=m1.data[2+0*3] * m2.data[0+1*3] + m1.data[2+1*3] * m2.data[1+1*3] + m1.data[2+2*3] * m2.data[2+1*3];
		real d22=m1.data[2+0*3] * m2.data[0+2*3] + m1.data[2+1*3] * m2.data[1+2*3] + m1.data[2+2*3] * m2.data[2+2*3];

		m2.data[0+0*3]=d00; m2.data[0+1*3]=d01; m2.data[0+2*3]=d02;
		m2.data[1+0*3]=d10; m2.data[1+1*3]=d11; m2.data[1+2*3]=d12;
		m2.data[2+0*3]=d20; m2.data[2+1*3]=d21; m2.data[2+2*3]=d22;
	}

	public static void mul(Vector3 r,Matrix3x3 m,Vector3 v){
		r.x=m.data[0+0*3]*v.x + m.data[0+1*3]*v.y + m.data[0+2*3]*v.z;
		r.y=m.data[1+0*3]*v.x + m.data[1+1*3]*v.y + m.data[1+2*3]*v.z;
		r.z=m.data[2+0*3]*v.x + m.data[2+1*3]*v.y + m.data[2+2*3]*v.z;
	}

	public static void mul(Vector3 v,Matrix3x3 m){
		real tx=m.data[0+0*3]*v.x + m.data[0+1*3]*v.y + m.data[0+2*3]*v.z;
		real ty=m.data[1+0*3]*v.x + m.data[1+1*3]*v.y + m.data[1+2*3]*v.z;
		real tz=m.data[2+0*3]*v.x + m.data[2+1*3]*v.y + m.data[2+2*3]*v.z;
		v.x=tx;
		v.y=ty;
		v.z=tz;
	}

	public static void transpose(Matrix3x3 r,Matrix3x3 m){
		r.data[0+0*3]=m.data[0+0*3]; r.data[0+1*3]=m.data[1+0*3]; r.data[0+2*3]=m.data[2+0*3];
		r.data[1+0*3]=m.data[0+1*3]; r.data[1+1*3]=m.data[1+1*3]; r.data[1+2*3]=m.data[2+1*3];
		r.data[2+0*3]=m.data[0+2*3]; r.data[2+1*3]=m.data[1+2*3]; r.data[2+2*3]=m.data[2+2*3];
	}

	public static real determinant(Matrix3x3 m){
		return
			-m.data[0+0*3]*m.data[1+1*3]*m.data[2+2*3] + m.data[0+0*3]*m.data[2+1*3]*m.data[1+2*3] +
			 m.data[0+1*3]*m.data[1+0*3]*m.data[2+2*3] - m.data[0+1*3]*m.data[2+0*3]*m.data[1+2*3] -
			 m.data[0+2*3]*m.data[1+0*3]*m.data[2+1*3] + m.data[0+2*3]*m.data[2+0*3]*m.data[1+1*3];
	}

	public static boolean invert(Matrix3x3 r,Matrix3x3 m){
		real det=1.0f/determinant(m);
		if(det==0.0f){
			return false;
		}
		else{
			r.data[0+0*3]=-(det*((m.data[1+1*3]*m.data[2+2*3]) - (m.data[2+1*3]*m.data[1+2*3])));
			r.data[1+0*3]= (det*((m.data[1+0*3]*m.data[2+2*3]) - (m.data[2+0*3]*m.data[1+2*3])));
			r.data[2+0*3]=-(det*((m.data[1+0*3]*m.data[2+1*3]) - (m.data[2+0*3]*m.data[1+1*3])));

			r.data[0+1*3]= (det*((m.data[0+1*3]*m.data[2+2*3]) - (m.data[2+1*3]*m.data[0+2*3])));
			r.data[1+1*3]=-(det*((m.data[0+0*3]*m.data[2+2*3]) - (m.data[2+0*3]*m.data[0+2*3])));
			r.data[2+1*3]= (det*((m.data[0+0*3]*m.data[2+1*3]) - (m.data[2+0*3]*m.data[0+1*3])));

			r.data[0+2*3]=-(det*((m.data[0+1*3]*m.data[1+2*3]) - (m.data[1+1*3]*m.data[0+2*3])));
			r.data[1+2*3]= (det*((m.data[0+0*3]*m.data[1+2*3]) - (m.data[1+0*3]*m.data[0+2*3])));
			r.data[2+2*3]=-(det*((m.data[0+0*3]*m.data[1+1*3]) - (m.data[1+0*3]*m.data[0+1*3])));

			return true;
		}
	}

	// Matrix4x4 basic operations
	public static void mul(Matrix4x4 r,Matrix4x4 m1,Matrix4x4 m2){
		r.data[0+0*4]=(m1.data[0+0*4]*m2.data[0+0*4]) + (m1.data[0+1*4]*m2.data[1+0*4]) + (m1.data[0+2*4]*m2.data[2+0*4]) + (m1.data[0+3*4]*m2.data[3+0*4]);
		r.data[0+1*4]=(m1.data[0+0*4]*m2.data[0+1*4]) + (m1.data[0+1*4]*m2.data[1+1*4]) + (m1.data[0+2*4]*m2.data[2+1*4]) + (m1.data[0+3*4]*m2.data[3+1*4]);
		r.data[0+2*4]=(m1.data[0+0*4]*m2.data[0+2*4]) + (m1.data[0+1*4]*m2.data[1+2*4]) + (m1.data[0+2*4]*m2.data[2+2*4]) + (m1.data[0+3*4]*m2.data[3+2*4]);
		r.data[0+3*4]=(m1.data[0+0*4]*m2.data[0+3*4]) + (m1.data[0+1*4]*m2.data[1+3*4]) + (m1.data[0+2*4]*m2.data[2+3*4]) + (m1.data[0+3*4]*m2.data[3+3*4]);

		r.data[1+0*4]=(m1.data[1+0*4]*m2.data[0+0*4]) + (m1.data[1+1*4]*m2.data[1+0*4]) + (m1.data[1+2*4]*m2.data[2+0*4]) + (m1.data[1+3*4]*m2.data[3+0*4]);
		r.data[1+1*4]=(m1.data[1+0*4]*m2.data[0+1*4]) + (m1.data[1+1*4]*m2.data[1+1*4]) + (m1.data[1+2*4]*m2.data[2+1*4]) + (m1.data[1+3*4]*m2.data[3+1*4]);
		r.data[1+2*4]=(m1.data[1+0*4]*m2.data[0+2*4]) + (m1.data[1+1*4]*m2.data[1+2*4]) + (m1.data[1+2*4]*m2.data[2+2*4]) + (m1.data[1+3*4]*m2.data[3+2*4]);
		r.data[1+3*4]=(m1.data[1+0*4]*m2.data[0+3*4]) + (m1.data[1+1*4]*m2.data[1+3*4]) + (m1.data[1+2*4]*m2.data[2+3*4]) + (m1.data[1+3*4]*m2.data[3+3*4]);

		r.data[2+0*4]=(m1.data[2+0*4]*m2.data[0+0*4]) + (m1.data[2+1*4]*m2.data[1+0*4]) + (m1.data[2+2*4]*m2.data[2+0*4]) + (m1.data[2+3*4]*m2.data[3+0*4]);
		r.data[2+1*4]=(m1.data[2+0*4]*m2.data[0+1*4]) + (m1.data[2+1*4]*m2.data[1+1*4]) + (m1.data[2+2*4]*m2.data[2+1*4]) + (m1.data[2+3*4]*m2.data[3+1*4]);
		r.data[2+2*4]=(m1.data[2+0*4]*m2.data[0+2*4]) + (m1.data[2+1*4]*m2.data[1+2*4]) + (m1.data[2+2*4]*m2.data[2+2*4]) + (m1.data[2+3*4]*m2.data[3+2*4]);
		r.data[2+3*4]=(m1.data[2+0*4]*m2.data[0+3*4]) + (m1.data[2+1*4]*m2.data[1+3*4]) + (m1.data[2+2*4]*m2.data[2+3*4]) + (m1.data[2+3*4]*m2.data[3+3*4]);

		r.data[3+0*4]=(m1.data[3+0*4]*m2.data[0+0*4]) + (m1.data[3+1*4]*m2.data[1+0*4]) + (m1.data[3+2*4]*m2.data[2+0*4]) + (m1.data[3+3*4]*m2.data[3+0*4]);
		r.data[3+1*4]=(m1.data[3+0*4]*m2.data[0+1*4]) + (m1.data[3+1*4]*m2.data[1+1*4]) + (m1.data[3+2*4]*m2.data[2+1*4]) + (m1.data[3+3*4]*m2.data[3+1*4]);
		r.data[3+2*4]=(m1.data[3+0*4]*m2.data[0+2*4]) + (m1.data[3+1*4]*m2.data[1+2*4]) + (m1.data[3+2*4]*m2.data[2+2*4]) + (m1.data[3+3*4]*m2.data[3+2*4]);
		r.data[3+3*4]=(m1.data[3+0*4]*m2.data[0+3*4]) + (m1.data[3+1*4]*m2.data[1+3*4]) + (m1.data[3+2*4]*m2.data[2+3*4]) + (m1.data[3+3*4]*m2.data[3+3*4]);
	}

	public static void postMul(Matrix4x4 m1,Matrix4x4 m2){
		real d00=(m1.data[0+0*4]*m2.data[0+0*4]) + (m1.data[0+1*4]*m2.data[1+0*4]) + (m1.data[0+2*4]*m2.data[2+0*4]) + (m1.data[0+3*4]*m2.data[3+0*4]);
		real d01=(m1.data[0+0*4]*m2.data[0+1*4]) + (m1.data[0+1*4]*m2.data[1+1*4]) + (m1.data[0+2*4]*m2.data[2+1*4]) + (m1.data[0+3*4]*m2.data[3+1*4]);
		real d02=(m1.data[0+0*4]*m2.data[0+2*4]) + (m1.data[0+1*4]*m2.data[1+2*4]) + (m1.data[0+2*4]*m2.data[2+2*4]) + (m1.data[0+3*4]*m2.data[3+2*4]);
		real d03=(m1.data[0+0*4]*m2.data[0+3*4]) + (m1.data[0+1*4]*m2.data[1+3*4]) + (m1.data[0+2*4]*m2.data[2+3*4]) + (m1.data[0+3*4]*m2.data[3+3*4]);

		real d10=(m1.data[1+0*4]*m2.data[0+0*4]) + (m1.data[1+1*4]*m2.data[1+0*4]) + (m1.data[1+2*4]*m2.data[2+0*4]) + (m1.data[1+3*4]*m2.data[3+0*4]);
		real d11=(m1.data[1+0*4]*m2.data[0+1*4]) + (m1.data[1+1*4]*m2.data[1+1*4]) + (m1.data[1+2*4]*m2.data[2+1*4]) + (m1.data[1+3*4]*m2.data[3+1*4]);
		real d12=(m1.data[1+0*4]*m2.data[0+2*4]) + (m1.data[1+1*4]*m2.data[1+2*4]) + (m1.data[1+2*4]*m2.data[2+2*4]) + (m1.data[1+3*4]*m2.data[3+2*4]);
		real d13=(m1.data[1+0*4]*m2.data[0+3*4]) + (m1.data[1+1*4]*m2.data[1+3*4]) + (m1.data[1+2*4]*m2.data[2+3*4]) + (m1.data[1+3*4]*m2.data[3+3*4]);

		real d20=(m1.data[2+0*4]*m2.data[0+0*4]) + (m1.data[2+1*4]*m2.data[1+0*4]) + (m1.data[2+2*4]*m2.data[2+0*4]) + (m1.data[2+3*4]*m2.data[3+0*4]);
		real d21=(m1.data[2+0*4]*m2.data[0+1*4]) + (m1.data[2+1*4]*m2.data[1+1*4]) + (m1.data[2+2*4]*m2.data[2+1*4]) + (m1.data[2+3*4]*m2.data[3+1*4]);
		real d22=(m1.data[2+0*4]*m2.data[0+2*4]) + (m1.data[2+1*4]*m2.data[1+2*4]) + (m1.data[2+2*4]*m2.data[2+2*4]) + (m1.data[2+3*4]*m2.data[3+2*4]);
		real d23=(m1.data[2+0*4]*m2.data[0+3*4]) + (m1.data[2+1*4]*m2.data[1+3*4]) + (m1.data[2+2*4]*m2.data[2+3*4]) + (m1.data[2+3*4]*m2.data[3+3*4]);

		real d30=(m1.data[3+0*4]*m2.data[0+0*4]) + (m1.data[3+1*4]*m2.data[1+0*4]) + (m1.data[3+2*4]*m2.data[2+0*4]) + (m1.data[3+3*4]*m2.data[3+0*4]);
		real d31=(m1.data[3+0*4]*m2.data[0+1*4]) + (m1.data[3+1*4]*m2.data[1+1*4]) + (m1.data[3+2*4]*m2.data[2+1*4]) + (m1.data[3+3*4]*m2.data[3+1*4]);
		real d32=(m1.data[3+0*4]*m2.data[0+2*4]) + (m1.data[3+1*4]*m2.data[1+2*4]) + (m1.data[3+2*4]*m2.data[2+2*4]) + (m1.data[3+3*4]*m2.data[3+2*4]);
		real d33=(m1.data[3+0*4]*m2.data[0+3*4]) + (m1.data[3+1*4]*m2.data[1+3*4]) + (m1.data[3+2*4]*m2.data[2+3*4]) + (m1.data[3+3*4]*m2.data[3+3*4]);

		m1.data[0+0*4]=d00; m1.data[0+1*4]=d01; m1.data[0+2*4]=d02; m1.data[0+3*4]=d03;
		m1.data[1+0*4]=d10; m1.data[1+1*4]=d11; m1.data[1+2*4]=d12; m1.data[1+3*4]=d13;
		m1.data[2+0*4]=d20; m1.data[2+1*4]=d21; m1.data[2+2*4]=d22; m1.data[2+3*4]=d23;
		m1.data[3+0*4]=d30; m1.data[3+1*4]=d31; m1.data[3+2*4]=d32; m1.data[3+3*4]=d33;
	}

	public static void preMul(Matrix4x4 m2,Matrix4x4 m1){
		real d00=(m1.data[0+0*4]*m2.data[0+0*4]) + (m1.data[0+1*4]*m2.data[1+0*4]) + (m1.data[0+2*4]*m2.data[2+0*4]) + (m1.data[0+3*4]*m2.data[3+0*4]);
		real d01=(m1.data[0+0*4]*m2.data[0+1*4]) + (m1.data[0+1*4]*m2.data[1+1*4]) + (m1.data[0+2*4]*m2.data[2+1*4]) + (m1.data[0+3*4]*m2.data[3+1*4]);
		real d02=(m1.data[0+0*4]*m2.data[0+2*4]) + (m1.data[0+1*4]*m2.data[1+2*4]) + (m1.data[0+2*4]*m2.data[2+2*4]) + (m1.data[0+3*4]*m2.data[3+2*4]);
		real d03=(m1.data[0+0*4]*m2.data[0+3*4]) + (m1.data[0+1*4]*m2.data[1+3*4]) + (m1.data[0+2*4]*m2.data[2+3*4]) + (m1.data[0+3*4]*m2.data[3+3*4]);

		real d10=(m1.data[1+0*4]*m2.data[0+0*4]) + (m1.data[1+1*4]*m2.data[1+0*4]) + (m1.data[1+2*4]*m2.data[2+0*4]) + (m1.data[1+3*4]*m2.data[3+0*4]);
		real d11=(m1.data[1+0*4]*m2.data[0+1*4]) + (m1.data[1+1*4]*m2.data[1+1*4]) + (m1.data[1+2*4]*m2.data[2+1*4]) + (m1.data[1+3*4]*m2.data[3+1*4]);
		real d12=(m1.data[1+0*4]*m2.data[0+2*4]) + (m1.data[1+1*4]*m2.data[1+2*4]) + (m1.data[1+2*4]*m2.data[2+2*4]) + (m1.data[1+3*4]*m2.data[3+2*4]);
		real d13=(m1.data[1+0*4]*m2.data[0+3*4]) + (m1.data[1+1*4]*m2.data[1+3*4]) + (m1.data[1+2*4]*m2.data[2+3*4]) + (m1.data[1+3*4]*m2.data[3+3*4]);

		real d20=(m1.data[2+0*4]*m2.data[0+0*4]) + (m1.data[2+1*4]*m2.data[1+0*4]) + (m1.data[2+2*4]*m2.data[2+0*4]) + (m1.data[2+3*4]*m2.data[3+0*4]);
		real d21=(m1.data[2+0*4]*m2.data[0+1*4]) + (m1.data[2+1*4]*m2.data[1+1*4]) + (m1.data[2+2*4]*m2.data[2+1*4]) + (m1.data[2+3*4]*m2.data[3+1*4]);
		real d22=(m1.data[2+0*4]*m2.data[0+2*4]) + (m1.data[2+1*4]*m2.data[1+2*4]) + (m1.data[2+2*4]*m2.data[2+2*4]) + (m1.data[2+3*4]*m2.data[3+2*4]);
		real d23=(m1.data[2+0*4]*m2.data[0+3*4]) + (m1.data[2+1*4]*m2.data[1+3*4]) + (m1.data[2+2*4]*m2.data[2+3*4]) + (m1.data[2+3*4]*m2.data[3+3*4]);

		real d30=(m1.data[3+0*4]*m2.data[0+0*4]) + (m1.data[3+1*4]*m2.data[1+0*4]) + (m1.data[3+2*4]*m2.data[2+0*4]) + (m1.data[3+3*4]*m2.data[3+0*4]);
		real d31=(m1.data[3+0*4]*m2.data[0+1*4]) + (m1.data[3+1*4]*m2.data[1+1*4]) + (m1.data[3+2*4]*m2.data[2+1*4]) + (m1.data[3+3*4]*m2.data[3+1*4]);
		real d32=(m1.data[3+0*4]*m2.data[0+2*4]) + (m1.data[3+1*4]*m2.data[1+2*4]) + (m1.data[3+2*4]*m2.data[2+2*4]) + (m1.data[3+3*4]*m2.data[3+2*4]);
		real d33=(m1.data[3+0*4]*m2.data[0+3*4]) + (m1.data[3+1*4]*m2.data[1+3*4]) + (m1.data[3+2*4]*m2.data[2+3*4]) + (m1.data[3+3*4]*m2.data[3+3*4]);

		m2.data[0+0*4]=d00; m2.data[0+1*4]=d01; m2.data[0+2*4]=d02; m2.data[0+3*4]=d03;
		m2.data[1+0*4]=d10; m2.data[1+1*4]=d11; m2.data[1+2*4]=d12; m2.data[1+3*4]=d13;
		m2.data[2+0*4]=d20; m2.data[2+1*4]=d21; m2.data[2+2*4]=d22; m2.data[2+3*4]=d23;
		m2.data[3+0*4]=d30; m2.data[3+1*4]=d31; m2.data[3+2*4]=d32; m2.data[3+3*4]=d33;
	}

	public static void mul(Vector4 r,Matrix4x4 m,Vector4 v){
		r.x=(m.data[0+0*4]*v.x) + (m.data[0+1*4]*v.y) + (m.data[0+2*4]*v.z) + (m.data[0+3*4]*v.w);
		r.y=(m.data[1+0*4]*v.x) + (m.data[1+1*4]*v.y) + (m.data[1+2*4]*v.z) + (m.data[1+3*4]*v.w);
		r.z=(m.data[2+0*4]*v.x) + (m.data[2+1*4]*v.y) + (m.data[2+2*4]*v.z) + (m.data[2+3*4]*v.w);
		r.w=(m.data[3+0*4]*v.x) + (m.data[3+1*4]*v.y) + (m.data[3+2*4]*v.z) + (m.data[3+3*4]*v.w);
	}

	public static void mul(Vector4 r,Matrix4x4 m){
		real tx=(m.data[0+0*4]*r.x) + (m.data[0+1*4]*r.y) + (m.data[0+2*4]*r.z) + (m.data[0+3*4]*r.w);
		real ty=(m.data[1+0*4]*r.x) + (m.data[1+1*4]*r.y) + (m.data[1+2*4]*r.z) + (m.data[1+3*4]*r.w);
		real tz=(m.data[2+0*4]*r.x) + (m.data[2+1*4]*r.y) + (m.data[2+2*4]*r.z) + (m.data[2+3*4]*r.w);
		real tw=(m.data[3+0*4]*r.x) + (m.data[3+1*4]*r.y) + (m.data[3+2*4]*r.z) + (m.data[3+3*4]*r.w);
		r.x=tx;
		r.y=ty;
		r.z=tz;
		r.w=tw;
	}

	public static void mul(Vector3 r,Matrix4x4 m,Vector3 v){
		r.x=(m.data[0+0*4]*v.x) + (m.data[0+1*4]*v.y) + (m.data[0+2*4]*v.z);
		r.y=(m.data[1+0*4]*v.x) + (m.data[1+1*4]*v.y) + (m.data[1+2*4]*v.z);
		r.z=(m.data[2+0*4]*v.x) + (m.data[2+1*4]*v.y) + (m.data[2+2*4]*v.z);
	}

	public static void mul(Vector3 r,Matrix4x4 m){
		real tx=(m.data[0+0*4]*r.x) + (m.data[0+1*4]*r.y) + (m.data[0+2*4]*r.z);
		real ty=(m.data[1+0*4]*r.x) + (m.data[1+1*4]*r.y) + (m.data[1+2*4]*r.z);
		real tz=(m.data[2+0*4]*r.x) + (m.data[2+1*4]*r.y) + (m.data[2+2*4]*r.z);
		r.x=tx;
		r.y=ty;
		r.z=tz;
	}

	public static void mulPoint3Fast(Vector3 r,Matrix4x4 m,Vector3 p){
		r.x=(m.data[0+0*4]*p.x) + (m.data[0+1*4]*p.y) + (m.data[0+2*4]*p.z) + m.data[0+3*4];
		r.y=(m.data[1+0*4]*p.x) + (m.data[1+1*4]*p.y) + (m.data[1+2*4]*p.z) + m.data[1+3*4];
		r.z=(m.data[2+0*4]*p.x) + (m.data[2+1*4]*p.y) + (m.data[2+2*4]*p.z) + m.data[2+3*4];
	}

	public static void mulPoint3Fast(Vector3 r,Matrix4x4 m){
		real tx=(m.data[0+0*4]*r.x) + (m.data[0+1*4]*r.y) + (m.data[0+2*4]*r.z) + m.data[0+3*4];
		real ty=(m.data[1+0*4]*r.x) + (m.data[1+1*4]*r.y) + (m.data[1+2*4]*r.z) + m.data[1+3*4];
		real tz=(m.data[2+0*4]*r.x) + (m.data[2+1*4]*r.y) + (m.data[2+2*4]*r.z) + m.data[2+3*4];
		r.x=tx;
		r.y=ty;
		r.z=tz;
	}

	public static void mulPoint3Full(Vector3 r,Matrix4x4 m,Vector3 p){
		r.x=(m.data[0+0*4]*p.x) + (m.data[0+1*4]*p.y) + (m.data[0+2*4]*p.z) + m.data[0+3*4];
		r.y=(m.data[1+0*4]*p.x) + (m.data[1+1*4]*p.y) + (m.data[1+2*4]*p.z) + m.data[1+3*4];
		r.z=(m.data[2+0*4]*p.x) + (m.data[2+1*4]*p.y) + (m.data[2+2*4]*p.z) + m.data[2+3*4];
		real iw=(1.0f/((m.data[3+0*4]*p.x) + (m.data[3+1*4]*p.y) + (m.data[3+2*4]*p.z) + m.data[3+3*4]));
		r.x*=iw;
		r.y*=iw;
		r.z*=iw;
	}

	public static void mulPoint3Full(Vector3 r,Matrix4x4 m){
		real tx=(m.data[0+0*4]*r.x) + (m.data[0+1*4]*r.y) + (m.data[0+2*4]*r.z) + m.data[0+3*4];
		real ty=(m.data[1+0*4]*r.x) + (m.data[1+1*4]*r.y) + (m.data[1+2*4]*r.z) + m.data[1+3*4];
		real tz=(m.data[2+0*4]*r.x) + (m.data[2+1*4]*r.y) + (m.data[2+2*4]*r.z) + m.data[2+3*4];
		real iw=(1.0f/((m.data[3+0*4]*r.x) + (m.data[3+1*4]*r.y) + (m.data[3+2*4]*r.z) + m.data[3+3*4]));
		r.x=tx*iw;
		r.y=ty*iw;
		r.z=tz*iw;
	}

	public static void transpose(Matrix4x4 r,Matrix4x4 m){
		r.data[0+0*4]=m.data[0+0*4]; r.data[0+1*4]=m.data[1+0*4]; r.data[0+2*4]=m.data[2+0*4]; r.data[0+3*4]=m.data[3+0*4];
		r.data[1+0*4]=m.data[0+1*4]; r.data[1+1*4]=m.data[1+1*4]; r.data[1+2*4]=m.data[2+1*4]; r.data[1+3*4]=m.data[3+1*4];
		r.data[2+0*4]=m.data[0+2*4]; r.data[2+1*4]=m.data[1+2*4]; r.data[2+2*4]=m.data[2+2*4]; r.data[2+3*4]=m.data[3+2*4];
		r.data[3+0*4]=m.data[0+3*4]; r.data[3+1*4]=m.data[1+3*4]; r.data[3+2*4]=m.data[2+3*4]; r.data[3+3*4]=m.data[3+3*4];
	}

	public static real determinant(Matrix4x4 m){
		real det1 = (m.data[1+2*4]*m.data[2+3*4]) - (m.data[2+2*4]*m.data[1+3*4]);
		real det2 = (m.data[1+1*4]*m.data[2+3*4]) - (m.data[2+1*4]*m.data[1+3*4]);
		real det3 = (m.data[1+1*4]*m.data[2+2*4]) - (m.data[2+1*4]*m.data[1+2*4]);
		real det4 = (m.data[1+0*4]*m.data[2+3*4]) - (m.data[2+0*4]*m.data[1+3*4]);
		real det5 = (m.data[1+0*4]*m.data[2+2*4]) - (m.data[2+0*4]*m.data[1+2*4]);
		real det6 = (m.data[1+0*4]*m.data[2+1*4]) - (m.data[2+0*4]*m.data[1+1*4]);

		return -(m.data[3+0*4]*((m.data[0+1*4]*det1) - (m.data[0+2*4]*det2) + (m.data[0+3*4]*det3))) +
				(m.data[3+1*4]*((m.data[0+0*4]*det1) - (m.data[0+2*4]*det4) + (m.data[0+3*4]*det5))) -
				(m.data[3+2*4]*((m.data[0+0*4]*det2) - (m.data[0+1*4]*det4) + (m.data[0+3*4]*det6))) +
				(m.data[3+3*4]*((m.data[0+0*4]*det3) - (m.data[0+1*4]*det5) + (m.data[0+2*4]*det6)));
	}

	public static boolean invert(Matrix4x4 r,Matrix4x4 m){
		real det=determinant(m);

		if(abs(det)==0.0f){
			return false;
		}
		else{
			det=1.0f/det;
			r.data[0+0*4] = (det*((m.data[1+1*4]*((m.data[2+2*4]*m.data[3+3*4]) - (m.data[2+3*4]*m.data[3+2*4]))) + (m.data[1+2*4]*((m.data[2+3*4]*m.data[3+1*4]) - (m.data[2+1*4]*m.data[3+3*4]))) + (m.data[1+3*4]*((m.data[2+1*4]*m.data[3+2*4]) - (m.data[2+2*4]*m.data[3+1*4])))));
			r.data[0+1*4] = (det*((m.data[2+1*4]*((m.data[0+2*4]*m.data[3+3*4]) - (m.data[0+3*4]*m.data[3+2*4]))) + (m.data[2+2*4]*((m.data[0+3*4]*m.data[3+1*4]) - (m.data[0+1*4]*m.data[3+3*4]))) + (m.data[2+3*4]*((m.data[0+1*4]*m.data[3+2*4]) - (m.data[0+2*4]*m.data[3+1*4])))));
			r.data[0+2*4] = (det*((m.data[3+1*4]*((m.data[0+2*4]*m.data[1+3*4]) - (m.data[0+3*4]*m.data[1+2*4]))) + (m.data[3+2*4]*((m.data[0+3*4]*m.data[1+1*4]) - (m.data[0+1*4]*m.data[1+3*4]))) + (m.data[3+3*4]*((m.data[0+1*4]*m.data[1+2*4]) - (m.data[0+2*4]*m.data[1+1*4])))));
			r.data[0+3*4] = (det*((m.data[0+1*4]*((m.data[1+3*4]*m.data[2+2*4]) - (m.data[1+2*4]*m.data[2+3*4]))) + (m.data[0+2*4]*((m.data[1+1*4]*m.data[2+3*4]) - (m.data[1+3*4]*m.data[2+1*4]))) + (m.data[0+3*4]*((m.data[1+2*4]*m.data[2+1*4]) - (m.data[1+1*4]*m.data[2+2*4])))));

			r.data[1+0*4] = (det*((m.data[1+2*4]*((m.data[2+0*4]*m.data[3+3*4]) - (m.data[2+3*4]*m.data[3+0*4]))) + (m.data[1+3*4]*((m.data[2+2*4]*m.data[3+0*4]) - (m.data[2+0*4]*m.data[3+2*4]))) + (m.data[1+0*4]*((m.data[2+3*4]*m.data[3+2*4]) - (m.data[2+2*4]*m.data[3+3*4])))));
			r.data[1+1*4] = (det*((m.data[2+2*4]*((m.data[0+0*4]*m.data[3+3*4]) - (m.data[0+3*4]*m.data[3+0*4]))) + (m.data[2+3*4]*((m.data[0+2*4]*m.data[3+0*4]) - (m.data[0+0*4]*m.data[3+2*4]))) + (m.data[2+0*4]*((m.data[0+3*4]*m.data[3+2*4]) - (m.data[0+2*4]*m.data[3+3*4])))));
			r.data[1+2*4] = (det*((m.data[3+2*4]*((m.data[0+0*4]*m.data[1+3*4]) - (m.data[0+3*4]*m.data[1+0*4]))) + (m.data[3+3*4]*((m.data[0+2*4]*m.data[1+0*4]) - (m.data[0+0*4]*m.data[1+2*4]))) + (m.data[3+0*4]*((m.data[0+3*4]*m.data[1+2*4]) - (m.data[0+2*4]*m.data[1+3*4])))));
			r.data[1+3*4] = (det*((m.data[0+2*4]*((m.data[1+3*4]*m.data[2+0*4]) - (m.data[1+0*4]*m.data[2+3*4]))) + (m.data[0+3*4]*((m.data[1+0*4]*m.data[2+2*4]) - (m.data[1+2*4]*m.data[2+0*4]))) + (m.data[0+0*4]*((m.data[1+2*4]*m.data[2+3*4]) - (m.data[1+3*4]*m.data[2+2*4])))));

			r.data[2+0*4] = (det*((m.data[1+3*4]*((m.data[2+0*4]*m.data[3+1*4]) - (m.data[2+1*4]*m.data[3+0*4]))) + (m.data[1+0*4]*((m.data[2+1*4]*m.data[3+3*4]) - (m.data[2+3*4]*m.data[3+1*4]))) + (m.data[1+1*4]*((m.data[2+3*4]*m.data[3+0*4]) - (m.data[2+0*4]*m.data[3+3*4])))));
			r.data[2+1*4] = (det*((m.data[2+3*4]*((m.data[0+0*4]*m.data[3+1*4]) - (m.data[0+1*4]*m.data[3+0*4]))) + (m.data[2+0*4]*((m.data[0+1*4]*m.data[3+3*4]) - (m.data[0+3*4]*m.data[3+1*4]))) + (m.data[2+1*4]*((m.data[0+3*4]*m.data[3+0*4]) - (m.data[0+0*4]*m.data[3+3*4])))));
			r.data[2+2*4] = (det*((m.data[3+3*4]*((m.data[0+0*4]*m.data[1+1*4]) - (m.data[0+1*4]*m.data[1+0*4]))) + (m.data[3+0*4]*((m.data[0+1*4]*m.data[1+3*4]) - (m.data[0+3*4]*m.data[1+1*4]))) + (m.data[3+1*4]*((m.data[0+3*4]*m.data[1+0*4]) - (m.data[0+0*4]*m.data[1+3*4])))));
			r.data[2+3*4] = (det*((m.data[0+3*4]*((m.data[1+1*4]*m.data[2+0*4]) - (m.data[1+0*4]*m.data[2+1*4]))) + (m.data[0+0*4]*((m.data[1+3*4]*m.data[2+1*4]) - (m.data[1+1*4]*m.data[2+3*4]))) + (m.data[0+1*4]*((m.data[1+0*4]*m.data[2+3*4]) - (m.data[1+3*4]*m.data[2+0*4])))));

			r.data[3+0*4] = (det*((m.data[1+0*4]*((m.data[2+2*4]*m.data[3+1*4]) - (m.data[2+1*4]*m.data[3+2*4]))) + (m.data[1+1*4]*((m.data[2+0*4]*m.data[3+2*4]) - (m.data[2+2*4]*m.data[3+0*4]))) + (m.data[1+2*4]*((m.data[2+1*4]*m.data[3+0*4]) - (m.data[2+0*4]*m.data[3+1*4])))));
			r.data[3+1*4] = (det*((m.data[2+0*4]*((m.data[0+2*4]*m.data[3+1*4]) - (m.data[0+1*4]*m.data[3+2*4]))) + (m.data[2+1*4]*((m.data[0+0*4]*m.data[3+2*4]) - (m.data[0+2*4]*m.data[3+0*4]))) + (m.data[2+2*4]*((m.data[0+1*4]*m.data[3+0*4]) - (m.data[0+0*4]*m.data[3+1*4])))));
			r.data[3+2*4] = (det*((m.data[3+0*4]*((m.data[0+2*4]*m.data[1+1*4]) - (m.data[0+1*4]*m.data[1+2*4]))) + (m.data[3+1*4]*((m.data[0+0*4]*m.data[1+2*4]) - (m.data[0+2*4]*m.data[1+0*4]))) + (m.data[3+2*4]*((m.data[0+1*4]*m.data[1+0*4]) - (m.data[0+0*4]*m.data[1+1*4])))));
			r.data[3+3*4] = (det*((m.data[0+0*4]*((m.data[1+1*4]*m.data[2+2*4]) - (m.data[1+2*4]*m.data[2+1*4]))) + (m.data[0+1*4]*((m.data[1+2*4]*m.data[2+0*4]) - (m.data[1+0*4]*m.data[2+2*4]))) + (m.data[0+2*4]*((m.data[1+0*4]*m.data[2+1*4]) - (m.data[1+1*4]*m.data[2+0*4])))));

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
		real cx=cos(x); \
		real sx=sin(x); \
		\
		r.setAt(0,0,ONE);	r.setAt(0,1,0);		r.setAt(0,2,0); \
		r.setAt(1,0,0);		r.setAt(1,1,cx);	r.setAt(1,2,-sx); \
		r.setAt(2,0,0);		r.setAt(2,1,sx);	r.setAt(2,2,cx);

	#define setMatrixFromY(r,y) \
		real cy=cos(y); \
		real sy=sin(y); \
		\
		r.setAt(0,0,cy);	r.setAt(0,1,0);		r.setAt(0,2,sy); \
		r.setAt(1,0,0);		r.setAt(1,1,ONE);	r.setAt(1,2,0); \
		r.setAt(2,0,-sy);	r.setAt(2,1,0);		r.setAt(2,2,cy);

	#define setMatrixFromZ(r,z) \
		real cz=cos(z); \
		real sz=sin(z); \
		\
		r.setAt(0,0,cz);	r.setAt(0,1,-sz);	r.setAt(0,2,0); \
		r.setAt(1,0,sz);	r.setAt(1,1,cz);	r.setAt(1,2,0); \
		r.setAt(2,0,0);		r.setAt(2,1,0);		r.setAt(2,2,ONE);

	#define setMatrixFromAxisAngle(r,axis,angle) \
		real c=cos(angle); \
		real s=sin(angle); \
		real t=ONE-c; \
		r.setAt(0,0,c + axis.x*axis.x*t); \
		r.setAt(1,1,c + axis.y*axis.y*t); \
		r.setAt(2,2,c + axis.z*axis.z*t); \
		\
		real tmp1=axis.x*axis.y*t; \
		real tmp2=axis.z*s; \
		r.setAt(1,0,tmp1+tmp2); \
		r.setAt(0,1,tmp1-tmp2); \
		\
		tmp1=axis.x*axis.z*t; \
		tmp2=axis.y*s; \
		r.setAt(2,0,tmp1-tmp2); \
		r.setAt(0,2,tmp1+tmp2); \
		\
		tmp1=axis.y*axis.z*t; \
		tmp2=axis.x*s; \
		r.setAt(2,1,tmp1+tmp2); \
		r.setAt(1,2,tmp1-tmp2);

	#define setMatrixFromQuaternion(r,q) \
		real tx  = 2.0f*q.x; \
		real ty  = 2.0f*q.y; \
		real tz  = 2.0f*q.z; \
		real twx = tx*q.w; \
		real twy = ty*q.w; \
		real twz = tz*q.w; \
		real txx = tx*q.x; \
		real txy = ty*q.x; \
		real txz = tz*q.x; \
		real tyy = ty*q.y; \
		real tyz = tz*q.y; \
		real tzz = tz*q.z; \
		\
		r.setAt(0,0,ONE-(tyy+tzz));	r.setAt(0,1,txy-twz);		r.setAt(0,2,txz+twy); \
		r.setAt(1,0,txy+twz);		r.setAt(1,1,ONE-(txx+tzz));	r.setAt(1,2,tyz-twx); \
		r.setAt(2,0,txz-twy);		r.setAt(2,1,tyz+twx);		r.setAt(2,2,ONE-(txx+tyy));

	// Algorithm from Tomas Moller, 1999
	#define setMatrixFromVector3ToVector3(r,from,to,epsilon) \
		Vector3 v=new Vector3(); \
		cross(v,from,to); \
		real e=dot(from,to); \
		\
		if(e>ONE-epsilon){ \
			r.setAt(0,0,ONE);	r.setAt(0,1,0);		r.setAt(0,2,0); \
			r.setAt(1,0,0);		r.setAt(1,1,ONE);	r.setAt(1,2,0); \
			r.setAt(2,0,0);		r.setAt(2,1,0);		r.setAt(2,2,ONE); \
		} \
		else if(e<-ONE+epsilon){ \
			Vector3 up=new Vector3(),left=new Vector3(); \
			real fxx,fyy,fzz,fxy,fxz,fyz; \
			real uxx,uyy,uzz,uxy,uxz,uyz; \
			real lxx,lyy,lzz,lxy,lxz,lyz; \
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
			real hvx,hvz,hvxy,hvxz,hvyz; \
			real h=div(ONE-e,dot(v,v)); \
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

	public static void setMatrix3x3FromX(Matrix3x3 r,real x){ setMatrixFromX(r,x); }

	public static void setMatrix3x3FromY(Matrix3x3 r,real y){ setMatrixFromY(r,y); }

	public static void setMatrix3x3FromZ(Matrix3x3 r,real z){ setMatrixFromZ(r,z); }

	public static void setMatrix3x3FromAxisAngle(Matrix3x3 r,Vector3 axis,real angle){ setMatrixFromAxisAngle(r,axis,angle); }

	public static void setMatrix3x3FromQuaternion(Matrix3x3 r,Quaternion q){ setMatrixFromQuaternion(r,q); }

	public static void setMatrix3x3FromEulerAngleXYZ(Matrix3x3 r,EulerAngle euler){ setMatrixFromEulerAngleXYZ(Matrix3x3,r,euler); }

	public static void setMatrix3x3FromVector3ToVector3(Matrix3x3 r,Vector3 from,Vector3 to,real epsilon){ setMatrixFromVector3ToVector3(r,from,to,epsilon); }

	public static void setMatrix3x3FromMatrix4x4(Matrix3x3 r,Matrix4x4 m){
		r.data[0+0*3]=m.data[0+0*4]; r.data[1+0*3]=m.data[1+0*4]; r.data[2+0*3]=m.data[2+0*4];
		r.data[0+1*3]=m.data[0+1*4]; r.data[1+1*3]=m.data[1+1*4]; r.data[2+1*3]=m.data[2+1*4];
		r.data[0+2*3]=m.data[0+2*4]; r.data[1+2*3]=m.data[1+2*4]; r.data[2+2*3]=m.data[2+2*4];
	}

	// Matrix4x4 advanced operations
	public static void setAxesFromMatrix4x4(Matrix4x4 m,Vector3 xAxis,Vector3 yAxis,Vector3 zAxis){ setAxesFromMatrix(m,xAxis,yAxis,zAxis); }

	public static void setMatrix4x4FromAxes(Matrix4x4 m,Vector3 xAxis,Vector3 yAxis,Vector3 zAxis){ setMatrixFromAxes(m,xAxis,yAxis,zAxis); }

	public static void setMatrix4x4FromX(Matrix4x4 r,real x){ setMatrixFromX(r,x); }

	public static void setMatrix4x4FromY(Matrix4x4 r,real y){ setMatrixFromY(r,y); }

	public static void setMatrix4x4FromZ(Matrix4x4 r,real z){ setMatrixFromZ(r,z); }

	public static void setMatrix4x4FromAxisAngle(Matrix4x4 r,Vector3 axis,real angle){ setMatrixFromAxisAngle(r,axis,angle); }

	public static void setMatrix4x4FromQuaternion(Matrix4x4 r,Quaternion q){ setMatrixFromQuaternion(r,q); }

	public static void setMatrix4x4FromEulerAngleXYZ(Matrix4x4 r,EulerAngle euler){ setMatrixFromEulerAngleXYZ(Matrix4x4,r,euler); }

	public static void setMatrix4x4FromVector3ToVector3(Matrix4x4 r,Vector3 from,Vector3 to,real epsilon){ setMatrixFromVector3ToVector3(r,from,to,epsilon); }

	public static void setMatrix4x4FromMatrix3x3(Matrix4x4 r,Matrix3x3 m){
		r.data[0+0*4]=m.data[0+0*3]; r.data[1+0*4]=m.data[1+0*3]; r.data[2+0*4]=m.data[2+0*3];
		r.data[0+1*4]=m.data[0+1*3]; r.data[1+1*4]=m.data[1+1*3]; r.data[2+1*4]=m.data[2+1*3];
		r.data[0+2*4]=m.data[0+2*3]; r.data[1+2*4]=m.data[1+2*3]; r.data[2+2*4]=m.data[2+2*3];
	}

	public static void setMatrix4x4FromTranslate(Matrix4x4 r,real x,real y,real z){
		r.data[0+3*4]=x;
		r.data[1+3*4]=y;
		r.data[2+3*4]=z;
	}

	public static void setMatrix4x4FromTranslate(Matrix4x4 r,Vector3 translate){
		r.data[0+3*4]=translate.x;
		r.data[1+3*4]=translate.y;
		r.data[2+3*4]=translate.z;
	}

	public static void setMatrix4x4FromScale(Matrix4x4 r,real x,real y,real z){
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
		r.data[0+0*4]=(rotate.data[0+0*3]*scale.x);
		r.data[1+0*4]=(rotate.data[1+0*3]*scale.x);
		r.data[2+0*4]=(rotate.data[2+0*3]*scale.x);
		r.data[0+1*4]=(rotate.data[0+1*3]*scale.y);
		r.data[1+1*4]=(rotate.data[1+1*3]*scale.y);
		r.data[2+1*4]=(rotate.data[2+1*3]*scale.y);
		r.data[0+2*4]=(rotate.data[0+2*3]*scale.z);
		r.data[1+2*4]=(rotate.data[1+2*3]*scale.z);
		r.data[2+2*4]=(rotate.data[2+2*3]*scale.z);
	}

	public static void setMatrix4x4FromTranslateRotateScale(Matrix4x4 r,Vector3 translate,Matrix3x3 rotate,Vector3 scale){
		r.data[0+3*4]=translate.x;
		r.data[1+3*4]=translate.y;
		r.data[2+3*4]=translate.z;
		r.data[0+0*4]=(rotate.data[0+0*3]*scale.x);
		r.data[1+0*4]=(rotate.data[1+0*3]*scale.x);
		r.data[2+0*4]=(rotate.data[2+0*3]*scale.x);
		r.data[0+1*4]=(rotate.data[0+1*3]*scale.y);
		r.data[1+1*4]=(rotate.data[1+1*3]*scale.y);
		r.data[2+1*4]=(rotate.data[2+1*3]*scale.y);
		r.data[0+2*4]=(rotate.data[0+2*3]*scale.z);
		r.data[1+2*4]=(rotate.data[1+2*3]*scale.z);
		r.data[2+2*4]=(rotate.data[2+2*3]*scale.z);
	}

	public static void setMatrix4x4AsTextureRotation(Matrix4x4 r){
		r.data[0+3*4]=r.data[0+0*4]*-HALF + r.data[0+1*4]*-HALF + HALF;
		r.data[1+3*4]=r.data[1+0*4]*-HALF + r.data[1+1*4]*-HALF + HALF;
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

	public static void setMatrix4x4FromOrtho(Matrix4x4 r,real leftDist,real rightDist,real bottomDist,real topDist,real nearDist,real farDist){
		r.set(
			2.0f/(rightDist-leftDist),0,0,-(rightDist+leftDist)/(rightDist-leftDist),
			0,2.0f/(topDist-bottomDist),0,-(topDist+bottomDist)/(topDist-bottomDist),
			0,0,-2.0f/(farDist-nearDist),-(farDist+nearDist)/(farDist-nearDist),
			0,0,0,1.0f
		);
	}

	public static void setMatrix4x4FromFrustum(Matrix4x4 r,real leftDist,real rightDist,real bottomDist,real topDist,real nearDist,real farDist){
		r.set(
			(2.0f*nearDist)/(rightDist-leftDist),0,(rightDist+leftDist)/(rightDist-leftDist),0,
			0,(2.0f*nearDist)/(topDist-bottomDist),(topDist+bottomDist)/(topDist-bottomDist),0,
			0,0,-(farDist+nearDist)/(farDist-nearDist),-(2*farDist*nearDist)/(farDist-nearDist),
			0,0,-1.0f,0
		);
	}

	public static void setMatrix4x4FromPerspectiveX(Matrix4x4 r,real fovx,real aspect,real nearDist,real farDist){
		real h=tan(fovx/2.0f);

		r.set(
			1.0f/h,0,0,0,
			0,1.0f/(h*aspect),0,0,
			0,0,(farDist+nearDist)/(nearDist-farDist),(farDist*nearDist*2.0f)/(nearDist-farDist),
			0,0,-1.0f,0
		);
	}

	public static void setMatrix4x4FromPerspectiveY(Matrix4x4 r,real fovy,real aspect,real nearDist,real farDist){
		real h=tan(fovy/2.0f);

		r.set(
			1.0f/(h*aspect),0,0,0,
			0,1.0f/h,0,0,
			0,0,(farDist+nearDist)/(nearDist-farDist),(farDist*nearDist*2.0f)/(nearDist-farDist),
			0,0,-1.0f,0
		);
	}

	// Axis Angle operations
	public static real setAxisAngleFromQuaternion(Vector3 axis,Quaternion q,real epsilon){
		real angle=2.0f*acos(q.w);

		real sinHalfAngle=sin(angle*0.5f);
		if(sinHalfAngle>epsilon){
			real invSinHalfAngle=1.0f/sinHalfAngle;
			axis.x=q.x*invSinHalfAngle;
			axis.y=q.y*invSinHalfAngle;
			axis.z=q.z*invSinHalfAngle;

			real l=1.0f/(sqrt((axis.x*axis.x+axis.y*axis.y+axis.z*axis.z)));
			axis.x*=l;
			axis.y*=l;
			axis.z*=l;
		}
		else{
			axis.x=1.0f;
			axis.y=0.0f;
			axis.z=0.0f;
		}

		return angle;
	}

	// Quaternion operations
	public static void mul(Quaternion r,Quaternion q1,Quaternion q2){
		r.x=+q1.x*q2.w+q1.y*q2.z-q1.z*q2.y+q1.w*q2.x;
		r.y=-q1.x*q2.z+q1.y*q2.w+q1.z*q2.x+q1.w*q2.y;
		r.z=+q1.x*q2.y-q1.y*q2.x+q1.z*q2.w+q1.w*q2.z;
		r.w=-q1.x*q2.x-q1.y*q2.y-q1.z*q2.z+q1.w*q2.w;
	}

	public static void postMul(Quaternion q1,Quaternion q2){
		real x=+q1.x*q2.w+q1.y*q2.z-q1.z*q2.y+q1.w*q2.x;
		real y=-q1.x*q2.z+q1.y*q2.w+q1.z*q2.x+q1.w*q2.y;
		real z=+q1.x*q2.y-q1.y*q2.x+q1.z*q2.w+q1.w*q2.z;
		real w=-q1.x*q2.x-q1.y*q2.y-q1.z*q2.z+q1.w*q2.w;
		q1.x=x;
		q1.y=y;
		q1.z=z;
		q1.w=w;
	}

	public static void preMul(Quaternion q2,Quaternion q1){
		real x=+q1.x*q2.w+q1.y*q2.z-q1.z*q2.y+q1.w*q2.x;
		real y=-q1.x*q2.z+q1.y*q2.w+q1.z*q2.x+q1.w*q2.y;
		real z=+q1.x*q2.y-q1.y*q2.x+q1.z*q2.w+q1.w*q2.z;
		real w=-q1.x*q2.x-q1.y*q2.y-q1.z*q2.z+q1.w*q2.w;
		q2.x=x;
		q2.y=y;
		q2.z=z;
		q2.w=w;
	}

	public static real length(Quaternion q){
		return sqrt(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
	}

	public static real length(Quaternion q1,Quaternion q2){
		real x=q1.x-q2.x;
		real y=q1.y-q2.y;
		real z=q1.z-q2.z;
		real w=q1.w-q2.w;
		return sqrt((x*x + y*y + z*z + w*w));
	}

	public static real lengthSquared(Quaternion q){
		return q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
	}

	public static real lengthSquared(Quaternion q1,Quaternion q2){
		real x=q1.x-q2.x;
		real y=q1.y-q2.y;
		real z=q1.z-q2.z;
		real w=q1.w-q2.w;
		return x*x + y*y + z*z + w*w;
	}

	public static void normalize(Quaternion q){
		real l=1.0f/length(q);
		q.x*=l;
		q.y*=l;
		q.z*=l;
		q.w*=l;
	}

	public static void normalize(Quaternion r,Quaternion q){
		real l=1.0f/length(q);
		r.x=q.x*l;
		r.y=q.y*l;
		r.z=q.z*l;
		r.w=q.w*l;
	}

	public static boolean normalizeCarefully(Quaternion q,real epsilon){
		real l=length(q);
		if(l>epsilon){
			l=1.0f/l;
			q.x*=l;
			q.y*=l;
			q.z*=l;
			q.w*=l;
			return true;
		}
		return false;
	}

	public static boolean normalizeCarefully(Quaternion r,Quaternion q,real epsilon){
		real l=length(q);
		if(l>epsilon){
			l=1.0f/l;
			r.x=q.x*l;
			r.y=q.y*l;
			r.z=q.z*l;
			r.w=q.w*l;
			return true;
		}
		return false;
	}

	public static real dot(Quaternion q1,Quaternion q2){
		return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
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
		real i=1.0f/((q.x*q.x)+(q.y*q.y)+(q.z*q.z)+(q.w*q.w));
		q.x*=-i;
		q.y*=-i;
		q.z*=-i;
		q.w*=i;
	}

	public static void invert(Quaternion r,Quaternion q){
		real i=1.0f/((q.x*q.x)+(q.y*q.y)+(q.z*q.z)+(q.w*q.w));
		r.x=q.x*-i;
		r.y=q.y*-i;
		r.z=q.z*-i;
		r.w=q.w*i;
	}

	private static int[] quaternionFromMatrix3x3_next={1,2,0};
	public static void setQuaternionFromMatrix3x3(Quaternion r,Matrix3x3 mat){
		// Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
		// article "Quaternion Calculus and Fast Animation".

		real trace=mat.at(0,0)+mat.at(1,1)+mat.at(2,2);
		real root;

		if(trace>0){
			// |w| > 1/2, may as well choose w > 1/2
			root=sqrt(trace+ONE); // 2w
			r.w=0.5f*root;
			root=0.5f/root; // 1/(4w)
			r.x=(mat.at(2,1)-mat.at(1,2))*root;
			r.y=(mat.at(0,2)-mat.at(2,0))*root;
			r.z=(mat.at(1,0)-mat.at(0,1))*root;
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

			root=sqrt(mat.at(i,i)-mat.at(j,j)-mat.at(k,k)+1.0f);
			trace=0.5f*root; // trace used as temp in the below code, and as a dummy assignment
			trace=(i==0?(r.x=trace):(i==1?(r.y=trace):(r.z=trace)));
			root=0.5f/root;
			r.w=(mat.at(k,j)-mat.at(j,k))*root;
			trace=(mat.at(j,i)+mat.at(i,j))*root;
			trace=(j==0?(r.x=trace):(j==1?(r.y=trace):(r.z=trace)));
			trace=(mat.at(k,i)+mat.at(i,k))*root;
			trace=(k==0?(r.x=trace):(k==1?(r.y=trace):(r.z=trace)));
		}
	}

	public static void setQuaternionFromAxisAngle(Quaternion r,Vector3 axis,real angle){
		real halfAngle=angle*0.5f;
		real sinHalfAngle=sin(halfAngle);
		r.x=axis.x*sinHalfAngle;
		r.y=axis.y*sinHalfAngle;
		r.z=axis.z*sinHalfAngle;
		r.w=cos(halfAngle);
	}

	public static void lerp(Quaternion r,Quaternion q1,Quaternion q2,real t){
		real cosom=dot(q1,q2);

		real scl1=(1.0f-t);
		real scl2=0;
		if(cosom<0.0f){
			scl2=-t;
		}
		else{
			scl2=t;
		}

		r.x=scl1*q1.x + scl2*q2.x;
		r.y=scl1*q1.y + scl2*q2.y;
		r.z=scl1*q1.z + scl2*q2.z;
		r.w=scl1*q1.w + scl2*q2.w;
	}

	// The logic behind this slerp function is from the GGT (http://ggt.sourceforge.net)
	public static void slerp(Quaternion r,Quaternion q1,Quaternion q2,real t){
		real cosom=dot(q1,q2);

		real scl2sign=1.0f;
		if(cosom<0.0f){
			cosom=-cosom;
			scl2sign=-1.0f;
		}

		real scl1=0;
		real scl2=0;
		if((1.0f-cosom) > 0.0001){
			// Standard slerp
			real omega=acos(cosom);
			real sinom=sin(omega);
			scl1=sin((1.0f-t) * omega)/sinom;
			scl2=sin(t*omega)/sinom;
		}
		else{
			// Very close, just lerp
			scl1=1.0f-t;
			scl2=t;
		}
		scl2*=scl2sign;

		r.x=scl1*q1.x + scl2*q2.x;
		r.y=scl1*q1.y + scl2*q2.y;
		r.z=scl1*q1.z + scl2*q2.z;
		r.w=scl1*q1.w + scl2*q2.w;
	}

	// Segment operations
	public static void getClosestPointOnSegment(Vector3 result,Segment segment,Vector3 point){
		Vector3 o=segment.origin;
		Vector3 d=segment.direction;
		
		if(d.x==0 && d.y==0 && d.z==0){
			result=o;
		}
		else{
			real u=(d.x*(point.x-o.x)+d.y*(point.y-o.y)+d.z*(point.z-o.z))/(d.x*d.x+d.y*d.y+d.z*d.z);
			if(u<0) u=0;
			else if(u>1) u=1;
			result.x=o.x+u*(d.x);
			result.y=o.y+u*(d.y);
			result.z=o.z+u*(d.z);
		}
	}

	public static void getClosestPointsOnSegments(Vector3 point1,Vector3 point2,Segment seg1,Segment seg2,real epsilon){
		// For a full discussion of this method see Dan Sunday's Geometry Algorithms web site: http://www.geometryalgorithms.com/
		real a=dot(seg1.direction,seg1.direction);
		real b=dot(seg1.direction,seg2.direction);
		real c=dot(seg2.direction,seg2.direction);

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
		real d=dot(seg1.direction,point1);
		real e=dot(seg2.direction,point1);
		real denom=a*c-b*b;
		
		real u1=0;
		real u2=0;
		real u1N=0;
		real u2N=0;
		real u1D=denom;
		real u2D=denom;
		
		if(denom<0.000001){
			// Segments are practically parallel
			u1N=0;
			u1D=1;
			u2N=e;
			u2D=c;
		}
		else{
			// Find the closest point on 2 infinite lines
			u1N=(b*e-c*d);
			u2N=(a*e-b*d);
			
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
		
		u1=u1N/u1D;
		u2=u2N/u2D;
		
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

		real fx=(real)(x-(int)viewportX)*2.0f/(real)viewportWidth-1.0f;
		real fy=(real)((int)viewportHeight-(y-(int)viewportY))*2.0f/(real)viewportHeight-1.0f;

		result.origin.set(fx,fy,-1.0f);
		mulPoint3Full(result.origin,scratchMatrix);

		result.direction.set(fx,fy,1.0f);
		mulPoint3Full(result.direction,scratchMatrix);

		sub(result.direction,result.origin);

		return true;
	}

	// Plane operations
	public static void project(Plane plane,Vector3 result,Vector3 point){
		result.x=point.x - plane.normal.x*plane.distance;
		result.y=point.y - plane.normal.y*plane.distance;
		result.z=point.z - plane.normal.z*plane.distance;

		real f=plane.normal.x*result.x + plane.normal.y*result.y + plane.normal.z*result.z;

		result.x=point.x - plane.normal.x*f;
		result.y=point.y - plane.normal.y*f;
		result.z=point.z - plane.normal.z*f;
	}

	public static void project(Plane plane,Vector3 point){
		real tx=point.x - plane.normal.x*plane.distance;
		real ty=point.y - plane.normal.y*plane.distance;
		real tz=point.z - plane.normal.z*plane.distance;

		real f=plane.normal.x*tx + plane.normal.y*ty + plane.normal.z*tz;

		point.x=point.x - plane.normal.x*f;
		point.y=point.y - plane.normal.y*f;
		point.z=point.z - plane.normal.z*f;
	}

	public static void normalize(Plane plane){
		real l=1.0f/length(plane.normal);
		plane.normal.x*=l;
		plane.normal.y*=l;
		plane.normal.z*=l;
		plane.distance*=l;
	}

	public static void normalize(Plane r,Plane plane){
		real l=1.0f/length(plane.normal);
		r.normal.x=plane.normal.x*l;
		r.normal.y=plane.normal.y*l;
		r.normal.z=plane.normal.z*l;
		r.distance=plane.distance*l;
	}

	public static real length(Plane p,Vector3 v){
		return p.normal.x*v.x + p.normal.y*v.y + p.normal.z*v.z - p.distance;
	}

	public static boolean getIntersectionOfThreePlanes(Vector3 result,Plane p1,Plane p2,Plane p3,real epsilon){
		Vector3 p2xp3=new Vector3();
		cross(p2xp3,p2.normal,p3.normal);
		real den=dot(p1.normal,p2xp3);
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
		real determinant=lengthSquared(plane1.normal)*lengthSquared(plane2.normal) - (dot(plane1.normal,plane2.normal)*dot(plane1.normal,plane2.normal));

		real c1=(plane1.distance*lengthSquared(plane2.normal) - plane2.distance*dot(plane1.normal,plane2.normal))/determinant;
		real c2=(plane2.distance*lengthSquared(plane1.normal) - plane1.distance*dot(plane1.normal,plane2.normal))/determinant;

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

		real f=0;

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

		r.origin.x=0.5f*(mins.x+maxs.x);
		r.origin.y=0.5f*(mins.y+maxs.y);
		r.origin.z=0.5f*(mins.z+maxs.z);

		r.setRadius(sqrt((maxs.x-r.origin.x)*(maxs.x-r.origin.x)+(maxs.y-r.origin.y)*(maxs.y-r.origin.y)+(maxs.z-r.origin.z)*(maxs.z-r.origin.z)));
	}

	public static void findFitSphere(Sphere r,AABox box){
		Vector3 mins=box.mins;
		Vector3 maxs=box.maxs;

		r.origin.x=0.5f*(mins.x+maxs.x);
		r.origin.y=0.5f*(mins.y+maxs.y);
		r.origin.z=0.5f*(mins.z+maxs.z);

		r.setRadius(sqrt((maxs.x-r.origin.x)*(maxs.x-r.origin.x)+(maxs.y-r.origin.y)*(maxs.y-r.origin.y)+(maxs.z-r.origin.z)*(maxs.z-r.origin.z))/sqrt(3));
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
		Vector3 maxs=box.mins;

		r.origin.x=0.5f*(mins.x+maxs.x);
		r.origin.y=0.5f*(mins.y+maxs.y);
		r.origin.z=mins.z;

		r.direction.z=(maxs.z-mins.z);

		r.setRadius(sqrt((maxs.x-r.origin.x)*(maxs.x-r.origin.x)+(maxs.y-r.origin.y)*(maxs.y-r.origin.y)));
	}

	void findFitCapsule(Capsule r,AABox box){
		Vector3 mins=box.mins;
		Vector3 maxs=box.mins;

		r.origin.x=0.5f*(mins.x+maxs.x);
		r.origin.y=0.5f*(mins.y+maxs.y);

		r.setRadius(sqrt((maxs.x-r.origin.x)*(maxs.x-r.origin.x)+(maxs.y-r.origin.y)*(maxs.y-r.origin.y))/SQRT_2);

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
		real s=0;
		real d=0;

		if(sphere.origin.x<box.mins.x){
			s=sphere.origin.x-box.mins.x;
			d+=s*s;
		}
		else if(sphere.origin.x>box.maxs.x){
			s=sphere.origin.x-box.maxs.x;
			d+=s*s;
		}

		if(sphere.origin.y<box.mins.y){
			s=sphere.origin.y-box.mins.y;
			d+=s*s;
		}
		else if(sphere.origin.y>box.maxs.y){
			s=sphere.origin.y-box.maxs.y;
			d+=s*s;
		}

		if(sphere.origin.z<box.mins.z){
			s=sphere.origin.z-box.mins.z;
			d+=s*s;
		}
		else if(sphere.origin.z>box.maxs.z){
			s=sphere.origin.z-box.maxs.z;
			d+=s*s;
		}

		return d<=sphere.radius*sphere.radius;
	}

	public static real findIntersection(Segment segment,Plane plane,Vector3 point,Vector3 normal){
		real d=dot(plane.normal,segment.direction);
		if(d!=0){
			real t=(plane.distance-dot(plane.normal,segment.origin))/d;
			mul(point,segment.direction,t);
			add(point,segment.origin);
			normal.set(plane.normal);
			return t;
		}
		else{
			return -1;
		}
	}

	public static real findIntersection(Segment segment,Sphere sphere,Vector3 point,Vector3 normal){
		Vector3 segOrigin=segment.origin;
		Vector3 segDirection=segment.direction;
		Vector3 sphOrigin=sphere.origin;
		real sphRadius=sphere.radius;

		// Quadratic Q(t) = a*t^2 + 2*b*t + c
		Vector3 diff=new Vector3();
		sub(diff,segOrigin,sphOrigin);
		real a=lengthSquared(segDirection);
		if(a<=0.0f){
			return -1.0f;
		}
		real b=dot(diff,segDirection);
		real c=lengthSquared(diff)-square(sphRadius);
		real time1=-1.0f;
		real time2=-1.0f;

		// If no real roots, then no intersection
		real discr=b*b - a*c;
		if(discr<0.0f){
			time1=-1.0f;
		}
		else if(discr>0.0f){
			real root=sqrt(discr);
			real invA=1.0f/a;
			time1=((-b)-root)*invA;
			time2=((-b)+root)*invA;

			if(time1>1.0f || time2<0.0f){
				time1=-1.0f;
			}
			else if(time1>=0.0f){
				mul(point,segDirection,time1);
				add(point,segOrigin);
			}
			else{
				mul(point,segDirection,time2);
				add(point,segOrigin);
			}
		}
		else{
			time1=-b/a;
			if(0.0f<=time1 && time1<=1){
				mul(point,segDirection,time1);
				add(point,segOrigin);
			}
			else{
				time1=-1.0f;
			}
		}

		if(time1!=-1.0f){
			sub(normal,point,sphOrigin);
			normalizeCarefully(normal,0);
		}
		return time1;
	}

	public static real findIntersection(Segment segment,AABox box,Vector3 point,Vector3 normal){
		Vector3 segOrigin=segment.origin;
		Vector3 segDirection=segment.direction;
		Vector3 boxMins=box.mins;
		Vector3 boxMaxs=box.maxs;
		
		boolean inside=true;
		char quadX,quadY,quadZ;
		int whichPlane;
		real maxTX=0,maxTY=0,maxTZ=0;
		real candPlaneX=0,candPlaneY=0,candPlaneZ=0;
		real candNormX=-1.0f;
		real candNormY=-1.0f;
		real candNormZ=-1.0f;
		real time=-1.0f;

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
			candNormX=1.0f;
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
			candNormY=1.0f;
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
			candNormZ=1.0f;
			inside=false;
		}
		else{
			quadZ=2; // Middle
		}

		// Inside
		if(inside){
			return -2.0f;
		}

		// Calculate t distances to candidate planes
		// X
		if(quadX!=2 /*Middle*/ && segDirection.x!=0){
			maxTX=(candPlaneX-segOrigin.x)/segDirection.x;
		}
		else{
			maxTX=-1.0f;
		}
		// Y
		if(quadY!=2 /*Middle*/ && segDirection.y!=0){
			maxTY=(candPlaneY-segOrigin.y)/segDirection.y;
		}
		else{
			maxTY=-1.0f;
		}
		// Z
		if(quadZ!=2 /*Middle*/ && segDirection.z!=0){
			maxTZ=(candPlaneZ-segOrigin.z)/segDirection.z;
		}
		else{
			maxTZ=-1.0f;
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
		if(time<0.0f || time>1.0f){
			return -1.0f;
		}
		// X
		if(whichPlane!=0){
			point.x=segOrigin.x+time*segDirection.x;
			if(point.x<boxMins.x || point.x>boxMaxs.x){
				return -1.0f;
			}
		}
		else{
			point.x=candPlaneX;
		}
		// Y
		if(whichPlane!=1){
			point.y=segOrigin.y+time*segDirection.y;
			if(point.y<boxMins.y || point.y>boxMaxs.y){
				return -1.0f;
			}
		}
		else{
			point.y=candPlaneY;
		}
		// Z
		if(whichPlane!=2){
			point.z=segOrigin.z+time*segDirection.z;
			if(point.z<boxMins.z || point.z>boxMaxs.z){
				return -1.0f;
			}
		}
		else{
			point.z=candPlaneZ;
		}
		
		return time;
	}
}
