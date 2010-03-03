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

#ifndef TOADLET_TADPOLE_NODE_PHYSICALLYTRACEABLE_H
#define TOADLET_TADPOLE_NODE_PHYSICALLYTRACEABLE_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Collision.h>

namespace toadlet{
namespace tadpole{
namespace node{

class PhysicallyTraceable{
public:
	virtual ~PhysicallyTraceable(){}

	// TODO: I'd like to generalize this to just be one method with some Geometry overload
	virtual void traceSegment(Collision &result,const Segment &segment)=0;
	virtual void traceSphere(Collision &result,const Segment &segment,const Sphere &sphere)=0;
	virtual void traceAABox(Collision &result,const Segment &segment,const AABox &box)=0;
};

}
}
}

#endif
