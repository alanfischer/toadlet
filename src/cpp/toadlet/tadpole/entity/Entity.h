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

#ifndef TOADLET_TADPOLE_ENTITY_ENTITY_H
#define TOADLET_TADPOLE_ENTITY_ENTITY_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/egg/IntrusivePointer.h>
#include <toadlet/egg/WeakPointer.h>
#include <toadlet/tadpole/entity/EntityDestroyedListener.h>

#ifndef TOADLET_ENTITY
	#define TOADLET_ENTITY(Class,SuperClass) \
		typedef SuperClass super; \
		TOADLET_INTRUSIVE_POINTERS(Class)
#endif

namespace toadlet{
namespace tadpole{

class Engine;

namespace entity{

class ParentEntity;
class Scene;

class TOADLET_API Entity{
public:
	TOADLET_INTRUSIVE_POINTERS(Entity);

	Entity();
	virtual ~Entity();
	virtual Entity *create(Engine *engine);
	virtual void destroy();

	inline bool destroyed() const{return !mCreated;}
	virtual bool isParent() const{return false;}
	virtual bool isRenderable() const{return false;}
	virtual bool isLight() const{return false;} // This one at least should be moved out to visualUpdate() in the LightEntity

	virtual void setEntityDestroyedListener(EntityDestroyedListener *listener,bool owns);
	inline EntityDestroyedListener *getEntityDestroyedListener() const{return mEntityDestroyedListener;}
	virtual void removeAllEntityDestroyedListeners();

	virtual void parentChanged(ParentEntity *newParent){mParent=newParent;}
	inline ParentEntity *getParent() const{return mParent;}

	virtual void setTranslate(const Vector3 &translate);
	virtual void setTranslate(scalar x,scalar y,scalar z);
	inline const Vector3 &getTranslate() const{return mTranslate;}

	virtual void setRotate(const Matrix3x3 &rotate);
	virtual void setRotate(scalar x,scalar y,scalar z,scalar angle);
	inline const Matrix3x3 &getRotate() const{return mRotate;}

	virtual void setScale(const Vector3 &scale);
	virtual void setScale(scalar x,scalar y,scalar z);
	inline const Vector3 &getScale() const{return mScale;}

	inline bool isIdentityTransform() const{return mIdentityTransform;}

	virtual void setScope(int scope);
	inline int getScope() const{return mScope;}

	virtual void setBoundingRadius(scalar boundingRadius);
	inline scalar getBoundingRadius() const{return mBoundingRadius;}

	virtual void setReceiveUpdates(bool receiveUpdates);
	inline bool getReceiveUpdates() const{return mReceiveUpdates;}

	/// Only called if the Entity registers itself with the Scene in registerUpdateEntity.
	/// Dont forget to call unregisterUpdateEntity in its destroy.
	virtual void logicUpdate(int dt){}
	virtual void visualUpdate(int dt){}

	void modified();
	bool modifiedSinceLastLogicFrame() const;
	bool modifiedSinceLastVisualFrame() const;

	inline Engine *getEngine() const{return mEngine;}

	inline egg::PointerCounter *getCounter() const{return mCounter;}
	inline const Matrix4x4 &getVisualTransform() const{return mVisualTransform;}
	inline const Matrix4x4 &getVisualWorldTransform() const{return mVisualWorldTransform;}

	inline void internal_setManaged(bool managed){mManaged=managed;}
	inline bool internal_getManaged() const{return mManaged;}

protected:
	void setVisualTransformTranslate(const Vector3 &translate);
	void setVisualTransformRotateScale(const Matrix3x3 &rotate,const Vector3 &scale);

	// Allocation items
	egg::PointerCounter *mCounter;
	bool mManaged;

	// Engine items
	bool mCreated;
	Engine *mEngine;

	// Entity items
	EntityDestroyedListener *mEntityDestroyedListener;
	bool mOwnsEntityDestroyedListener;

	egg::IntrusivePointer<ParentEntity> mParent;

	bool mIdentityTransform;
	Vector3 mTranslate;
	Matrix3x3 mRotate;
	Vector3 mScale;
	int mScope;
	scalar mBoundingRadius;
	bool mReceiveUpdates;

	int mModifiedLogicFrame;
	int mModifiedVisualFrame;
	int mWorldModifiedLogicFrame;
	int mWorldModifiedVisualFrame;

	Matrix4x4 mVisualTransform;
	Sphere mVisualWorldBound;
	Matrix4x4 mVisualWorldTransform;

	Vector3 cache_setRotate_vector;

	friend class Scene;
};

}
}
}

#endif
