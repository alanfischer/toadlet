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

#ifndef TOADLET_EGG_MATHFIXED_CAPSULE_H
#define TOADLET_EGG_MATHFIXED_CAPSULE_H

#include <toadlet/egg/mathfixed/BaseMath.h>
#include <toadlet/egg/mathfixed/Vector3.h>
#include <toadlet/egg/mathfixed/Matrix3x3.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

class Capsule{
public:
	inline Capsule():radius(0){}

	inline Capsule(const Vector3 &origin1,const Vector3 &direction1,fixed radius1):origin(origin1),direction(direction1),radius(radius1){}

	inline Capsule &set(const Capsule &capsule){
		origin.x=capsule.origin.x;
		origin.y=capsule.origin.y;
		origin.z=capsule.origin.z;
		
		direction.x=capsule.direction.x;
		direction.y=capsule.direction.y;
		direction.z=capsule.direction.z;
		
		radius=capsule.radius;

		return *this;
	}

	inline Capsule &set(const Vector3 &origin1,const Vector3 &direction1,fixed radius1){
		origin.x=origin1.x;
		origin.y=origin1.y;
		origin.z=origin1.z;
		
		direction.x=direction1.x;
		direction.y=direction1.y;
		direction.z=direction1.z;
		
		radius=radius1;

		return *this;
	}

	inline Capsule &reset(){
		origin.x=0;
		origin.y=0;
		origin.z=0;
		
		direction.x=0;
		direction.y=0;
		direction.z=0;
		
		radius=0;

		return *this;
	}

	inline void setOrigin(const Vector3 &origin1){
		origin=origin1;
	}

	inline void setOrigin(fixed x,fixed y,fixed z){
		origin.x=x;
		origin.y=y;
		origin.z=z;
	}

	inline Vector3 &getOrigin(){
		return origin;
	}

	inline const Vector3 &getOrigin() const{
		return origin;
	}
	
	inline void setDirection(const Vector3 &direction1){
		direction=direction1;
	}

	inline void setDirection(fixed x,fixed y,fixed z){
		direction.x=x;
		direction.y=y;
		direction.z=z;
	}

	inline Vector3 &getDirection(){
		return direction;
	}

	inline const Vector3 &getDirection() const{
		return direction;
	}

	inline void setRadius(fixed radius1){
		radius=radius1;
	}

	inline fixed getRadius() const{
		return radius;
	}

	void TOADLET_API setLength(fixed length);

	fixed TOADLET_API getLength() const;

	void TOADLET_API rotate(const Matrix3x3 &rotation);

	inline bool equals(const Capsule &capsule) const{
		return (capsule.origin.equals(origin) && capsule.direction.equals(direction) && capsule.radius==radius);
	}

	inline bool operator==(const Capsule &capsule) const{
		return (capsule.origin==origin && capsule.direction==direction && capsule.radius==radius);
	}

	inline bool operator!=(const Capsule &capsule) const{
		return (capsule.origin!=origin || capsule.direction!=direction || capsule.radius!=radius);
	}

	inline Capsule operator+(const Vector3 &vec) const{
		return Capsule(origin+vec,direction,radius);
	}

	inline void operator+=(const Vector3 &vec){
		origin+=vec;
	}

	inline Capsule operator-(const Vector3 &vec) const{
		return Capsule(origin-vec,direction,radius);
	}

	inline void operator-=(const Vector3 &vec){
		origin-=vec;
	}

	Vector3 origin;
	Vector3 direction;
	fixed radius;
};

}
}
}

#endif
