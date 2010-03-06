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

#ifndef TOADLET_EGG_MATHFIXED_VECTOR2_H
#define TOADLET_EGG_MATHFIXED_VECTOR2_H

#include <toadlet/egg/mathfixed/BaseMath.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

class Vector2{
public:
	fixed x,y;

	inline Vector2():x(0),y(0){}

	inline Vector2(fixed x1,fixed y1):x(x1),y(y1){}

	inline Vector2(fixed v[]):x(v[0]),y(v[1]){}

	inline Vector2 &set(const Vector2 &v){
		x=v.x;
		y=v.y;

		return *this;
	}

	inline Vector2 &set(fixed x1,fixed y1){
		x=x1;
		y=y1;

		return *this;
	}

	inline Vector2 &reset(){
		x=0;
		y=0;

		return *this;
	}

	inline fixed *getData(){return &x;}
	inline const fixed *getData() const{return &x;}

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

	inline Vector2 operator*(fixed f) const{
		return Vector2(Math::mul(x,f),Math::mul(y,f));
	}

	inline void operator*=(fixed f){
		x=Math::mul(x,f);
		y=Math::mul(y,f);
	}

	inline Vector2 operator*(const Vector2 &v) const{
		return Vector2(Math::mul(x,v.x),Math::mul(y,v.y));
	}

	inline void operator*=(const Vector2 &v){
		x=Math::mul(x,v.x);
		y=Math::mul(y,v.y);
	}

	inline Vector2 operator/(fixed f) const{
		f=Math::div(Math::ONE,f);
		return Vector2(Math::mul(x,f),Math::mul(y,f));
	}

	inline void operator/=(fixed f){
		f=Math::div(Math::ONE,f);
		x=Math::mul(x,f);
		y=Math::mul(y,f);
	}

	inline Vector2 operator/(const Vector2 &v) const{
		return Vector2(Math::div(x,v.x),Math::div(y,v.y));
	}

	inline void operator/=(const Vector2 &v){
		x=Math::div(x,v.x);
		y=Math::div(y,v.y);
	}

	inline Vector2 operator-() const{
		return Vector2(-x,-y);
	}

	inline fixed &operator[](int i){
		return *(&x+i);
	}

	inline fixed operator[](int i) const{
		return *(&x+i);
	}
};

inline Vector2 operator*(fixed f,const Vector2 &v){
	return Vector2(Math::mul(v.x,f),Math::mul(v.y,f));
}

inline Vector2 operator/(fixed f,const Vector2 &v){
	f=Math::div(Math::ONE,f);
	return Vector2(Math::mul(v.x,f),Math::mul(v.y,f));
}

}
}
}

#endif
