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

#ifndef TOADLET_EGG_MATHFIXED_SEGMENT_H
#define TOADLET_EGG_MATHFIXED_SEGMENT_H

#include <toadlet/egg/mathfixed/Vector3.h>

namespace toadlet{
namespace egg{
namespace mathfixed{

class TOADLET_API TOADLET_ALIGNED Segment{
public:
	inline Segment(){}

	inline Segment &set(const Segment &segment){
		origin.x=segment.origin.x;
		origin.y=segment.origin.y;
		origin.z=segment.origin.z;

		direction.x=segment.direction.x;
		direction.y=segment.direction.y;
		direction.z=segment.direction.z;

		return *this;
	}

	inline Segment &setStartEnd(const Vector3 &start,const Vector3 &end){
		origin.x=start.x;
		origin.y=start.y;
		origin.z=start.z;

		direction.x=end.x-start.x;
		direction.y=end.y-start.y;
		direction.z=end.z-start.z;

		return *this;
	}

	inline Segment &setStartEnd(fixed startX,fixed startY,fixed startZ,fixed endX,fixed endY,fixed endZ){
		origin.x=startX;
		origin.y=startY;
		origin.z=startZ;

		direction.x=endX-startX;
		direction.y=endY-startY;
		direction.z=endZ-startZ;

		return *this;
	}

	inline Segment &setStartDir(const Vector3 &start,const Vector3 &dir){
		origin.x=start.x;
		origin.y=start.y;
		origin.z=start.z;

		direction.x=dir.x;
		direction.y=dir.y;
		direction.z=dir.z;

		return *this;
	}

	inline Segment &setStartDir(fixed startX,fixed startY,fixed startZ,fixed dirX,fixed dirY,fixed dirZ){
		origin.x=startX;
		origin.y=startY;
		origin.z=startZ;

		direction.x=dirX;
		direction.y=dirY;
		direction.z=dirZ;

		return *this;
	}

	inline Segment &reset(){
		origin.x=0;
		origin.y=0;
		origin.z=0;

		direction.x=0;
		direction.y=0;
		direction.z=0;

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

	inline Vector3 &getOrigin(){
		return origin;
	}

	inline const Vector3 &getOrigin() const{
		return origin;
	}

	inline void setDirection(const Vector3 &direction1){
		direction.x=direction1.x;
		direction.y=direction1.y;
		direction.z=direction1.z;
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
	
	inline Vector3 &getEndPoint(Vector3 &endPoint) const{
		endPoint.x=origin.x+direction.x;
		endPoint.y=origin.y+direction.y;
		endPoint.z=origin.z+direction.z;
		return endPoint;
	}

	inline bool equals(const Segment &segment) const{
		return (segment.origin.equals(origin) && segment.direction.equals(direction));
	}

	inline bool operator==(const Segment &segment) const{
		return equals(segment);
	}

	inline bool operator!=(const Segment &segment) const{
		return !equals(segment);
	}

	Vector3 origin;
	Vector3 direction;
};

}
}
}

#endif
