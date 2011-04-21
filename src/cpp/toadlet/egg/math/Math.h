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

#ifndef TOADLET_EGG_MATH_MATH_H
#define TOADLET_EGG_MATH_MATH_H

#include <toadlet/egg/math/BaseMath.h>
#include <toadlet/egg/math/Vector2.h>
#include <toadlet/egg/math/Vector3.h>
#include <toadlet/egg/math/Vector4.h>
#include <toadlet/egg/math/Matrix3x3.h>
#include <toadlet/egg/math/Matrix4x4.h>
#include <toadlet/egg/math/Quaternion.h>
#include <toadlet/egg/math/EulerAngle.h>
#include <toadlet/egg/math/Segment.h>
#include <toadlet/egg/math/Plane.h>
#include <toadlet/egg/math/Sphere.h>
#include <toadlet/egg/math/AABox.h>
#include <toadlet/egg/math/Capsule.h>
#include <toadlet/egg/math/ConvexSolid.h>

namespace toadlet{
namespace egg{
namespace math{

namespace Math{
	const Vector2 ZERO_VECTOR2;
	const Vector2 ONE_VECTOR2(ONE,ONE);
	const Vector2 X_UNIT_VECTOR2(ONE,0);
	const Vector2 NEG_X_UNIT_VECTOR2(-ONE,0);
	const Vector2 Y_UNIT_VECTOR2(0,ONE);
	const Vector2 NEG_Y_UNIT_VECTOR2(0,-ONE);
	const Vector3 ZERO_VECTOR3;
	const Vector3 ONE_VECTOR3(ONE,ONE,ONE);
	const Vector3 X_UNIT_VECTOR3(ONE,0,0);
	const Vector3 NEG_X_UNIT_VECTOR3(-ONE,0,0);
	const Vector3 Y_UNIT_VECTOR3(0,ONE,0);
	const Vector3 NEG_Y_UNIT_VECTOR3(0,-ONE,0);
	const Vector3 Z_UNIT_VECTOR3(0,0,ONE);
	const Vector3 NEG_Z_UNIT_VECTOR3(0,0,-ONE);
	const Vector4 ZERO_VECTOR4;
	const Vector4 ONE_VECTOR4(ONE,ONE,ONE,ONE);
	const Vector4 X_UNIT_VECTOR4(ONE,0,0,0);
	const Vector4 NEG_X_UNIT_VECTOR4(-ONE,0,0,0);
	const Vector4 Y_UNIT_VECTOR4(0,ONE,0,0);
	const Vector4 NEG_Y_UNIT_VECTOR4(0,-ONE,0,0);
	const Vector4 Z_UNIT_VECTOR4(0,0,ONE,0);
	const Vector4 NEG_Z_UNIT_VECTOR4(0,0,-ONE,0);
	const Vector4 W_UNIT_VECTOR4(0,0,0,ONE);
	const Vector4 NEG_W_UNIT_VECTOR4(0,0,0,-ONE);
	const Matrix3x3 IDENTITY_MATRIX3X3;
	const Matrix4x4 IDENTITY_MATRIX4X4;
	const Quaternion IDENTITY_QUATERNION;
	const EulerAngle IDENTITY_EULERANGLE;
	const AABox ZERO_AABOX;

	// Vector2 operations
	inline void neg(Vector2 &r,const Vector2 &v){
		r.x=-v.x;
		r.y=-v.y;
	}

	inline void neg(Vector2 &v){
		v.x=-v.x;
		v.y=-v.y;
	}

	inline void add(Vector2 &r,const Vector2 &v1,const Vector2 &v2){
		r.x=v1.x+v2.x;
		r.y=v1.y+v2.y;
	}

	inline void add(Vector2 &r,const Vector2 &v){
		r.x+=v.x;
		r.y+=v.y;
	}
	
	inline void sub(Vector2 &r,const Vector2 &v1,const Vector2 &v2){
		r.x=v1.x-v2.x;
		r.y=v1.y-v2.y;
	}
	
	inline void sub(Vector2 &r,const Vector2 &v){
		r.x-=v.x;
		r.y-=v.y;
	}

	inline void mul(Vector2 &r,const Vector2 &v,real f){
		r.x=v.x*f;
		r.y=v.y*f;
	}
	
	inline void mul(Vector2 &r,real f){
		r.x*=f;
		r.y*=f;
	}

	inline void mul(Vector2 &r,const Vector2 &v1,const Vector2 &v2){
		r.x=v1.x*v2.x;
		r.y=v1.y*v2.y;
	}

	inline void mul(Vector2 &r,const Vector2 &v){
		r.x*=v.x;
		r.y*=v.y;
	}

	inline void div(Vector2 &r,const Vector2 &v,real f){
		real i=1.0/f;
		r.x=v.x*i;
		r.y=v.y*i;
	}
	
	inline void div(Vector2 &r,real f){
		real i=1.0/f;
		r.x*=i;
		r.y*=i;
	}

	inline void div(Vector2 &r,const Vector2 &v1,const Vector2 &v2){
		r.x=v1.x/v2.x;
		r.y=v1.y/v2.y;
	}

	inline void div(Vector2 &r,const Vector2 &v){
		r.x/=v.x;
		r.y/=v.y;
	}

	inline void madd(Vector2 &r,const Vector2 &b,real c,const Vector2 &a){
		r.x=madd(b.x,c,a.x);
		r.y=madd(b.y,c,a.y);
	}

	inline void madd(Vector2 &r,real c,const Vector2 &a){
		r.x=madd(r.x,c,a.x);
		r.y=madd(r.y,c,a.y);
	}

	inline void msub(Vector2 &r,const Vector2 &b,real c,const Vector2 &a){
		r.x=msub(b.x,c,a.x);
		r.y=msub(b.y,c,a.y);
	}

	inline void msub(Vector2 &r,real c,const Vector2 &a){
		r.x=msub(r.x,c,a.x);
		r.y=msub(r.y,c,a.y);
	}

	inline real lengthSquared(const Vector2 &v){
		return v.x*v.x + v.y*v.y;
	}

	inline real lengthSquared(const Vector2 &v1,const Vector2 &v2){
		real x=v1.x-v2.x;
		real y=v1.y-v2.y;
		return x*x + y*y;
	}

	inline real length(const Vector2 &v){
		return sqrt(lengthSquared(v));
	}

	inline real length(const Vector2 &v1,const Vector2 &v2){
		return sqrt(lengthSquared(v1,v2));
	} 

	inline void normalize(Vector2 &v){
		real l=1.0/length(v);
		v.x*=l;
		v.y*=l;
	}

	inline void normalize(Vector2 &r,const Vector2 &v){
		real l=1.0/length(v);
		r.x=v.x*l;
		r.y=v.y*l;
	}

	inline bool normalizeCarefully(Vector2 &v,real epsilon){
		real l=length(v);
		if(l>epsilon){
			l=1.0/l;
			v.x*=l;
			v.y*=l;
			return true;
		}
		return false;
	}

	inline bool normalizeCarefully(Vector2 &r,const Vector2 &v,real epsilon){
		real l=length(v);
		if(l>epsilon){
			l=1.0/l;
			r.x=v.x*l;
			r.y=v.y*l;
			return true;
		}
		return false;
	}

	inline real dot(const Vector2 &v1,const Vector2 &v2){
		return v1.x*v2.x + v1.y*v2.y;
	}

	inline void lerp(Vector2 &r,const Vector2 &v1,const Vector2 &v2,real t){
		sub(r,v2,v1);
		mul(r,t);
		add(r,v1);
	}

	// Vector3 operations
	inline void neg(Vector3 &r,const Vector3 &v){
		r.x=-v.x;
		r.y=-v.y;
		r.z=-v.z;
	}

	inline void neg(Vector3 &v){
		v.x=-v.x;
		v.y=-v.y;
		v.z=-v.z;
	}

	inline void add(Vector3 &r,const Vector3 &v1,const Vector3 &v2){
		r.x=v1.x+v2.x;
		r.y=v1.y+v2.y;
		r.z=v1.z+v2.z;
	}

	inline void add(Vector3 &r,const Vector3 &v){
		r.x+=v.x;
		r.y+=v.y;
		r.z+=v.z;
	}
	
	inline void sub(Vector3 &r,const Vector3 &v1,const Vector3 &v2){
		r.x=v1.x-v2.x;
		r.y=v1.y-v2.y;
		r.z=v1.z-v2.z;
	}
	
	inline void sub(Vector3 &r,const Vector3 &v){
		r.x-=v.x;
		r.y-=v.y;
		r.z-=v.z;
	}

