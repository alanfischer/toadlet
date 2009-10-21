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

#ifndef TOADLET_EGG_MATHFIXED_VECTOR3_H
#define TOADLET_EGG_MATHFIXED_VECTOR3_H

#include <toadlet/egg/mathfixed/BaseMath.h>
#include <toadlet/egg/mathfixed/Vector2.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

class Vector3{
public:
	fixed x,y,z;

	Vector3():x(0),y(0),z(0){}

	Vector3(fixed x1,fixed y1,fixed z1):x(x1),y(y1),z(z1){}

	Vector3(const Vector2 &v,fixed z1):x(v.x),y(v.y),z(z1){}

	inline Vector3 &set(const Vector3 &v){
		x=v.x;
		y=v.y;
		z=v.z;

		return *this;
	}

	inline Vector3 &set(fixed x1,fixed y1,fixed z1){
		x=x1;
		y=y1;
		z=z1;

		return *this;
	}

	inline Vector3 &set(const Vector2 &v,fixed z1){
		x=v.x;
		y=v.y;
		z=z1;

		return *this;
	}

	inline Vector3 &reset(){
		x=0;
		y=0;
		z=0;

		return *this;
	}

	inline fixed *getData(){return &x;}
	inline const fixed *getData() const{return &x;}

	inline bool equals(const Vector3 &vec) const{
		return (vec.x==x && vec.y==y && vec.z==z);
	}

	inline bool operator==(const Vector3 &vec) const{
		return (vec.x==x && vec.y==y && vec.z==z);
	}

	inline bool operator!=(const Vector3 &vec) const{
		return (vec.x!=x || vec.y!=y || vec.z!=z);
	}

	inline Vector3 operator+(const Vector3 &vec) const{
		return Vector3(x+vec.x,y+vec.y,z+vec.z);
	}

	inline void operator+=(const Vector3 &vec){
		x+=vec.x;
		y+=vec.y;
		z+=vec.z;
	}

	inline Vector3 operator-(const Vector3 &vec) const{
		return Vector3(x-vec.x,y-vec.y,z-vec.z);
	}

	inline void operator-=(const Vector3 &vec){
		x-=vec.x;
		y-=vec.y;
		z-=vec.z;
	}

	inline Vector3 operator*(fixed f) const{
		return Vector3(Math::mul(x,f),Math::mul(y,f),Math::mul(z,f));
	}

	inline void operator*=(fixed f){
		x=Math::mul(x,f);
		y=Math::mul(y,f);
		z=Math::mul(z,f);
	}

	inline Vector3 operator*(const Vector3 &vec) const{
		return Vector3(Math::mul(x,vec.x),Math::mul(y,vec.y),Math::mul(z,vec.z));
	}

	inline void operator*=(const Vector3 &vec){
		x=Math::mul(x,vec.x);
		y=Math::mul(y,vec.y);
		z=Math::mul(z,vec.z);
	}

	inline Vector3 operator/(fixed f) const{
		f=Math::div(Math::ONE,f);
		return Vector3(Math::mul(x,f),Math::mul(y,f),Math::mul(z,f));
	}

	inline void operator/=(fixed f){
		f=Math::mul(Math::ONE,f);
		x=Math::mul(x,f);
		y=Math::mul(y,f);
		z=Math::mul(z,f);
	}

	inline Vector3 operator/(const Vector3 &vec) const{
		return Vector3(Math::mul(x,vec.x),Math::mul(y,vec.y),Math::mul(z,vec.z));
	}

	inline void operator/=(const Vector3 &vec){
		x=Math::mul(x,vec.x);
		y=Math::mul(y,vec.y);
		z=Math::mul(z,vec.z);
	}

	inline Vector3 operator-() const{
		return Vector3(-x,-y,-z);
	}

	inline fixed &operator[](int i){
		return *(&x+i);
	}

	inline fixed operator[](int i) const{
		return *(&x+i);
	}
};

inline Vector3 operator*(fixed f,const Vector3 &vec){
	return Vector3(Math::mul(vec.x,f),Math::mul(vec.y,f),Math::mul(vec.z,f));
}

inline Vector3 operator/(fixed f,const Vector3 &vec){
	f=Math::div(Math::ONE,f);
	return Vector3(Math::mul(vec.x,f),Math::mul(vec.y,f),Math::mul(vec.z,f));
}

}
}
}

#endif
 
