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
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/node/MeshNode.h>
#include <toadlet/tadpole/plugins/hop/HopScene.h>
#include <toadlet/tadpole/plugins/hop/HopCollisionListener.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API HopEntity:public node::ParentNode,public hop::CollisionListener{
public:
	TOADLET_NODE(HopEntity,node::ParentNode);

	// TODO: Rename this to fit with new enum standards when reworking client/server
	enum{
		ENTITY_BITS_NONE=0,
		ENTITY_BITS_ALL=0xFFFFFFFF,

		ENTITY_BIT_SHAPE=1<<1,
		ENTITY_BIT_MASS=1<<2,
		ENTITY_BIT_POSITION=1<<3,
		ENTITY_BIT_VELOCITY=1<<4,
		ENTITY_BIT_FORCE=1<<5,
		ENTITY_BIT_GRAVITY=1<<6,
		ENTITY_BIT_CO_RESTITUTION=1<<7,
		ENTITY_BIT_CO_RESTITUTIONOVERRIDE=1<<8,
		ENTITY_BIT_CO_STATICFRICTION=1<<9,
		ENTITY_BIT_CO_DYNAMICFRICTION=1<<10,
		ENTITY_BIT_CO_EFFECTIVEDRAG=1<<11,

		ENTITY_BITS_PHYSICS_UPDATE=ENTITY_BIT_POSITION | ENTITY_BIT_VELOCITY,
	};

	const static int NETWORKID_NOT_NETWORKED=-1;

	HopEntity();
	virtual Node *create(Engine *engine);
	virtual void destroy();

	/// Can be used to specify this is an instance of a custom class
	virtual bool isCustom() const{return false;}

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
	virtual void setPosition(const Vector3 &position){setTranslate(position);}
	virtual const Vector3 &getPosition(){return getTranslate();}

	virtual void setVelocity(const Vector3 &velocity);
	virtual void setVelocity(scalar x,scalar y,scalar z);
	virtual const Vector3 &getVelocity() const{return mSolid->getVelocity();}

	virtual void addForce(const Vector3 &force);
	virtual const Vector3 &getForce() const{return mSolid->getForce();}
	virtual void clearForce();

	virtual void setLocalGravity(const Vector3 &gravity);
	virtual const Vector3 &getLocalGravity() const{return mSolid->getLocalGravity();}

	virtual void setWorldGravity();
	virtual bool hasLocalGravity() const{return mSolid->hasLocalGravity();}

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

	virtual void addShape(hop::Shape::ptr shape);
	virtual void removeShape(hop::Shape *shape);
	virtual void removeAllShapes();
	virtual hop::Shape::ptr getShape(int i) const{return mSolid->getShape(i);}
	virtual int getNumShapes() const{return mSolid->getNumShapes();}

	inline HopScene::ptr getScene() const{return mScene;}
	inline hop::Solid::ptr getSolid() const{return mSolid;}
	virtual HopEntity::ptr getTouching() const;

	virtual void setCollisionListener(HopCollisionListener *listener);

	virtual void think();

	virtual void setNextThink(int nextThink){mNextThink=nextThink;}
	inline int getNextThink() const{return mNextThink;}

	virtual void changeNetworkID(int id){mScene->setHopEntityNetworkID(this,id);}
	inline int getNetworkID() const{return mNetworkID;}
	inline bool isNetworked() const{return mNetworkID!=NETWORKID_NOT_NETWORKED;}

	void resetModifiedFields(){mModifiedFields=0;}
	void addModifiedFields(int fields){mModifiedFields|=fields;}
	inline int getModifiedFields() const{return mModifiedFields;}

	void setShadowMesh(mesh::Mesh::ptr shadow,scalar scale,scalar testLength,scalar offset);
	inline mesh::Mesh::ptr setShadowMesh() const{return mShadowMesh;}

	virtual void parentChanged(node::ParentNode *parent);

	virtual void collision(const hop::Collision &c);

//protected:
	virtual void preLogicUpdateLoop(int dt);
	virtual void postLogicUpdate(int dt);
	virtual void interpolatePhysicalParameters(scalar f);

protected:
	virtual void castShadow();
	virtual void showCollisionVolumes(bool show);
	virtual void updateVolumes(bool interpolate);

	int mNextThink;
	hop::Solid::ptr mSolid;
	HopScene::ptr mScene;
	HopCollisionListener *mListener;
	HopCollision mHopCollision;
	Vector3 mLastPosition;
	bool mActivePrevious;

	int mNetworkID;
	uint32 mModifiedFields;

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
