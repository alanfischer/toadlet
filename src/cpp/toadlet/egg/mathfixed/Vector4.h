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

class TOADLET_API Vector4{
public:
	fixed x,y,z,w;

	inline Vector4():x(0),y(0),z(0),w(0){}

	inline Vector4(fixed x1,fixed y1,fixed z1,fixed w1):x(x1),y(y1),z(z1),w(w1){}

	inline Vector4(fixed v[]):x(v[0]),y(v[1]),z(v[2]),w(v[3]){}

	inline Vector4(const Vector2 &v,fixed z1,fixed w1):x(v.x),y(v.y),z(z1),w(w1){}

	inline Vector4(const Vector3 &v,fixed w1):x(v.x),y(v.y),z(v.z),w(w1){}

	explicit inline Vector4(uint32 rgba){setRGBA(rgba);}

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

	inline Vector4 &setRGBA(uint32 rgba){
		x=Math::fromInt((rgba&0xFF000000)>>24)/255;
		y=Math::fromInt((rgba&0x00FF0000)>>16)/255;
		z=Math::fromInt((rgba&0x0000FF00)>>8 )/255;
		w=Math::fromInt((rgba&0x000000FF)>>0 )/255;

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

	inline bool operator==(const Vector4 &v) const{
		return (v.x==x && v.y==y && v.z==z && v.w==w);
	}

	inline bool operator!=(const Vector4 &v) const{
		return (v.x!=x || v.y!=y || v.z!=z || v.w!=w);
	}

	inline Vector4 operator+(const Vector4 &v) const{
		return Vector4(x+v.x,y+v.y,z+v.z,w+v.w);
	}

	inline void operator+=(const Vector4 &v){
		x+=v.x;
		y+=v.y;
		z+=v.z;
		w+=v.w;
	}

	inline Vector4 operator-(const Vector4 &v) const{
		return Vector4(x-v.x,y-v.y,z-v.z,w-v.w);
	}

	inline void operator-=(const Vector4 &v){
		x-=v.x;
		y-=v.y;
		z-=v.z;
		w-=v.w;
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

	inline Vector4 operator*(const Vector4 &v) const{
		return Vector4(x*v.x,y*v.y,z*v.z,w*v.w);
	}

	inline void operator*=(const Vector4 &v){
		x=Math::mul(x,v.x);
		y=Math::mul(y,v.y);
		z=Math::mul(z,v.z);
		w=Math::mul(w,v.w);
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

	inline void operator/=(const Vector4 &v){
		x=Math::div(x,v.x);
		y=Math::div(y,v.y);
		z=Math::div(z,v.z);
		w=Math::div(w,v.w);
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

inline Vector4 operator*(fixed f,const Vector4 &v){
	return Vector4(Math::mul(v.x,f),Math::mul(v.y,f),Math::mul(v.z,f),Math::mul(v.w,f));
}

inline Vector4 operator/(fixed f,const Vector4 &v){
	f=Math::div(Math::ONE,f);
	return Vector4(Math::mul(v.x,f),Math::mul(v.y,f),Math::mul(v.z,f),Math::mul(v.w,f));
}

}
}
}

#endif