	inline void mul(Vector3 &r,const Vector3 &v,real f){
		r.x=v.x*f;
		r.y=v.y*f;
		r.z=v.z*f;
	}
	
	inline void mul(Vector3 &r,real f){
		r.x*=f;
		r.y*=f;
		r.z*=f;
	}

	inline void mul(Vector3 &r,const Vector3 &v1,const Vector3 &v2){
		r.x=v1.x*v2.x;
		r.y=v1.y*v2.y;
		r.z=v1.z*v2.z;
	}

	inline void mul(Vector3 &r,const Vector3 &v){
		r.x*=v.x;
		r.y*=v.y;
		r.z*=v.z;
	}

	inline void div(Vector3 &r,const Vector3 &v,real f){
		real i=1.0/f;
		r.x=v.x*i;
		r.y=v.y*i;
		r.z=v.z*i;
	}
	
	inline void div(Vector3 &r,real f){
		real i=1.0/f;
		r.x*=i;
		r.y*=i;
		r.z*=i;
	}

	inline void div(Vector3 &r,const Vector3 &v1,const Vector3 &v2){
		r.x=v1.x/v2.x;
		r.y=v1.y/v2.y;
		r.y=v1.z/v2.z;
	}

	inline void div(Vector3 &r,const Vector3 &v){
		r.x/=v.x;
		r.y/=v.y;
		r.z/=v.z;
	}

	inline void madd(Vector3 &r,const Vector3 &b,real c,const Vector3 &a){
		r.x=madd(b.x,c,a.x);
		r.y=madd(b.y,c,a.y);
		r.z=madd(b.z,c,a.z);
	}

	inline void madd(Vector3 &r,real c,const Vector3 &a){
		r.x=madd(r.x,c,a.x);
		r.y=madd(r.y,c,a.y);
		r.z=madd(r.z,c,a.z);
	}

	inline void msub(Vector3 &r,const Vector3 &b,real c,const Vector3 &a){
		r.x=msub(b.x,c,a.x);
		r.y=msub(b.y,c,a.y);
		r.z=msub(b.z,c,a.z);
	}

	inline void msub(Vector3 &r,real c,const Vector3 &a){
		r.x=msub(r.x,c,a.x);
		r.y=msub(r.y,c,a.y);
		r.z=msub(r.z,c,a.z);
	}

	inline real lengthSquared(const Vector3 &v){
		return v.x*v.x + v.y*v.y + v.z*v.z;
	}

	inline real lengthSquared(const Vector3 &v1,const Vector3 &v2){
		real x=v1.x-v2.x;
		real y=v1.y-v2.y;
		real z=v1.z-v2.z;
		return x*x + y*y + z*z;
	}

	inline real length(const Vector3 &v){
		return sqrt(lengthSquared(v));
	}

	inline real length(const Vector3 &v1,const Vector3 &v2){
		return sqrt(lengthSquared(v1,v2));
	}

	inline void normalize(Vector3 &v){
		real l=1.0/length(v);
		v.x*=l;
		v.y*=l;
		v.z*=l;
	}

	inline void normalize(Vector3 &r,const Vector3 &v){
		real l=1.0/length(v);
		r.x=v.x*l;
		r.y=v.y*l;
		r.z=v.z*l;
	}

	inline bool normalizeCarefully(Vector3 &v,real epsilon){
		real l=length(v);
		if(l>epsilon){
			l=1.0/l;
			v.x*=l;
			v.y*=l;
			v.z*=l;
			return true;
		}
		return false;
	}

	inline bool normalizeCarefully(Vector3 &r,const Vector3 &v,real epsilon){
		real l=length(v);
		if(l>epsilon){
			l=1.0/l;
			r.x=v.x*l;
			r.y=v.y*l;
			r.z=v.z*l;
			return true;
		}
		return false;
	}

	inline real dot(const Vector3 &v1,const Vector3 &v2){
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z;
	}

	inline void lerp(Vector3 &r,const Vector3 &v1,const Vector3 &v2,real t){
		sub(r,v2,v1);
		mul(r,t);
		add(r,v1);
	}

	inline void cross(Vector3 &r,const Vector3 &v1,const Vector3 &v2){
		r.x=v1.y*v2.z-v1.z*v2.y;
		r.y=v1.z*v2.x-v1.x*v2.z;
		r.z=v1.x*v2.y-v1.y*v2.x;
	}

	// Vector4 operations
	inline void neg(Vector4 &r,const Vector4 &v){
		r.x=-v.x;
		r.y=-v.y;
		r.z=-v.z;
		r.w=-v.w;
	}

	inline void neg(Vector4 &v){
		v.x=-v.x;
		v.y=-v.y;
		v.z=-v.z;
		v.w=-v.w;
	}

	inline void add(Vector4 &r,const Vector4 &v1,const Vector4 &v2){
		r.x=v1.x+v2.x;
		r.y=v1.y+v2.y;
		r.z=v1.z+v2.z;
		r.w=v1.w+v2.w;
	}

	inline void add(Vector4 &r,const Vector4 &v){
		r.x+=v.x;
		r.y+=v.y;
		r.z+=v.z;
		r.w+=v.w;
	}
	
	inline void sub(Vector4 &r,const Vector4 &v1,const Vector4 &v2){
		r.x=v1.x-v2.x;
		r.y=v1.y-v2.y;
		r.z=v1.z-v2.z;
		r.w=v1.w-v2.w;
	}
	
	inline void sub(Vector4 &r,const Vector4 &v){
		r.x-=v.x;
		r.y-=v.y;
		r.z-=v.z;
		r.w-=v.w;
	}

	inline void mul(Vector4 &r,const Vector4 &v,real f){
		r.x=v.x*f;
		r.y=v.y*f;
		r.z=v.z*f;
		r.w=v.w*f;
	}
	
	inline void mul(Vector4 &r,real f){
		r.x*=f;
		r.y*=f;
		r.z*=f;
		r.w*=f;
	}

	inline void mul(Vector4 &r,const Vector4 &v1,const Vector4 &v2){
		r.x=v1.x*v2.x;
		r.y=v1.y*v2.y;
		r.z=v1.z*v2.z;
		r.w=v1.z*v2.w;
	}

	inline void mul(Vector4 &r,const Vector4 &v){
		r.x*=v.x;
		r.y*=v.y;
		r.z*=v.z;
		r.w*=v.w;
	}

	inline void div(Vector4 &r,const Vector4 &v,real f){
		real i=1.0/f;
		r.x=v.x*i;
		r.y=v.y*i;
		r.z=v.z*i;
		r.w=v.w*i;
	}
	
	inline void div(Vector4 &r,real f){
		real i=1.0/f;
		r.x*=i;
		r.y*=i;
		r.z*=i;
		r.w*=i;
	}

	inline void div(Vector4 &r,const Vector4 &v1,const Vector4 &v2){
		r.x=v1.x/v2.x;
		r.y=v1.y/v2.y;
		r.z=v1.z/v2.z;
		r.w=v1.w/v2.w;
	}

	inline void div(Vector4 &r,const Vector4 &v){
		r.x/=v.x;
		r.y/=v.y;
		r.z/=v.z;
		r.w/=v.w;
	}

	inline void madd(Vector4 &r,const Vector4 &b,real c,const Vector4 &a){
		r.x=madd(b.x,c,a.x);
		r.y=madd(b.y,c,a.y);
		r.z=madd(b.z,c,a.z);
		r.w=madd(b.w,c,a.w);
	}

	inline void madd(Vector4 &r,real c,const Vector4 &a){
		r.x=madd(r.x,c,a.x);
		r.y=madd(r.y,c,a.y);
		r.z=madd(r.z,c,a.z);
		r.w=madd(r.w,c,a.w);
	}

	inline void msub(Vector4 &r,const Vector4 &b,real c,const Vector4 &a){
		r.x=msub(b.x,c,a.x);
		r.y=msub(b.y,c,a.y);
		r.z=msub(b.z,c,a.z);
		r.w=msub(b.w,c,a.w);
	}

	inline void msub(Vector4 &r,real c,const Vector4 &a){
		r.x=msub(r.x,c,a.x);
		r.y=msub(r.y,c,a.y);
		r.z=msub(r.z,c,a.z);
		r.w=msub(r.w,c,a.w);
	}

	inline real lengthSquared(const Vector4 &v){
		return v.x*v.x + v.y*v.y + v.z*v.z + v.w*v.w;
	}

	inline real lengthSquared(const Vector4 &v1,const Vector4 &v2){
		real x=v1.x-v2.x;
		real y=v1.y-v2.y;
		real z=v1.z-v2.z;
		real w=v1.w-v2.w;
		return x*x + y*y + z*z + w*w;
	}

