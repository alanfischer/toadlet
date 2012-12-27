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

#ifndef TOADLET_HOP_TRACEABLE_H
#define TOADLET_HOP_TRACEABLE_H

#include <toadlet/hop/Collision.h>

namespace toadlet{
namespace hop{

class Solid;

class Traceable{
public:
	virtual ~Traceable(){}

	virtual void getBound(AABox &result)=0;
	virtual void traceSegment(Collision &result,const Vector3 &position,const Segment &segment)=0;
	virtual void traceSolid(Collision &result,Solid *solid,const Vector3 &position,const Segment &segment)=0;
};

}
}

#endif
