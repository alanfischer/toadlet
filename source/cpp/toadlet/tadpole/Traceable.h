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

#ifndef TOADLET_TADPOLE_TRACEABLE_H
#define TOADLET_TADPOLE_TRACEABLE_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Bound.h>
#include <toadlet/tadpole/Collision.h>

namespace toadlet{
namespace tadpole{

class Traceable:public Interface{
public:
	TOADLET_INTERFACE(Traceable);

	virtual ~Traceable(){}

	virtual Bound *getTraceableBound() const=0;
	virtual void traceSegment(Collision &result,const Vector3 &position,const Segment &segment,const Vector3 &size)=0;
};

}
}

#endif