	inline real length(const Vector4 &v){
		return sqrt(lengthSquared(v));
	}

	inline real length(const Vector4 &v1,const Vector4 &v2){
		return sqrt(lengthSquared(v1,v2));
	}

	inline void normalize(Vector4 &v){
		real l=1.0/length(v);
		v.x*=l;
		v.y*=l;
		v.z*=l;
		v.w*=l;
	}

	inline void normalize(Vector4 &r,const Vector4 &v){
		real l=1.0/length(v);
		r.x=v.x*l;
		r.y=v.y*l;
		r.z=v.z*l;
		r.w=v.w*l;
	}

	inline bool normalizeCarefully(Vector4 &v,real epsilon){
		real l=length(v);
		if(l>epsilon){
			l=1.0/l;
			v.x*=l;
			v.y*=l;
			v.z*=l;
			v.w*=l;
			return true;
		}
		return false;
	}

	inline bool normalizeCarefully(Vector4 &r,const Vector4 &v,real epsilon){
		real l=length(v);
		if(l>epsilon){
			l=1.0/l;
			r.x=v.x*l;
			r.y=v.y*l;
			r.z=v.z*l;
			r.w=v.w*l;
			return true;
		}
		return false;
	}

	inline real dot(const Vector4 &v1,const Vector4 &v2){
		return v1.x*v2.x + v1.y*v2.y + v1.z*v2.z + v1.w*v2.w;
	}

	inline void lerp(Vector4 &r,const Vector4 &v1,const Vector4 &v2,real t){
		sub(r,v2,v1);
		mul(r,t);
		add(r,v1);
	}

	// EulerAngle operations
	TOADLET_API bool setEulerAngleXYZFromMatrix3x3(EulerAngle &r,const Matrix3x3 &m,real epsilon);

	TOADLET_API bool setEulerAngleXYZFromQuaternion(EulerAngle &r,const Quaternion &q,real epsilon);

