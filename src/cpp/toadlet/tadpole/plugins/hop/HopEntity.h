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

class TOADLET_API HopEntity:public node::ParentNode,public hop::TraceCallback,public hop::CollisionListener{
public:
	TOADLET_NODE(HopEntity,node::ParentNode);

	HopEntity();
	virtual Node *create(node::Scene *scene);
	virtual void destroy();

	virtual Node *isEntity(){return this;}

	virtual void setCollisionBits(int bits);
	virtual int getCollisionBits() const{return mSolid->getCollisionBits();}

	virtual void setCollideWithBits(int bits);
	virtual int getCollideWithBits() const{return mSolid->getCollideWithBits();}

	virtual void setMass(scalar mass);
	virtual scalar getMass() const{return mSolid->getMass();}

	virtual void setInfiniteMass();
	virtual bool hasInfiniteMass() const{return mSolid->hasInfiniteMass();}

	virtual void setTranslate(const Vector3 &translate);
	virtual void setTranslate(scalar x,scalar y,scalar z);
	virtual void setTransform(const Matrix4x4 &transform);
	virtual void setPosition(const Vector3 &position){setTranslate(position);}
	virtual const Vector3 &getPosition(){return getTranslate();}

	virtual void setVelocity(const Vector3 &velocity);
	virtual void setVelocity(scalar x,scalar y,scalar z);
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

	virtual void setTraceableShape(Traceable *traceable);
	virtual void addShape(hop::Shape::ptr shape);
	virtual void removeShape(hop::Shape *shape);
	virtual void removeAllShapes();
	virtual hop::Shape::ptr getShape(int i) const{return mSolid->getShape(i);}
	virtual int getNumShapes() const{return mSolid->getNumShapes();}

	inline hop::Solid::ptr getSolid() const{return mSolid;}

	virtual void think();
	virtual void touch(const Collision &c);

	virtual void setNextThink(int nextThink){mNextThink=nextThink;}
	inline int getNextThink() const{return mNextThink;}

	void setShadowMesh(mesh::Mesh::ptr shadow,scalar scale,scalar testLength,scalar offset);
	inline mesh::Mesh::ptr setShadowMesh() const{return mShadowMesh;}

	// Node callbacks
	virtual void parentChanged(node::ParentNode *parent);

	// TraceCallback callbacks
	virtual void getBound(AABox &result);
	virtual void traceSegment(hop::Collision &result,const Segment &segment);
	virtual void traceSolid(hop::Collision &result,const Segment &segment,const hop::Solid *solid);

	// CollisionListener callbacks
	virtual void collision(const hop::Collision &c);

	virtual void preLogicUpdateLoop(int dt);
	virtual void logicUpdate(int dt);
	virtual void postLogicUpdate(int dt);
	virtual void renderUpdate(node::CameraNode *camera,RenderQueue *queue);
	virtual void updateRenderTransforms();
	virtual void interpolatePhysicalParameters(scalar f);

protected:
	virtual void castShadow();
	virtual void showCollisionVolumes(bool show);
	virtual void updateVolumes(bool interpolate);

	int mNextThink;
	hop::Solid::ptr mSolid;
	hop::Shape::ptr mTraceableShape;
	Traceable *mTraceable;
	
	HopScene *mHopScene;
	Vector3 mLastPosition;
	bool mActivePrevious;

	scalar mShadowTestLength;
	scalar mShadowOffset;
	mesh::Mesh::ptr mShadowMesh;
	Material::ptr mShadowMaterial;
	node::MeshNode::ptr mShadowNode;

	node::ParentNode::ptr mVolumeNode;

	Vector3 cache_setVelocity_velocity;
	Vector3 cache_interpolatePhysicalParameters_interpolate;

	friend class HopScene;
};

}
}

#endif
