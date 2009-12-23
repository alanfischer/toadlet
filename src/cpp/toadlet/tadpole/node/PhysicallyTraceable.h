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

namespace toadlet{
namespace tadpole{
namespace node{

// This is pretty much a thin interface for SceneNodes which allow us to expose some physical tracing to things like hop,
// In theory this could be replaced by hop::Manager, but that would impose a dependence of hop upon tadpole, and also be hop specific instead of usable by other physics engines
class PhysicallyTraceable{
public:
	virtual ~PhysicallyTraceable(){}

	virtual scalar traceSegment(Vector3 &normal,const Segment &segment)=0;
	virtual scalar traceSphere(Vector3 &normal,const Segment &segment,const Sphere &sphere)=0;
	virtual scalar traceAABox(Vector3 &normal,const Segment &segment,const AABox &box)=0;
};

}
}
}

#endif