	// Matrix3x3 basic operations
	inline void mul(Matrix3x3 &r,const Matrix3x3 &m1,const Matrix3x3 &m2){
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

	inline void postMul(Matrix3x3 &m1,const Matrix3x3 &m2){
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

	inline void preMul(Matrix3x3 &m2,const Matrix3x3 &m1){
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

	inline void mul(Vector3 &r,const Matrix3x3 &m,const Vector3 &v){
		r.x=m.data[0+0*3]*v.x + m.data[0+1*3]*v.y + m.data[0+2*3]*v.z;
		r.y=m.data[1+0*3]*v.x + m.data[1+1*3]*v.y + m.data[1+2*3]*v.z;
		r.z=m.data[2+0*3]*v.x + m.data[2+1*3]*v.y + m.data[2+2*3]*v.z;
	}

	inline void mul(Vector3 &v,const Matrix3x3 &m){
		real tx=m.data[0+0*3]*v.x + m.data[0+1*3]*v.y + m.data[0+2*3]*v.z;
		real ty=m.data[1+0*3]*v.x + m.data[1+1*3]*v.y + m.data[1+2*3]*v.z;
		real tz=m.data[2+0*3]*v.x + m.data[2+1*3]*v.y + m.data[2+2*3]*v.z;
		v.x=tx;
		v.y=ty;
		v.z=tz;
	}

	TOADLET_API void transpose(Matrix3x3 &r,const Matrix3x3 &m);

	TOADLET_API real determinant(const Matrix3x3 &m);

	TOADLET_API bool invert(Matrix3x3 &r,const Matrix3x3 m);

	// Matrix4x4 basic operations
	inline void mul(Matrix4x4 &r,const Matrix4x4 &m1,const Matrix4x4 &m2){
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

	inline void postMul(Matrix4x4 &m1,const Matrix4x4 &m2){
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

	inline void preMul(Matrix4x4 &m2,const Matrix4x4 &m1){
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

	inline void mul(Vector4 &r,const Matrix4x4 &m,const Vector4 &v){
		r.x=(m.data[0+0*4]*v.x) + (m.data[0+1*4]*v.y) + (m.data[0+2*4]*v.z) + (m.data[0+3*4]*v.w);
		r.y=(m.data[1+0*4]*v.x) + (m.data[1+1*4]*v.y) + (m.data[1+2*4]*v.z) + (m.data[1+3*4]*v.w);
		r.z=(m.data[2+0*4]*v.x) + (m.data[2+1*4]*v.y) + (m.data[2+2*4]*v.z) + (m.data[2+3*4]*v.w);
		r.w=(m.data[3+0*4]*v.x) + (m.data[3+1*4]*v.y) + (m.data[3+2*4]*v.z) + (m.data[3+3*4]*v.w);
	}

	inline void mul(Vector4 &r,const Matrix4x4 &m){
		real tx=(m.data[0+0*4]*r.x) + (m.data[0+1*4]*r.y) + (m.data[0+2*4]*r.z) + (m.data[0+3*4]*r.w);
		real ty=(m.data[1+0*4]*r.x) + (m.data[1+1*4]*r.y) + (m.data[1+2*4]*r.z) + (m.data[1+3*4]*r.w);
		real tz=(m.data[2+0*4]*r.x) + (m.data[2+1*4]*r.y) + (m.data[2+2*4]*r.z) + (m.data[2+3*4]*r.w);
		real tw=(m.data[3+0*4]*r.x) + (m.data[3+1*4]*r.y) + (m.data[3+2*4]*r.z) + (m.data[3+3*4]*r.w);
		r.x=tx;
		r.y=ty;
		r.z=tz;
		r.w=tw;
	}

	inline void mul(Vector3 &r,const Matrix4x4 &m,const Vector3 &v){
		r.x=(m.data[0+0*4]*v.x) + (m.data[0+1*4]*v.y) + (m.data[0+2*4]*v.z);
		r.y=(m.data[1+0*4]*v.x) + (m.data[1+1*4]*v.y) + (m.data[1+2*4]*v.z);
		r.z=(m.data[2+0*4]*v.x) + (m.data[2+1*4]*v.y) + (m.data[2+2*4]*v.z);
	}

	inline void mul(Vector3 &r,const Matrix4x4 &m){
		real tx=(m.data[0+0*4]*r.x) + (m.data[0+1*4]*r.y) + (m.data[0+2*4]*r.z);
		real ty=(m.data[1+0*4]*r.x) + (m.data[1+1*4]*r.y) + (m.data[1+2*4]*r.z);
		real tz=(m.data[2+0*4]*r.x) + (m.data[2+1*4]*r.y) + (m.data[2+2*4]*r.z);
		r.x=tx;
		r.y=ty;
		r.z=tz;
	}

	inline void mulPoint3Fast(Vector3 &r,const Matrix4x4 &m,const Vector3 &p){
		r.x=(m.data[0+0*4]*p.x) + (m.data[0+1*4]*p.y) + (m.data[0+2*4]*p.z) + m.data[0+3*4];
		r.y=(m.data[1+0*4]*p.x) + (m.data[1+1*4]*p.y) + (m.data[1+2*4]*p.z) + m.data[1+3*4];
		r.z=(m.data[2+0*4]*p.x) + (m.data[2+1*4]*p.y) + (m.data[2+2*4]*p.z) + m.data[2+3*4];
	}

	inline void mulPoint3Fast(Vector3 &r,const Matrix4x4 &m){
		real tx=(m.data[0+0*4]*r.x) + (m.data[0+1*4]*r.y) + (m.data[0+2*4]*r.z) + m.data[0+3*4];
		real ty=(m.data[1+0*4]*r.x) + (m.data[1+1*4]*r.y) + (m.data[1+2*4]*r.z) + m.data[1+3*4];
		real tz=(m.data[2+0*4]*r.x) + (m.data[2+1*4]*r.y) + (m.data[2+2*4]*r.z) + m.data[2+3*4];
		r.x=tx;
		r.y=ty;
		r.z=tz;
	}

	inline void mulPoint3Full(Vector3 &r,const Matrix4x4 &m,const Vector3 &p){
		r.x=(m.data[0+0*4]*p.x) + (m.data[0+1*4]*p.y) + (m.data[0+2*4]*p.z) + m.data[0+3*4];
		r.y=(m.data[1+0*4]*p.x) + (m.data[1+1*4]*p.y) + (m.data[1+2*4]*p.z) + m.data[1+3*4];
		r.z=(m.data[2+0*4]*p.x) + (m.data[2+1*4]*p.y) + (m.data[2+2*4]*p.z) + m.data[2+3*4];
		real iw=(1.0f/((m.data[3+0*4]*p.x) + (m.data[3+1*4]*p.y) + (m.data[3+2*4]*p.z) + m.data[3+3*4]));
		r.x*=iw;
		r.y*=iw;
		r.z*=iw;
	}

	inline void mulPoint3Full(Vector3 &r,const Matrix4x4 &m){
		real tx=(m.data[0+0*4]*r.x) + (m.data[0+1*4]*r.y) + (m.data[0+2*4]*r.z) + m.data[0+3*4];
		real ty=(m.data[1+0*4]*r.x) + (m.data[1+1*4]*r.y) + (m.data[1+2*4]*r.z) + m.data[1+3*4];
		real tz=(m.data[2+0*4]*r.x) + (m.data[2+1*4]*r.y) + (m.data[2+2*4]*r.z) + m.data[2+3*4];
		real iw=(1.0f/((m.data[3+0*4]*r.x) + (m.data[3+1*4]*r.y) + (m.data[3+2*4]*r.z) + m.data[3+3*4]));
		r.x=tx*iw;
		r.y=ty*iw;
		r.z=tz*iw;
	}

	TOADLET_API void transpose(Matrix4x4 &r,const Matrix4x4 &m);

	TOADLET_API real determinant(const Matrix4x4 &m);

	TOADLET_API bool invert(Matrix4x4 &r,const Matrix4x4 &m);

	// Generic Matrix operations
	template<class Matrix>
	inline void setAxesFromMatrix(const Matrix &m,Vector3 &xAxis,Vector3 &yAxis,Vector3 &zAxis){
		xAxis.x=m.at(0,0); yAxis.x=m.at(0,1); zAxis.x=m.at(0,2);
		xAxis.y=m.at(1,0); yAxis.y=m.at(1,1); zAxis.y=m.at(1,2);
		xAxis.z=m.at(2,0); yAxis.z=m.at(2,1); zAxis.z=m.at(2,2);
	}

	template<class Matrix>
	inline void setMatrixFromAxes(Matrix &m,const Vector3 &xAxis,const Vector3 &yAxis,const Vector3 &zAxis){
		m.setAt(0,0,xAxis.x); m.setAt(0,1,yAxis.x); m.setAt(0,2,zAxis.x);
		m.setAt(1,0,xAxis.y); m.setAt(1,1,yAxis.y); m.setAt(1,2,zAxis.y);
		m.setAt(2,0,xAxis.z); m.setAt(2,1,yAxis.z); m.setAt(2,2,zAxis.z);
	}

	template<class Matrix>
	inline void setMatrixFromX(Matrix &r,real x){
		real cx=cos(x);
		real sx=sin(x);

		r.setAt(0,0,ONE);	r.setAt(0,1,0);		r.setAt(0,2,0);
		r.setAt(1,0,0);		r.setAt(1,1,cx);	r.setAt(1,2,-sx);
		r.setAt(2,0,0);		r.setAt(2,1,sx);	r.setAt(2,2,cx);
	}

	template<class Matrix>
	inline void setMatrixFromY(Matrix &r,real y){
		real cy=cos(y);
		real sy=sin(y);

		r.setAt(0,0,cy);	r.setAt(0,1,0);		r.setAt(0,2,sy);
		r.setAt(1,0,0);		r.setAt(1,1,ONE);	r.setAt(1,2,0);
		r.setAt(2,0,-sy);	r.setAt(2,1,0);		r.setAt(2,2,cy);
	}

	template<class Matrix>
	inline void setMatrixFromZ(Matrix &r,real z){
		real cz=cos(z);
		real sz=sin(z);

		r.setAt(0,0,cz);	r.setAt(0,1,-sz);	r.setAt(0,2,0);
		r.setAt(1,0,sz);	r.setAt(1,1,cz);	r.setAt(1,2,0);
		r.setAt(2,0,0);		r.setAt(2,1,0);		r.setAt(2,2,ONE);
	}

	template<class Matrix>
	void setMatrixFromAxisAngle(Matrix &r,const Vector3 &axis,real angle){
		real c=cos(angle);
		real s=sin(angle);
		real t=ONE-c;
		r.setAt(0,0,c + axis.x*axis.x*t);
		r.setAt(1,1,c + axis.y*axis.y*t);
		r.setAt(2,2,c + axis.z*axis.z*t);

		real tmp1=axis.x*axis.y*t;
		real tmp2=axis.z*s;
		r.setAt(1,0,tmp1+tmp2);
		r.setAt(0,1,tmp1-tmp2);

		tmp1=axis.x*axis.z*t;
		tmp2=axis.y*s;
		r.setAt(2,0,tmp1-tmp2);
		r.setAt(0,2,tmp1+tmp2);

		tmp1=axis.y*axis.z*t;
		tmp2=axis.x*s;
		r.setAt(2,1,tmp1+tmp2);
		r.setAt(1,2,tmp1-tmp2);
	}

	template<class Matrix>
	void setMatrixFromQuaternion(Matrix &r,const Quaternion &q){
		real tx  = 2.0*q.x;
		real ty  = 2.0*q.y;
		real tz  = 2.0*q.z;
		real twx = tx*q.w;
		real twy = ty*q.w;
		real twz = tz*q.w;
		real txx = tx*q.x;
		real txy = ty*q.x;
		real txz = tz*q.x;
		real tyy = ty*q.y;
		real tyz = tz*q.y;
		real tzz = tz*q.z;

		r.setAt(0,0,ONE-(tyy+tzz));	r.setAt(0,1,txy-twz);		r.setAt(0,2,txz+twy);
		r.setAt(1,0,txy+twz);		r.setAt(1,1,ONE-(txx+tzz));	r.setAt(1,2,tyz-twx);
		r.setAt(2,0,txz-twy);		r.setAt(2,1,tyz+twx);		r.setAt(2,2,ONE-(txx+tyy));
	}

	// Algorithm from Tomas Moller, 1999
	template<class Matrix>
	void setMatrixFromVector3ToVector3(Matrix &r,const Vector3 &from,const Vector3 &to,real epsilon){
		Vector3 v;
		cross(v,from,to);
		real e=dot(from,to);

		if(e>ONE-epsilon){ // "from" almost or equal to "to"-vector?
			r.setAt(0,0,ONE);	r.setAt(0,1,0);		r.setAt(0,2,0);
			r.setAt(1,0,0);		r.setAt(1,1,ONE);	r.setAt(1,2,0);
			r.setAt(2,0,0);		r.setAt(2,1,0);		r.setAt(2,2,ONE);
		}
		else if(e<-ONE+epsilon){ // "from" almost or equal to negated "to"?
			Vector3 up,left;
			real fxx,fyy,fzz,fxy,fxz,fyz;
			real uxx,uyy,uzz,uxy,uxz,uyz;
			real lxx,lyy,lzz,lxy,lxz,lyz;

			left.x=0;left.y=from.z;left.z=-from.y;
			if(dot(left,left)<epsilon){ // Was left=CROSS(from,(1,0,0)) a good
				left.x=-from.z;left.y=0;left.z=from.x;
			}

			normalizeCarefully(left,0);
			cross(up,left,from);

			// Now we have a coordinate system, i.e., a basis;
			// M=(from, up, left), and we want to rotate to:
			// N=(-from, up, -left). This is done with the matrix:
			// N*M^T where M^T is the transpose of M
			fxx=mul(-from.x,from.x); fyy=mul(-from.y,from.y); fzz=mul(-from.z,from.z);
			fxy=mul(-from.x,from.y); fxz=mul(-from.x,from.z); fyz=mul(-from.y,from.z);

			uxx=mul(up.x,up.x); uyy=mul(up.y,up.y); uzz=mul(up.z,up.z);
			uxy=mul(up.x,up.y); uxz=mul(up.x,up.z); uyz=mul(up.y,up.z);

			lxx=mul(-left.y,left.x); lyy=mul(-left.y,left.y); lzz=mul(-left.z,left.z);
			lxy=mul(-left.x,left.y); lxz=mul(-left.x,left.z); lyz=mul(-left.y,left.z);

			// Symmetric matrix
			r.setAt(0,0,fxx+uxx+lxx);	r.setAt(0,1,r.at(1,0));		r.setAt(0,2,r.at(2,0));
			r.setAt(1,0,fxy+uxy+lxy);	r.setAt(1,1,fyy+uyy+lyy);	r.setAt(1,2,r.at(2,1));
			r.setAt(2,0,fxz+uxz+lxz);	r.setAt(2,1,fyz+uyz+lyz);	r.setAt(2,2,fzz+uzz+lzz);
		}
		else{ // The most common case, unless "from"="to", or "from"=-"to" 
			//   ...otherwise use this hand optimized version (9 mults less)
			real hvx,hvz,hvxy,hvxz,hvyz;
			real h=div(ONE-e,dot(v,v));
			hvx=mul(h,v.x);
			hvz=mul(h,v.z);
			hvxy=mul(hvx,v.y);
			hvxz=mul(hvx,v.z);
			hvyz=mul(hvz,v.y);
			r.setAt(0,0,e+mul(hvx,v.x));	r.setAt(0,1,hvxy+v.z);					r.setAt(0,2,hvxz-v.y);
			r.setAt(1,0,hvxy-v.z);			r.setAt(1,1,e+mul(h,mul(v.y,v.y)));		r.setAt(1,2,hvyz+v.x);
			r.setAt(2,0,hvxz+v.y);			r.setAt(2,1,hvyz-v.x);					r.setAt(2,2,e+mul(hvz,v.z));
		}
	}

	template<class Matrix>
	void setMatrixFromEulerAngleXYZ(Matrix &r,const EulerAngle &euler){
		real cx=cos(euler.x);
		real sx=sin(euler.x);
		real cy=cos(euler.y);
		real sy=sin(euler.y);
		real cz=cos(euler.z);
		real sz=sin(euler.z);
		real cxsy=(cx*sy);
		real sxsy=(sx*sy);
		
		r.setAt(0,0,(cx*cy));
		r.setAt(0,1,(sx*sz) - (cxsy*cz));
		r.setAt(0,2,(cxsy*sz) + (sx*cz));
		r.setAt(1,0,sy);
		r.setAt(1,1,(cy*cz));
		r.setAt(1,2,-(cy*sz));
		r.setAt(2,0,-(sx*cy));
		r.setAt(2,1,(sxsy*cz) + (cx*sz));
		r.setAt(2,2,-(sxsy*sz) + (cx*cz));
	}

	// Matrix3x3 advanced operations
	inline void setAxesFromMatrix3x3(const Matrix3x3 &m,Vector3 &xAxis,Vector3 &yAxis,Vector3 &zAxis){ setAxesFromMatrix(m,xAxis,yAxis,zAxis); }

	inline void setMatrix3x3FromAxes(Matrix3x3 &m,const Vector3 &xAxis,const Vector3 &yAxis,const Vector3 &zAxis){ setMatrixFromAxes(m,xAxis,yAxis,zAxis); }

	inline void setMatrix3x3FromX(Matrix3x3 &r,real x){ setMatrixFromX(r,x); }

	inline void setMatrix3x3FromY(Matrix3x3 &r,real y){ setMatrixFromY(r,y); }

	inline void setMatrix3x3FromZ(Matrix3x3 &r,real z){ setMatrixFromZ(r,z); }

	inline void setMatrix3x3FromAxisAngle(Matrix3x3 &r,const Vector3 &axis,real angle){ setMatrixFromAxisAngle(r,axis,angle); }

	inline void setMatrix3x3FromQuaternion(Matrix3x3 &r,const Quaternion &q){ setMatrixFromQuaternion(r,q); }

	inline void setMatrix3x3FromEulerAngleXYZ(Matrix3x3 &r,const EulerAngle &euler){ setMatrixFromEulerAngleXYZ(r,euler); }

	inline void setMatrix3x3FromVector3ToVector3(Matrix3x3 &r,const Vector3 &from,const Vector3 &to,real epsilon){ setMatrixFromVector3ToVector3(r,from,to,epsilon); }

	inline void setMatrix3x3FromMatrix4x4(Matrix3x3 &r,const Matrix4x4 &m){
		r.data[0+0*3]=m.data[0+0*4]; r.data[1+0*3]=m.data[1+0*4]; r.data[2+0*3]=m.data[2+0*4];
		r.data[0+1*3]=m.data[0+1*4]; r.data[1+1*3]=m.data[1+1*4]; r.data[2+1*3]=m.data[2+1*4];
		r.data[0+2*3]=m.data[0+2*4]; r.data[1+2*3]=m.data[1+2*4]; r.data[2+2*3]=m.data[2+2*4];
	}

	inline void setMatrix3x3FromMatrix4x4Transpose(Matrix3x3 &r,const Matrix4x4 &m){
		r.data[0+0*3]=m.data[0+0*4]; r.data[1+0*3]=m.data[0+1*4]; r.data[2+0*3]=m.data[0+2*4];
		r.data[0+1*3]=m.data[1+0*4]; r.data[1+1*3]=m.data[1+1*4]; r.data[2+1*3]=m.data[1+2*4];
		r.data[0+2*3]=m.data[2+0*4]; r.data[1+2*3]=m.data[2+1*4]; r.data[2+2*3]=m.data[2+2*4];
	}

	// Matrix4x4 advanced operations
	inline void setAxesFromMatrix4x4(const Matrix4x4 &m,Vector3 &xAxis,Vector3 &yAxis,Vector3 &zAxis){ setAxesFromMatrix(m,xAxis,yAxis,zAxis); }

	inline void setMatrix4x4FromAxes(Matrix4x4 &m,const Vector3 &xAxis,const Vector3 &yAxis,const Vector3 &zAxis){ setMatrixFromAxes(m,xAxis,yAxis,zAxis); }

	inline void setMatrix4x4FromX(Matrix4x4 &r,real x){ setMatrixFromX(r,x); }

	inline void setMatrix4x4FromY(Matrix4x4 &r,real y){ setMatrixFromY(r,y); }

	inline void setMatrix4x4FromZ(Matrix4x4 &r,real z){ setMatrixFromZ(r,z); }

	inline void setMatrix4x4FromAxisAngle(Matrix4x4 &r,const Vector3 &axis,real angle){ setMatrixFromAxisAngle(r,axis,angle); }

	inline void setMatrix4x4FromQuaternion(Matrix4x4 &r,const Quaternion &q){ setMatrixFromQuaternion(r,q); }

	inline void setMatrix4x4FromEulerAngleXYZ(Matrix4x4 &r,const EulerAngle &euler){ setMatrixFromEulerAngleXYZ(r,euler); }

	inline void setMatrix4x4FromVector3ToVector3(Matrix4x4 &r,const Vector3 &from,const Vector3 &to,real epsilon){ setMatrixFromVector3ToVector3(r,from,to,epsilon); }

	inline void setMatrix4x4FromMatrix3x3(Matrix4x4 &r,const Matrix3x3 &m){
		r.data[0+0*4]=m.data[0+0*3]; r.data[1+0*4]=m.data[1+0*3]; r.data[2+0*4]=m.data[2+0*3];
		r.data[0+1*4]=m.data[0+1*3]; r.data[1+1*4]=m.data[1+1*3]; r.data[2+1*4]=m.data[2+1*3];
		r.data[0+2*4]=m.data[0+2*3]; r.data[1+2*4]=m.data[1+2*3]; r.data[2+2*4]=m.data[2+2*3];
	}

	inline void setMatrix4x4FromMatrix3x3Transpose(Matrix4x4 &r,const Matrix3x3 &m){
		r.data[0+0*4]=m.data[0+0*3]; r.data[1+0*4]=m.data[0+1*3]; r.data[2+0*4]=m.data[0+2*3];
		r.data[0+1*4]=m.data[1+0*3]; r.data[1+1*4]=m.data[1+1*3]; r.data[2+1*4]=m.data[1+2*3];
		r.data[0+2*4]=m.data[2+0*3]; r.data[1+2*4]=m.data[2+1*3]; r.data[2+2*4]=m.data[2+2*3];
	}

	inline void setMatrix4x4FromTranslate(Matrix4x4 &r,float x,float y,float z){
		r.data[0+3*4]=x;
		r.data[1+3*4]=y;
		r.data[2+3*4]=z;
	}

	inline void setMatrix4x4FromTranslate(Matrix4x4 &r,const Vector3 &translate){
		r.data[0+3*4]=translate.x;
		r.data[1+3*4]=translate.y;
		r.data[2+3*4]=translate.z;
	}

	inline void setTranslateFromMatrix4x4(Vector3 &r,const Matrix4x4 &m){
		r.x=m.data[0+3*4];
		r.y=m.data[1+3*4];
		r.z=m.data[2+3*4];
	}

	inline void setMatrix4x4FromScale(Matrix4x4 &r,float x,float y,float z){
		r.data[0+0*4]=x;
		r.data[1+1*4]=y;
		r.data[2+2*4]=z;
	}

	inline void setMatrix4x4FromScale(Matrix4x4 &r,const Vector3 &scale){
		r.data[0+0*4]=scale.x;
		r.data[1+1*4]=scale.y;
		r.data[2+2*4]=scale.z;
	}

	inline void setScaleFromMatrix4x4(Vector3 &r,const Matrix4x4 &m){
		r.x=sqrt(m.data[0+0*4]*m.data[0+0*4] + m.data[1+0*4]*m.data[1+0*4] + m.data[2+0*4]*m.data[2+0*4]);
		r.y=sqrt(m.data[0+1*4]*m.data[0+1*4] + m.data[1+1*4]*m.data[1+1*4] + m.data[2+1*4]*m.data[2+1*4]);
		r.z=sqrt(m.data[0+2*4]*m.data[0+2*4] + m.data[1+2*4]*m.data[1+2*4] + m.data[2+2*4]*m.data[2+2*4]);
	}

	inline void setRotateFromMatrix4x4(Matrix3x3 &r,const Matrix4x4 &m){
		real x=sqrt(m.data[0+0*4]*m.data[0+0*4] + m.data[1+0*4]*m.data[1+0*4] + m.data[2+0*4]*m.data[2+0*4]);
		real y=sqrt(m.data[0+1*4]*m.data[0+1*4] + m.data[1+1*4]*m.data[1+1*4] + m.data[2+1*4]*m.data[2+1*4]);
		real z=sqrt(m.data[0+2*4]*m.data[0+2*4] + m.data[1+2*4]*m.data[1+2*4] + m.data[2+2*4]*m.data[2+2*4]);
		x=1.0/x;
		y=1.0/y;
		z=1.0/z;
		r.data[0+0*3]=m.data[0+0*4]*x;
		r.data[1+0*3]=m.data[1+0*4]*x;
		r.data[2+0*3]=m.data[2+0*4]*x;
		r.data[0+1*3]=m.data[0+1*4]*y;
		r.data[1+1*3]=m.data[1+1*4]*y;
		r.data[2+1*3]=m.data[2+1*4]*y;
		r.data[0+2*3]=m.data[0+2*4]*z;
		r.data[1+2*3]=m.data[1+2*4]*z;
		r.data[2+2*3]=m.data[2+2*4]*z;
	}

	inline void setRotateFromMatrix4x4(Matrix3x3 &r,const Matrix4x4 &m,const Vector3 &scale){
		real x=1.0/scale.x;
		real y=1.0/scale.y;
		real z=1.0/scale.z;
		r.data[0+0*3]=m.data[0+0*4]*x;
		r.data[1+0*3]=m.data[1+0*4]*x;
		r.data[2+0*3]=m.data[2+0*4]*x;
		r.data[0+1*3]=m.data[0+1*4]*y;
		r.data[1+1*3]=m.data[1+1*4]*y;
		r.data[2+1*3]=m.data[2+1*4]*y;
		r.data[0+2*3]=m.data[0+2*4]*z;
		r.data[1+2*3]=m.data[1+2*4]*z;
		r.data[2+2*3]=m.data[2+2*4]*z;
	}

	inline void setMatrix4x4FromRotateScale(Matrix4x4 &r,const Matrix3x3 &rotate,const Vector3 &scale){
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

	inline void setMatrix4x4FromTranslateRotateScale(Matrix4x4 &r,const Vector3 &translate,const Matrix3x3 &rotate,const Vector3 &scale){
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

	inline void setMatrix4x4FromTranslateRotateScale(Matrix4x4 &r,const Vector3 &translate,const Quaternion &rotate,const Vector3 &scale){
		setMatrix4x4FromQuaternion(r,rotate);
		r.data[0+3*4]=translate.x;
		r.data[1+3*4]=translate.y;
		r.data[2+3*4]=translate.z;
		r.data[0+0*4]*=scale.x;
		r.data[1+0*4]*=scale.x;
		r.data[2+0*4]*=scale.x;
		r.data[0+1*4]*=scale.y;
		r.data[1+1*4]*=scale.y;
		r.data[2+1*4]*=scale.y;
		r.data[0+2*4]*=scale.z;
		r.data[1+2*4]*=scale.z;
		r.data[2+2*4]*=scale.z;
	}

	inline void setMatrix4x4AsTextureRotation(Matrix4x4 &r){
		r.data[0+3*4]=r.data[0+0*4]*-HALF + r.data[0+1*4]*-HALF + HALF;
		r.data[1+3*4]=r.data[1+0*4]*-HALF + r.data[1+1*4]*-HALF + HALF;
		r.data[2+3*4]=0;
	}

	TOADLET_API void setMatrix4x4FromLookAt(Matrix4x4 &r,const Vector3 &eye,const Vector3 &point,const Vector3 &up,bool camera);

	TOADLET_API void setMatrix4x4FromLookDir(Matrix4x4 &r,const Vector3 &eye,const Vector3 &dir,const Vector3 &up,bool camera);

	TOADLET_API void setMatrix4x4FromOrtho(Matrix4x4 &r,real leftDist,real rightDist,real bottomDist,real topDist,real nearDist,real farDist);

	TOADLET_API void setMatrix4x4FromFrustum(Matrix4x4 &r,real leftDist,real rightDist,real bottomDist,real topDist,real nearDist,real farDist);

	TOADLET_API void setMatrix4x4FromPerspectiveX(Matrix4x4 &r,real fovx,real aspect,real nearDist,real farDist);

	TOADLET_API void setMatrix4x4FromPerspectiveY(Matrix4x4 &r,real fovy,real aspect,real nearDist,real farDist);

	TOADLET_API void setMatrix4x4FromObliquePlane(Matrix4x4 &r,const Plane &plane,const Vector3 &dir);

	TOADLET_API void setMatrix4x4FromPerspectivePlane(Matrix4x4 &r,const Plane &plane,const Vector3 &eye);

	// Axis Angle operations
	TOADLET_API real setAxisAngleFromQuaternion(Vector3 &axis,const Quaternion &q,real epsilon);

	// Quaternion operations
	inline void mul(Quaternion &r,const Quaternion &q1,const Quaternion &q2){
		r.x=+q1.x*q2.w+q1.y*q2.z-q1.z*q2.y+q1.w*q2.x;
		r.y=-q1.x*q2.z+q1.y*q2.w+q1.z*q2.x+q1.w*q2.y;
		r.z=+q1.x*q2.y-q1.y*q2.x+q1.z*q2.w+q1.w*q2.z;
		r.w=-q1.x*q2.x-q1.y*q2.y-q1.z*q2.z+q1.w*q2.w;
	}

	inline void postMul(Quaternion &q1,const Quaternion &q2){
		real x=+q1.x*q2.w+q1.y*q2.z-q1.z*q2.y+q1.w*q2.x;
		real y=-q1.x*q2.z+q1.y*q2.w+q1.z*q2.x+q1.w*q2.y;
		real z=+q1.x*q2.y-q1.y*q2.x+q1.z*q2.w+q1.w*q2.z;
		real w=-q1.x*q2.x-q1.y*q2.y-q1.z*q2.z+q1.w*q2.w;
		q1.x=x;
		q1.y=y;
		q1.z=z;
		q1.w=w;
	}

	inline void preMul(Quaternion &q2,const Quaternion &q1){
		real x=+q1.x*q2.w+q1.y*q2.z-q1.z*q2.y+q1.w*q2.x;
		real y=-q1.x*q2.z+q1.y*q2.w+q1.z*q2.x+q1.w*q2.y;
		real z=+q1.x*q2.y-q1.y*q2.x+q1.z*q2.w+q1.w*q2.z;
		real w=-q1.x*q2.x-q1.y*q2.y-q1.z*q2.z+q1.w*q2.w;
		q2.x=x;
		q2.y=y;
		q2.z=z;
		q2.w=w;
	}

	inline void mul(Vector3 &r,const Quaternion &q){
		real x=+q.y*r.z-q.z*r.y+q.w*r.x;
		real y=-q.x*r.z+q.z*r.x+q.w*r.y;
		real z=+q.x*r.y-q.y*r.x+q.w*r.z;
		real w=-q.x*r.x-q.y*r.y-q.z*r.z;

		r.x=+x*+q.w+y*-q.z-z*-q.y+w*-q.x;
		r.y=-x*-q.z+y*+q.w+z*-q.x+w*-q.y;
		r.z=+x*-q.y-y*-q.x+z*+q.w+w*-q.z;
	}

	inline void mul(Vector3 &r,const Quaternion &q,const Vector3 &v){
		real x=+q.y*v.z-q.z*v.y+q.w*v.x;
		real y=-q.x*v.z+q.z*v.x+q.w*v.y;
		real z=+q.x*v.y-q.y*v.x+q.w*v.z;
		real w=-q.x*v.x-q.y*v.y-q.z*v.z;

		r.x=+x*+q.w+y*-q.z-z*-q.y+w*-q.x;
		r.y=-x*-q.z+y*+q.w+z*-q.x+w*-q.y;
		r.z=+x*-q.y-y*-q.x+z*+q.w+w*-q.z;
	}

	inline real lengthSquared(const Quaternion &q){
		return q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w;
	}

	inline real lengthSquared(const Quaternion &q1,const Quaternion &q2){
		real x=q1.x-q2.x;
		real y=q1.y-q2.y;
		real z=q1.z-q2.z;
		real w=q1.w-q2.w;
		return x*x + y*y + z*z + w*w;
	}

	inline real length(const Quaternion &q){
		return sqrt(lengthSquared(q));
	}

	inline real length(const Quaternion &q1,const Quaternion &q2){
		return sqrt(lengthSquared(q1,q2));
	}

	inline void normalize(Quaternion &q){
		real l=1.0/length(q);
		q.x*=l;
		q.y*=l;
		q.z*=l;
		q.w*=l;
	}

	inline void normalize(Quaternion &r,const Quaternion &q){
		real l=1.0/length(q);
		r.x=q.x*l;
		r.y=q.y*l;
		r.z=q.z*l;
		r.w=q.w*l;
	}

	inline bool normalizeCarefully(Quaternion &q,real epsilon){
		real l=length(q);
		if(l>epsilon){
			l=1.0/l;
			q.x*=l;
			q.y*=l;
			q.z*=l;
			q.w*=l;
			return true;
		}
		return false;
	}

	inline bool normalizeCarefully(Quaternion &r,const Quaternion &q,real epsilon){
		real l=length(q);
		if(l>epsilon){
			l=1.0/l;
			r.x=q.x*l;
			r.y=q.y*l;
			r.z=q.z*l;
			r.w=q.w*l;
			return true;
		}
		return false;
	}

	inline real dot(const Quaternion &q1,const Quaternion &q2){
		return q1.x*q2.x + q1.y*q2.y + q1.z*q2.z + q1.w*q2.w;
	}

	inline void conjugate(Quaternion &q){
		q.x=-q.x;
		q.y=-q.y;
		q.z=-q.z;
	}

	inline void conjugate(Quaternion &r,const Quaternion &q){
		r.x=-q.x;
		r.y=-q.y;
		r.z=-q.z;
		r.w=q.w;
	}

	inline void invert(Quaternion &q){
		real i=1.0/((q.x*q.x)+(q.y*q.y)+(q.z*q.z)+(q.w*q.w));
		q.x*=-i;
		q.y*=-i;
		q.z*=-i;
		q.w*=i;
	}

	inline void invert(Quaternion &r,const Quaternion &q){
		real i=1.0/((q.x*q.x)+(q.y*q.y)+(q.z*q.z)+(q.w*q.w));
		r.x=q.x*-i;
		r.y=q.y*-i;
		r.z=q.z*-i;
		r.w=q.w*i;
	}

	TOADLET_API void setQuaternionFromMatrix3x3(Quaternion &r,const Matrix3x3 &m);

	TOADLET_API void setQuaternionFromMatrix4x4(Quaternion &r,const Matrix4x4 &m);

	inline void setQuaternionFromAxes(Quaternion &r,const Vector3 &xAxis,const Vector3 &yAxis,const Vector3 &zAxis){
		Matrix3x3 cacheMatrix;
		setMatrix3x3FromAxes(cacheMatrix,xAxis,yAxis,zAxis);
		setQuaternionFromMatrix3x3(r,cacheMatrix);
		normalizeCarefully(r,0);
	}

	inline void setAxesFromQuaternion(const Quaternion &q,Vector3 &xAxis,Vector3 &yAxis,Vector3 &zAxis){
		Matrix3x3 cacheMatrix;
		setMatrix3x3FromQuaternion(cacheMatrix,q);
		setAxesFromMatrix3x3(cacheMatrix,xAxis,yAxis,zAxis);
	}

	inline void setQuaternionFromAxisAngle(Quaternion &r,const Vector3 &axis,real angle){
		real halfAngle=angle*0.5;
		real sinHalfAngle=sin(halfAngle);
		r.x=axis.x*sinHalfAngle;
		r.y=axis.y*sinHalfAngle;
		r.z=axis.z*sinHalfAngle;
		r.w=cos(halfAngle);
	}

	inline void setQuaternionFromEulerAngleXYZ(Quaternion &r,const EulerAngle &euler){
		real sx=euler.x/2;
		real cx=cos(sx);
		sx=sin(sx);
		real sy=euler.y/2;
		real cy=cos(sy);
		sy=sin(sy);
		real sz=euler.z/2;
		real cz=cos(sz);
		sz=sin(sz);
		real cxcy=cx*cy;
		real sxsy=sx*sy;
		real cxsy=cx*sy;
		real sxcy=sx*cy;

		r.w=(cxcy*cz) - (sxsy*sz);
  		r.x=(cxcy*sz) + (sxsy*cz);
		r.y=(sxcy*cz) + (cxsy*sz);
		r.z=(cxsy*cz) - (sxcy*sz);
	}

	TOADLET_API void lerp(Quaternion &r,const Quaternion &q1,const Quaternion &q2,real t);

	TOADLET_API void slerp(Quaternion &r,const Quaternion &q1,const Quaternion &q2,real t);

	// Segment operations
	TOADLET_API void project(Vector3 &result,const Segment &segment,const Vector3 &point,bool limitToSegment);

	TOADLET_API void project(Vector3 &point1,Vector3 &point2,const Segment &seg1,const Segment &seg2,real epsilon);

	TOADLET_API bool unprojectSegment(Segment &result,const Matrix4x4 &projViewMatrix,int x,int y,unsigned int viewportX,unsigned int viewportY,unsigned int viewportWidth,unsigned int viewportHeight,Matrix4x4 &scratchMatrix);
	inline bool unprojectSegment(Segment &result,const Matrix4x4 &projViewMatrix,int x,int y,unsigned int viewportX,unsigned int viewportY,unsigned int viewportWidth,unsigned int viewportHeight){Matrix4x4 scratchMatrix;return unprojectSegment(result,projViewMatrix,x,y,viewportX,viewportY,viewportWidth,viewportHeight,scratchMatrix);}

	// Plane operations
	inline void neg(Plane &r,const Plane &p){
		r.normal.x=-p.normal.x;
		r.normal.y=-p.normal.y;
		r.normal.z=-p.normal.z;
		r.distance=-p.distance;
	}

	inline void neg(Plane &p){
		p.normal.x=-p.normal.x;
		p.normal.y=-p.normal.y;
		p.normal.z=-p.normal.z;
		p.distance=-p.distance;
	}

	inline void project(const Plane &plane,Vector3 &result,const Vector3 &point){
		result.x=point.x - plane.normal.x*plane.distance;
		result.y=point.y - plane.normal.y*plane.distance;
		result.z=point.z - plane.normal.z*plane.distance;

		real f=plane.normal.x*result.x + plane.normal.y*result.y + plane.normal.z*result.z;

		result.x=point.x - plane.normal.x*f;
		result.y=point.y - plane.normal.y*f;
		result.z=point.z - plane.normal.z*f;
	}

	inline void project(const Plane &plane,Vector3 &point){
		real tx=point.x - plane.normal.x*plane.distance;
		real ty=point.y - plane.normal.y*plane.distance;
		real tz=point.z - plane.normal.z*plane.distance;

		real f=plane.normal.x*tx + plane.normal.y*ty + plane.normal.z*tz;

		point.x=point.x - plane.normal.x*f;
		point.y=point.y - plane.normal.y*f;
		point.z=point.z - plane.normal.z*f;
	}

	inline void normalize(Plane &plane){
		real l=1.0/length(plane.normal);
		plane.normal.x*=l;
		plane.normal.y*=l;
		plane.normal.z*=l;
		plane.distance*=l;
	}

	inline void normalize(Plane &r,const Plane &plane){
		real l=1.0/length(plane.normal);
		r.normal.x=plane.normal.x*l;
		r.normal.y=plane.normal.y*l;
		r.normal.z=plane.normal.z*l;
		r.distance=plane.distance*l;
	}

	inline real length(const Plane &p,const Vector3 &v){
		return p.normal.x*v.x + p.normal.y*v.y + p.normal.z*v.z - p.distance;
	}

	TOADLET_API bool getIntersectionOfThreePlanes(Vector3 &result,const Plane &p1,const Plane &p2,const Plane &p3,real epsilon);

	TOADLET_API void getLineOfIntersection(Segment &result,const Plane &plane1,const Plane &plane2);

	// AABox operations
	inline void add(AABox &r,const AABox &b,const Vector3 &p){
		r.mins.x=b.mins.x+p.x;
		r.mins.y=b.mins.y+p.y;
		r.mins.z=b.mins.z+p.z;

		r.maxs.x=b.maxs.x+p.x;
		r.maxs.y=b.maxs.y+p.y;
		r.maxs.z=b.maxs.z+p.z;
	}

	inline void add(AABox &b,const Vector3 &p){
		b.mins.x+=p.x;
		b.mins.y+=p.y;
		b.mins.z+=p.z;

		b.maxs.x+=p.x;
		b.maxs.y+=p.y;
		b.maxs.z+=p.z;
	}

	inline void sub(AABox &r,const AABox &b,const Vector3 &p){
		r.mins.x=b.mins.x-p.x;
		r.mins.y=b.mins.y-p.y;
		r.mins.z=b.mins.z-p.z;

		r.maxs.x=b.maxs.x-p.x;
		r.maxs.y=b.maxs.y-p.y;
		r.maxs.z=b.maxs.z-p.z;
	}

	inline void sub(AABox &b,const Vector3 &p){
		b.mins.x-=p.x;
		b.mins.y-=p.y;
		b.mins.z-=p.z;

		b.maxs.x-=p.x;
		b.maxs.y-=p.y;
		b.maxs.z-=p.z;
	}

	template<class Rotation>
	inline void rotateAABox(AABox &r,const Rotation &rotation,const AABox &b,Vector3 buffer[8]){
		int i;
		Vector3 temp;
		b.getVertexes(buffer);

		for(i=0;i<8;++i){
			Math::mul(temp,rotation,buffer[i]);
			if(i==0){
				r.mins.set(temp);
				r.maxs.set(temp);
			}
			else{
				if(r.mins.x>temp.x)
					r.mins.x=temp.x;
				if(r.mins.y>temp.y)
					r.mins.y=temp.y;
				if(r.mins.z>temp.z)
					r.mins.z=temp.z;
				if(r.maxs.x<temp.x)
					r.maxs.x=temp.x;
				if(r.maxs.y<temp.y)
					r.maxs.y=temp.y;
				if(r.maxs.z<temp.z)
					r.maxs.z=temp.z;
			}
		}
	}

	inline void mul(AABox &r,const Quaternion &rotation,const AABox &b){
		Vector3 buffer[8];
		rotateAABox(r,rotation,b,buffer);
	}

	inline void mul(AABox &r,const Matrix3x3 &rotation,const AABox &b){
		Vector3 buffer[8];
		rotateAABox(r,rotation,b,buffer);
	}

	TOADLET_API void findBoundingBox(AABox &r,const Sphere &sphere);

	TOADLET_API void findBoundingBox(AABox &r,const Capsule &capsule);

	TOADLET_API void findBoundingBox(AABox &r,const Vector3 *vertexes,int amount);

	TOADLET_API void findFitBox(AABox &r,const Vector3 *vertexes,int amount);

	// Sphere operations
	inline void add(Sphere &r,const Sphere &s,const Vector3 &p){
		r.origin.x=s.origin.x+p.x;
		r.origin.y=s.origin.y+p.y;
		r.origin.z=s.origin.z+p.z;
	}

	inline void add(Sphere &s,const Vector3 &p){
		s.origin.x+=p.x;
		s.origin.y+=p.y;
		s.origin.z+=p.z;
	}

	inline void sub(Sphere &r,const Sphere &s,const Vector3 &p){
		r.origin.x=s.origin.x-p.x;
		r.origin.y=s.origin.y-p.y;
		r.origin.z=s.origin.z-p.z;
	}

	inline void sub(Sphere &s,const Vector3 &p){
		s.origin.x-=p.x;
		s.origin.y-=p.y;
		s.origin.z-=p.z;
	}
	
	TOADLET_API void findBoundingSphere(Sphere &r,const AABox &box);
	
	TOADLET_API void findFitSphere(Sphere &r,const AABox &box);

	// Capsule operations
	inline void add(Capsule &r,const Capsule &c,const Vector3 &p){
		r.origin.x=c.origin.x+p.x;
		r.origin.y=c.origin.y+p.y;
		r.origin.z=c.origin.z+p.z;
	}

	inline void add(Capsule &c,const Vector3 &p){
		c.origin.x+=p.x;
		c.origin.y+=p.y;
		c.origin.z+=p.z;
	}

	inline void sub(Capsule &r,const Capsule &c,const Vector3 &p){
		r.origin.x=c.origin.x-p.x;
		r.origin.y=c.origin.y-p.y;
		r.origin.z=c.origin.z-p.z;
	}

	inline void sub(Capsule &c,const Vector3 &p){
		c.origin.x-=p.x;
		c.origin.y-=p.y;
		c.origin.z-=p.z;
	}

	TOADLET_API void findBoundingCapsule(Capsule &r,const AABox &box);

	TOADLET_API void findFitCapsule(Capsule &r,const AABox &box);

	// Color operations
	inline uint32 swapColor(uint32 bytes){
		return
			((bytes&0xFF000000)>>24) |
			((bytes&0x00FF0000)>>8 ) |
			((bytes&0x0000FF00)<<8 ) |
			((bytes&0x000000FF)<<24) ;
	}

	inline uint32 lerpColor(uint32 c1,uint32 c2,scalar t){
		return
			(((int)lerp((c1&0xFF000000)>>24,(c2&0xFF000000)>>24,t))<<24) |
			(((int)lerp((c1&0x00FF0000)>>16,(c2&0x00FF0000)>>16,t))<<16) |
			(((int)lerp((c1&0x0000FF00)>>8 ,(c2&0x0000FF00)>>8 ,t))<<8 ) |
			(((int)lerp((c1&0x000000FF)    ,(c2&0x000000FF)    ,t))    ) ;
	}

	inline void setHSVA(Vector4 &rgba,const Vector4 &hsva){
		scalar h=hsva.x,s=hsva.y,v=hsva.z;
		scalar r=0,g=0,b=0;

		scalar hi=Math::floor(h*6);
		scalar f=(h*6)-hi;
		scalar p=Math::mul(v,Math::ONE-s);
		scalar q=Math::mul(v,Math::ONE-Math::mul(f,s));
		scalar t=Math::mul(v,Math::ONE-Math::mul(Math::ONE-f,s));
		switch(Math::toInt(hi)){
			case(0):
				r=v;g=t;b=p;
			break;
			case(1):
				r=q;g=v;b=p;
			break;
			case(2):
				r=p;g=v;b=t;
			break;
			case(3):
				r=p;g=q;b=v;
			break;
			case(4):
				r=t;g=p;b=v;
			break;
			case(5):
				r=v;g=p;b=q;
			break;
		}

		rgba.set(r,g,b,hsva.w);
	}

	inline void getHSVA(Vector4 &hsva,const Vector4 &rgba){
		scalar r=rgba.x,g=rgba.y,b=rgba.z;
		scalar h=0,s=0,v=0;

		scalar mx=Math::maxVal(Math::maxVal(r,g),b);
		scalar mn=Math::minVal(Math::minVal(r,g),b);

		if(mx==mn){
			h=0;
		}
		else if(mx==r){
			h=Math::div(g-b,mx-mn)/6;
		}
		else if(mx==g){
			h=Math::div(b-r,mx-mn)/6 + Math::THIRD;
		}
		else{
			h=Math::div(r-g,mx-mn)/6 + Math::TWO_THIRDS;
		}

		if(mx==0){
			s=0;
		}
		else{
			s=Math::ONE-Math::div(mn,mx);
		}

		v=mx;

		hsva.set(h,s,v,rgba.w);
	}

	// Intersection operations
	TOADLET_API bool testInside(const Plane &plane,const Vector3 &point);
	TOADLET_API bool testInside(const Sphere &sphere,const Vector3 &point);
	TOADLET_API bool testInside(const AABox &box,const Vector3 &point);

	TOADLET_API bool testIntersection(const AABox &box1,const AABox &box2);
	TOADLET_API bool testIntersection(const Sphere &sphere1,const Sphere &sphere2);
	TOADLET_API bool testIntersection(const Sphere &sphere,const AABox &box);

	TOADLET_API real findIntersection(const Segment &segment,const Plane &plane,Vector3 &point,Vector3 &normal);
	TOADLET_API real findIntersection(const Segment &segment,const Sphere &sphere,Vector3 &point,Vector3 &normal);
	TOADLET_API real findIntersection(const Segment &segment,const AABox &box,Vector3 &point,Vector3 &normal);
}

}
}
}

#endif

