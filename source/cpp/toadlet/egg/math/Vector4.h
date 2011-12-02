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

class TOADLET_API TOADLET_ALIGNED Vector4{
public:
	real x,y,z,w;

	inline Vector4():x(0),y(0),z(0),w(0){}

	inline Vector4(real x1,real y1,real z1,real w1):x(x1),y(y1),z(z1),w(w1){}

	inline Vector4(real v[]):x(v[0]),y(v[1]),z(v[2]),w(v[3]){}

	inline Vector4(const Vector2 &v,real z1,real w1):x(v.x),y(v.y),z(z1),w(w1){}

	inline Vector4(const Vector3 &v,real w1):x(v.x),y(v.y),z(v.z),w(w1){}

	explicit inline Vector4(uint32 rgba){setRGBA(rgba);}

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

	inline Vector4 &setRGBA(uint32 rgba){
		x=Math::fromInt((rgba&0xFF000000)>>24)/255;
		y=Math::fromInt((rgba&0x00FF0000)>>16)/255;
		z=Math::fromInt((rgba&0x0000FF00)>>8 )/255;
		w=Math::fromInt((rgba&0x000000FF)>>0 )/255;

		return *this;
	}
	
	inline Vector4 &setARGB(uint32 argb){
		x=Math::fromInt((argb&0x00FF0000)>>16)/255;
		y=Math::fromInt((argb&0x0000FF00)>>8 )/255;
		z=Math::fromInt((argb&0x000000FF)>>0 )/255;
		w=Math::fromInt((argb&0xFF000000)>>24)/255;

		return *this;
	}

	inline uint32 getRGBA() const{
		return ((uint32)Math::toInt(x*255))<<24 |
			((uint32)Math::toInt(y*255))<<16 |
			((uint32)Math::toInt(z*255))<<8  |
			((uint32)Math::toInt(w*255))<<0;
	}

	inline uint32 getARGB() const{
		return ((uint32)Math::toInt(x*255))<<16  |
			((uint32)Math::toInt(y*255))<<8  |
			((uint32)Math::toInt(z*255))<<0  |
			((uint32)Math::toInt(w*255))<<24;
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

	inline bool equals(const Vector4 &v) const{
		return (v.x==x && v.y==y && v.z==z && v.w==w);
	}

	inline bool operator==(const Vector4 &v) const{
		return equals(v);
	}

	inline bool operator!=(const Vector4 &v) const{
		return !equals(v);
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

	inline Vector4 operator*(real f) const{
		return Vector4(x*f,y*f,z*f,w*f);
	}

	inline void operator*=(real f){
		x*=f;
		y*=f;
		z*=f;
		w*=f;
	}

	inline Vector4 operator*(const Vector4 &v) const{
		return Vector4(x*v.x,y*v.y,z*v.z,w*v.w);
	}

	inline void operator*=(const Vector4 &v){
		x*=v.x;
		y*=v.y;
		z*=v.z;
		w*=v.w;
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

	inline void operator/=(const Vector4 &v){
		x/=v.x;
		y/=v.y;
		z/=v.z;
		w/=v.w;
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

inline Vector4 operator*(real f,const Vector4 &v){
	return Vector4(v.x*f,v.y*f,v.z*f,v.w*f);
}

inline Vector4 operator/(real f,const Vector4 &v){
	f=1.0/f;
	return Vector4(v.x*f,v.y*f,v.z*f,v.w*f);
}

}
}
}

#endif
