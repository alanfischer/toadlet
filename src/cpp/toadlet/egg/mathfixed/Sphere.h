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

#ifndef TOADLET_EGG_MATHFIXED_SPHERE_H
#define TOADLET_EGG_MATHFIXED_SPHERE_H

#include <toadlet/egg/mathfixed/Vector3.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

class Sphere{
public:
	inline Sphere():radius(0){}

	inline Sphere(fixed radius):radius(radius){}

	inline Sphere(const Vector3 &origin1,fixed radius1):origin(origin1),radius(radius1){}

	inline Sphere &set(const Sphere &sphere){
		origin.x=sphere.origin.x;
		origin.y=sphere.origin.y;
		origin.z=sphere.origin.z;

		radius=sphere.radius;

		return *this;
	}

	inline Sphere &set(const Vector3 &origin1,fixed radius1){
		origin.x=origin1.x;
		origin.y=origin1.y;
		origin.z=origin1.z;

		radius=radius1;

		return *this;
	}

	inline Sphere &reset(){
		origin.x=0;
		origin.y=0;
		origin.z=0;

		radius=0;

		return *this;
	}

	inline void setOrigin(const Vector3 &origin1){
		origin.x=origin1.x;
		origin.y=origin1.y;
		origin.z=origin1.z;
	}

	inline void setOrigin(fixed x,fixed y,fixed z){
		origin.x=x;
		origin.y=y;
		origin.z=z;
	}

	inline Vector3& getOrigin(){
		return origin;
	}

	inline const Vector3& getOrigin() const{
		return origin;
	}

	inline void setRadius(fixed radius1){
		radius=radius1;
	}

	inline fixed getRadius() const{
		return radius;
	}

	inline bool equals(const Sphere &sphere) const{
		return (sphere.origin.equals(origin) && sphere.radius==radius);
	}

	inline bool operator==(const Sphere &sphere) const{
		return (sphere.origin==origin && sphere.radius==radius);
	}

	inline bool operator!=(const Sphere &sphere) const{
		return (sphere.origin!=origin || sphere.radius!=radius);
	}

	inline Sphere operator+(const Vector3 &vec) const{
		return Sphere(origin+vec,radius);
	}

	inline void operator+=(const Vector3 &vec){
		origin+=vec;
	}

	inline Sphere operator-(const Vector3 &vec) const{
		return Sphere(origin-vec,radius);
	}

	inline void operator-=(const Vector3 &vec){
		origin-=vec;
	}

	Vector3 origin;
	fixed radius;
};

}
}
}

#endif
