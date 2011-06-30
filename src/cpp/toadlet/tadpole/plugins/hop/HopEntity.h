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

#ifndef TOADLET_TADPOLE_HOPENTITY_H
#define TOADLET_TADPOLE_HOPENTITY_H

#include <toadlet/egg/MathConversion.h>
#include <toadlet/tadpole/Traceable.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/plugins/hop/HopScene.h>

namespace toadlet{
namespace tadpole{

/// This ties a hop::Solid to a tadpole::node::ParentNode.  It will adjust its node transform based on the hop::Solid.
///  It's localBound is by default set to the localBound of the hop::Solid
class TOADLET_API HopEntity:public ParentNode,public TraceCallback,public CollisionListener{
public:
	enum TriggerType{
		TriggerType_TOGGLE,
		TriggerType_ON,
		TriggerType_OFF,
	};

	TOADLET_NODE(HopEntity,ParentNode);

	HopEntity();
	virtual Node *create(Scene *scene);

	virtual Node *isEntity(){return this;}

	virtual void setCollisionBits(int bits);
	virtual int getCollisionBits() const{return mSolid->getCollisionBits();}

	virtual void setCollideWithBits(int bits);
	virtual int getCollideWithBits() const{return mSolid->getCollideWithBits();}

	virtual void setMass(scalar mass);
	virtual scalar getMass() const{return mSolid->getMass();}

	virtual void setInfiniteMass();
	virtual bool hasInfiniteMass() const{return mSolid->hasInfiniteMass();}

	virtual void setPosition(const Vector3 &position);
	virtual void setPositionDirect(const Vector3 &position);
	virtual const Vector3 &getPosition(){return mSolid->getPosition();}

	virtual void setVelocity(const Vector3 &velocity);
	virtual const Vector3 &getVelocity() const{return mSolid->getVelocity();}

	virtual void addForce(const Vector3 &force);
	virtual const Vector3 &getForce() const{return mSolid->getForce();}
	virtual void clearForce();

	virtual void setCoefficientOfGravity(scalar coeff);
	virtual scalar getCoefficientOfGravity() const{return mSolid->getCoefficientOfGravity();}

	virtual void setCoefficientOfRestitution(scalar coeff);
	virtual scalar getCoefficientOfRestitution() const{return mSolid->getCoefficientOfRestitution();}

	virtual void setCoefficientOfRestitutionOverride(bool over);
	virtual bool getCoefficientOfRestitutionOverride() const{return mSolid->getCoefficientOfRestitutionOverride();}

	virtual void setCoefficientOfStaticFriction(scalar coeff);
	virtual scalar getCoefficientOfStaticFriction() const{return mSolid->getCoefficientOfStaticFriction();}

	virtual void setCoefficientOfDynamicFriction(scalar coeff);
	virtual scalar getCoefficientOfDynamicFriction() const{return mSolid->getCoefficientOfDynamicFriction();}

	virtual void setCoefficientOfEffectiveDrag(scalar coeff);
	virtual scalar getCoefficientOfEffectiveDrag() const{return mSolid->getCoefficientOfEffectiveDrag();}

	virtual void setInternalScope(int scope){mSolid->setInternalScope(scope);}
	virtual int getInternalScope() const{return mSolid->getInternalScope();}

	virtual const AABox &getBound(){return mSolid->getLocalBound();}

	virtual void setScope(int scope){mSolid->setScope(scope); super::setScope(scope);}

	virtual void setNextThink(int think);

	virtual void setTraceableShape(Traceable *traceable,Node *traceableNode=NULL);
	virtual Shape::ptr getTraceableShape() const{return mTraceableShape;}
	virtual void addShape(Shape::ptr shape);
	virtual void removeShape(Shape *shape);
	virtual void removeAllShapes();
	virtual Shape::ptr getShape(int i) const{return mSolid->getShape(i);}
	virtual int getNumShapes() const{return mSolid->getNumShapes();}

	// Shortcuts for a single shape
	virtual void setShape(Shape::ptr shape){if(mSolid->getNumShapes()>0){removeAllShapes();}addShape(shape);}
	virtual Shape::ptr getShape() const{return mSolid->getNumShapes()>0?mSolid->getShape(0):NULL;}

	virtual void touch(const tadpole::Collision &c){}
	virtual void think(){}
	virtual void trigger(TriggerType type=TriggerType_TOGGLE,HopEntity *triggerer=NULL){if(mTriggerTarget!=NULL){mTriggerTarget->trigger(type,triggerer!=NULL?triggerer:this);}}

	inline Solid::ptr getSolid() const{return mSolid;}

	virtual void setCollisionVolumesVisible(bool visible);

	void preSimulate();
	void postSimulate();

	void updatePosition(const Vector3 &position);
	void lerpPosition(scalar fraction);

	// Node callbacks
	virtual void spacialUpdated();
	virtual void parentChanged(ParentNode *parent);
	virtual void logicUpdate(int dt,int scope);
	virtual void frameUpdate(int dt,int scope);

	// TraceCallback callbacks
	virtual void getBound(AABox &result);
	virtual void traceSegment(hop::Collision &result,const Vector3 &position,const Segment &segment);
	virtual void traceSolid(hop::Collision &result,Solid *solid,const Vector3 &position,const Segment &segment);

	// CollisionListener callbacks
	virtual void collision(const hop::Collision &c);

protected:
	virtual void updateCollisionVolumes();

	Solid::ptr mSolid;
	Vector3 mOldPosition,mNewPosition,mCurrentPosition;
	bool mSkipNextPreSimulate;
	Shape::ptr mTraceableShape;
	Traceable *mTraceable;
	Node::ptr mTraceableNode;
	ParentNode::ptr mVolumeNode;
	int mNextThink;
	HopEntity::wptr mTriggerTarget;

	HopScene *mHopScene;

	friend class HopScene;
};

}
}

#endif
