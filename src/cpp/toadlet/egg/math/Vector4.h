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

#ifndef TOADLET_EGG_MATH_VECTOR4_H
#define TOADLET_EGG_MATH_VECTOR4_H

#include <toadlet/egg/math/BaseMath.h>
#include <toadlet/egg/math/Vector3.h>

namespace toadlet{
namespace egg{
namespace math{

class Vector4{
public:
	real x,y,z,w;

	Vector4():x(0),y(0),z(0),w(0){}

	Vector4(real x1,real y1,real z1,real w1):x(x1),y(y1),z(z1),w(w1){}

	Vector4(const Vector2 &v,real z1,real w1):x(v.x),y(v.y),z(z1),w(w1){}

	Vector4(const Vector3 &v,real w1):x(v.x),y(v.y),z(v.z),w(w1){}

	inline Vector4 &set(const Vector4 &v){
		x=v.x;
		y=v.y;
		z=v.z;
		w=v.w;

		return *this;
	}

	inline Vector4 &set(real x1,real y1,real z1,real w1){
		x=x1;
		y=y1;
		z=z1;
		w=w1;

		return *this;
	}

	inline Vector4 &set(const Vector2 &v,real z1,real w1){
		x=v.x;
		y=v.y;
		z=z1;
		w=w1;

		return *this;
	}

	inline Vector4 &set(const Vector3 &v,real w1){
		x=v.x;
		y=v.y;
		z=v.z;
		w=w1;

		return *this;
	}

	inline Vector4 &reset(){
		x=0;
		y=0;
		z=0;
		w=0;

		return *this;
	}

	inline real *getData(){return &x;}
	inline const real *getData() const{return &x;}

	inline bool equals(const Vector4 &vec) const{
		return (vec.x==x && vec.y==y && vec.z==z && vec.w==w);
	}

	inline bool operator==(const Vector4 &vec) const{
		return (vec.x==x && vec.y==y && vec.z==z && vec.w==w);
	}

	inline bool operator!=(const Vector4 &vec) const{
		return (vec.x!=x || vec.y!=y || vec.z!=z || vec.w!=w);
	}

	inline Vector4 operator+(const Vector4 &vec) const{
		return Vector4(x+vec.x,y+vec.y,z+vec.z,w+vec.w);
	}

	inline void operator+=(const Vector4 &vec){
		x+=vec.x;
		y+=vec.y;
		z+=vec.z;
		w+=vec.w;
	}

	inline Vector4 operator-(const Vector4 &vec) const{
		return Vector4(x-vec.x,y-vec.y,z-vec.z,w-vec.w);
	}

	inline void operator-=(const Vector4 &vec){
		x-=vec.x;
		y-=vec.y;
		z-=vec.z;
		w-=vec.w;
	}

	inline Vector4 operator*(real f) const{
		return Vector4(x*f,y*f,z*f,w*f);
	}

	inline void operator*=(real f){
		x*=f;
		y*=f;
		z*=f;
		w*=f;
	}

	inline Vector4 operator*(const Vector4 &vec) const{
		return Vector4(x*vec.x,y*vec.y,z*vec.z,w*vec.w);
	}

	inline void operator*=(const Vector4 &vec){
		x*=vec.x;
		y*=vec.y;
		z*=vec.z;
		w*=vec.w;
	}

	inline Vector4 operator/(real f) const{
		f=1.0/f;
		return Vector4(x*f,y*f,z*f,w*f);
	}

	inline void operator/=(real f){
		f=1.0/f;
		x*=f;
		y*=f;
		z*=f;
		w*=f;
	}

	inline void operator/=(const Vector4 &vec){
		x/=vec.x;
		y/=vec.y;
		z/=vec.z;
		w/=vec.w;
	}

	inline Vector4 operator-() const{
		return Vector4(-x,-y,-z,-w);
	}

	inline real &operator[](int i){
		return *(&x+i);
	}

	inline real operator[](int i) const{
		return *(&x+i);
	}
};

inline Vector4 operator*(real f,const Vector4 &vec){
	return Vector4(vec.x*f,vec.y*f,vec.z*f,vec.w*f);
}

inline Vector4 operator/(real f,const Vector4 &vec){
	f=1.0/f;
	return Vector4(vec.x*f,vec.y*f,vec.z*f,vec.w*f);
}

}
}
}

#endif
