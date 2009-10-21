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

#ifndef TOADLET_EGG_MATH_VECTOR3_H
#define TOADLET_EGG_MATH_VECTOR3_H

#include <toadlet/egg/math/BaseMath.h>
#include <toadlet/egg/math/Vector2.h>

namespace toadlet{
namespace egg{
namespace math{

class Vector3{
public:
	real x,y,z;

	Vector3():x(0),y(0),z(0){}

	Vector3(real x1,real y1,real z1):x(x1),y(y1),z(z1){}

	Vector3(const Vector2 &v,real z1):x(v.x),y(v.y),z(z1){}

	inline Vector3 &set(const Vector3 &v){
		x=v.x;
		y=v.y;
		z=v.z;

		return *this;
	}

	inline Vector3 &set(real x1,real y1,real z1){
		x=x1;
		y=y1;
		z=z1;

		return *this;
	}

	inline Vector3 &set(const Vector2 &v,real z1){
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

	inline real *getData(){return &x;}
	inline const real *getData() const{return &x;}

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

	inline Vector3 operator*(real f) const{
		return Vector3(x*f,y*f,z*f);
	}

	inline void operator*=(real f){
		x*=f;
		y*=f;
		z*=f;
	}

	inline Vector3 operator*(const Vector3 &vec) const{
		return Vector3(x*vec.x,y*vec.y,z*vec.z);
	}

	inline void operator*=(const Vector3 &vec){
		x*=vec.x;
		y*=vec.y;
		z*=vec.z;
	}

	inline Vector3 operator/(real f) const{
		f=1.0/f;
		return Vector3(x*f,y*f,z*f);
	}

	inline void operator/=(real f){
		f=1.0/f;
		x*=f;
		y*=f;
		z*=f;
	}

	inline Vector3 operator/(const Vector3 &vec) const{
		return Vector3(x/vec.x,y/vec.y,z/vec.z);
	}

	inline void operator/=(const Vector3 &vec){
		x/=vec.x;
		y/=vec.y;
		z/=vec.z;
	}

	inline Vector3 operator-() const{
		return Vector3(-x,-y,-z);
	}

	inline real &operator[](int i){
		return *(&x+i);
	}

	inline real operator[](int i) const{
		return *(&x+i);
	}
};

inline Vector3 operator*(real f,const Vector3 &vec){
	return Vector3(vec.x*f,vec.y*f,vec.z*f);
}

inline Vector3 operator/(real f,const Vector3 &vec){
	f=1.0/f;
	return Vector3(vec.x*f,vec.y*f,vec.z*f);
}

}
}
}

#endif
 
