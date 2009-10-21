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

#ifndef TOADLET_EGG_MATHFIXED_VECTOR4_H
#define TOADLET_EGG_MATHFIXED_VECTOR4_H

#include <toadlet/egg/mathfixed/BaseMath.h>
#include <toadlet/egg/mathfixed/Vector3.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

class Vector4{
public:
	fixed x,y,z,w;

	Vector4():x(0),y(0),z(0),w(0){}

	Vector4(fixed x1,fixed y1,fixed z1,fixed w1):x(x1),y(y1),z(z1),w(w1){}

	Vector4(const Vector2 &v,fixed z1,fixed w1):x(v.x),y(v.y),z(z1),w(w1){}

	Vector4(const Vector3 &v,fixed w1):x(v.x),y(v.y),z(v.z),w(w1){}

	inline Vector4 &set(const Vector4 &v){
		x=v.x;
		y=v.y;
		z=v.z;
		w=v.w;

		return *this;
	}

	inline Vector4 &set(fixed x1,fixed y1,fixed z1,fixed w1){
		x=x1;
		y=y1;
		z=z1;
		w=w1;

		return *this;
	}

	inline Vector4 &set(const Vector2 &v,fixed z1,fixed w1){
		x=v.x;
		y=v.y;
		z=z1;
		w=w1;

		return *this;
	}

	inline Vector4 &set(const Vector3 &v,fixed w1){
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

	inline fixed *getData(){return &x;}
	inline const fixed *getData() const{return &x;}

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

	inline Vector4 operator*(fixed f) const{
		return Vector4(Math::mul(x,f),Math::mul(y,f),Math::mul(z,f),Math::mul(w,f));
	}

	inline void operator*=(fixed f){
		x=Math::mul(x,f);
		y=Math::mul(y,f);
		z=Math::mul(z,f);
		w=Math::mul(w,f);
	}

	inline Vector4 operator*(const Vector4 &vec) const{
		return Vector4(x*vec.x,y*vec.y,z*vec.z,w*vec.w);
	}

	inline void operator*=(const Vector4 &vec){
		x=Math::mul(x,vec.x);
		y=Math::mul(y,vec.y);
		z=Math::mul(z,vec.z);
		w=Math::mul(w,vec.w);
	}

	inline Vector4 operator/(fixed f) const{
		f=Math::div(Math::ONE,f);
		return Vector4(Math::mul(x,f),Math::mul(y,f),Math::mul(z,f),Math::mul(w,f));
	}

	inline void operator/=(fixed f){
		f=Math::div(Math::ONE,f);
		x=Math::mul(x,f);
		y=Math::mul(y,f);
		z=Math::mul(z,f);
		w=Math::mul(w,f);
	}

	inline void operator/=(const Vector4 &vec){
		x=Math::div(x,vec.x);
		y=Math::div(y,vec.y);
		z=Math::div(z,vec.z);
		w=Math::div(w,vec.w);
	}

	inline Vector4 operator-() const{
		return Vector4(-x,-y,-z,-w);
	}

	inline fixed &operator[](int i){
		return *(&x+i);
	}

	inline fixed operator[](int i) const{
		return *(&x+i);
	}
};

inline Vector4 operator*(fixed f,const Vector4 &vec){
	return Vector4(Math::mul(vec.x,f),Math::mul(vec.y,f),Math::mul(vec.z,f),Math::mul(vec.w,f));
}

inline Vector4 operator/(fixed f,const Vector4 &vec){
	f=Math::div(Math::ONE,f);
	return Vector4(Math::mul(vec.x,f),Math::mul(vec.y,f),Math::mul(vec.z,f),Math::mul(vec.w,f));
}

}
}
}

#endif
