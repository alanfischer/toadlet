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

#ifndef TOADLET_HOP_SIMULATOR_H
#define TOADLET_HOP_SIMULATOR_H

#include <toadlet/egg/Collection.h>
#include <toadlet/hop/Types.h>
#include <toadlet/hop/Constraint.h>
#include <toadlet/hop/Solid.h>
#include <toadlet/hop/Collision.h>
#include <toadlet/hop/Manager.h>

namespace toadlet{
namespace hop{

class TOADLET_API Simulator{
public:
	enum Integrator{
		Integrator_EULER,
		Integrator_VERLET,
		Integrator_IMPROVED,
		Integrator_HEUN,
		Integrator_RUNGE_KUTTA,
	};

	Simulator();
	virtual ~Simulator();

	#if defined(TOADLET_FIXED_POINT)
		void setEpsilonBits(int epsilonBits);
		int getEpsilonBits() const;
	#else
		void setEpsilon(scalar epsilon);
		scalar getEpsilon() const;
	#endif

	void setIntegrator(Integrator integrator);
	Integrator getIntegrator() const;

	void setMaxVelocityComponent(scalar maxVelocityComponent);
	scalar getMaxVelocityComponent() const;

	void setMaxForceComponent(scalar maxForceComponent);
	scalar getMaxForceComponent() const;

	void setFluidVelocity(const Vector3 &fluidVelocity);
	const Vector3 &getFluidVelocity() const;

	void setGravity(const Vector3 &gravity);
	const Vector3 &getGravity() const;

	void setManager(Manager *manager);
	Manager *getManager() const;

	void setMicroCollisionThreshold(scalar threshold);
	scalar getMicroCollisionThreshold() const;

	void setDeactivateSpeed(scalar speed);
	void setDeactivateCount(int count);

	void addSolid(Solid::ptr solid);
	void removeSolid(Solid *solid);
	int getNumSolids() const{return mSolids.size();}
	Solid::ptr getSolid(int i) const{return mSolids[i];}

	void addConstraint(Constraint::ptr constraint);
	void removeConstraint(Constraint *constraint);
	int getNumConstraints() const{return mConstraints.size();}
	Constraint::ptr getConstraint(int i) const{return mConstraints[i];}

	void update(int dt); // milliseconds

	int findSolidsInAABox(const AABox &box,Solid *solids[],int maxSolids) const;
	int findSolidsInSphere(const Sphere &sphere,Solid *solids[],int maxSolids) const;

	void traceSegment(Collision &result,const Segment &segment,int collideWithBits,Solid *ignore);

	void capVector3(Vector3 &vector,scalar value) const;
	void convertToEpsilonOffset(Vector3 &offset) const;
	void clampPosition(Vector3 &position) const;
	bool toSmall(const Vector3 &dist) const;

protected:
	/// Depends upon mSpacialCollection being update, so not public
	void traceSolid(Collision &result,Solid *solid,const Segment &segment);

	void testSolid(Collision &result,Solid *solid1,Solid *solid2,const Segment &segment);
	void testSegment(Collision &result,Solid *solid,const Segment &segment);

	void traceAABox(Collision &c,const Segment &segment,const AABox &box);
	void traceSphere(Collision &c,const Segment &segment,const Sphere &sphere);
	void traceCapsule(Collision &c,const Segment &segment,const Capsule &capsule);
	void traceConvexSolid(Collision &c,const Segment &segment,const ConvexSolid &convexSolid);

	void frictionLink(Vector3 &result,Solid *solid,const Vector3 &solidVelocity,Solid *hitSolid,const Vector3 &hitSolidNormal,const Vector3 &appliedForce,scalar fdt);
	void constraintLink(Vector3 &result,Solid *solid,const Vector3 &solidPosition,const Vector3 &solidVelocity);
	void updateAcceleration(Vector3 &result,Solid *solid,const Vector3 &x,const Vector3 &v,scalar fdt);
	void integrationStep(Solid *solid,const Vector3 &x,const Vector3 &v,const Vector3 &dx,const Vector3 &dv,scalar fdt,Vector3 &resultX,Vector3 &resultV);

	Integrator mIntegrator;

	Vector3 mFluidVelocity;
	Vector3 mGravity;

	#if defined(TOADLET_FIXED_POINT)
		int mEpsilonBits;
	#else
		scalar mOneOverEpsilon;
	#endif
	scalar mEpsilon;
	scalar mHalfEpsilon;
	scalar mQuarterEpsilon;

	scalar mMaxVelocityComponent;
	scalar mMaxForceComponent;
	egg::Collection<Collision> mCollisions;
	egg::Collection<Solid::ptr> mSolids;
	egg::Collection<Constraint::ptr> mConstraints;
	egg::Collection<Solid*> mSpacialCollection;
	int mNumSpacialCollection;
	bool mReportingCollisions;

	scalar mMicroCollisionThreshold;

	scalar mDeactivateSpeed;
	int mDeactivateCount;

	Manager *mManager;

	Vector3 cache_update_oldPosition;
	Vector3 cache_update_newPosition;
	Vector3 cache_update_velocity;
	Vector3 cache_update_temp;
	Vector3 cache_update_t;
	Vector3 cache_update_leftOver;
	Vector3 cache_update_dx1;
	Vector3 cache_update_dx2;
	Vector3 cache_update_dv1;
	Vector3 cache_update_dv2;
	Segment cache_update_path;
	AABox cache_update_box;
	Collision cache_update_c;
	Vector3 cache_traceSegment_endPoint;
	AABox cache_traceSegment_total;
	Collision cache_traceSegment_collision;
	Collision cache_traceSolid_collision;
	Collision cache_testSolid_collision;
	AABox cache_testSolid_box;
	AABox cache_testSolid_box1;
	Vector3 cache_testSolid_origin;
	Sphere cache_testSolid_sphere;
	Capsule cache_testSolid_capsule;
	Vector3 cache_testSolid_direction;
	Collision cache_testSegment_collision;
	AABox cache_testSegment_box;
	Sphere cache_testSegment_sphere;
	Capsule cache_testSegment_capsule;
	Vector3 cache_traceSphere_n;
	Vector3 cache_traceCapsule_p1;
	Vector3 cache_traceCapsule_p2;
	Segment cache_traceCapsule_s;
	Sphere cache_traceCapsule_sphere;
	Vector3 cache_frictionLink_vr;
	Vector3 cache_frictionLink_ff;
	Vector3 cache_frictionLink_fs;
	Vector3 cache_frictionLink_normVr;
	Vector3 cache_constraintLink_tx;
	Vector3 cache_constraintLink_tv;
	Vector3 cache_updateAcceleration_frictionForce;
	Vector3 cache_updateAcceleration_constraintForce;
	Vector3 cache_updateAcceleration_fluidForce;
	Vector3 cache_integrationStep_tx;
	Vector3 cache_integrationStep_tv;

	friend class Shape;
	friend class Solid;
};

}
}

#endif

