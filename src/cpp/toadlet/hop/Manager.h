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

#ifndef TOADLET_HOP_MANAGER_H
#define TOADLET_HOP_MANAGER_H

#include <toadlet/hop/Collision.h>
#include <toadlet/hop/Solid.h>

namespace toadlet{
namespace hop{

class Manager{
public:
	virtual ~Manager(){}

	virtual void traceSegment(Collision &result,const Segment &segment)=0;
	virtual void traceSolid(Collision &result,const Segment &segment,const Solid *solid)=0;
	virtual void preUpdate(int dt,scalar fdt)=0;
	virtual void postUpdate(int dt,scalar fdt)=0;
	virtual void preUpdate(Solid *solid,int dt,scalar fdt)=0;
	virtual void intraUpdate(Solid *solid,int dt,scalar fdt)=0;
	virtual bool collisionResponse(Solid *solid,Vector3 &position,Vector3 &remainder,Collision &collision)=0;
	virtual void postUpdate(Solid *solid,int dt,scalar fdt)=0;
};

}
}

#endif
