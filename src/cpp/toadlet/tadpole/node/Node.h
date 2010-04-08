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

#ifndef TOADLET_TADPOLE_NODE_NODE_H
#define TOADLET_TADPOLE_NODE_NODE_H

#include <toadlet/egg/Event.h>
#include <toadlet/egg/IntrusivePointer.h>
#include <toadlet/egg/WeakPointer.h>
#include <toadlet/egg/Type.h>
#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/node/NodeListener.h>
#include <toadlet/tadpole/node/NodeInterpolator.h>

#ifndef TOADLET_NODE
	#define TOADLET_NODE(Class,SuperClass) \
		typedef SuperClass super; \
		typedef toadlet::egg::Type<Class,toadlet::tadpole::node::Node> ThisType; \
		static const ThisType &type(); \
		virtual const toadlet::egg::BaseType<toadlet::tadpole::node::Node> &getType(){return Class::type();} \
		TOADLET_INTRUSIVE_POINTERS(Class)
#endif

#ifndef TOADLET_NONINSTANCIABLENODE
	#define TOADLET_NONINSTANCIABLENODE(Class,SuperClass) \
		typedef SuperClass super; \
		typedef toadlet::egg::NonInstantiableType<Class,toadlet::tadpole::node::Node> ThisType; \
		static const ThisType &type(); \
		virtual const toadlet::egg::BaseType<toadlet::tadpole::node::Node> &getType(){return Class::type();} \
		TOADLET_INTRUSIVE_POINTERS(Class)
#endif

#ifndef TOADLET_NODE_IMPLEMENT
	#define TOADLET_NODE_IMPLEMENT(Class,FullName) \
		const Class::ThisType &Class::type(){static ThisType t(FullName);return t;}
#endif

namespace toadlet{
namespace tadpole{

class Engine;
class RenderQueue;
class Scene;

namespace node{

class CameraNode;
class ParentNode;

class TOADLET_API Node{
public:
	TOADLET_NODE(Node,Node);

	enum{
		Transform_BIT_TRANSLATE=1<<0,
		Transform_BIT_ROTATE=	1<<1,
		Transform_BIT_SCALE=	1<<2,
	};

	Node();
	virtual ~Node();
	virtual Node *create(Scene *scene);
	inline bool created() const{return mCreated;}
	virtual void destroy();

	virtual ParentNode *isParent(){return NULL;}
	virtual Node *isEntity(){return NULL;}

	inline int getHandle() const{return mHandle;}

	virtual void addNodeListener(NodeListener::ptr listener);
	virtual void removeNodeListener(NodeListener::ptr listener);
	virtual void removeAllNodeListeners(){mNodeListeners=NULL;}
	
	virtual void setNodeInterpolator(NodeInterpolator::ptr interpolator){mNodeInterpolator=interpolator;}
	inline NodeInterpolator::ptr getNodeInterpolator() const{return mNodeInterpolator;}
	virtual void setNodeInterpolatorEnabled(bool enabled){mNodeInterpolatorEnabled=enabled;}
	inline bool getNodeInterpolatorEnabled() const{return mNodeInterpolatorEnabled;}

	virtual void parentChanged(ParentNode *parent);
	ParentNode *getParent() const;
	virtual void parentDataChanged(void *parentData);
	void *getParentData() const{return mParentData;}

	virtual void setDependsUpon(Node *node){mDependsUpon=node;}
	inline Node *getDependsUpon() const{return mDependsUpon;}

	virtual void setTranslate(const Vector3 &translate);
	virtual void setTranslate(scalar x,scalar y,scalar z);
	inline const Vector3 &getTranslate() const{return mTranslate;}
	inline const Vector3 &getWorldTranslate() const{return mWorldTranslate;}

	virtual void setRotate(const Matrix3x3 &rotate);
	virtual void setRotate(const Quaternion &rotate);
	virtual void setRotate(scalar x,scalar y,scalar z,scalar angle);
	inline const Quaternion &getRotate() const{return mRotate;}
	inline const Quaternion &getWorldRotate() const{return mWorldRotate;}

	virtual void setScale(const Vector3 &scale);
	virtual void setScale(scalar x,scalar y,scalar z);
	virtual void setScale(scalar s);
	inline const Vector3 &getScale() const{return mScale;}
	inline const Vector3 &getWorldScale() const{return mWorldScale;}

