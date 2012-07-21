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

#ifndef TOADLET_TADPOLE_HOPCOMPONENT_H
#define TOADLET_TADPOLE_HOPCOMPONENT_H

#include <toadlet/tadpole/PhysicsComponent.h>
#include <toadlet/tadpole/BaseComponent.h>
#include "HopManager.h"

namespace toadlet{
namespace tadpole{

class TOADLET_API HopComponent:public BaseComponent,public PhysicsComponent{
public:
	TOADLET_COMPONENT(HopComponent,PhysicsComponent);

	HopComponent(HopManager *manager);

	void destroy();

	bool parentChanged(Node *node);

	void setPosition(const Vector3 &position);
	const Vector3 &getPosition() const{return mSolid->getPosition();}

	void setVelocity(const Vector3 &velocity){mSolid->setVelocity(velocity);}
	const Vector3 &getVelocity() const{return mSolid->getVelocity();}

	void setGravity(scalar gravity){mSolid->setCoefficientOfGravity(gravity);}
	scalar getGravity() const{return mSolid->getCoefficientOfGravity();}

	void setBound(Bound *bound);

	void addShape(Shape *shape);
	void removeShape(Shape *shape);

	void transformChanged();

	void logicUpdate(int dt,int scope){}
	void frameUpdate(int dt,int scope){}

	bool getActive() const{return mSolid->active();}
	Bound *getBound() const{return mBound;}

	inline Solid *getSolid(){return mSolid;}

protected:
	friend class HopManager;

	void preSimulate();
	void postSimulate();
	void updatePosition(const Vector3 &position);
	void lerpPosition(scalar fraction);

	HopManager *mManager;
	Solid::ptr mSolid;
	Vector3 mOldPosition,mNewPosition,mCurrentPosition;
	bool mSkipNextPreSimulate;
	Bound::ptr mBound;
};

}
}

#endif
