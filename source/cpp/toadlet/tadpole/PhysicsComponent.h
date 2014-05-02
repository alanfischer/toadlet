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

#ifndef TOADLET_TADPOLE_PHYSICSCOMPONENT_H
#define TOADLET_TADPOLE_PHYSICSCOMPONENT_H

#include <toadlet/tadpole/Component.h>
#include <toadlet/tadpole/Bound.h>
#include <toadlet/tadpole/Spacial.h>
#include <toadlet/tadpole/PhysicsTraceable.h>
#include <toadlet/tadpole/PhysicsCollisionListener.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API PhysicsComponent:public Component,public Spacial{
public:
	TOADLET_INTERFACE(PhysicsComponent);

	virtual void addCollisionListener(PhysicsCollisionListener *listener)=0;
	virtual void removeCollisionListener(PhysicsCollisionListener *listener)=0;

	virtual void setBound(Bound *bound)=0;
	virtual void setTraceable(PhysicsTraceable *traceable)=0;

	virtual void setPosition(const Vector3 &position)=0;
	virtual const Vector3 &getPosition() const=0;

	virtual void setVelocity(const Vector3 &velocity)=0;
	virtual const Vector3 &getVelocity() const=0;

	virtual void addForce(const Vector3 &force)=0;

	virtual void setMass(scalar mass)=0;
	virtual scalar getMass() const=0;

	virtual void setGravity(scalar gravity)=0;
	virtual scalar getGravity() const=0;

	virtual void setScope(int scope)=0;
	virtual int getScope() const=0;

	virtual void setCollisionScope(int scope)=0;
	virtual int getCollisionScope() const=0;

	virtual void setCollideWithScope(int scope)=0;
	virtual int getCollideWithScope() const=0;

	virtual void collision(const PhysicsCollision &collision)=0;
};

}
}

#endif
