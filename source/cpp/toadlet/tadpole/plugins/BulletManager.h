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

#ifndef TOADLET_TADPOLE_BULLETMANAGER_H
#define TOADLET_TADPOLE_BULLETMANAGER_H

#include <btBulletDynamicsCommon.h>
#include <toadlet/tadpole/PhysicsCollision.h>
#include <toadlet/tadpole/PhysicsManager.h>
#include <toadlet/tadpole/PhysicsTraceable.h>
#include <toadlet/tadpole/sensor/BoundingVolumeSensor.h>

namespace toadlet{
namespace tadpole{

inline void setVector3(btVector3 &r,const Vector3 &v){r.setValue(v.x,v.y,v.z);}
inline void setVector4(btVector4 &r,const Vector4 &v){r.setValue(v.x,v.y,v.z,v.w);}
inline void setQuaternion(btQuaternion &r,const Quaternion &q){r.setValue(q.x,q.y,q.z,q.w);}

inline void setVector3(Vector3 &r,const btVector3 &v){r.set(v.getX(),v.getY(),v.getZ());}
inline void setVector4(Vector4 &r,const btVector4 &v){r.set(v.getX(),v.getY(),v.getZ(),v.getW());}
inline void setQuaternion(Quaternion &r,const btQuaternion &q){r.set(q.getX(),q.getY(),q.getZ(),q.getW());}

class BulletComponent;

class TOADLET_API BulletManager:public Object,public PhysicsManager{
public:
	TOADLET_IOBJECT(BulletManager);

	BulletManager(Scene *scene);
	virtual ~BulletManager();

	PhysicsComponent *createPhysicsComponent();

	btDynamicsWorld *getWorld() const{return mWorld;}

	void setGravity(const Vector3 &gravity){btVector3 v;setVector3(v,gravity);mWorld->setGravity(v);}
	const Vector3 &getGravity() const{setVector3(mGravity,mWorld->getGravity());return mGravity;}

	void setTraceable(PhysicsTraceable *traceable){}
	void setTraceableCollisionScope(int scope){}
	int getTraceableCollisionScope() const{return -1;}

	void traceSegment(PhysicsCollision &result,const Segment &segment,int collideWithScope,Node *ignore){}
	void traceNode(PhysicsCollision &result,Node *node,const Segment &segment,int collideWithScope){}
	void testNode(PhysicsCollision &result,Node *node,const Segment &segment,Node *node2){}

	void logicUpdate(int dt,int scope,Node *node);
	void frameUpdate(int dt,int scope,Node *node);

	void componentCreated(BulletComponent *component){mComponents.add(component);}
	void componentDestroyed(BulletComponent *component){mComponents.remove(component);}

protected:
	Scene::ptr mScene;
	btDiscreteDynamicsWorld *mWorld;
	mutable Vector3 mGravity;
	Collection<BulletComponent*> mComponents;
};

}
}

#endif
