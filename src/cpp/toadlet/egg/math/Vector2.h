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

#ifndef TOADLET_EGG_MATH_VECTOR2_H
#define TOADLET_EGG_MATH_VECTOR2_H

#include <toadlet/egg/math/BaseMath.h>

namespace toadlet{
namespace egg{
namespace math{

class Vector2{
public:
	real x,y;

	inline Vector2():x(0),y(0){}

	inline Vector2(real x1,real y1):x(x1),y(y1){}

	inline Vector2(real v[]):x(v[0]),y(v[1]){}

	inline Vector2 &set(const Vector2 &v){
		x=v.x;
		y=v.y;

		return *this;
	}

	inline Vector2 &set(real x1,real y1){
		x=x1;
		y=y1;

		return *this;
	}

	inline Vector2 &reset(){
		x=0;
		y=0;

		return *this;
	}

	inline real *getData(){return &x;}
	inline const real *getData() const{return &x;}

	inline bool equals(const Vector2 &v) const{
		return (v.x==x && v.y==y);
	}

	inline bool operator==(const Vector2 &v) const{
		return (v.x==x && v.y==y);
	}

	inline bool operator!=(const Vector2 &v) const{
		return (v.x!=x || v.y!=y);
	}

	inline Vector2 operator+(const Vector2 &v) const{
		return Vector2(x+v.x,y+v.y);
	}

	inline void operator+=(const Vector2 &v){
		x+=v.x;
		y+=v.y;
	}

	inline Vector2 operator-(const Vector2 &v) const{
		return Vector2(x-v.x,y-v.y);
	}

	inline void operator-=(const Vector2 &v){
		x-=v.x;
		y-=v.y;
	}

	inline Vector2 operator*(real f) const{
		return Vector2(x*f,y*f);
	}

	inline void operator*=(real f){
		x*=f;
		y*=f;
	}

	inline Vector2 operator*(const Vector2 &v) const{
		return Vector2(x*v.x,y*v.y);
	}

	inline void operator*=(const Vector2 &v){
		x*=v.x;
		y*=v.y;
	}

	inline Vector2 operator/(real f) const{
		f=1.0/f;
		return Vector2(x*f,y*f);
	}

	inline void operator/=(real f){
		f=1.0/f;
		x*=f;
		y*=f;
	}

	inline Vector2 operator/(const Vector2 &v) const{
		return Vector2(x/v.x,y/v.y);
	}

	inline void operator/=(const Vector2 &v){
		x/=v.x;
		y/=v.y;
	}

	inline Vector2 operator-() const{
		return Vector2(-x,-y);
	}

	inline real &operator[](int i){
		return *(&x+i);
	}

	inline real operator[](int i) const{
		return *(&x+i);
	}
};

inline Vector2 operator*(real f,const Vector2 &v){
	return Vector2(v.x*f,v.y*f);
}

inline Vector2 operator/(real f,const Vector2 &v){
	f=1.0/f;
	return Vector2(v.x*f,v.y*f);
}

}
}
}

#endif
