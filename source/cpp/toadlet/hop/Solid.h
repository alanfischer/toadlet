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

#ifndef TOADLET_HOP_SOLID_H
#define TOADLET_HOP_SOLID_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/Object.h>
#include <toadlet/hop/Types.h>
#include <toadlet/hop/CollisionListener.h>
#include <toadlet/hop/Shape.h>

namespace toadlet{
namespace hop{

class Constraint;
class Simulator;
class Manager;

class TOADLET_API Solid:public Object{
public:
	TOADLET_OBJECT(Solid);

	const static scalar INFINITE_MASS;	

	Solid();
	virtual ~Solid();
	virtual void destroy();

	void reset();

	void setScope(int scope){mScope=scope;}
	int getScope() const{return mScope;}

	void setInternalScope(int scope){mInternalScope=scope;}
	int getInternalScope() const{return mInternalScope;}

	void setCollisionBits(int bits){mCollisionBits=bits;}
	int getCollisionBits() const{return mCollisionBits;}

	void setCollideWithBits(int bits){mCollideWithBits=bits;}
	int getCollideWithBits() const{return mCollideWithBits;}

	void setMass(scalar mass);
	scalar getMass() const{return mMass;}

	void setInfiniteMass();
	bool hasInfiniteMass() const;

	void setPosition(const Vector3 &position);
	const Vector3 &getPosition() const{return mPosition;}

	void setVelocity(const Vector3 &velocity);
	const Vector3 &getVelocity() const{return mVelocity;}

	void addForce(const Vector3 &force);
	const Vector3 &getForce() const{return mForce;}
	void clearForce(){mForce.reset();}

	void setCoefficientOfGravity(scalar coeff){mCoefficientOfGravity=coeff;}
	scalar getCoefficientOfGravity() const{return mCoefficientOfGravity;}

	void setCoefficientOfRestitution(scalar coeff){mCoefficientOfRestitution=coeff;}
	scalar getCoefficientOfRestitution() const{return mCoefficientOfRestitution;}

	void setCoefficientOfRestitutionOverride(bool over){mCoefficientOfRestitutionOverride=over;}
	bool getCoefficientOfRestitutionOverride() const{return mCoefficientOfRestitutionOverride;}

	void setCoefficientOfStaticFriction(scalar coeff){mCoefficientOfStaticFriction=coeff;}
	scalar getCoefficientOfStaticFriction() const{return mCoefficientOfStaticFriction;}

	void setCoefficientOfDynamicFriction(scalar coeff){mCoefficientOfDynamicFriction=coeff;}
	scalar getCoefficientOfDynamicFriction() const{return mCoefficientOfDynamicFriction;}

	void setCoefficientOfEffectiveDrag(scalar coeff){mCoefficientOfEffectiveDrag=coeff;}
	scalar getCoefficientOfEffectiveDrag() const{return mCoefficientOfEffectiveDrag;}

	void addShape(Shape *shape);
	void removeShape(Shape *shape);
	void removeAllShapes();
	Shape *getShape(int i) const{return mShapes[i];}
	int getNumShapes() const{return mShapes.size();}
	int getShapeTypes() const{return mShapeTypes;}

	inline const AABox &getLocalBound() const{return mLocalBound;}
	inline const AABox &getWorldBound() const{return mWorldBound;}

	/// @todo: Fix touching when manually running the simulation, like we do in networking.
	//  I'm not sure how this will be done quite yet, but we'll figure something out.
	Solid *getTouching() const{return mTouching;}
	const Vector3 &getTouchingNormal() const{return mTouchingNormal;}

	void setCollisionListener(CollisionListener *listener){mCollisionListener=listener;}
	CollisionListener *getCollisionListener() const{return mCollisionListener;}

	void setUserData(void *data){mUserData=data;}
	void *getUserData() const{return mUserData;}

	void activate();
	void setStayActive(bool active);
	void deactivate();
	bool active() const{return (mActive && mSimulator!=NULL);}

	void setDoUpdateCallback(bool callback){mDoUpdateCallback=callback;}
	void setManager(Manager *manager){mManager=manager;}

	inline Simulator *getSimulator() const{return mSimulator;}

	void setPositionDirect(const Vector3 &position);

	void updateLocalBound();

protected:
	void internal_setSimulator(Simulator *s);
	void internal_addConstraint(Constraint *constraint){mConstraints.add(constraint);}
	void internal_removeConstraint(Constraint *constraint){mConstraints.remove(constraint);}

	int mScope;
	int mInternalScope;
	int mCollisionBits;
	int mCollideWithBits;
	scalar mMass;
	scalar mInvMass;
	Vector3 mPosition;
	Vector3 mVelocity;
	Vector3 mForce;
	scalar mCoefficientOfGravity;
	scalar mCoefficientOfRestitution;
	bool mCoefficientOfRestitutionOverride;
	scalar mCoefficientOfStaticFriction;
	scalar mCoefficientOfDynamicFriction;
	scalar mCoefficientOfEffectiveDrag;

	Collection<Shape::ptr> mShapes;
	int mShapeTypes;
	AABox mLocalBound;
	AABox mWorldBound;

	Collection<Constraint*> mConstraints;

	CollisionListener *mCollisionListener;
	void *mUserData;

	bool mActive;
	int mDeactivateCount;	

	Solid *mTouched1;
	Vector3 mTouched1Normal;
	Solid *mTouched2;
	Vector3 mTouched2Normal;
	Solid *mTouching;
	Vector3 mTouchingNormal;

	bool mDoUpdateCallback;
	Manager *mManager;

	Simulator *mSimulator;

	friend class Constraint;
	friend class Shape;
	friend class Simulator;
};

}
}

#endif

