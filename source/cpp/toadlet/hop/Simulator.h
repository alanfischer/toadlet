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
#include <toadlet/egg/Object.h>
#include <toadlet/hop/Types.h>
#include <toadlet/hop/Constraint.h>
#include <toadlet/hop/Solid.h>
#include <toadlet/hop/Collision.h>
#include <toadlet/hop/Manager.h>

namespace toadlet{
namespace hop{

class TOADLET_API Simulator:public Object{
public:
	TOADLET_OBJECT(Simulator);

	enum Integrator{
		Integrator_EULER,
		Integrator_IMPROVED,
		Integrator_HEUN,
		Integrator_RUNGE_KUTTA,
	};

	enum Scope{
		Scope_REPORT_COLLISIONS=1<<30,
	};

	Simulator();
	virtual ~Simulator();

	#if defined(TOADLET_FIXED_POINT)
		void setEpsilonBits(int epsilonBits);
		int getEpsilonBits() const{return mEpsilonBits;}
	#else
		void setEpsilon(scalar epsilon);
		scalar getEpsilon() const{return mEpsilon;}
	#endif

	void setIntegrator(Integrator integrator){mIntegrator=integrator;}
	Integrator getIntegrator() const{return mIntegrator;}

	void setSnapToGrid(bool snap){mSnapToGrid=snap;}
	bool getSnapToGrid() const{return mSnapToGrid;}

	void setAverageNormals(bool average){mAverageNormals=average;}
	bool getAverageNormals() const{return mAverageNormals;}

	void setMaxPositionComponent(scalar maxPositionComponent){mMaxPositionComponent=maxPositionComponent;}
	scalar getMaxPositionComponent() const{return mMaxPositionComponent;}

	void setMaxVelocityComponent(scalar maxVelocityComponent){mMaxVelocityComponent=maxVelocityComponent;}
	scalar getMaxVelocityComponent() const{return mMaxVelocityComponent;}

	void setMaxForceComponent(scalar maxForceComponent){mMaxForceComponent=maxForceComponent;}
	scalar getMaxForceComponent() const{return mMaxForceComponent;}

	void setFluidVelocity(const Vector3 &fluidVelocity){mFluidVelocity.set(fluidVelocity);}
	const Vector3 &getFluidVelocity() const{return mFluidVelocity;}

	void setGravity(const Vector3 &gravity);
	const Vector3 &getGravity() const{return mGravity;}

	void setManager(Manager *manager){mManager=manager;}
	Manager *getManager() const{return mManager;}

	void setMicroCollisionThreshold(scalar threshold){mMicroCollisionThreshold=threshold;}
	scalar getMicroCollisionThreshold() const{return mMicroCollisionThreshold;}

	void setDeactivateSpeed(scalar speed){mDeactivateSpeed=speed;}
	void setDeactivateCount(int count){mDeactivateCount=count;}

	void addSolid(Solid *solid);
	void removeSolid(Solid *solid);
	int getNumSolids() const{return mSolids.size();}
	Solid *getSolid(int i) const{return mSolids[i];}

	void addConstraint(Constraint *constraint);
	void removeConstraint(Constraint *constraint);
	int getNumConstraints() const{return mConstraints.size();}
	Constraint *getConstraint(int i) const{return mConstraints[i];}

	void update(int dt,int scope=0,Solid *solid=NULL); // milliseconds
	void updateSolid(Solid *solid,int dt,scalar fdt);

	int findSolidsInAABox(const AABox &box,Solid *solids[],int maxSolids) const;

	void traceSegment(Collision &result,const Segment &segment,int collideWithBits=-1,Solid *ignore=NULL);
	void traceSolid(Collision &result,Solid *solid,const Segment &segment,int collideWithBits=-1);
	void testSegment(Collision &result,const Segment &segment,Solid *solid);
	void testSolid(Collision &result,Solid *solid1,const Segment &segment,Solid *solid2);

	void capVector3(Vector3 &vector,scalar value) const;
	void calculateEpsilonOffset(Vector3 &result,const Vector3 &direction,const Vector3 &normal) const;
	void snapToGrid(Vector3 &position) const;
	bool tooSmall(const Vector3 &dist,scalar epsilon) const;

	int countActiveSolids();

protected:
	void reportCollisions();

	void traceSegmentWithCurrentSpacials(Collision &result,const Segment &segment,int collideWithBits,Solid *ignore);
	void traceSolidWithCurrentSpacials(Collision &result,Solid *solid,const Segment &segment,int collideWithBits);

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

	bool mSnapToGrid;
	bool mAverageNormals;
	scalar mMaxPositionComponent;
	scalar mMaxVelocityComponent;
	scalar mMaxForceComponent;
	Collection<Collision> mCollisions;
	int mNumCollisions;
	Collection<Solid::ptr> mSolids;
	Collection<Constraint::ptr> mConstraints;
	Collection<Solid*> mSpacialCollection;
	int mNumSpacialCollection;
	bool mReportingCollisions;

	scalar mMicroCollisionThreshold;

	scalar mDeactivateSpeed;
	int mDeactivateCount;

	Manager *mManager;

	Vector3 cache_update_oldPosition;
	Vector3 cache_update_newPosition;
	Vector3 cache_update_oldVelocity;
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

