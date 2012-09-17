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

#ifndef TOADLET_TADPOLE_PHYSICSMANAGER_H
#define TOADLET_TADPOLE_PHYSICSMANAGER_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/PhysicsCollision.h>
#include <toadlet/tadpole/Traceable.h>

namespace toadlet{
namespace tadpole{

class PhysicsComponent;

class TOADLET_API PhysicsManager:public Interface{
public:
	TOADLET_INTERFACE(PhysicsManager);

	virtual PhysicsComponent *createPhysicsComponent()=0;
	
	virtual void setGravity(const Vector3 &gravity)=0;

	virtual void setTraceable(Traceable *traceable)=0;

	virtual void traceSegment(PhysicsCollision &result,const Segment &segment,int collideWithScope,Node *ignore)=0;
	virtual void traceNode(PhysicsCollision &result,Node *node,const Segment &segment,int collideWithScope)=0;
	virtual void testNode(PhysicsCollision &result,Node *node,const Segment &segment,Node *ndoe2)=0;

	virtual void logicUpdate(int dt,int scope,Node *node)=0;
	virtual void frameUpdate(int dt,int scope,Node *node)=0;
};

}
}

#endif