	virtual void setTransform(const Matrix4x4 &transform);
	inline const Matrix4x4 &getWorldTransform() const{return mWorldTransform;}
	
	inline bool isIdentityTransform() const{return mIdentityTransform;}

	virtual void findTransformTo(Matrix4x4 &result,Node *node);

	virtual void handleEvent(const egg::Event::ptr &event){}

	virtual void setScope(int scope){mScope=scope;}
	inline int getScope() const{return mScope;}

	virtual void setName(const egg::String &name){mName=name;}
	inline const egg::String &getName() const{return mName;}

	virtual void setLocalBound(const Sphere &bound);
	inline const Sphere &getLocalBound() const{return mLocalBound;}
	inline const Sphere &getWorldBound() const{return mWorldBound;}

	virtual void logicUpdate(int dt);
	virtual void frameUpdate(int dt);
	virtual void queueRenderables(CameraNode *camera,RenderQueue *queue){}

	virtual void activate();
	virtual void deactivate();
	inline bool active() const{return mActive;}

	inline Engine *getEngine() const{return mEngine;}
	inline Scene *getScene() const{return mScene;}

	inline egg::PointerCounter *pointerCounter() const{return mPointerCounter;}
	inline void internal_setManaged(bool managed){mManaged=managed;}
	inline bool internal_getManaged() const{return mManaged;}

	static void mul(Sphere &r,const Vector3 &translate,const Quaternion &rotate,const Vector3 &scale,const Sphere &s){
		Math::mul(r.origin,scale,s.radius);
		r.radius=Math::maxVal(Math::maxVal(r.origin.x,r.origin.y),r.origin.z);
		Math::mul(r.origin,rotate,s.origin);
		Math::add(r.origin,translate);
	}

	// Merge two spheres, passing along -1 radius, and ignoring 0 radius
	static void merge(Sphere &r,const Sphere &s){
		if(r.radius>0 && s.radius>0){
			Vector3 origin=(r.origin+s.origin)/2;
			scalar radius=Math::maxVal(Math::length(r.origin-origin)+r.radius,Math::length(s.origin-origin)+s.radius);
			r.origin=origin;
			r.radius=radius;
		}
		else if(r.radius==0 && s.radius>0){
			r.set(s);
		}
		else if(s.radius<0){
			r.radius=-Math::ONE;
		}
	}

	static void set(Sphere &sphere,const AABox &box){
		Math::add(sphere.origin,box.mins,box.maxs);
		Math::div(sphere.origin,Math::TWO);
		scalar x=Math::maxVal(sphere.origin.x-box.mins.x,box.maxs.x-sphere.origin.x);
		scalar y=Math::maxVal(sphere.origin.y-box.mins.y,box.maxs.y-sphere.origin.y);
		scalar z=Math::maxVal(sphere.origin.z-box.mins.z,box.maxs.z-sphere.origin.z);
		sphere.radius=Math::sqrt(Math::square(x)+Math::square(y)+Math::square(z));
	}

	static void set(AABox &box,const Sphere &sphere){
		box.set(sphere.radius);
		Math::add(box,sphere.origin);
	}

protected:
	void transformUpdated(int transformBits);

	// Allocation items
	egg::PointerCounter *mPointerCounter;
	bool mManaged;

	// Engine items
	bool mCreated;
	Engine *mEngine;
	Scene *mScene;
	int mHandle;

	// Node items
	egg::Collection<NodeListener::ptr>::ptr mNodeListeners;
	NodeInterpolator::ptr mNodeInterpolator;
	bool mNodeInterpolatorEnabled;

	Node::ptr mParent;
	void *mParentData;
	
	Node::ptr mDependsUpon;

	bool mIdentityTransform;
	Vector3 mTranslate;
	Quaternion mRotate;
	Vector3 mScale;
	Sphere mLocalBound;
	Vector3 mWorldTranslate;
	Quaternion mWorldRotate;
	Vector3 mWorldScale;
	Sphere mWorldBound;
	Matrix4x4 mWorldTransform;

	int mScope;
	egg::String mName;

	bool mActive;
	int mDeactivateCount;
	int mLastLogicFrame;
	int mLastFrame;

	Vector3 cache_setRotate_vector;
	Matrix3x3 cache_setTransform_matrix;

	friend class Scene;
};

}
}
}

#endif
