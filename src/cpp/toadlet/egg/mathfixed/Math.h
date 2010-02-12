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

#ifndef TOADLET_EGG_MATHFIXED_MATH_H
#define TOADLET_EGG_MATHFIXED_MATH_H

#include <toadlet/egg/mathfixed/BaseMath.h>
#include <toadlet/egg/mathfixed/Vector2.h>
#include <toadlet/egg/mathfixed/Vector3.h>
#include <toadlet/egg/mathfixed/Vector4.h>
#include <toadlet/egg/mathfixed/Matrix3x3.h>
#include <toadlet/egg/mathfixed/Matrix4x4.h>
#include <toadlet/egg/mathfixed/Quaternion.h>
#include <toadlet/egg/mathfixed/EulerAngle.h>
#include <toadlet/egg/mathfixed/Segment.h>
#include <toadlet/egg/mathfixed/Plane.h>
#include <toadlet/egg/mathfixed/Sphere.h>
#include <toadlet/egg/mathfixed/AABox.h>
#include <toadlet/egg/mathfixed/Capsule.h>
#include <toadlet/egg/mathfixed/ConvexSolid.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

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
	inline void neg(Vector2 &r,Vector2 &v){
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

	inline void mul(Vector2 &r,const Vector2 &v,fixed f){
		r.x=TOADLET_MUL_XX(v.x,f);
		r.y=TOADLET_MUL_XX(v.y,f);
	}
	
	inline void mul(Vector2 &r,fixed f){
		r.x=TOADLET_MUL_XX(r.x,f);
		r.y=TOADLET_MUL_XX(r.y,f);
	}

	inline void mul(Vector2 &r,const Vector2 &v1,const Vector2 &v2){
		r.x=TOADLET_MUL_XX(v1.x,v2.x);
		r.y=TOADLET_MUL_XX(v1.y,v2.y);
	}

	inline void mul(Vector2 &r,const Vector2 &v){
		r.x=TOADLET_MUL_XX(r.x,v.x);
		r.y=TOADLET_MUL_XX(r.y,v.y);
	}

	inline void div(Vector2 &r,const Vector2 &v,fixed f){
		fixed i=TOADLET_DIV_XX(ONE,f);
		r.x=TOADLET_MUL_XX(v.x,i);
		r.y=TOADLET_MUL_XX(v.y,i);
	}

	inline void div(Vector2 &r,fixed f){
		fixed i=TOADLET_DIV_XX(ONE,f);
		r.x=TOADLET_MUL_XX(r.x,i);
		r.y=TOADLET_MUL_XX(r.y,i);
	}

	inline void div(Vector2 &r,const Vector2 &v1,const Vector2 &v2){
		r.x=TOADLET_DIV_XX(v1.x,v2.x);
		r.y=TOADLET_DIV_XX(v1.y,v2.y);
	}

	inline void div(Vector2 &r,const Vector2 &v){
		r.x=TOADLET_DIV_XX(r.x,v.x);
		r.y=TOADLET_DIV_XX(r.y,v.y);
	}

	inline void madd(Vector2 &r,const Vector2 &b,fixed c,const Vector2 &a){
		r.x=TOADLET_MADD_XXX(b.x,c,a.x);
		r.y=TOADLET_MADD_XXX(b.y,c,a.y);
	}

	inline void madd(Vector2 &r,fixed c,const Vector2 &a){
		r.x=TOADLET_MADD_XXX(r.x,c,a.x);
		r.y=TOADLET_MADD_XXX(r.y,c,a.y);
	}

	inline void msub(Vector2 &r,const Vector2 &b,fixed c,const Vector2 &a){
		r.x=TOADLET_MSUB_XXX(b.x,c,a.x);
		r.y=TOADLET_MSUB_XXX(b.y,c,a.y);
	}

	inline void msub(Vector2 &r,fixed c,const Vector2 &a){
		r.x=TOADLET_MSUB_XXX(r.x,c,a.x);
		r.y=TOADLET_MSUB_XXX(r.y,c,a.y);
	}

	inline fixed lengthSquared(const Vector2 &v){
		fixed r=TOADLET_MUL_XX(v.x,v.x) + TOADLET_MUL_XX(v.y,v.y);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	inline fixed lengthSquared(const Vector2 &v1,const Vector2 &v2){
		fixed x=v1.x-v2.x;
		fixed y=v1.y-v2.y;
		fixed r=TOADLET_MUL_XX(x,x) + TOADLET_MUL_XX(y,y);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	inline fixed length(const Vector2 &v){
		return sqrt(lengthSquared(v));
	}

	inline fixed length(const Vector2 &v1,const Vector2 &v2){
		return sqrt(lengthSquared(v1,v2));
	}

	inline void normalize(Vector2 &v){
		fixed l=TOADLET_DIV_XX(ONE,length(v));
		v.x=TOADLET_MUL_XX(v.x,l);
		v.y=TOADLET_MUL_XX(v.x,l);
	}

	inline void normalize(Vector2 &r,const Vector2 &v){
		fixed l=TOADLET_DIV_XX(ONE,length(v));
		r.x=TOADLET_MUL_XX(v.x,l);
		r.y=TOADLET_MUL_XX(v.y,l);
	}

	inline bool normalizeCarefully(Vector2 &v,fixed epsilon){
		fixed l=length(v);
		if(l>epsilon){
			l=TOADLET_DIV_XX(ONE,l);
			v.x=TOADLET_MUL_XX(v.x,l);
			v.y=TOADLET_MUL_XX(v.y,l);
			return true;
		}
		return false;
	}

	inline bool normalizeCarefully(Vector2 &r,const Vector2 &v,fixed epsilon){
		fixed l=length(v);
		if(l>epsilon){
			l=TOADLET_DIV_XX(ONE,l);
			r.x=TOADLET_MUL_XX(v.x,l);
			r.y=TOADLET_MUL_XX(v.y,l);
			return true;
		}
		return false;
	}

	inline fixed dot(const Vector2 &v1,const Vector2 &v2){
		return TOADLET_MUL_XX(v1.x,v2.x) + TOADLET_MUL_XX(v1.y,v2.y);
	}

	inline void lerp(Vector2 &r,const Vector2 &v1,const Vector2 &v2,fixed t){
		sub(r,v2,v1);
		mul(r,t);
		add(r,v1);
	}

	// Vector3 operations
	inline void neg(Vector3 &r,Vector3 &v){
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

	inline void mul(Vector3 &r,const Vector3 &v,fixed f){
		r.x=TOADLET_MUL_XX(v.x,f);
		r.y=TOADLET_MUL_XX(v.y,f);
		r.z=TOADLET_MUL_XX(v.z,f);
	}
	
	inline void mul(Vector3 &r,fixed f){
		r.x=TOADLET_MUL_XX(r.x,f);
		r.y=TOADLET_MUL_XX(r.y,f);
		r.z=TOADLET_MUL_XX(r.z,f);
	}

	inline void mul(Vector3 &r,const Vector3 &v1,const Vector3 &v2){
		r.x=TOADLET_MUL_XX(v1.x,v2.x);
		r.y=TOADLET_MUL_XX(v1.y,v2.y);
		r.z=TOADLET_MUL_XX(v1.z,v2.z);
	}

	inline void mul(Vector3 &r,const Vector3 &v){
		r.x=TOADLET_MUL_XX(r.x,v.x);
		r.y=TOADLET_MUL_XX(r.y,v.y);
		r.z=TOADLET_MUL_XX(r.z,v.z);
	}

	inline void div(Vector3 &r,const Vector3 &v,fixed f){
		fixed i=TOADLET_DIV_XX(ONE,f);
		r.x=TOADLET_MUL_XX(v.x,i);
		r.y=TOADLET_MUL_XX(v.y,i);
		r.z=TOADLET_MUL_XX(v.z,i);
	}
	
	inline void div(Vector3 &r,fixed f){
		fixed i=TOADLET_DIV_XX(ONE,f);
		r.x=TOADLET_MUL_XX(r.x,i);
		r.y=TOADLET_MUL_XX(r.y,i);
		r.z=TOADLET_MUL_XX(r.z,i);
	}

	inline void div(Vector3 &r,const Vector3 &v1,const Vector3 &v2){
		r.x=TOADLET_DIV_XX(v1.x,v2.x);
		r.y=TOADLET_DIV_XX(v1.y,v2.y);
		r.z=TOADLET_DIV_XX(v1.z,v2.z);
	}

	inline void div(Vector3 &r,const Vector3 &v){
		r.x=TOADLET_DIV_XX(r.x,v.x);
		r.y=TOADLET_DIV_XX(r.y,v.y);
		r.z=TOADLET_DIV_XX(r.z,v.z);
	}

	inline void madd(Vector3 &r,const Vector3 &b,fixed c,const Vector3 &a){
		r.x=TOADLET_MADD_XXX(b.x,c,a.x);
		r.y=TOADLET_MADD_XXX(b.y,c,a.y);
		r.z=TOADLET_MADD_XXX(b.z,c,a.z);
	}

	inline void madd(Vector3 &r,fixed c,const Vector3 &a){
		r.x=TOADLET_MADD_XXX(r.x,c,a.x);
		r.y=TOADLET_MADD_XXX(r.y,c,a.y);
		r.z=TOADLET_MADD_XXX(r.z,c,a.z);
	}

	inline void msub(Vector3 &r,const Vector3 &b,fixed c,const Vector3 &a){
		r.x=TOADLET_MSUB_XXX(b.x,c,a.x);
		r.y=TOADLET_MSUB_XXX(b.y,c,a.y);
		r.z=TOADLET_MSUB_XXX(b.z,c,a.z);
	}

	inline void msub(Vector3 &r,fixed c,const Vector3 &a){
		r.x=TOADLET_MSUB_XXX(r.x,c,a.x);
		r.y=TOADLET_MSUB_XXX(r.y,c,a.y);
		r.z=TOADLET_MSUB_XXX(r.z,c,a.z);
	}

	inline fixed lengthSquared(const Vector3 &v){
		fixed r=TOADLET_MUL_XX(v.x,v.x) + TOADLET_MUL_XX(v.y,v.y) + TOADLET_MUL_XX(v.z,v.z);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	inline fixed lengthSquared(const Vector3 &v1,const Vector3 &v2){
		fixed x=v1.x-v2.x;
		fixed y=v1.y-v2.y;
		fixed z=v1.z-v2.z;
		fixed r=TOADLET_MUL_XX(x,x) + TOADLET_MUL_XX(y,y) + TOADLET_MUL_XX(z,z);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	inline fixed length(const Vector3 &v){
		return sqrt(lengthSquared(v));
	}

	inline fixed length(const Vector3 &v1,const Vector3 &v2){
		return sqrt(lengthSquared(v1,v2));
	}

	inline void normalize(Vector3 &v){
		fixed l=TOADLET_DIV_XX(ONE,length(v));
		v.x=TOADLET_MUL_XX(v.x,l);
		v.y=TOADLET_MUL_XX(v.y,l);
		v.z=TOADLET_MUL_XX(v.z,l);
	}

	inline void normalize(Vector3 &r,const Vector3 &v){
		fixed l=TOADLET_DIV_XX(ONE,length(v));
		r.x=TOADLET_MUL_XX(v.x,l);
		r.y=TOADLET_MUL_XX(v.y,l);
		r.z=TOADLET_MUL_XX(v.z,l);
	}

	inline bool normalizeCarefully(Vector3 &v,fixed epsilon){
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
	
	inline bool normalizeCarefully(Vector3 &r,const Vector3 &v,fixed epsilon){
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

	inline fixed dot(const Vector3 &v1,const Vector3 &v2){
		return TOADLET_MUL_XX(v1.x,v2.x) + TOADLET_MUL_XX(v1.y,v2.y) + TOADLET_MUL_XX(v1.z,v2.z);
	}

	inline void lerp(Vector3 &r,const Vector3 &v1,const Vector3 &v2,fixed t){
		sub(r,v2,v1);
		mul(r,t);
		add(r,v1);
	}

	inline void cross(Vector3 &r,const Vector3 &v1,const Vector3 &v2){
		r.x=TOADLET_MUL_XX(v1.y,v2.z) - TOADLET_MUL_XX(v1.z,v2.y);
		r.y=TOADLET_MUL_XX(v1.z,v2.x) - TOADLET_MUL_XX(v1.x,v2.z);
		r.z=TOADLET_MUL_XX(v1.x,v2.y) - TOADLET_MUL_XX(v1.y,v2.x);
	}

	// Vector4 operations
	inline void neg(Vector4 &r,Vector4 &v){
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

	inline void mul(Vector4 &r,const Vector4 &v,fixed f){
		r.x=TOADLET_MUL_XX(v.x,f);
		r.y=TOADLET_MUL_XX(v.y,f);
		r.z=TOADLET_MUL_XX(v.z,f);
		r.w=TOADLET_MUL_XX(v.w,f);
	}
	
	inline void mul(Vector4 &r,fixed f){
		r.x=TOADLET_MUL_XX(r.x,f);
		r.y=TOADLET_MUL_XX(r.y,f);
		r.z=TOADLET_MUL_XX(r.z,f);
		r.w=TOADLET_MUL_XX(r.w,f);
	}

	inline void mul(Vector4 &r,const Vector4 &v1,const Vector4 &v2){
		r.x=TOADLET_MUL_XX(v1.x,v2.x);
		r.y=TOADLET_MUL_XX(v1.y,v2.y);
		r.z=TOADLET_MUL_XX(v1.z,v2.z);
		r.w=TOADLET_MUL_XX(v1.w,v2.w);
	}

	inline void mul(Vector4 &r,const Vector4 &v){
		r.x=TOADLET_MUL_XX(r.x,v.x);
		r.y=TOADLET_MUL_XX(r.y,v.y);
		r.z=TOADLET_MUL_XX(r.z,v.z);
		r.w=TOADLET_MUL_XX(r.w,v.w);
	}

	inline void div(Vector4 &r,const Vector4 &v,fixed f){
		fixed i=TOADLET_DIV_XX(ONE,f);
		r.x=TOADLET_MUL_XX(v.x,i);
		r.y=TOADLET_MUL_XX(v.y,i);
		r.z=TOADLET_MUL_XX(v.z,i);
		r.w=TOADLET_MUL_XX(v.w,i);
	}
	
	inline void div(Vector4 &r,fixed f){
		fixed i=TOADLET_DIV_XX(ONE,f);
		r.x=TOADLET_MUL_XX(r.x,i);
		r.y=TOADLET_MUL_XX(r.y,i);
		r.z=TOADLET_MUL_XX(r.z,i);
		r.w=TOADLET_MUL_XX(r.w,i);
	}

	inline void div(Vector4 &r,const Vector4 &v1,const Vector4 &v2){
		r.x=TOADLET_DIV_XX(v1.x,v2.x);
		r.y=TOADLET_DIV_XX(v1.y,v2.y);
		r.z=TOADLET_DIV_XX(v1.z,v2.z);
		r.w=TOADLET_DIV_XX(v1.w,v2.w);
	}

	inline void div(Vector4 &r,const Vector4 &v){
		r.x=TOADLET_DIV_XX(r.x,v.x);
		r.y=TOADLET_DIV_XX(r.y,v.y);
		r.z=TOADLET_DIV_XX(r.z,v.z);
		r.w=TOADLET_DIV_XX(r.w,v.w);
	}

	inline void madd(Vector4 &r,const Vector4 &b,fixed c,const Vector4 &a){
		r.x=TOADLET_MADD_XXX(b.x,c,a.x);
		r.y=TOADLET_MADD_XXX(b.y,c,a.y);
		r.z=TOADLET_MADD_XXX(b.z,c,a.z);
		r.w=TOADLET_MADD_XXX(b.w,c,a.w);
	}

	inline void madd(Vector4 &r,fixed c,const Vector4 &a){
		r.x=TOADLET_MADD_XXX(r.x,c,a.x);
		r.y=TOADLET_MADD_XXX(r.y,c,a.y);
		r.z=TOADLET_MADD_XXX(r.z,c,a.z);
		r.w=TOADLET_MADD_XXX(r.w,c,a.w);
	}

	inline void msub(Vector4 &r,const Vector4 &b,fixed c,const Vector4 &a){
		r.x=TOADLET_MSUB_XXX(b.x,c,a.x);
		r.y=TOADLET_MSUB_XXX(b.y,c,a.y);
		r.z=TOADLET_MSUB_XXX(b.z,c,a.z);
		r.w=TOADLET_MSUB_XXX(b.w,c,a.w);
	}

	inline void msub(Vector4 &r,fixed c,const Vector4 &a){
		r.x=TOADLET_MSUB_XXX(r.x,c,a.x);
		r.y=TOADLET_MSUB_XXX(r.y,c,a.y);
		r.z=TOADLET_MSUB_XXX(r.z,c,a.z);
		r.w=TOADLET_MSUB_XXX(r.w,c,a.w);
	}

	inline fixed lengthSquared(const Vector4 &v){
		fixed r=TOADLET_MUL_XX(v.x,v.x) + TOADLET_MUL_XX(v.y,v.y) + TOADLET_MUL_XX(v.z,v.z) + TOADLET_MUL_XX(v.w,v.w);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	inline fixed lengthSquared(const Vector4 &v1,const Vector4 &v2){
		fixed x=v1.x-v2.x;
		fixed y=v1.y-v2.y;
		fixed z=v1.z-v2.z;
		fixed w=v1.w-v2.w;
		fixed r=TOADLET_MUL_XX(x,x) + TOADLET_MUL_XX(y,y) + TOADLET_MUL_XX(z,z) + TOADLET_MUL_XX(w,w);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	inline fixed length(const Vector4 &v){
		return sqrt(lengthSquared(v));
	}

	inline fixed length(const Vector4 &v1,const Vector4 &v2){
		return sqrt(lengthSquared(v1,v2));
	}

	inline void normalize(Vector4 &v){
		fixed l=TOADLET_DIV_XX(ONE,length(v));
		v.x=TOADLET_MUL_XX(v.x,l);
		v.y=TOADLET_MUL_XX(v.y,l);
		v.z=TOADLET_MUL_XX(v.z,l);
		v.w=TOADLET_MUL_XX(v.w,l);
	}

	inline void normalize(Vector4 &r,const Vector4 &v){
		fixed l=TOADLET_DIV_XX(ONE,length(v));
		r.x=TOADLET_MUL_XX(v.x,l);
		r.y=TOADLET_MUL_XX(v.y,l);
		r.z=TOADLET_MUL_XX(v.z,l);
		r.w=TOADLET_MUL_XX(v.w,l);
	}

	inline bool normalizeCarefully(Vector4 &v,fixed epsilon){
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

	inline bool normalizeCarefully(Vector4 &r,const Vector4 &v,fixed epsilon){
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

	inline fixed dot(const Vector4 &v1,const Vector4 &v2){
		return TOADLET_MUL_XX(v1.x,v2.x) + TOADLET_MUL_XX(v1.y,v2.y) + TOADLET_MUL_XX(v1.z,v2.z) + TOADLET_MUL_XX(v1.w,v2.w);
	}

	inline void lerp(Vector4 &r,const Vector4 &v1,const Vector4 &v2,fixed t){
		sub(r,v2,v1);
		mul(r,t);
		add(r,v1);
	}

	// EulerAngle operations
	TOADLET_API bool setEulerAngleXYZFromMatrix3x3(EulerAngle &r,const Matrix3x3 &m);

	// Matrix3x3 basic operations
	inline void mul(Matrix3x3 &r,const Matrix3x3 &m1,const Matrix3x3 &m2){
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

	inline void postMul(Matrix3x3 &m1,const Matrix3x3 &m2){
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

	inline void preMul(Matrix3x3 &m2,const Matrix3x3 &m1){
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

	inline void mul(Vector3 &r,const Matrix3x3 &m,const Vector3 &v){
		r.x=TOADLET_MUL_XX(m.data[0+0*3],v.x) + TOADLET_MUL_XX(m.data[0+1*3],v.y) + TOADLET_MUL_XX(m.data[0+2*3],v.z);
		r.y=TOADLET_MUL_XX(m.data[1+0*3],v.x) + TOADLET_MUL_XX(m.data[1+1*3],v.y) + TOADLET_MUL_XX(m.data[1+2*3],v.z);
		r.z=TOADLET_MUL_XX(m.data[2+0*3],v.x) + TOADLET_MUL_XX(m.data[2+1*3],v.y) + TOADLET_MUL_XX(m.data[2+2*3],v.z);
	}

	inline void mul(Vector3 &r,const Matrix3x3 &m){
		fixed tx=TOADLET_MUL_XX(m.data[0+0*3],r.x) + TOADLET_MUL_XX(m.data[0+1*3],r.y) + TOADLET_MUL_XX(m.data[0+2*3],r.z);
		fixed ty=TOADLET_MUL_XX(m.data[1+0*3],r.x) + TOADLET_MUL_XX(m.data[1+1*3],r.y) + TOADLET_MUL_XX(m.data[1+2*3],r.z);
		fixed tz=TOADLET_MUL_XX(m.data[2+0*3],r.x) + TOADLET_MUL_XX(m.data[2+1*3],r.y) + TOADLET_MUL_XX(m.data[2+2*3],r.z);
		r.x=tx;
		r.y=ty;
		r.z=tz;
	}

	TOADLET_API void transpose(Matrix3x3 &r,const Matrix3x3 &m);

	TOADLET_API fixed determinant(const Matrix3x3 &m);

	TOADLET_API bool invert(Matrix3x3 &r,const Matrix3x3 m);

	// Matrix4x4 basic operations
	inline void mul(Matrix4x4 &r,const Matrix4x4 &m1,const Matrix4x4 &m2){
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

	inline void postMul(Matrix4x4 &m1,const Matrix4x4 &m2){
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

	inline void preMul(Matrix4x4 &m2,const Matrix4x4 &m1){
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

	inline void mul(Vector4 &r,const Matrix4x4 &m,const Vector4 &v){
		r.x=TOADLET_MUL_XX(m.data[0+0*4],v.x) + TOADLET_MUL_XX(m.data[0+1*4],v.y) + TOADLET_MUL_XX(m.data[0+2*4],v.z) + TOADLET_MUL_XX(m.data[0+3*4],v.w);
		r.y=TOADLET_MUL_XX(m.data[1+0*4],v.x) + TOADLET_MUL_XX(m.data[1+1*4],v.y) + TOADLET_MUL_XX(m.data[1+2*4],v.z) + TOADLET_MUL_XX(m.data[1+3*4],v.w);
		r.z=TOADLET_MUL_XX(m.data[2+0*4],v.x) + TOADLET_MUL_XX(m.data[2+1*4],v.y) + TOADLET_MUL_XX(m.data[2+2*4],v.z) + TOADLET_MUL_XX(m.data[2+3*4],v.w);
		r.w=TOADLET_MUL_XX(m.data[3+0*4],v.x) + TOADLET_MUL_XX(m.data[3+1*4],v.y) + TOADLET_MUL_XX(m.data[3+2*4],v.z) + TOADLET_MUL_XX(m.data[3+3*4],v.w);
	}

	inline void mul(Vector4 &r,const Matrix4x4 &m){
		fixed tx=TOADLET_MUL_XX(m.data[0+0*4],r.x) + TOADLET_MUL_XX(m.data[0+1*4],r.y) + TOADLET_MUL_XX(m.data[0+2*4],r.z) + TOADLET_MUL_XX(m.data[0+3*4],r.w);
		fixed ty=TOADLET_MUL_XX(m.data[1+0*4],r.x) + TOADLET_MUL_XX(m.data[1+1*4],r.y) + TOADLET_MUL_XX(m.data[1+2*4],r.z) + TOADLET_MUL_XX(m.data[1+3*4],r.w);
		fixed tz=TOADLET_MUL_XX(m.data[2+0*4],r.x) + TOADLET_MUL_XX(m.data[2+1*4],r.y) + TOADLET_MUL_XX(m.data[2+2*4],r.z) + TOADLET_MUL_XX(m.data[2+3*4],r.w);
		fixed tw=TOADLET_MUL_XX(m.data[3+0*4],r.x) + TOADLET_MUL_XX(m.data[3+1*4],r.y) + TOADLET_MUL_XX(m.data[3+2*4],r.z) + TOADLET_MUL_XX(m.data[3+3*4],r.w);
		r.x=tx;
		r.y=ty;
		r.z=tz;
		r.w=tw;
	}

	inline void mul(Vector3 &r,const Matrix4x4 &m,const Vector3 &v){
		r.x=TOADLET_MUL_XX(m.data[0+0*4],v.x) + TOADLET_MUL_XX(m.data[0+1*4],v.y) + TOADLET_MUL_XX(m.data[0+2*4],v.z);
		r.y=TOADLET_MUL_XX(m.data[1+0*4],v.x) + TOADLET_MUL_XX(m.data[1+1*4],v.y) + TOADLET_MUL_XX(m.data[1+2*4],v.z);
		r.z=TOADLET_MUL_XX(m.data[2+0*4],v.x) + TOADLET_MUL_XX(m.data[2+1*4],v.y) + TOADLET_MUL_XX(m.data[2+2*4],v.z);
	}

	inline void mul(Vector3 &r,const Matrix4x4 &m){
		fixed tx=TOADLET_MUL_XX(m.data[0+0*4],r.x) + TOADLET_MUL_XX(m.data[0+1*4],r.y) + TOADLET_MUL_XX(m.data[0+2*4],r.z);
		fixed ty=TOADLET_MUL_XX(m.data[1+0*4],r.x) + TOADLET_MUL_XX(m.data[1+1*4],r.y) + TOADLET_MUL_XX(m.data[1+2*4],r.z);
		fixed tz=TOADLET_MUL_XX(m.data[2+0*4],r.x) + TOADLET_MUL_XX(m.data[2+1*4],r.y) + TOADLET_MUL_XX(m.data[2+2*4],r.z);
		r.x=tx;
		r.y=ty;
		r.z=tz;
	}

	inline void mulPoint3Fast(Vector3 &r,const Matrix4x4 &m,const Vector3 &p){
		r.x=TOADLET_MUL_XX(m.data[0+0*4],p.x) + TOADLET_MUL_XX(m.data[0+1*4],p.y) + TOADLET_MUL_XX(m.data[0+2*4],p.z) + m.data[0+3*4];
		r.y=TOADLET_MUL_XX(m.data[1+0*4],p.x) + TOADLET_MUL_XX(m.data[1+1*4],p.y) + TOADLET_MUL_XX(m.data[1+2*4],p.z) + m.data[1+3*4];
		r.z=TOADLET_MUL_XX(m.data[2+0*4],p.x) + TOADLET_MUL_XX(m.data[2+1*4],p.y) + TOADLET_MUL_XX(m.data[2+2*4],p.z) + m.data[2+3*4];
	}

	inline void mulPoint3Fast(Vector3 &r,const Matrix4x4 &m){
		fixed tx=TOADLET_MUL_XX(m.data[0+0*4],r.x) + TOADLET_MUL_XX(m.data[0+1*4],r.y) + TOADLET_MUL_XX(m.data[0+2*4],r.z) + m.data[0+3*4];
		fixed ty=TOADLET_MUL_XX(m.data[1+0*4],r.x) + TOADLET_MUL_XX(m.data[1+1*4],r.y) + TOADLET_MUL_XX(m.data[1+2*4],r.z) + m.data[1+3*4];
		fixed tz=TOADLET_MUL_XX(m.data[2+0*4],r.x) + TOADLET_MUL_XX(m.data[2+1*4],r.y) + TOADLET_MUL_XX(m.data[2+2*4],r.z) + m.data[2+3*4];
		r.x=tx;
		r.y=ty;
		r.z=tz;
	}

	inline void mulPoint3Full(Vector3 &r,const Matrix4x4 &m,const Vector3 &p){
		r.x=TOADLET_MUL_XX(m.data[0+0*4],p.x) + TOADLET_MUL_XX(m.data[0+1*4],p.y) + TOADLET_MUL_XX(m.data[0+2*4],p.z) + m.data[0+3*4];
		r.y=TOADLET_MUL_XX(m.data[1+0*4],p.x) + TOADLET_MUL_XX(m.data[1+1*4],p.y) + TOADLET_MUL_XX(m.data[1+2*4],p.z) + m.data[1+3*4];
		r.z=TOADLET_MUL_XX(m.data[2+0*4],p.x) + TOADLET_MUL_XX(m.data[2+1*4],p.y) + TOADLET_MUL_XX(m.data[2+2*4],p.z) + m.data[2+3*4];
		fixed iw=TOADLET_DIV_XX(ONE,TOADLET_MUL_XX(m.data[3+0*4],p.x) + TOADLET_MUL_XX(m.data[3+1*4],p.y) + TOADLET_MUL_XX(m.data[3+2*4],p.z) + m.data[3+3*4]);
		r.x=TOADLET_MUL_XX(r.x,iw);
		r.y=TOADLET_MUL_XX(r.y,iw);
		r.z=TOADLET_MUL_XX(r.z,iw);
	}

	inline void mulPoint3Full(Vector3 &r,const Matrix4x4 &m){
		fixed tx=TOADLET_MUL_XX(m.data[0+0*4],r.x) + TOADLET_MUL_XX(m.data[0+1*4],r.y) + TOADLET_MUL_XX(m.data[0+2*4],r.z) + m.data[0+3*4];
		fixed ty=TOADLET_MUL_XX(m.data[1+0*4],r.x) + TOADLET_MUL_XX(m.data[1+1*4],r.y) + TOADLET_MUL_XX(m.data[1+2*4],r.z) + m.data[1+3*4];
		fixed tz=TOADLET_MUL_XX(m.data[2+0*4],r.x) + TOADLET_MUL_XX(m.data[2+1*4],r.y) + TOADLET_MUL_XX(m.data[2+2*4],r.z) + m.data[2+3*4];
		fixed iw=TOADLET_DIV_XX(ONE,TOADLET_MUL_XX(m.data[3+0*4],r.x) + TOADLET_MUL_XX(m.data[3+1*4],r.y) + TOADLET_MUL_XX(m.data[3+2*4],r.z) + m.data[3+3*4]);
		r.x=TOADLET_MUL_XX(tx,iw);
		r.y=TOADLET_MUL_XX(ty,iw);
		r.z=TOADLET_MUL_XX(tz,iw);
	}

	TOADLET_API void transpose(Matrix4x4 &r,const Matrix4x4 &m);

	TOADLET_API fixed determinant(const Matrix4x4 &m);

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
	inline void setMatrixFromX(Matrix &r,fixed x){
		fixed cx=cos(x);
		fixed sx=sin(x);

		r.setAt(0,0,ONE);	r.setAt(0,1,0);		r.setAt(0,2,0);
		r.setAt(1,0,0);		r.setAt(1,1,cx);	r.setAt(1,2,-sx);
		r.setAt(2,0,0);		r.setAt(2,1,sx);	r.setAt(2,2,cx);
	}

	template<class Matrix>
	inline void setMatrixFromY(Matrix &r,fixed y){
		fixed cy=cos(y);
		fixed sy=sin(y);

		r.setAt(0,0,cy);	r.setAt(0,1,0);		r.setAt(0,2,sy);
		r.setAt(1,0,0);		r.setAt(1,1,ONE);	r.setAt(1,2,0);
		r.setAt(2,0,-sy);	r.setAt(2,1,0);		r.setAt(2,2,cy);
	}

	template<class Matrix>
	inline void setMatrixFromZ(Matrix &r,fixed z){
		fixed cz=cos(z);
		fixed sz=sin(z);

		r.setAt(0,0,cz);	r.setAt(0,1,-sz);	r.setAt(0,2,0);
		r.setAt(1,0,sz);	r.setAt(1,1,cz);	r.setAt(1,2,0);
		r.setAt(2,0,0);		r.setAt(2,1,0);		r.setAt(2,2,ONE);
	}

	template<class Matrix>
	void setMatrixFromAxisAngle(Matrix &r,const Vector3 &axis,fixed angle){
		fixed c=cos(angle);
		fixed s=sin(angle);
		fixed t=ONE-c;
		r.setAt(0,0,c + TOADLET_MUL_XX(axis.x,TOADLET_MUL_XX(axis.x,t)));
		r.setAt(1,1,c + TOADLET_MUL_XX(axis.y,TOADLET_MUL_XX(axis.y,t)));
		r.setAt(2,2,c + TOADLET_MUL_XX(axis.z,TOADLET_MUL_XX(axis.z,t)));

		fixed tmp1=TOADLET_MUL_XX(axis.x,TOADLET_MUL_XX(axis.y,t));
		fixed tmp2=TOADLET_MUL_XX(axis.z,s);
		r.setAt(1,0,tmp1+tmp2);
		r.setAt(0,1,tmp1-tmp2);

		tmp1=TOADLET_MUL_XX(axis.x,TOADLET_MUL_XX(axis.z,t));
		tmp2=TOADLET_MUL_XX(axis.y,s);
		r.setAt(2,0,tmp1-tmp2);
		r.setAt(0,2,tmp1+tmp2);

		tmp1=TOADLET_MUL_XX(axis.y,TOADLET_MUL_XX(axis.z,t));
		tmp2=TOADLET_MUL_XX(axis.x,s);
		r.setAt(2,1,tmp1+tmp2);
		r.setAt(1,2,tmp1-tmp2);
	}

	template<class Matrix>
	void setMatrixFromQuaternion(Matrix &r,const Quaternion &q){
		fixed tx  = q.x<<1;
		fixed ty  = q.y<<1;
		fixed tz  = q.z<<1;
		fixed twx = TOADLET_MUL_XX(tx,q.w);
		fixed twy = TOADLET_MUL_XX(ty,q.w);
		fixed twz = TOADLET_MUL_XX(tz,q.w);
		fixed txx = TOADLET_MUL_XX(tx,q.x);
		fixed txy = TOADLET_MUL_XX(ty,q.x);
		fixed txz = TOADLET_MUL_XX(tz,q.x);
		fixed tyy = TOADLET_MUL_XX(ty,q.y);
		fixed tyz = TOADLET_MUL_XX(tz,q.y);
		fixed tzz = TOADLET_MUL_XX(tz,q.z);

		r.setAt(0,0,ONE-(tyy+tzz));	r.setAt(0,1,txy-twz);		r.setAt(0,2,txz+twy);
		r.setAt(1,0,txy+twz);		r.setAt(1,1,ONE-(txx+tzz));	r.setAt(1,2,tyz-twx);
		r.setAt(2,0,txz-twy);		r.setAt(2,1,tyz+twx);		r.setAt(2,2,ONE-(txx+tyy));
	}

	// Algorithm from Tomas Moller, 1999
	template<class Matrix>
	void setMatrixFromVector3ToVector3(Matrix &r,const Vector3 &from,const Vector3 &to,fixed epsilon){
		Vector3 v;
		cross(v,from,to);
		fixed e=dot(from,to);

		if(e>ONE-epsilon){ // "from" almost or equal to "to"-vector?
			r.setAt(0,0,ONE);	r.setAt(0,1,0);		r.setAt(0,2,0);
			r.setAt(1,0,0);		r.setAt(1,1,ONE);	r.setAt(1,2,0);
			r.setAt(2,0,0);		r.setAt(2,1,0);		r.setAt(2,2,ONE);
		}
		else if(e<-ONE+epsilon){ // "from" almost or equal to negated "to"?
			Vector3 up,left;
			fixed fxx,fyy,fzz,fxy,fxz,fyz;
			fixed uxx,uyy,uzz,uxy,uxz,uyz;
			fixed lxx,lyy,lzz,lxy,lxz,lyz;

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
			fixed hvx,hvz,hvxy,hvxz,hvyz;
			fixed h=div(ONE-e,dot(v,v));
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
		fixed cx=cos(euler.x);
		fixed sx=sin(euler.x);
		fixed cy=cos(euler.y);
		fixed sy=sin(euler.y);
		fixed cz=cos(euler.z);
		fixed sz=sin(euler.z);
		fixed cxsy=TOADLET_MUL_XX(cx,sy);
		fixed cycz=TOADLET_MUL_XX(cy,cz);
		fixed sxsy=TOADLET_MUL_XX(sx,sy);
		
		r.setAt(0,0,TOADLET_MUL_XX(cx,cy));
		r.setAt(0,1,TOADLET_MUL_XX(sx,sz) - TOADLET_MUL_XX(cxsy,cz));
		r.setAt(0,2,TOADLET_MUL_XX(cxsy,sz) + TOADLET_MUL_XX(sx,cz));
		r.setAt(1,0,sy);
		r.setAt(1,1,cycz);
		r.setAt(1,2,-cycz);
		r.setAt(2,0,-TOADLET_MUL_XX(sx,cy));
		r.setAt(2,1,TOADLET_MUL_XX(sxsy,cz) + TOADLET_MUL_XX(cx,sz));
		r.setAt(2,2,-TOADLET_MUL_XX(sxsy,sz) + TOADLET_MUL_XX(cx,cz));
	}

	// Matrix3x3 advanced operations
	inline void setAxesFromMatrix3x3(const Matrix3x3 &m,Vector3 &xAxis,Vector3 &yAxis,Vector3 &zAxis){ setAxesFromMatrix(m,xAxis,yAxis,zAxis); }

	inline void setMatrix3x3FromAxes(Matrix3x3 &m,const Vector3 &xAxis,const Vector3 &yAxis,const Vector3 &zAxis){ setMatrixFromAxes(m,xAxis,yAxis,zAxis); }

	inline void setMatrix3x3FromX(Matrix3x3 &r,fixed x){ setMatrixFromX(r,x); }

	inline void setMatrix3x3FromY(Matrix3x3 &r,fixed y){ setMatrixFromY(r,y); }

	inline void setMatrix3x3FromZ(Matrix3x3 &r,fixed z){ setMatrixFromZ(r,z); }

	inline void setMatrix3x3FromAxisAngle(Matrix3x3 &r,const Vector3 &axis,fixed angle){ setMatrixFromAxisAngle(r,axis,angle); }

	inline void setMatrix3x3FromQuaternion(Matrix3x3 &r,const Quaternion &q){ setMatrixFromQuaternion(r,q); }

	inline void setMatrix3x3FromEulerAngleXYZ(Matrix3x3 &r,const EulerAngle &euler){ setMatrixFromEulerAngleXYZ(r,euler); }

	inline void setMatrix3x3FromVector3ToVector3(Matrix3x3 &r,const Vector3 &from,const Vector3 &to,fixed epsilon){ setMatrixFromVector3ToVector3(r,from,to,epsilon); }

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

	inline void setMatrix4x4FromX(Matrix4x4 &r,fixed x){ setMatrixFromX(r,x); }

	inline void setMatrix4x4FromY(Matrix4x4 &r,fixed y){ setMatrixFromY(r,y); }

	inline void setMatrix4x4FromZ(Matrix4x4 &r,fixed z){ setMatrixFromZ(r,z); }

	inline void setMatrix4x4FromAxisAngle(Matrix4x4 &r,const Vector3 &axis,fixed angle){ setMatrixFromAxisAngle(r,axis,angle); }

	inline void setMatrix4x4FromQuaternion(Matrix4x4 &r,const Quaternion &q){ setMatrixFromQuaternion(r,q); }

	inline void setMatrix4x4FromEulerAngleXYZ(Matrix4x4 &r,const EulerAngle &euler){ setMatrixFromEulerAngleXYZ(r,euler); }

	inline void setMatrix4x4FromVector3ToVector3(Matrix4x4 &r,const Vector3 &from,const Vector3 &to,fixed epsilon){ setMatrixFromVector3ToVector3(r,from,to,epsilon); }

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

	inline void setMatrix4x4FromTranslate(Matrix4x4 &r,fixed x,fixed y,fixed z){
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

	inline void setMatrix4x4FromScale(Matrix4x4 &r,fixed x,fixed y,fixed z){
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
		r.x=sqrt(TOADLET_MUL_XX(m.data[0+0*4],m.data[0+0*4]) + TOADLET_MUL_XX(m.data[1+0*4],m.data[1+0*4]) + TOADLET_MUL_XX(m.data[2+0*4],m.data[2+0*4]));
		r.y=sqrt(TOADLET_MUL_XX(m.data[0+1*4],m.data[0+1*4]) + TOADLET_MUL_XX(m.data[1+1*4],m.data[1+1*4]) + TOADLET_MUL_XX(m.data[2+1*4],m.data[2+1*4]));
		r.z=sqrt(TOADLET_MUL_XX(m.data[0+2*4],m.data[0+2*4]) + TOADLET_MUL_XX(m.data[1+2*4],m.data[1+2*4]) + TOADLET_MUL_XX(m.data[2+2*4],m.data[2+2*4]));
	}

	inline void setRotateFromMatrix4x4(Matrix3x3 &r,const Matrix4x4 &m){
		fixed x=sqrt(TOADLET_MUL_XX(m.data[0+0*4],m.data[0+0*4]) + TOADLET_MUL_XX(m.data[1+0*4],m.data[1+0*4]) + TOADLET_MUL_XX(m.data[2+0*4],m.data[2+0*4]));
		fixed y=sqrt(TOADLET_MUL_XX(m.data[0+1*4],m.data[0+1*4]) + TOADLET_MUL_XX(m.data[1+1*4],m.data[1+1*4]) + TOADLET_MUL_XX(m.data[2+1*4],m.data[2+1*4]));
		fixed z=sqrt(TOADLET_MUL_XX(m.data[0+2*4],m.data[0+2*4]) + TOADLET_MUL_XX(m.data[1+2*4],m.data[1+2*4]) + TOADLET_MUL_XX(m.data[2+2*4],m.data[2+2*4]));
		x=TOADLET_DIV_XX(ONE,x);
		y=TOADLET_DIV_XX(ONE,y);
		z=TOADLET_DIV_XX(ONE,z);
		r.data[0+0*3]=TOADLET_MUL_XX(m.data[0+0*4],x);
		r.data[1+0*3]=TOADLET_MUL_XX(m.data[1+0*4],x);
		r.data[2+0*3]=TOADLET_MUL_XX(m.data[2+0*4],x);
		r.data[0+1*3]=TOADLET_MUL_XX(m.data[0+1*4],y);
		r.data[1+1*3]=TOADLET_MUL_XX(m.data[1+1*4],y);
		r.data[2+1*3]=TOADLET_MUL_XX(m.data[2+1*4],y);
		r.data[0+2*3]=TOADLET_MUL_XX(m.data[0+2*4],z);
		r.data[1+2*3]=TOADLET_MUL_XX(m.data[1+2*4],z);
		r.data[2+2*3]=TOADLET_MUL_XX(m.data[2+2*4],z);
	}

	inline void setRotateFromMatrix4x4(Matrix3x3 &r,const Matrix4x4 &m,const Vector3 &scale){
		fixed x=TOADLET_DIV_XX(ONE,scale.x);
		fixed y=TOADLET_DIV_XX(ONE,scale.y);
		fixed z=TOADLET_DIV_XX(ONE,scale.z);
		r.data[0+0*3]=TOADLET_MUL_XX(m.data[0+0*4],x);
		r.data[1+0*3]=TOADLET_MUL_XX(m.data[1+0*4],x);
		r.data[2+0*3]=TOADLET_MUL_XX(m.data[2+0*4],x);
		r.data[0+1*3]=TOADLET_MUL_XX(m.data[0+1*4],y);
		r.data[1+1*3]=TOADLET_MUL_XX(m.data[1+1*4],y);
		r.data[2+1*3]=TOADLET_MUL_XX(m.data[2+1*4],y);
		r.data[0+2*3]=TOADLET_MUL_XX(m.data[0+2*4],z);
		r.data[1+2*3]=TOADLET_MUL_XX(m.data[1+2*4],z);
		r.data[2+2*3]=TOADLET_MUL_XX(m.data[2+2*4],z);
	}

	inline void setMatrix4x4FromRotateScale(Matrix4x4 &r,const Matrix3x3 &rotate,const Vector3 &scale){
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

	inline void setMatrix4x4FromTranslateRotateScale(Matrix4x4 &r,const Vector3 &translate,const Matrix3x3 &rotate,const Vector3 &scale){
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

	inline void setMatrix4x4AsTextureRotation(Matrix4x4 &r){
		r.data[0+3*4]=TOADLET_MUL_XX(r.data[0+0*4],-HALF) + TOADLET_MUL_XX(r.data[0+1*4],-HALF) + HALF;
		r.data[1+3*4]=TOADLET_MUL_XX(r.data[1+0*4],-HALF) + TOADLET_MUL_XX(r.data[1+1*4],-HALF) + HALF;
		r.data[2+3*4]=0;
	}

	TOADLET_API void setMatrix4x4FromLookAt(Matrix4x4 &r,const Vector3 &eye,const Vector3 &point,const Vector3 &up,bool camera);

	TOADLET_API void setMatrix4x4FromLookDir(Matrix4x4 &r,const Vector3 &eye,const Vector3 &dir,const Vector3 &up,bool camera);

	TOADLET_API void setMatrix4x4FromOrtho(Matrix4x4 &r,fixed leftDist,fixed rightDist,fixed bottomDist,fixed topDist,fixed nearDist,fixed farDist);

	TOADLET_API void setMatrix4x4FromFrustum(Matrix4x4 &r,fixed leftDist,fixed rightDist,fixed bottomDist,fixed topDist,fixed nearDist,fixed farDist);

	TOADLET_API void setMatrix4x4FromPerspectiveX(Matrix4x4 &r,fixed fovx,fixed aspect,fixed nearDist,fixed farDist);

	TOADLET_API void setMatrix4x4FromPerspectiveY(Matrix4x4 &r,fixed fovy,fixed aspect,fixed nearDist,fixed farDist);

	// Axis Angle operations
	TOADLET_API fixed setAxisAngleFromQuaternion(Vector3 &axis,const Quaternion &q,fixed epsilon);

	// Quaternion operations
	inline void mul(Quaternion &r,const Quaternion &q1,const Quaternion &q2){
		r.x=TOADLET_MUL_XX(+q1.x,q2.w)+TOADLET_MUL_XX(q1.y,q2.z)-TOADLET_MUL_XX(q1.z,q2.y)+TOADLET_MUL_XX(q1.w,q2.x);
		r.y=TOADLET_MUL_XX(-q1.x,q2.z)+TOADLET_MUL_XX(q1.y,q2.w)+TOADLET_MUL_XX(q1.z,q2.x)+TOADLET_MUL_XX(q1.w,q2.y);
		r.z=TOADLET_MUL_XX(+q1.x,q2.y)-TOADLET_MUL_XX(q1.y,q2.x)+TOADLET_MUL_XX(q1.z,q2.w)+TOADLET_MUL_XX(q1.w,q2.z);
		r.w=TOADLET_MUL_XX(-q1.x,q2.x)-TOADLET_MUL_XX(q1.y,q2.y)-TOADLET_MUL_XX(q1.z,q2.z)+TOADLET_MUL_XX(q1.w,q2.w);
	}

	inline void postMul(Quaternion &q1,const Quaternion &q2){
		fixed x=TOADLET_MUL_XX(+q1.x,q2.w)+TOADLET_MUL_XX(q1.y,q2.z)-TOADLET_MUL_XX(q1.z,q2.y)+TOADLET_MUL_XX(q1.w,q2.x);
		fixed y=TOADLET_MUL_XX(-q1.x,q2.z)+TOADLET_MUL_XX(q1.y,q2.w)+TOADLET_MUL_XX(q1.z,q2.x)+TOADLET_MUL_XX(q1.w,q2.y);
		fixed z=TOADLET_MUL_XX(+q1.x,q2.y)-TOADLET_MUL_XX(q1.y,q2.x)+TOADLET_MUL_XX(q1.z,q2.w)+TOADLET_MUL_XX(q1.w,q2.z);
		fixed w=TOADLET_MUL_XX(-q1.x,q2.x)-TOADLET_MUL_XX(q1.y,q2.y)-TOADLET_MUL_XX(q1.z,q2.z)+TOADLET_MUL_XX(q1.w,q2.w);
		q1.x=x;
		q1.y=y;
		q1.z=z;
		q1.w=w;
	}

	inline void preMul(Quaternion &q2,const Quaternion &q1){
		fixed x=TOADLET_MUL_XX(+q1.x,q2.w)+TOADLET_MUL_XX(q1.y,q2.z)-TOADLET_MUL_XX(q1.z,q2.y)+TOADLET_MUL_XX(q1.w,q2.x);
		fixed y=TOADLET_MUL_XX(-q1.x,q2.z)+TOADLET_MUL_XX(q1.y,q2.w)+TOADLET_MUL_XX(q1.z,q2.x)+TOADLET_MUL_XX(q1.w,q2.y);
		fixed z=TOADLET_MUL_XX(+q1.x,q2.y)-TOADLET_MUL_XX(q1.y,q2.x)+TOADLET_MUL_XX(q1.z,q2.w)+TOADLET_MUL_XX(q1.w,q2.z);
		fixed w=TOADLET_MUL_XX(-q1.x,q2.x)-TOADLET_MUL_XX(q1.y,q2.y)-TOADLET_MUL_XX(q1.z,q2.z)+TOADLET_MUL_XX(q1.w,q2.w);
		q2.x=x;
		q2.y=y;
		q2.z=z;
		q2.w=w;
	}

	inline void mul(Vector3 &r,const Quaternion &q){
		fixed x=TOADLET_MUL_XX(+q.y,r.z)-TOADLET_MUL_XX(q.z,r.y)+TOADLET_MUL_XX(q.w,r.x);
		fixed y=TOADLET_MUL_XX(-q.x,r.z)+TOADLET_MUL_XX(q.z,r.x)+TOADLET_MUL_XX(q.w,r.y);
		fixed z=TOADLET_MUL_XX(+q.x,r.y)-TOADLET_MUL_XX(q.y,r.x)+TOADLET_MUL_XX(q.w,r.z);
		fixed w=TOADLET_MUL_XX(-q.x,r.x)-TOADLET_MUL_XX(q.y,r.y)-TOADLET_MUL_XX(q.z,r.z);

		r.x=TOADLET_MUL_XX(+x,+q.w)+TOADLET_MUL_XX(y,-q.z)-TOADLET_MUL_XX(z,-q.y)+TOADLET_MUL_XX(w,-q.x);
		r.y=TOADLET_MUL_XX(-x,-q.z)+TOADLET_MUL_XX(y,+q.w)+TOADLET_MUL_XX(z,-q.x)+TOADLET_MUL_XX(w,-q.y);
		r.z=TOADLET_MUL_XX(+x,-q.y)-TOADLET_MUL_XX(y,-q.x)+TOADLET_MUL_XX(z,+q.w)+TOADLET_MUL_XX(w,-q.z);
	}

	inline void mul(Vector3 &r,const Quaternion &q,const Vector3 &v){
		fixed x=TOADLET_MUL_XX(+q.y,v.z)-TOADLET_MUL_XX(q.z,v.y)+TOADLET_MUL_XX(q.w,v.x);
		fixed y=TOADLET_MUL_XX(-q.x,v.z)+TOADLET_MUL_XX(q.z,v.x)+TOADLET_MUL_XX(q.w,v.y);
		fixed z=TOADLET_MUL_XX(+q.x,v.y)-TOADLET_MUL_XX(q.y,v.x)+TOADLET_MUL_XX(q.w,v.z);
		fixed w=TOADLET_MUL_XX(-q.x,v.x)-TOADLET_MUL_XX(q.y,v.y)-TOADLET_MUL_XX(q.z,v.z);

		r.x=TOADLET_MUL_XX(+x,+q.w)+TOADLET_MUL_XX(y,-q.z)-TOADLET_MUL_XX(z,-q.y)+TOADLET_MUL_XX(w,-q.x);
		r.y=TOADLET_MUL_XX(-x,-q.z)+TOADLET_MUL_XX(y,+q.w)+TOADLET_MUL_XX(z,-q.x)+TOADLET_MUL_XX(w,-q.y);
		r.z=TOADLET_MUL_XX(+x,-q.y)-TOADLET_MUL_XX(y,-q.x)+TOADLET_MUL_XX(z,+q.w)+TOADLET_MUL_XX(w,-q.z);
	}


	inline fixed lengthSquared(const Quaternion &q){
		fixed r=TOADLET_MUL_XX(q.x,q.x) + TOADLET_MUL_XX(q.y,q.y) + TOADLET_MUL_XX(q.z,q.z) + TOADLET_MUL_XX(q.w,q.w);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	inline fixed lengthSquared(const Quaternion &q1,const Quaternion &q2){
		fixed x=q1.x-q2.x;
		fixed y=q1.y-q2.y;
		fixed z=q1.z-q2.z;
		fixed w=q1.w-q2.w;
		fixed r=TOADLET_MUL_XX(x,x) + TOADLET_MUL_XX(y,y) + TOADLET_MUL_XX(z,z) + TOADLET_MUL_XX(w,w);
		TOADLET_CHECK_OVERFLOW(r,"overflow in lengthSquared");
		return r;
	}

	inline fixed length(const Quaternion &q){
		return sqrt(lengthSquared(q));
	}

	inline fixed length(const Quaternion &q1,const Quaternion &q2){
		return sqrt(lengthSquared(q1,q2));
	}

	inline void normalize(Quaternion &q){
		fixed l=TOADLET_DIV_XX(ONE,length(q));
		q.x=TOADLET_MUL_XX(q.x,l);
		q.y=TOADLET_MUL_XX(q.y,l);
		q.z=TOADLET_MUL_XX(q.z,l);
		q.w=TOADLET_MUL_XX(q.w,l);
	}

	inline void normalize(Quaternion &r,const Quaternion &q){
		fixed l=TOADLET_DIV_XX(ONE,length(q));
		r.x=TOADLET_MUL_XX(q.x,l);
		r.y=TOADLET_MUL_XX(q.y,l);
		r.z=TOADLET_MUL_XX(q.z,l);
		r.w=TOADLET_MUL_XX(q.w,l);
	}

	inline bool normalizeCarefully(Quaternion &q,fixed epsilon){
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

	inline bool normalizeCarefully(Quaternion &r,const Quaternion &q,fixed epsilon){
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

	inline fixed dot(const Quaternion &q1,const Quaternion &q2){
		return TOADLET_MUL_XX(q1.x,q2.x) + TOADLET_MUL_XX(q1.y,q2.y) + TOADLET_MUL_XX(q1.z,q2.z) + TOADLET_MUL_XX(q1.w,q2.w);
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
		fixed i=TOADLET_DIV_XX(ONE,TOADLET_MUL_XX(q.x,q.x)+TOADLET_MUL_XX(q.y,q.y)+TOADLET_MUL_XX(q.z,q.z)+TOADLET_MUL_XX(q.w,q.w));
		q.x=TOADLET_MUL_XX(q.x,-i);
		q.y=TOADLET_MUL_XX(q.y,-i);
		q.z=TOADLET_MUL_XX(q.z,-i);
		q.w=TOADLET_MUL_XX(q.w,i);
	}

	inline void invert(Quaternion &r,const Quaternion &q){
		fixed i=TOADLET_DIV_XX(ONE,TOADLET_MUL_XX(q.x,q.x)+TOADLET_MUL_XX(q.y,q.y)+TOADLET_MUL_XX(q.z,q.z)+TOADLET_MUL_XX(q.w,q.w));
		r.x=TOADLET_MUL_XX(q.x,-i);
		r.y=TOADLET_MUL_XX(q.y,-i);
		r.z=TOADLET_MUL_XX(q.z,-i);
		r.w=TOADLET_MUL_XX(q.w,i);
	}

	TOADLET_API void setQuaternionFromMatrix3x3(Quaternion &r,const Matrix3x3 &m);

	TOADLET_API void setQuaternionFromMatrix4x4(Quaternion &r,const Matrix4x4 &m);

	inline void setQuaternionFromAxisAngle(Quaternion &r,const Vector3 &axis,fixed angle){
		fixed halfAngle=angle>>1;
		fixed sinHalfAngle=sin(halfAngle);
		r.x=TOADLET_MUL_XX(axis.x,sinHalfAngle);
		r.y=TOADLET_MUL_XX(axis.y,sinHalfAngle);
		r.z=TOADLET_MUL_XX(axis.z,sinHalfAngle);
		r.w=cos(halfAngle);
	}

	inline void setQuaternionFromEulerAngleXYZ(Quaternion &r,const EulerAngle &euler){
		fixed sx=euler.x>>1;
		fixed cx=cos(sx);
		sx=sin(sx);
		fixed sy=euler.y>>1;
		fixed cy=cos(sy);
		sy=sin(sy);
		fixed sz=euler.z>>1;
		fixed cz=cos(sz);
		sz=sin(sz);
		fixed cxcy=TOADLET_MUL_XX(cx,cy);
		fixed sxsy=TOADLET_MUL_XX(sx,sy);
		fixed cxsy=TOADLET_MUL_XX(cx,sy);
		fixed sxcy=TOADLET_MUL_XX(sx,cy);

		r.w=TOADLET_MUL_XX(cxcy,cz) - TOADLET_MUL_XX(sxsy,sz);
  		r.x=TOADLET_MUL_XX(cxcy,sz) + TOADLET_MUL_XX(sxsy,cz);
		r.y=TOADLET_MUL_XX(sxcy,cz) + TOADLET_MUL_XX(cxsy,sz);
		r.z=TOADLET_MUL_XX(cxsy,cz) - TOADLET_MUL_XX(sxcy,sz);
	}

	TOADLET_API void lerp(Quaternion &r,const Quaternion &q1,const Quaternion &q2,fixed t);

	TOADLET_API void slerp(Quaternion &r,const Quaternion &q1,const Quaternion &q2,fixed t);

	// Segment operations
	TOADLET_API void getClosestPointOnSegment(Vector3 &result,const Segment &segment,const Vector3 &point);

	TOADLET_API void getClosestPointsOnSegments(Vector3 &point1,Vector3 &point2,const Segment &seg1,const Segment &seg2,fixed epsilon);

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
		result.x=point.x - TOADLET_MUL_XX(plane.normal.x,plane.distance);
		result.y=point.y - TOADLET_MUL_XX(plane.normal.y,plane.distance);
		result.z=point.z - TOADLET_MUL_XX(plane.normal.z,plane.distance);

		fixed f=TOADLET_MUL_XX(plane.normal.x,result.x) + TOADLET_MUL_XX(plane.normal.y,result.y) + TOADLET_MUL_XX(plane.normal.z,result.z);

		result.x=point.x - TOADLET_MUL_XX(plane.normal.x,f);
		result.y=point.y - TOADLET_MUL_XX(plane.normal.y,f);
		result.z=point.z - TOADLET_MUL_XX(plane.normal.z,f);
	}

	inline void project(const Plane &plane,Vector3 &point){
		fixed tx=point.x - TOADLET_MUL_XX(plane.normal.x,plane.distance);
		fixed ty=point.y - TOADLET_MUL_XX(plane.normal.y,plane.distance);
		fixed tz=point.z - TOADLET_MUL_XX(plane.normal.z,plane.distance);

		fixed f=TOADLET_MUL_XX(plane.normal.x,tx) + TOADLET_MUL_XX(plane.normal.y,ty) + TOADLET_MUL_XX(plane.normal.z,tz);

		point.x=point.x - TOADLET_MUL_XX(plane.normal.x,f);
		point.y=point.y - TOADLET_MUL_XX(plane.normal.y,f);
		point.z=point.z - TOADLET_MUL_XX(plane.normal.z,f);
	}

	inline void normalize(Plane &plane){
		fixed l=TOADLET_DIV_XX(ONE,length(plane.normal));
		plane.normal.x=TOADLET_MUL_XX(plane.normal.x,l);
		plane.normal.y=TOADLET_MUL_XX(plane.normal.y,l);
		plane.normal.z=TOADLET_MUL_XX(plane.normal.z,l);
		plane.distance=mul(plane.distance,l);
	}

	inline void normalize(Plane &r,const Plane &plane){
		fixed l=TOADLET_DIV_XX(ONE,length(plane.normal));
		r.normal.x=TOADLET_MUL_XX(plane.normal.x,l);
		r.normal.y=TOADLET_MUL_XX(plane.normal.y,l);
		r.normal.z=TOADLET_MUL_XX(plane.normal.z,l);
		r.distance=mul(plane.distance,l);
	}

	inline fixed length(const Plane &p,const Vector3 &v){
		return TOADLET_MUL_XX(p.normal.x,v.x) + TOADLET_MUL_XX(p.normal.y,v.y) + TOADLET_MUL_XX(p.normal.z,v.z) - p.distance;
	}

	TOADLET_API bool getIntersectionOfThreePlanes(Vector3 &result,const Plane &p1,const Plane &p2,const Plane &p3,fixed epsilon);

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

	// Intersection operations
	TOADLET_API bool testInside(const Vector3 &point,const Plane &plane);
	TOADLET_API bool testInside(const Vector3 &point,const Sphere &sphere);
	TOADLET_API bool testInside(const Vector3 &point,const AABox &box);

	TOADLET_API bool testIntersection(const AABox &box1,const AABox &box2);
	TOADLET_API bool testIntersection(const Sphere &sphere,const AABox &box);

	TOADLET_API fixed findIntersection(const Segment &segment,const Plane &plane,Vector3 &point,Vector3 &normal);
	TOADLET_API fixed findIntersection(const Segment &segment,const Sphere &sphere,Vector3 &point,Vector3 &normal);
	TOADLET_API fixed findIntersection(const Segment &segment,const AABox &box,Vector3 &point,Vector3 &normal);
}

}
}
}

#endif

