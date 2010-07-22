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

#ifndef TOADLET_EGG_MATH_AABOX_H
#define TOADLET_EGG_MATH_AABOX_H

#include <toadlet/egg/math/Vector3.h>
#include <toadlet/egg/math/Matrix3x3.h>
#include <toadlet/egg/math/Plane.h>

namespace toadlet{
namespace egg{
namespace math{

class TOADLET_API AABox{
public:
	inline AABox(){}
	
	inline AABox(const Vector3 &min,const Vector3 &max):mins(min),maxs(max){}

	inline AABox(real minx,real miny,real minz,real maxx,real maxy,real maxz):mins(minx,miny,minz),maxs(maxx,maxy,maxz){}

	inline AABox(real radius):mins(-radius,-radius,-radius),maxs(radius,radius,radius){}

	inline AABox &set(const AABox &box){
		mins.x=box.mins.x;
		mins.y=box.mins.y;
		mins.z=box.mins.z;

		maxs.x=box.maxs.x;
		maxs.y=box.maxs.y;
		maxs.z=box.maxs.z;

		return *this;
	}

	inline AABox &set(const Vector3 &mins1,const Vector3 &maxs1){
		mins.x=mins1.x;
		mins.y=mins1.y;
		mins.z=mins1.z;

		maxs.x=maxs1.x;
		maxs.y=maxs1.y;
		maxs.z=maxs1.z;

		return *this;
	}

	inline AABox &set(real minx,real miny,real minz,real maxx,real maxy,real maxz){
		mins.x=minx;
		mins.y=miny;
		mins.z=minz;

		maxs.x=maxx;
		maxs.y=maxy;
		maxs.z=maxz;

		return *this;
	}

	inline AABox &set(real radius){
		mins.x=-radius;
		mins.y=-radius;
		mins.z=-radius;

		maxs.x=radius;
		maxs.y=radius;
		maxs.z=radius;

		return *this;
	}

	inline AABox &reset(){
		mins.x=0;
		mins.y=0;
		mins.z=0;

		maxs.x=0;
		maxs.y=0;
		maxs.z=0;

		return *this;
	}

	inline void setMins(const Vector3 &mins1){
		mins.x=mins1.x;
		mins.y=mins1.y;
		mins.z=mins1.z;
	}
	
	inline void setMins(real x,real y,real z){
		mins.x=x;
		mins.y=y;
		mins.z=z;
	}

	inline Vector3 &getMins(){
		return mins;
	}

	inline const Vector3 &getMins() const{
		return mins;
	}

	inline void setMaxs(const Vector3 &maxs1){
		maxs.x=maxs1.x;
		maxs.y=maxs1.y;
		maxs.z=maxs1.z;
	}

	inline void setMaxs(real x,real y,real z){
		maxs.x=x;
		maxs.y=y;
		maxs.z=z;
	}

	inline Vector3 &getMaxs(){
		return maxs;
	}

	inline const Vector3 &getMaxs() const{
		return maxs;
	}

	inline void getVertexes(Vector3 *vertexes) const{
		vertexes[0].x=mins.x;vertexes[0].y=mins.y;vertexes[0].z=mins.z;
		vertexes[1].x=maxs.x;vertexes[1].y=mins.y;vertexes[1].z=mins.z;
		vertexes[2].x=mins.x;vertexes[2].y=maxs.y;vertexes[2].z=mins.z;
		vertexes[3].x=maxs.x;vertexes[3].y=maxs.y;vertexes[3].z=mins.z;
		vertexes[4].x=mins.x;vertexes[4].y=mins.y;vertexes[4].z=maxs.z;
		vertexes[5].x=maxs.x;vertexes[5].y=mins.y;vertexes[5].z=maxs.z;
		vertexes[6].x=mins.x;vertexes[6].y=maxs.y;vertexes[6].z=maxs.z;
		vertexes[7].x=maxs.x;vertexes[7].y=maxs.y;vertexes[7].z=maxs.z;
	}

	inline void findPVertex(Vector3 &r,const Vector3 &normal) const{
		r.x=normal.x>=0?maxs.x:mins.x;
		r.y=normal.y>=0?maxs.y:mins.y;
		r.z=normal.z>=0?maxs.z:mins.z;
	}

	inline float findPVertexLength(const Plane &plane) const{
		float x=plane.normal.x>=0?maxs.x:mins.x;
		float y=plane.normal.y>=0?maxs.y:mins.y;
		float z=plane.normal.z>=0?maxs.z:mins.z;
		return plane.normal.x*x + plane.normal.y*y + plane.normal.z*z + plane.distance;
	}

	inline void findNVertex(Vector3 &r,const Vector3 &normal) const{
		r.x=normal.x>=0?mins.x:maxs.x;
		r.y=normal.y>=0?mins.y:maxs.y;
		r.z=normal.z>=0?mins.z:maxs.z;
	}

	inline float findNVertexLength(const Plane &plane) const{
		float x=plane.normal.x>=0?mins.x:maxs.x;
		float y=plane.normal.y>=0?mins.y:maxs.y;
		float z=plane.normal.z>=0?mins.z:maxs.z;
		return plane.normal.x*x + plane.normal.y*y + plane.normal.z*z + plane.distance;
	}

	inline void mergeWith(const AABox &box){
		if(box.mins.x<mins.x)mins.x=box.mins.x;
		if(box.mins.y<mins.y)mins.y=box.mins.y;
		if(box.mins.z<mins.z)mins.z=box.mins.z;
		if(box.maxs.x>maxs.x)maxs.x=box.maxs.x;
		if(box.maxs.y>maxs.y)maxs.y=box.maxs.y;
		if(box.maxs.z>maxs.z)maxs.z=box.maxs.z;
	}

	inline void mergeWith(const Vector3 &vec){
		if(vec.x<mins.x)mins.x=vec.x;
		if(vec.y<mins.y)mins.y=vec.y;
		if(vec.z<mins.z)mins.z=vec.z;
		if(vec.x>maxs.x)maxs.x=vec.x;
		if(vec.y>maxs.y)maxs.y=vec.y;
		if(vec.z>maxs.z)maxs.z=vec.z;
	}
	
	void rotate(const Matrix3x3 &rotation);
	void rotate(const Matrix3x3 &rotation,Vector3 tempBuffer[8]);

	inline bool equals(const AABox &box) const{
		return (box.mins.equals(mins) && box.maxs.equals(maxs));
	}

	inline bool operator==(const AABox &box) const{
		return (box.mins==mins && box.maxs==maxs);
	}

	inline bool operator!=(const AABox &box) const{
		return (box.mins!=mins || box.maxs!=maxs);
	}

	inline AABox operator+(const Vector3 &vec) const{
		return AABox(mins+vec,maxs+vec);
	}

	inline void operator+=(const Vector3 &vec){
		mins+=vec;
		maxs+=vec;
	}

	inline AABox operator-(const Vector3 &vec) const{
		return AABox(mins-vec,maxs-vec);
	}

	inline void operator-=(const Vector3 &vec){
		mins-=vec;
		maxs-=vec;
	}

	inline AABox operator*(const Vector3 &vec) const{
		return AABox(mins*vec,maxs*vec);
	}

	inline AABox operator/(const Vector3 &vec) const{
		return AABox(mins/vec,maxs/vec);
	}

	inline void operator*=(real f){
		mins*=f;
		maxs*=f;
	}

	inline void operator*=(const Vector3 &vec){
		mins*=vec;
		maxs*=vec;
	}

	inline void operator/=(real f){
		mins/=f;
		maxs/=f;
	}

	Vector3 mins;
	Vector3 maxs;
};

}
}
}

#endif
