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

#include <toadlet/hop/Simulator.h>
#include <toadlet/egg/Extents.h>

#if defined(TOADLET_FIXED_POINT)
	using namespace toadlet::egg::mathfixed::Math;
#else
	using namespace toadlet::egg::math::Math;
#endif

// We need to help it figure out which div we want
#define div Math::div

namespace toadlet{
namespace hop{

Simulator::Simulator():
	mIntegrator(Integrator_HEUN),

	//mFluidVelocity,
	//mGravity,

	#if defined(TOADLET_FIXED_POINT)
		mEpsilonBits(0),
	#else
		mOneOverEpsilon(0),
	#endif
	mEpsilon(0),
	mHalfEpsilon(0),
	mQuarterEpsilon(0),

	mSnapToGrid(false),
	mAverageNormals(false),
	mMaxPositionComponent(0),
	mMaxVelocityComponent(0),
	mMaxForceComponent(0),
	//mCollisions,
	mNumCollisions(0),
	//mSolids,
	//mSpacialCollection,
	mNumSpacialCollection(0),
	mReportingCollisions(false),

	mMicroCollisionThreshold(ONE),

	mDeactivateSpeed(0),
	mDeactivateCount(0),

	mManager(NULL)
{
	setGravity(Vector3(0,0,-fromMilli(9810)));
	#if defined(TOADLET_FIXED_POINT)
		setEpsilonBits(4);
		setMaxPositionComponent(intToFixed(65536/2));
		setMaxVelocityComponent(intToFixed(104)); // A vector above 104,104,104 can have its length overflow
		setMaxForceComponent(intToFixed(104));
		setDeactivateSpeed(1<<8); // Some default
		setDeactivateCount(4); // 4 frames
	#else
		setEpsilon(.001);
		setMaxPositionComponent(100000);
		setMaxVelocityComponent(1000);
		setMaxForceComponent(1000);
		setDeactivateSpeed(mEpsilon*2); // Twice epsilon
		setDeactivateCount(4); // 4 frames
	#endif
}

Simulator::~Simulator(){
}

#if defined(TOADLET_FIXED_POINT)
	void Simulator::setEpsilonBits(int epsilonBits){
		mEpsilonBits=epsilonBits;
		mEpsilon=1<<mEpsilonBits;
		mHalfEpsilon=mEpsilon>>1;
		mQuarterEpsilon=mEpsilon>>2;
	}
#else
	void Simulator::setEpsilon(scalar epsilon){
		mEpsilon=epsilon;
		mOneOverEpsilon=1.0/mEpsilon;
		mHalfEpsilon=mEpsilon*0.5;
		mQuarterEpsilon=mEpsilon*0.25;
	}
#endif

void Simulator::setGravity(const Vector3 &gravity){
	mGravity.set(gravity);
	int i;
	for(i=0;i<mSolids.size();++i){
		mSolids[i]->activate();
	}
}

void Simulator::addSolid(Solid *solid){
	if(mSolids.contains(solid)){
		return;
	}

	mSolids.add(solid);

	solid->internal_setSimulator(this);
	solid->activate();

	mSpacialCollection.resize(mSolids.size());
}

void Simulator::removeSolid(Solid *solid){
	// We can't touch anything if we're not in the simulation
	// This will help in some logic code
	solid->mTouching=NULL;
	solid->mTouched1=NULL;
	solid->mTouched2=NULL;

	int i;
	for(i=0;i<mSolids.size();++i){
		Solid *s=mSolids[i];
		if(s->mTouching==solid){
			s->mTouching=NULL;
		}
		if(s->mTouched1==solid){
			s->mTouched1=NULL;
		}
		if(s->mTouched2==solid){
			s->mTouched2=NULL;
		}
	}

	// In case a solid is removed in the collision code, NULL it out
	if(mReportingCollisions){
		int i;
		for(i=0;i<mCollisions.size();++i){
			Collision &c=mCollisions[i];
			if(c.collider==solid){
				c.collider=NULL;
			}
			if(c.collidee==solid){
				c.collidee=NULL;
			}
		}
	}

	solid->internal_setSimulator(NULL);

	mSolids.remove(solid);
}

void Simulator::addConstraint(Constraint *constraint){
	if(mConstraints.contains(constraint)){
		return;
	}

	mConstraints.add(constraint);

	constraint->internal_setSimulator(this);
	constraint->activate();
}

void Simulator::removeConstraint(Constraint *constraint){
	constraint->internal_setSimulator(NULL);

	mConstraints.remove(constraint);
}

void Simulator::update(int dt,int scope,Solid *solid){
	Vector3 &oldPosition=cache_update_oldPosition;
	Vector3 &newPosition=cache_update_newPosition;
	Vector3 &oldVelocity=cache_update_oldVelocity;
	Vector3 &velocity=cache_update_velocity;
	Vector3 &temp=cache_update_temp;
	Vector3 &t=cache_update_t;
	Vector3 &leftOver=cache_update_leftOver;
	Vector3 &dx1=cache_update_dx1;
	Vector3 &dx2=cache_update_dx2;
	Vector3 &dv1=cache_update_dv1;
	Vector3 &dv2=cache_update_dv2;
	Segment &path=cache_update_path;
	scalar cor=0,impulse=0,oneOverMass=0,oneOverHitMass=0;
	int loop=0;
	Solid *hitSolid=NULL;
	Collision &c=cache_update_c.reset();
	int i,j;

	scalar fdt=fromMilli(dt);
	scalar hfdt=fdt/2;
	scalar qfdt=fdt/4;
	scalar sfdt=fdt/6;
	scalar ttfdt=fdt*2/3;

	if(mManager!=NULL){
		mManager->preUpdate(dt,fdt);
	}

	int numSolids=(solid!=NULL)?1:mSolids.size();
	for(i=0;i<numSolids;i++){
		if(numSolids>1 || solid==NULL){
			solid=mSolids[i];
		}

		if(solid->mActive==false || (scope!=0 && (solid->mScope&scope)==0)){
			continue;
		}

		if(solid->mDoUpdateCallback){
			(solid->mManager!=NULL?solid->mManager:mManager)->preUpdate(solid,dt,fdt);
		}

		oldPosition.set(solid->mPosition);
		oldVelocity.set(solid->mVelocity);

		// The cheapest, fastest, and worst integrator, it's 1st order
		// It may have trouble settling, so it is not recommended
		if(mIntegrator==Integrator_EULER){
			/*
				integrationStep(solid,oldPosition,solid->mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);
				newPosition=oldPosition+dx1*fdt;
				velocity=solid->mVelocity+dv1*fdt;
			*/

			integrationStep(solid,oldPosition,solid->mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);

			mul(newPosition,dx1,fdt);
			add(newPosition,oldPosition);
			mul(velocity,dv1,fdt);
			add(velocity,solid->mVelocity);
		}
		// Improved is 2nd order and a nice balance between speed and accuracy
		else if(mIntegrator==Integrator_IMPROVED){
			/*
				integrationStep(solid,oldPosition,solid->mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);
				integrationStep(solid,oldPosition,solid->mVelocity,dx1,dv1,fdt,dx2,dv2)
				newPosition=oldPosition+0.5*fdt*(dx1+dx2);
				velocity=solid->mVelocity+0.5*fdt*(dv1+dv2);
			*/

			integrationStep(solid,oldPosition,solid->mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);

			newPosition.set(dx1);
			velocity.set(dv1);

			integrationStep(solid,oldPosition,solid->mVelocity,dx1,dv1,fdt,dx2,dv2);

			add(newPosition,dx2);
			mul(newPosition,hfdt);
			add(newPosition,oldPosition);

			add(velocity,dv2);
			mul(velocity,hfdt);
			add(velocity,solid->mVelocity);
		}
		// Heun's method is 2nd order and so has the same speed as Improved, but it biases towards the 2nd portion of the integration step, and thus may be more stable
		else if(mIntegrator==Integrator_HEUN){
			/*
				integrationStep(solid,oldPosition,solid->mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);
				integrationStep(solid,oldPosition,solid->mVelocity,dx1,dv1,2.0f/3.0f*fdt,dx2,dv2);
				newPosition=oldPosition+0.25f*fdt*(dx1+3.0f*dx2);
				velocity=solid->mVelocity+0.25f*fdt*(dv1+3.0f*dv2);
			*/

			integrationStep(solid,oldPosition,solid->mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);

			newPosition.set(dx1);
			velocity.set(dv1);

			integrationStep(solid,oldPosition,solid->mVelocity,dx1,dv1,ttfdt,dx2,dv2);

			mul(dx2,THREE);
			add(newPosition,dx2);
			mul(newPosition,qfdt);
			add(newPosition,oldPosition);

			mul(dv2,THREE);
			add(velocity,dv2);
			mul(velocity,qfdt);
			add(velocity,solid->mVelocity);
		}
		// Runge Kutta is 4th order, decently fast, and very stable for larger step sizes
		else if(mIntegrator==Integrator_RUNGE_KUTTA){
			/*
				integrationStep(solid,oldPosition,solid->mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);
				integrationStep(solid,oldPosition,solid->mVelocity,dx1,dv1,0.5f*fdt,dx2,dv2);
				integrationStep(solid,oldPosition,solid->mVelocity,dx2,dv2,0.5f*fdt,dx3,dv3);
				integrationStep(solid,oldPosition,solid->mVelocity,dx3,dv3,fdt,dx4,dv4);
				newPosition=oldPosition+1.0f/6.0f*fdt*(dx1+2.0f*(dx2+dx3)+dx4);
				velocity=solid->mVelocity+1.0f/6.0f*fdt*(dv1+2.0f*(dv2+dv3)+dv4);
			*/

			integrationStep(solid,oldPosition,solid->mVelocity,ZERO_VECTOR3,ZERO_VECTOR3,fdt,dx1,dv1);

			newPosition.set(dx1);
			velocity.set(dv1);

			integrationStep(solid,oldPosition,solid->mVelocity,dx1,dv1,hfdt,dx2,dv2);

			mul(temp,dx2,TWO);
			add(newPosition,temp);
			mul(temp,dv2,TWO);
			add(velocity,temp);

			integrationStep(solid,oldPosition,solid->mVelocity,dx2,dv2,hfdt,dx1,dv1);

			mul(temp,dx1,TWO);
			add(newPosition,temp);
			mul(temp,dv1,TWO);
			add(velocity,temp);

			integrationStep(solid,oldPosition,solid->mVelocity,dx1,dv1,fdt,dx2,dv2);

			add(newPosition,dx2);
			mul(newPosition,sfdt);
			add(newPosition,oldPosition);

			add(velocity,dv2);
			mul(velocity,sfdt);
			add(velocity,solid->mVelocity);
		}

		// Clean up
		capVector3(velocity,mMaxVelocityComponent);
		solid->mVelocity.set(velocity);
		solid->clearForce();

		bool first=true;
		bool skip=false;

		if(solid->mDoUpdateCallback){
			(solid->mManager!=NULL?solid->mManager:mManager)->intraUpdate(solid,dt,fdt);
		}

		// In most cases we don't need to cap the oldPosition, but if our position is set outside of the max,
		//  then when we cap the newPosition, it will think we are attempting to move from the old to the new, instead of just setting there
		snapToGrid(oldPosition);
		capVector3(oldPosition,mMaxPositionComponent);
		snapToGrid(newPosition);
		capVector3(newPosition,mMaxPositionComponent);

		// Collect all possible solids in the whole movement area
		if(solid->mCollideWithScope!=0){
			sub(temp,newPosition,oldPosition);

			/// @todo: Removed this, maybe it should just go completely
			///  I had a 1 meter large sphere deactivated, sitting on top of an infinite mass plane, with -9.8 gravity, and attemping to set the velocity to 5 up, this causes it to just zero out
			/// @todo: Calculate this 0.5f from the coefficient of static friction between solid & mTouching
			//if(solid->mTouching!=NULL && toSmall(temp,0.5f)){
			//	newPosition.set(oldPosition);
			//	solid->mVelocity.set(oldVelocity);
			//	skip=true;
			//}
			//else
			{
				if(temp.x<0){temp.x=-temp.x;}
				if(temp.y<0){temp.y=-temp.y;}
				if(temp.z<0){temp.z=-temp.z;}

				scalar m=temp.x;
				if(temp.y>m){m=temp.y;}
				if(temp.z>m){m=temp.z;}
				m+=mEpsilon; // Move it out by epsilon, since we haven't snapped yet

				AABox &box=cache_update_box.set(solid->mLocalBound);

				add(box,newPosition);
				box.mins.x-=m;
				box.mins.y-=m;
				box.mins.z-=m;
				box.maxs.x+=m;
				box.maxs.y+=m;
				box.maxs.z+=m;

				mNumSpacialCollection=findSolidsInAABox(box,mSpacialCollection,mSpacialCollection.size());
			}
		}

		// Loop to use up available momentum
		loop=0;
		while(skip==false){
			if(first==false){
				snapToGrid(oldPosition);
				snapToGrid(newPosition);

				sub(temp,newPosition,oldPosition);
				if(toSmall(temp,mEpsilon)){
					newPosition.set(oldPosition);
					break;
				}
			}

			path.setStartEnd(oldPosition,newPosition);

			traceSolidWithCurrentSpacials(c,solid,path,solid->mCollideWithScope);
			if(c.time<ONE){
				// Calculate offset vector, and then resulting position
				snapToGrid(c.point);

				// Calculate oldPosition to be an epsilon back from where we can get to
				sub(leftOver,c.point,oldPosition);
				calculateEpsilonOffset(oldPosition,leftOver,c.normal);
				add(oldPosition,c.point);

				// Update leftOver, the energy we still have moving in this direction that we couldnt use
				sub(leftOver,newPosition,oldPosition);

				// If its a valid collision, and someone is listening, then store it
				if(c.collider!=solid->mTouching &&
					(solid->mCollisionListener!=NULL || (c.collider!=NULL && c.collider->mCollisionListener!=NULL)))
				{
					c.collidee=solid;
					if(c.collider!=NULL){
						sub(c.velocity,solid->mVelocity,c.collider->mVelocity);
					}
					else{
						c.velocity.set(solid->mVelocity);
					}

					if(mCollisions.size()<=mNumCollisions){
						mCollisions.resize(mNumCollisions+1);
					}

					mCollisions[mNumCollisions].set(c);
					mNumCollisions++;
				}
				hitSolid=c.collider;

				/// @todo: We need to add a flag to regather possible solids if this callback is performed
				bool responded=false;
				if(solid->mDoUpdateCallback){
					responded=(solid->mManager!=NULL?solid->mManager:mManager)->collisionResponse(solid,oldPosition,leftOver,c);
				}

				if(responded==false){
					// Conservation of momentum
					if(solid->mCoefficientOfRestitutionOverride || hitSolid==NULL){
						cor=solid->mCoefficientOfRestitution;
					}
					else{
						cor=(solid->mCoefficientOfRestitution+hitSolid->mCoefficientOfRestitution)/2;
					}

					if(hitSolid!=NULL){
						sub(temp,hitSolid->mVelocity,solid->mVelocity);
					}
					else{
						neg(temp,solid->mVelocity);
					}

					// Attempt to detect a microcollision
					if(dot(temp,c.normal)<mMicroCollisionThreshold){
						cor=0;
					}

					scalar numerator=mul(ONE+cor,dot(temp,c.normal));

					// Temp stores the velocity change on hitSolid
					temp.reset();

					if(solid->mMass!=0 && (hitSolid==NULL || hitSolid->mMass!=0)){
						oneOverMass=solid->mInvMass;
						oneOverHitMass=hitSolid!=NULL?hitSolid->mInvMass:0;

						// Check to make sure its not two infinite mass solids interacting
						if(oneOverMass+oneOverHitMass!=0){
							impulse=div(numerator,oneOverMass+oneOverHitMass);
						}
						else{
							impulse=0;
						}

						if(solid->mMass!=Solid::INFINITE_MASS){
							/*
								solid->mVelocity+=(c.normal*impulse)*oneOverMass;
							*/

							mul(t,c.normal,impulse);
							mul(t,oneOverMass);
							add(solid->mVelocity,t);
						}
						if(hitSolid!=NULL && hitSolid->mMass!=Solid::INFINITE_MASS){
							if(solid->mMass==Solid::INFINITE_MASS){
								// Infinite mass trains assume a CoefficientOfRestitution of 1 
								// for the hitSolid calculation to avoid having gravity jam objects against them 
								/*
									temp=c.normal*2.0f*dot(hitSolid->mVelocity-solid->mVelocity,c.normal);
								*/

								sub(temp,hitSolid->mVelocity,solid->mVelocity);
								mul(temp,c.normal,dot(temp,c.normal)/2);
							}
							else{
								/*
									temp=(c.normal*impulse)*oneOverHitMass;
								*/

								mul(temp,c.normal,impulse);
								mul(temp,oneOverHitMass);
							}
						}
					}
					else if(hitSolid!=NULL || hitSolid->mMass==0){
						mul(temp,c.normal,numerator);
					}
					else if(solid->mMass==0){
						mul(t,c.normal,numerator);
						add(solid->mVelocity,t);
					}

					// Only affect hitSolid if hitSolid would have collided with solid.
					if(hitSolid!=NULL && (hitSolid->mCollideWithScope&solid->mCollisionScope)!=0 &&
						(Math::abs(temp.x)>=mDeactivateSpeed || Math::abs(temp.y)>=mDeactivateSpeed || Math::abs(temp.z)>=mDeactivateSpeed))
					{
						hitSolid->activate();
						sub(hitSolid->mVelocity,temp);
					}
				}

				// Touching code
				solid->mTouched2=solid->mTouched1;
				solid->mTouched2Normal.set(solid->mTouched1Normal);
				if(solid->mTouched1==c.collider){
					solid->mTouching=c.collider;
					solid->mTouchingNormal.set(c.normal);
				}
				else{
					solid->mTouched1=c.collider;
					solid->mTouched1Normal.set(c.normal);
					solid->mTouching=NULL;
				}

				if(toSmall(leftOver,mEpsilon)){
					newPosition.set(oldPosition);
					break;
				}
				else if(loop>4){
					// We keep hitting something, so zero the velocity and break out
					solid->mVelocity.reset();
					newPosition.set(oldPosition);
					break;
				}
				else{
					// Calculate new destinations from coefficient of restitution applied to velocity
					if(normalizeCarefully(velocity,solid->mVelocity,mEpsilon)==false){
						newPosition.set(oldPosition);
						break;
					}
					else{
						/*
							velocity*=length(leftOver);
							velocity-=c.normal*dot(velocity,c.normal);
							newPosition=oldPosition+velocity;
						*/

						mul(velocity,length(leftOver));
						mul(temp,c.normal,dot(velocity,c.normal));
						sub(velocity,temp);
						add(newPosition,oldPosition,velocity);
					}
					first=false;
				}
			}
			else{
				break;
			}

			loop++;
		}

		// Check to see if we need to reset touching
		if(skip==false && c.time==ONE && loop==0){
			solid->mTouching=NULL;
			solid->mTouched1=NULL;
			solid->mTouched2=NULL;
		}

		if(solid->mDeactivateCount>=0){
			if(Math::abs(newPosition.x-solid->mPosition.x)<mDeactivateSpeed && Math::abs(newPosition.y-solid->mPosition.y)<mDeactivateSpeed && Math::abs(newPosition.z-solid->mPosition.z)<mDeactivateSpeed){
				solid->mDeactivateCount++;
				if(solid->mDeactivateCount>mDeactivateCount){
					for(j=solid->mConstraints.size()-1;j>=0;--j){
						Constraint *constraint=solid->mConstraints[j];
						Solid *startSolid=constraint->mStartSolid;
						Solid *endSolid=constraint->mEndSolid;
						if(startSolid!=solid){
							if(startSolid->mActive==true && startSolid->mDeactivateCount<=mDeactivateCount){
								break;
							}
						}
						else if(endSolid!=NULL){
							if(endSolid->mActive==true && endSolid->mDeactivateCount<=mDeactivateCount){
								break;
							}
						}
					}
					if(j<0){
						solid->deactivate();
					}
				}
			}
			else{
				solid->mDeactivateCount=0;
			}
		}

		solid->setPositionDirect(newPosition);

		if(solid->mDoUpdateCallback && mManager!=NULL){
			(solid->mManager!=NULL?solid->mManager:mManager)->postUpdate(solid,dt,fdt);
		}
	}

	if((scope&Scope_REPORT_COLLISIONS)!=0){
		reportCollisions();
	}

	if(mManager!=NULL){
		mManager->postUpdate(dt,fdt);
	}
}

void Simulator::reportCollisions(){
	mReportingCollisions=true;

	int i;
	for(i=0;i<mNumCollisions;++i){
		Collision &col=mCollisions[i];

		// We must do these as 2 separate ifs, because the collision functions may destroy the solids they are hitting
		if(col.collidee!=NULL){
			CollisionListener *listener=col.collidee->mCollisionListener;
			if(listener!=NULL && col.collider!=NULL && (col.collidee->mCollideWithScope&col.collider->mCollisionScope)!=0){
				listener->collision(col);
			}
		}
		if(col.collider!=NULL){
			CollisionListener *listener=col.collider->mCollisionListener;
			if(listener!=NULL && col.collidee!=NULL && (col.collider->mCollideWithScope&col.collidee->mCollisionScope)!=0){
				col.invert();
				listener->collision(col);
			}
		}
	}
	mNumCollisions=0;
	mReportingCollisions=false;
}

int Simulator::findSolidsInAABox(const AABox &box,Solid *solids[],int maxSolids) const{
	AABox expandedBox(box);
	expandedBox.mins.x-=mEpsilon;
	expandedBox.mins.y-=mEpsilon;
	expandedBox.mins.z-=mEpsilon;
	expandedBox.maxs.x+=mEpsilon;
	expandedBox.maxs.y+=mEpsilon;
	expandedBox.maxs.z+=mEpsilon;

	int amount=-1;
	if(mManager!=NULL){
		amount=mManager->findSolidsInAABox(expandedBox,solids,maxSolids);
	}

	if(amount==-1){
		amount=0;
		Solid *s=NULL;
		int i=0;
		for(i=0;i<mSolids.size();++i){
			s=mSolids[i];
			if(testIntersection(expandedBox,s->mWorldBound)){
				if(amount<maxSolids){
					solids[amount]=s;
				}
				amount++;
			}
		}
	}

	return amount;
}

void Simulator::traceSegment(Collision &result,const Segment &segment,int collideWithScope,Solid *ignore){
	Vector3 endPoint=cache_traceSegment_endPoint;
	segment.getEndPoint(endPoint);
	AABox total=cache_traceSegment_total.set(segment.origin,segment.origin);
	total.merge(endPoint);
	mNumSpacialCollection=findSolidsInAABox(total,mSpacialCollection,mSpacialCollection.size());

	traceSegmentWithCurrentSpacials(result,segment,collideWithScope,ignore);
}

void Simulator::traceSolid(Collision &result,Solid *solid,const Segment &segment,int collideWithScope){
	Vector3 &end=segment.getEndPoint(cache_testSolid_origin);
	AABox &box=cache_testSolid_box.set(segment.origin,segment.origin);
	box.merge(end);
	Math::add(box.mins,solid->mLocalBound.mins);
	Math::add(box.maxs,solid->mLocalBound.maxs);

	mNumSpacialCollection=findSolidsInAABox(box,mSpacialCollection,mSpacialCollection.size());

	traceSolidWithCurrentSpacials(result,solid,segment,collideWithScope);
}

void Simulator::testSegment(Collision &result,const Segment &segment,Solid *solid){
	Collision &collision=cache_testSegment_collision.reset();
	collision.collider=solid;

	const Collection<Shape::ptr> &shapes=solid->mShapes;
	int numShapes=shapes.size();

	Shape *shape;
	bool modifyScope=false;

	int i;
	for(i=0;i<numShapes;++i){
		// No need to reset collision or it's time here since the trace functions are gaurenteed to do that

		shape=shapes[i];
		switch(shape->mType){
			case Shape::Type_AABOX:
				{
					AABox &box=cache_testSegment_box.set(shape->mAABox);
					add(box,solid->mPosition);
					traceAABox(collision,segment,box);
				}
			break;
			case Shape::Type_SPHERE:
				{
					Sphere &sphere=cache_testSegment_sphere.set(shape->mSphere);
					add(sphere,solid->mPosition);
					traceSphere(collision,segment,sphere);
				}
			break;
			case Shape::Type_CAPSULE:
				{
					Capsule &capsule=cache_testSegment_capsule.set(shape->mCapsule);
					add(capsule,solid->mPosition);
					traceCapsule(collision,segment,capsule);
				}
			break;
			case Shape::Type_CONVEXSOLID:
				Error::unimplemented("traceSegment not implemented for Type_CONVEXSOLID"); 
			break;
			case Shape::Type_TRACEABLE:
				shape->mTraceable->traceSegment(collision,solid->mPosition,segment);
				modifyScope=true;
			break;
		}

		if(collision.time==0){
			collision.scope|=solid->mInternalScope; // Bitwise OR here since the above functions may also set the scope
		}

		int scope=result.scope;
		if(collision.time<ONE){
			if(collision.time<result.time){
				result.set(collision);
			}
			else if(result.time==collision.time){
				add(result.normal,collision.normal);
				bool b=normalizeCarefully(result.normal,mEpsilon);
				if(b==false){
					result.set(collision);
				}
			}
			modifyScope|=(collision.time==0);
		}

		if(modifyScope){
			result.scope=scope|collision.scope;
		}
		else{
			result.scope=scope;
		}
	}
}

void Simulator::testSolid(Collision &result,Solid *solid1,const Segment &segment,Solid *solid2){
	Collision &collision=cache_testSolid_collision.reset();
	collision.collider=solid2;

	const Collection<Shape::ptr> &shapes1=solid1->mShapes;
	int numShapes1=shapes1.size();
	const Collection<Shape::ptr> &shapes2=solid2->mShapes;
	int numShapes2=shapes2.size();
	Shape *shape1;
	Shape *shape2;
	bool modifyScope=false;

	int i,j;
	for(i=0;i<numShapes1;++i){
		shape1=shapes1[i];

		for(j=0;j<numShapes2;++j){
			shape2=shapes2[j];

			// No need to reset collision or it's time here since the trace functions are gaurenteed to do that
			modifyScope=false;

			// AABox collisions
			if(shape1->mType==Shape::Type_AABOX && shape2->mType==Shape::Type_AABOX){
				// Create one big box and then do our tests with a box and a point
				const AABox &box1=shape1->mAABox;
				const AABox &box2=shape2->mAABox;

				AABox &box=cache_testSolid_box.set(box2);
				add(box,solid2->mPosition);
				sub(box.maxs,box1.mins);
				sub(box.mins,box1.maxs);

				traceAABox(collision,segment,box);
			}
			else if(shape1->mType==Shape::Type_AABOX && shape2->mType==Shape::Type_SPHERE){
				// Sphere-Box collisions aren't perfect, since its more of a box-box collision, but oh well
				// Create one big box and then do our tests with a box and a point
				const AABox &box1=shape1->mAABox;
				AABox &box=cache_testSolid_box.set(shape2->mSphere.radius);
				add(box,shape2->mSphere.origin);

				add(box,solid2->mPosition);
				sub(box.maxs,box1.mins);
				sub(box.mins,box1.maxs);

				traceAABox(collision,segment,box);
			}
			else if(shape1->mType==Shape::Type_AABOX && shape2->mType==Shape::Type_CAPSULE){
				// Capsule-Box collisions aren't perfect, since its more of a box-box collision, but oh well
				// Create one big box and then do our tests with a box and a point
				const AABox &box1=shape1->mAABox;
				AABox &box=cache_testSolid_box;
				shape2->getBound(box);

				add(box,solid2->mPosition);
				sub(box.maxs,box1.mins);
				sub(box.mins,box1.maxs);

				traceAABox(collision,segment,box);
			}
			else if(shape1->mType==Shape::Type_AABOX && shape2->mType==Shape::Type_CONVEXSOLID){
				/// @todo: Create a specialized "super solid" by only bumping out plane normal components in the directions/sizes of the AABOX's
				//traceConvexSolid(collision,segment,convexsolid);
			}
			// Sphere collisions
			else if(shape1->mType==Shape::Type_SPHERE && shape2->mType==Shape::Type_AABOX){
				// Sphere-Box collisions aren't perfect, since its more of a box-box collision, but oh well
				// Create one big box and then do our tests with a box and a point
				AABox &box1=cache_testSolid_box1.set(shape1->mSphere.radius);
				add(box1,shape1->mSphere.origin);

				AABox &box=cache_testSolid_box.set(shape2->mAABox);
				add(box,solid2->mPosition);
				sub(box.maxs,box1.mins);
				sub(box.mins,box1.maxs);
	
				traceAABox(collision,segment,box);
			}
			else if(shape1->mType==Shape::Type_SPHERE && shape2->mType==Shape::Type_SPHERE){
				// Construct super sphere
				Vector3 &origin=cache_testSolid_origin.set(solid2->mPosition);

				sub(origin,shape1->mSphere.origin);
				add(origin,shape2->mSphere.origin);

				Sphere &sphere=cache_testSolid_sphere.set(origin,shape2->mSphere.radius+shape1->mSphere.radius);

				traceSphere(collision,segment,sphere);
			}
			else if(shape1->mType==Shape::Type_SPHERE && shape2->mType==Shape::Type_CAPSULE){
				// Construct super capsule
				Vector3 &origin=cache_testSolid_origin.set(solid2->mPosition);

				sub(origin,shape1->mSphere.origin);
				add(origin,shape2->mCapsule.origin);

				Capsule &capsule=cache_testSolid_capsule.set(origin,shape2->mCapsule.direction,shape2->mCapsule.radius+shape1->mSphere.radius);

				traceCapsule(collision,segment,capsule);
			}
			else if(shape1->mType==Shape::Type_SPHERE && shape2->mType==Shape::Type_CONVEXSOLID){
				// Construct the super convex solid by "bumping" all of the planes "outward"
				ConvexSolid convexsolid;
				convexsolid.set(shape2->mConvexSolid);
				for(int i=0;i<convexsolid.planes.size();++i){
					convexsolid.planes[i].distance+=shape1->mSphere.radius;
				}

				// Move the problem to the convex solid's origin
				Segment tmpseg;
				tmpseg.set(segment);
				sub(tmpseg.origin,solid2->getPosition());

				traceConvexSolid(collision,tmpseg,convexsolid);
			}
			// Capsule collisions
			else if(shape1->mType==Shape::Type_CAPSULE && shape2->mType==Shape::Type_AABOX){
				// Capsule-Box collisions aren't perfect, since its more of a box-box collision, but oh well
				// Create one big box and then do our tests with a box and a point
				AABox &box1=cache_testSolid_box1;
				shape1->getBound(box1);
				AABox &box=cache_testSolid_box.set(shape2->mAABox);

				add(box,solid2->mPosition);
				sub(box.maxs,box1.mins);
				sub(box.mins,box1.maxs);

				traceAABox(collision,segment,box);
			}
			else if(shape1->mType==Shape::Type_CAPSULE && shape2->mType==Shape::Type_SPHERE){
				// Construct super capsule
				Vector3 &origin=cache_testSolid_origin.set(solid2->mPosition);
				sub(origin,shape1->mCapsule.origin);
				add(origin,shape2->mSphere.origin);

				// You must invert the capsule direction when forming the supercapsule at the sphere's location
				Vector3 &direction=cache_testSolid_direction.set(shape1->mCapsule.direction);
				neg(direction);
				Capsule &capsule=cache_testSolid_capsule.set(origin,direction,shape1->mCapsule.radius+shape2->mSphere.radius);

				traceCapsule(collision,segment,capsule);
			}
			else if(shape1->mType==Shape::Type_CAPSULE && shape2->mType==Shape::Type_CAPSULE){
				// Construct super capsule
				Vector3 &origin=cache_testSolid_origin.set(solid2->mPosition);
				sub(origin,shape1->mCapsule.origin);
				add(origin,shape2->mCapsule.origin);

				Capsule &capsule=cache_testSolid_capsule.set(origin,shape1->mCapsule.direction,shape1->mCapsule.radius+shape2->mCapsule.radius);

				traceCapsule(collision,segment,capsule);
			}
			else if(shape1->mType==Shape::Type_CAPSULE && shape2->mType==Shape::Type_CONVEXSOLID){
				Error::unimplemented("from Type_CAPSULE to Type_CONVEXSOLID unimplemented");
			}
			// Convex solid collisions
			else if(shape1->mType==Shape::Type_CONVEXSOLID && shape2->mType==Shape::Type_AABOX){
				Error::unimplemented("from Type_CONVEXSOLID to Type_AABOX unimplemented");
			}
			else if(shape1->mType==Shape::Type_CONVEXSOLID && shape2->mType==Shape::Type_SPHERE){
				// Construct the mirrored super convex solid by "bumping" all of the planes "outward"
				// And mirroring their normals about the plane defined by the segment direction
				ConvexSolid convexsolid;
				convexsolid.set(shape1->mConvexSolid);
				for(int i=0;i<convexsolid.planes.size();++i){
					convexsolid.planes[i].distance+=shape1->mSphere.radius;
				}
				
				// Reverse the segment
				Segment tmpseg(segment);
				add(tmpseg.origin,segment.origin,segment.direction);
				neg(tmpseg.direction);

				// Now move the collision to the origin of the convex solid
				sub(tmpseg.origin,solid1->getPosition());

				traceConvexSolid(collision,tmpseg,convexsolid);
			}
			else if(shape1->mType==Shape::Type_CONVEXSOLID && shape2->mType==Shape::Type_CAPSULE){
				Error::unimplemented("from Type_CONVEXSOLID to Type_CAPSULE unimplemented");
			}
			else if(shape1->mType==Shape::Type_CONVEXSOLID && shape2->mType==Shape::Type_CONVEXSOLID){
				Error::unimplemented("from Type_CONVEXSOLID to Type_CONVEXSOLID unimplemented");
			}
			else if(shape1->mType==Shape::Type_TRACEABLE && shape2->mType!=Shape::Type_TRACEABLE){
				Segment isegment;
				isegment.origin.set(solid2->mPosition);
				Math::mul(isegment.direction,segment.direction,-Math::ONE);

				shape1->mTraceable->traceSolid(collision,solid2,segment.origin,isegment);

				// This will do most of the inverting, but the point still needs to be recalculated,
				//  since invert is mainly used for swapping reference solid
				collision.invert();

				/// @todo: The bottom is 100% correct, and I think the top one is correct, but I'd like to make sure,
				///  And then only keep the top, since it's more precise.
				#if 1
					Math::sub(isegment.origin,collision.point);
					Math::add(collision.point,segment.origin,isegment.origin);
				#else
					Math::madd(collision.point,segment.direction,collision.time,segment.origin);
				#endif
				modifyScope=true;
			}
			else if(shape1->mType!=Shape::Type_TRACEABLE && shape2->mType==Shape::Type_TRACEABLE){
				shape2->mTraceable->traceSolid(collision,solid1,solid2->mPosition,segment);
				modifyScope=true;
			}

			if(shape1->mType!=Shape::Type_TRACEABLE && shape2->mType!=Shape::Type_TRACEABLE && collision.time==0){
				collision.scope=solid2->mScope;
			}
			if(collision.time==0){
				collision.scope|=solid2->mInternalScope; // Bitwise OR here since the above functions may also set the scope
			}

			int scope=result.scope;
			if(collision.time<ONE){
				if(collision.time<result.time){
					result.set(collision);
				}
				else if(result.time==collision.time){
					add(result.normal,collision.normal);
					bool b=normalizeCarefully(result.normal,mEpsilon);
					if(b==false){
						result.set(collision);
					}
				}
				modifyScope|=(collision.time==0);
			}

			if(modifyScope){
				result.scope=scope|collision.scope;
			}
			else{
				result.scope=scope;
			}
		}
	}
}

void Simulator::capVector3(Vector3 &vector,scalar value) const{
	#if defined(TOADLET_FIXED_POINT)
		vector.x=TOADLET_MAX_XX(-value,vector.x);
		vector.x=TOADLET_MIN_XX(value,vector.x);
		vector.y=TOADLET_MAX_XX(-value,vector.y);
		vector.y=TOADLET_MIN_XX(value,vector.y);
		vector.z=TOADLET_MAX_XX(-value,vector.z);
		vector.z=TOADLET_MIN_XX(value,vector.z);
	#else
		vector.x=TOADLET_MAX_RR(-value,vector.x);
		vector.x=TOADLET_MIN_RR(value,vector.x);
		vector.y=TOADLET_MAX_RR(-value,vector.y);
		vector.y=TOADLET_MIN_RR(value,vector.y);
		vector.z=TOADLET_MAX_RR(-value,vector.z);
		vector.z=TOADLET_MIN_RR(value,vector.z);

		// This is to avoid invalid values that may arise in corner cases, or put in externally
		vector.x=Extents::isReal(vector.x)?vector.x:0;
		vector.y=Extents::isReal(vector.y)?vector.y:0;
		vector.z=Extents::isReal(vector.z)?vector.z:0;
	#endif
}

void Simulator::calculateEpsilonOffset(Vector3 &result,const Vector3 &direction,const Vector3 &normal) const{
	if(mSnapToGrid){
		if(normal.x>=mQuarterEpsilon)result.x=mEpsilon;
		else if(normal.x<=-mQuarterEpsilon)result.x=-mEpsilon;
		else result.x=0;

		if(normal.y>=mQuarterEpsilon)result.y=mEpsilon;
		else if(normal.y<=-mQuarterEpsilon)result.y=-mEpsilon;
		else result.y=0;

		if(normal.z>=mQuarterEpsilon)result.z=mEpsilon;
		else if(normal.z<=-mQuarterEpsilon)result.z=-mEpsilon;
		else result.z=0;
	}
	else{
		scalar length=Math::length(direction);
		if(length>mEpsilon){
			result.x=mul(div(-direction.x,length),mEpsilon);
			result.y=mul(div(-direction.y,length),mEpsilon);
			result.z=mul(div(-direction.z,length),mEpsilon);
		}
		else{
			result.x=0;
			result.y=0;
			result.z=0;
		}
	}
}

void Simulator::snapToGrid(Vector3 &pos) const{
	if(mSnapToGrid){
		#if defined(TOADLET_FIXED_POINT)
			pos.x=(((pos.x+(mHalfEpsilon*-(pos.x<0)))>>mEpsilonBits)<<mEpsilonBits);
			pos.y=(((pos.y+(mHalfEpsilon*-(pos.y<0)))>>mEpsilonBits)<<mEpsilonBits);
			pos.z=(((pos.z+(mHalfEpsilon*-(pos.z<0)))>>mEpsilonBits)<<mEpsilonBits);
		#else
			pos.x=(int)((pos.x+(mHalfEpsilon*-(pos.x<0)))*mOneOverEpsilon)*mEpsilon;
			pos.y=(int)((pos.y+(mHalfEpsilon*-(pos.y<0)))*mOneOverEpsilon)*mEpsilon;
			pos.z=(int)((pos.z+(mHalfEpsilon*-(pos.z<0)))*mOneOverEpsilon)*mEpsilon;
		#endif
	}
}

bool Simulator::toSmall(const Vector3 &value,scalar epsilon) const{
	return (value.x<epsilon && value.x>-epsilon && value.y<epsilon && value.y>-epsilon && value.z<epsilon && value.z>-epsilon);
}

void Simulator::traceSegmentWithCurrentSpacials(Collision &result,const Segment &segment,int collideWithScope,Solid *ignore){
	result.time=ONE;
	result.scope=0;

	Solid *solid2;

	Collision &collision=cache_traceSegment_collision.reset();
	int i;
	for(i=0;i<mNumSpacialCollection;++i){
		solid2=mSpacialCollection[i];
		if(solid2!=ignore && (collideWithScope&solid2->mCollisionScope)!=0){
			collision.time=ONE;
			testSegment(collision,segment,solid2);
			int scope=result.scope;
			if(collision.time<ONE){
				if(collision.time<result.time){
					result.set(collision);
				}
				else if(mAverageNormals && result.time==collision.time){
					add(result.normal,collision.normal);
					bool b=normalizeCarefully(result.normal,mEpsilon);
					if(b==false){
						result.set(collision);
					}
				}
			}
			result.scope=scope|collision.scope;
		}
	}

	if(mManager!=NULL){
		collision.time=ONE;
		mManager->traceSegment(collision,segment,collideWithScope);
		int scope=result.scope;
		if(collision.time<ONE){
			if(collision.time<result.time){
				result.set(collision);
			}
			else if(mAverageNormals && result.time==collision.time){
				add(result.normal,collision.normal);
				bool b=normalizeCarefully(result.normal,mEpsilon);
				if(b==false){
					result.set(collision);
				}
			}
		}
		result.scope=(scope|collision.scope);
	}

	if(result.time==Math::ONE){
		segment.getEndPoint(result.point);
	}
}

void Simulator::traceSolidWithCurrentSpacials(Collision &result,Solid *solid,const Segment &segment,int collideWithScope){
	result.time=ONE;

	if(collideWithScope==0){
		return;
	}

	Solid *solid2;

	Collision &collision=cache_traceSolid_collision.reset();
	int i;
	for(i=0;i<mNumSpacialCollection;++i){
		solid2=mSpacialCollection[i];
		if(solid!=solid2 && (collideWithScope&solid2->mCollisionScope)!=0){
			collision.time=ONE;
			testSolid(collision,solid,segment,solid2);
			int scope=result.scope;
			if(collision.time<ONE){
				if(collision.time<result.time){
					result.set(collision);
				}
				else if(mAverageNormals && result.time==collision.time){
					add(result.normal,collision.normal);
					bool b=normalizeCarefully(result.normal,mEpsilon);
					if(b==false){
						result.set(collision);
					}
				}
			}
			result.scope=scope|collision.scope;
		}
	}

	if(mManager!=NULL){
		collision.time=ONE;
		mManager->traceSolid(collision,solid,segment,collideWithScope);
		int scope=result.scope;
		if(collision.time<ONE){
			if(collision.time<result.time){
				result.set(collision);
			}
			else if(mAverageNormals && result.time==collision.time){
				add(result.normal,collision.normal);
				bool b=normalizeCarefully(result.normal,mEpsilon);
				if(b==false){
					result.set(collision);
				}
			}
		}
		result.scope=(scope|collision.scope);
	}

	if(result.time==Math::ONE){
		segment.getEndPoint(result.point);
	}
}

void Simulator::traceAABox(Collision &c,const Segment &segment,const AABox &box){
	if(testInside(box,segment.origin)){
		scalar x;
		#if defined(TOADLET_FIXED_POINT)
			x=segment.origin.x-box.mins.x;scalar dix=(x^(x>>31))-(x>>31);
			x=segment.origin.y-box.mins.y;scalar diy=(x^(x>>31))-(x>>31);
			x=segment.origin.z-box.mins.z;scalar diz=(x^(x>>31))-(x>>31);
			x=segment.origin.x-box.maxs.x;scalar dax=(x^(x>>31))-(x>>31);
			x=segment.origin.y-box.maxs.y;scalar day=(x^(x>>31))-(x>>31);
			x=segment.origin.z-box.maxs.z;scalar daz=(x^(x>>31))-(x>>31);
		#else
			x=segment.origin.x-box.mins.x;scalar dix=x<0?-x:x;
			x=segment.origin.y-box.mins.y;scalar diy=x<0?-x:x;
			x=segment.origin.z-box.mins.z;scalar diz=x<0?-x:x;
			x=segment.origin.x-box.maxs.x;scalar dax=x<0?-x:x;
			x=segment.origin.y-box.maxs.y;scalar day=x<0?-x:x;
			x=segment.origin.z-box.maxs.z;scalar daz=x<0?-x:x;
		#endif

		if(dix<=diy && dix<=diz && dix<=dax && dix<=day && dix<=daz){
			if(dot(segment.direction,NEG_X_UNIT_VECTOR3)>=0){
				return;
			}
			c.normal.set(NEG_X_UNIT_VECTOR3);
		}
		else if(diy<=diz && diy<=dax && diy<=day && diy<=daz){
			if(dot(segment.direction,NEG_Y_UNIT_VECTOR3)>=0){
				return;
			}
			c.normal.set(NEG_Y_UNIT_VECTOR3);
		}
		else if(diz<=dax && diz<=day && diz<=daz){
			if(dot(segment.direction,NEG_Z_UNIT_VECTOR3)>=0){
				return;
			}
			c.normal.set(NEG_Z_UNIT_VECTOR3);
		}
		else if(dax<=day && dax<=daz){
			if(dot(segment.direction,X_UNIT_VECTOR3)>=0){
				return;
			}
			c.normal.set(X_UNIT_VECTOR3);
		}
		else if(day<=daz){
			if(dot(segment.direction,Y_UNIT_VECTOR3)>=0){
				return;
			}
			c.normal.set(Y_UNIT_VECTOR3);
		}
		else{
			if(dot(segment.direction,Z_UNIT_VECTOR3)>=0){
				return;
			}
			c.normal.set(Z_UNIT_VECTOR3);
		}

		c.time=0;
		c.point.set(segment.origin);
	}
	else{
		c.time=findIntersection(segment,box,c.point,c.normal);
	}
}

void Simulator::traceSphere(Collision &c,const Segment &segment,const Sphere &sphere){
	if(testInside(sphere,segment.origin)){
		// Skip the 'closer in' checks on spheres, since now matter what we do,
		// Apparently we still slide in closer to spheres, very slightly.
		// I'll really need to look at this more some time
		// So we always just push anyone in a sphere out more
		{
			// If we're trying to trace farther into the sphere
			Vector3 &n=cache_traceSphere_n.set(segment.origin);

			sub(n,sphere.origin);
			normalizeCarefully(n,mEpsilon);
			// Maybe this calculation is inacurate to test if we might get 'epsilon bumped' in?
			if(dot(n,segment.direction)<=mEpsilon){
				c.time=0;
				c.point.set(segment.origin);
				normalizeCarefully(c.normal,n,mEpsilon);
			}
			else{
				c.time=ONE;
			}
		}
	}
	else{
		c.time=findIntersection(segment,sphere,c.point,c.normal);
	}
}

void Simulator::traceCapsule(Collision &c,const Segment &segment,const Capsule &capsule){
	Vector3 &p1=cache_traceCapsule_p1.reset();
	Vector3 &p2=cache_traceCapsule_p2.reset();
	Segment &s=cache_traceCapsule_s;
	
	// Capsules are really just segments, so test against a sphere located at the closest point of the capsule segment
	s.origin.set(capsule.origin);
	s.direction.set(capsule.direction);
	project(p1,p2,s,segment,mEpsilon);
	
	Sphere &sphere=cache_traceCapsule_sphere.set(p1,capsule.radius);
	traceSphere(c,segment,sphere);
}

void Simulator::traceConvexSolid(Collision &c,const Segment &segment,const ConvexSolid &convexSolid){
	int i,j;
	Vector3 u,v;
	scalar t;
	bool b;

	c.time=ONE;

	// Check the segment against each plane in the solid
	for(i=0;i<convexSolid.planes.size();++i){
		// line below stolen from findIntersection(), but we want to avoid unnecessary math so we dont use the whole function
		t=div(convexSolid.planes[i].distance-dot(convexSolid.planes[i].normal,segment.getOrigin()),dot(convexSolid.planes[i].normal,segment.getDirection()));

		// If we intersect the plane, compute the intersection point (u)
		if(t>=0 && t<=ONE){
			mul(u,segment.direction,t);
			add(u,segment.origin);

			// Now check to see if this intersection point falls within the bounds of this planar face
			// by checking it against all other planes, and seeing if dot(u-planepoint,planenormal)<0
			/// @todo: We could save some time here by precomputing "next door" surfaces for each surface, to cut down on the number of checks made
			b=true;
			for(j=0;j<convexSolid.planes.size();++j){
				if(i==j) continue;
				mul(v,convexSolid.planes[j].normal,convexSolid.planes[j].distance);
				sub(v,u,v);

				if(dot(v,convexSolid.planes[j].normal)>0){
					// If w falls outside of any plane, it is not a collision point for the plane i
					b=false;
					break;
				}
			}
			if(b){
				// This point (u) IS a valid collison for face i
				if(c.time==ONE){
					// This is the first valid collsion point
					c.time=t;
					c.point.set(u);
					c.normal.set(convexSolid.planes[i].normal);
				}
				else{
					// We found a second valid collsion point, meaning our object tried
					// to jump right though the convex solid in a single timestep
					// so, find the "first" point, eg the earlier one along our segment
					if(t<c.time){
						c.time=t;
						c.point.set(u);
						c.normal.set(convexSolid.planes[i].normal);
					}
					
					// Either way, this also means we can quit now - we've found everything
					i=convexSolid.planes.size();
				}
			}
		}
	}
}

void Simulator::frictionLink(Vector3 &result,Solid *solid,const Vector3 &solidVelocity,Solid *hitSolid,const Vector3 &hitSolidNormal,const Vector3 &appliedForce,scalar fdt){
	result.set(ZERO_VECTOR3);

	// Andrew's friction linking code
	if(solid->mMass>0 && hitSolid->mMass!=0 && (solid->mCoefficientOfStaticFriction>0 || solid->mCoefficientOfDynamicFriction>0)){
		scalar fn=0,lenVr=0;
		Vector3 &vr=cache_frictionLink_vr;
		Vector3 &ff=cache_frictionLink_ff;
		Vector3 &fs=cache_frictionLink_fs;
		Vector3 &normVr=cache_frictionLink_normVr;

		fn=mul(mul(dot(mGravity,hitSolidNormal),solid->mCoefficientOfGravity),solid->mMass)+dot(appliedForce,hitSolidNormal);

		// When calculating relative velocity, ignore any components perpendicular to the plane of contact
		// normVr acts as a temp here
		sub(vr,solidVelocity,hitSolid->mVelocity);
		mul(normVr,hitSolidNormal,dot(vr,hitSolidNormal));
		sub(vr,normVr);
		capVector3(vr,mMaxVelocityComponent);
		lenVr=length(vr);

		if(fn!=0 && lenVr>0){
			div(normVr,vr,lenVr);
			mul(ff,normVr,fn);
			mul(result,ff,solid->mCoefficientOfStaticFriction);

			// Multiply fdt in with result to avoid excessively large numbers in the calculation of fs
			mul(result,fdt);

			// This is the static force required to resist relative velocity and any applied forces in this time step * fdt
			// Note friction can only act in the plane of contact, and normVr again acts as a temp variable here
			mul(fs,vr,-solid->mMass);
			mul(normVr,hitSolidNormal,dot(appliedForce,hitSolidNormal));
			sub(normVr,appliedForce,normVr);
			mul(normVr,fdt);
			add(fs,normVr);

			capVector3(fs,mMaxForceComponent);

			if(lengthSquared(fs)>lengthSquared(result)){
				mul(result,ff,solid->mCoefficientOfDynamicFriction);
			}
			else{
				div(result,fs,fdt);
			}
		}
	}
}

void Simulator::constraintLink(Vector3 &result,Solid *solid,const Vector3 &solidPosition,const Vector3 &solidVelocity){
	Vector3 &tx=cache_constraintLink_tx;
	Vector3 &tv=cache_constraintLink_tv;
	Constraint *c=NULL;

	result.set(ZERO_VECTOR3);

	int i;
	for(i=0;i<solid->mConstraints.size();++i){
		c=solid->mConstraints[i];
		if(c->active()==false){
			continue;
		}
		else if(solid==c->mStartSolid){
			if(c->mEndSolid!=NULL){
				sub(tx,c->mEndSolid->mPosition,solidPosition);
				sub(tv,c->mEndSolid->mVelocity,solidVelocity);
			}
			else{
				sub(tx,c->mEndPoint,solidPosition);
				mul(tv,solidVelocity,-ONE);
			}
		}
		else if(solid==c->mEndSolid){
			sub(tx,c->mStartSolid->mPosition,solidPosition);
			sub(tv,c->mStartSolid->mVelocity,solidVelocity);
		}
		else{
			continue;
		}

		mul(tx,c->mSpringConstant);
		mul(tv,c->mDampingConstant);
		add(result,tx);
		add(result,tv);
	}
}

void Simulator::updateAcceleration(Vector3 &result,Solid *solid,const Vector3 &x,const Vector3 &v,scalar fdt){
	Vector3 &frictionForce=cache_updateAcceleration_frictionForce;
	Vector3 &constraintForce=cache_updateAcceleration_constraintForce;
	Vector3 &fluidForce=cache_updateAcceleration_fluidForce;

	/*
		result=mGravity*solid->mCoefficientOfGravity;
		if(solid->mMass!=0){
			constraintLink(constraintForce,solid,x,v,fdt);
			for(#NUM_TOUCHED_SOLIDS){
				frictionLink(frictionForce,solid,v,hitSolid,hitSolidNormal,constraintForce+solid->mForce,fdt);
			}
			dragForce=-mCoefficientOfEffectiveDrag*(v-mFluidVelocity);
			result+=1.0f/solid->mMass*(friction+solid->mForce+constraintForce+dragForce);
		}
	*/

	mul(result,mGravity,solid->mCoefficientOfGravity);

	if(solid->mMass!=0){
		constraintLink(constraintForce,solid,x,v);
		add(constraintForce,solid->mForce);
		if(solid->mTouched1!=NULL){
			frictionLink(frictionForce,solid,v,solid->mTouched1,solid->mTouched1Normal,constraintForce,fdt);
			add(constraintForce,frictionForce);
			if(solid->mTouched2!=NULL && solid->mTouched2!=solid->mTouched1){
				frictionLink(frictionForce,solid,v,solid->mTouched2,solid->mTouched2Normal,constraintForce,fdt);
				add(constraintForce,frictionForce);
			}
		}
		sub(fluidForce,mFluidVelocity,v);
		mul(fluidForce,solid->mCoefficientOfEffectiveDrag);
		add(constraintForce,fluidForce);
		mul(constraintForce,solid->mInvMass);
		add(result,constraintForce);
	}
}

void Simulator::integrationStep(Solid *solid,const Vector3 &x,const Vector3 &v,const Vector3 &dx,const Vector3 &dv,scalar fdt,Vector3 &resultX,Vector3 &resultV){
	Vector3 &tx=cache_integrationStep_tx;
	Vector3 &tv=cache_integrationStep_tv;

	/*
		tx=x+dx*fdt;
		tv=v+dv*fdt;
		resultX=tv;
		updateAcceleration(resultV,solid,tx,tv,fdt);
	*/

	mul(tx,dx,fdt);
	add(tx,x);
	mul(tv,dv,fdt);
	add(tv,v);
	resultX.set(tv);
	updateAcceleration(resultV,solid,tx,tv,fdt);
}

int Simulator::countActiveSolids(){
	int active=0;
	int i;
	for(i=0;i<mSolids.size();++i){
		if(mSolids[i]->mActive){
			active++;
		}
	}
	return active;
}

}
}

