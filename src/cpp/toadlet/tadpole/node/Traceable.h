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

#ifndef TOADLET_TADPOLE_NODE_TRACEABLE_H
#define TOADLET_TADPOLE_NODE_TRACEABLE_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Collision.h>

namespace toadlet{
namespace tadpole{
namespace node{

class Traceable{
public:
	virtual ~Traceable(){}

	virtual const Sphere &getLocalBound() const=0;
	// TODO: Could I replace these multiple functions with just 1 function with a Geometry object
	//  Could also take an Expandable interface, which would take a Normal and return a scalar, which indicates how far to expand along that normal
	virtual void traceSegment(Collision &result,const Segment &segment)=0;
//	virtual void traceSphere(Collision &result,const Segment &segment,const Sphere &sphere)=0;
//	virtual void traceAABox(Collision &result,const Segment &segment,const AABox &box)=0;
};

}
}
}

#endif
