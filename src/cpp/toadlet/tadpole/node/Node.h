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
//#include <toadlet/egg/Property.h>
#include <toadlet/egg/WeakPointer.h>
#include <toadlet/egg/Type.h>
#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Bound.h>
#include <toadlet/tadpole/Transform.h>
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

	enum TransformUpdate{
		TransformUpdate_BIT_TRANSLATE=1<<0,
		TransformUpdate_BIT_ROTATE=1<<1,
		TransformUpdate_BIT_SCALE=1<<2,
		TransformUpdate_BIT_BOUND=1<<3,
		TransformUpdate_BIT_INTERPOLATOR=1<<4,
	};

	Node();
	virtual ~Node();
	virtual Node *create(Scene *scene);
	inline bool created() const{return mCreated;}
	virtual void destroy();
	inline bool destroyed() const{return !mCreated;}

	virtual ParentNode *isParent(){return NULL;}
	virtual Node *isEntity(){return NULL;}

	inline int getUniqueHandle() const{return mUniqueHandle;}

	virtual void addNodeListener(NodeListener::ptr listener);
	virtual void removeNodeListener(NodeListener::ptr listener);
	virtual void removeAllNodeListeners(){mNodeListeners=NULL;}

	virtual void parentChanged(ParentNode *parent);
	ParentNode *getParent() const;
	virtual void parentDataChanged(void *parentData);
	void *getParentData() const{return mParentData;}

	virtual void setDependsUpon(Node *node){mDependsUpon=node;}
	inline Node *getDependsUpon() const{return mDependsUpon;}

	virtual void setTranslate(const Vector3 &translate);
	virtual void setTranslate(scalar x,scalar y,scalar z);
	inline const Vector3 &getTranslate() const{return mTransform->getTranslate();}

	virtual void setRotate(const Quaternion &rotate);
	virtual void setRotate(const Matrix3x3 &rotate);
	virtual void setRotate(const Vector3 &axis,scalar angle);
	inline const Quaternion &getRotate() const{return mTransform->getRotate();}

	virtual void setScale(const Vector3 &scale);
	virtual void setScale(scalar x,scalar y,scalar z);
	virtual void setScale(scalar s){setScale(s,s,s);}
	inline const Vector3 &getScale() const{return mTransform->getScale();}

	inline const Vector3 &getWorldTranslate() const{return mWorldTransform->getTranslate();}
	inline const Quaternion &getWorldRotate() const{return mWorldTransform->getRotate();}
	inline const Vector3 &getWorldScale() const{return mWorldTransform->getScale();}

	// I suppose we could modify the Transform interface to just be virtual getters only
	// Then the class Tranform would be a TranslateRotateScaleTransform
	// Then we could have a getTransform() method in node since it couldn't be set
	// OR: We could add a listener in the Transform class that notifys the node that it was modified
	// Then you can freely grab transforms and modify items in them without worry.
	// That would also make it easier for Animations, since we wouldn't need a Node animation
	// Instead just an Animation that modifys a Transform would suffice.
	// Though in the second case, we would still need some way of the Interpolators notifying that it was an interpolation change.
	virtual void setTransform(Transform *transform,int tu);
	inline Transform *getWorldTransform() const{return mWorldTransform;}

	virtual void setBound(Bound *bound);
	inline Bound::ptr getBound() const{return mBound;}
	inline Bound::ptr getWorldBound() const{return mWorldBound;}

	virtual void handleEvent(const egg::Event::ptr &event){}

	virtual void setScope(int scope){mScope=scope;}
	inline int getScope() const{return mScope;}

	virtual void setName(const egg::String &name){mName=name;}
	inline const egg::String &getName() const{return mName;}

	virtual void logicUpdate(int dt,int scope);
	virtual void frameUpdate(int dt,int scope);
	virtual void queueRenderables(CameraNode *camera,RenderQueue *queue){}

	virtual void setStayActive(bool active);
	virtual void activate();
	virtual void deactivate();
	virtual void tryDeactivate();
	inline bool active() const{return mActive;}
	bool getTransformUpdated();

	virtual void updateWorldTransform();
	virtual void transformUpdated(int tu);

	inline Engine *getEngine() const{return mEngine;}
	inline Scene *getScene() const{return mScene;}

	inline egg::PointerCounter *pointerCounter() const{return mPointerCounter;}
	inline void internal_setManaged(bool managed){mManaged=managed;}
	inline bool internal_getManaged() const{return mManaged;}

//	TOADLET_PROPERTY(Vector3,translate,Node,getTranslate,setTranslate);
//	TOADLET_PROPERTY(Quaternion,rotate,Node,getRotate,setRotate);
//	TOADLET_PROPERTY(Vector3,scale,Node,getScale,setScale);

//	TOADLET_PROPERTY(int,scope,Node,getScope,setScope);
//	TOADLET_PROPERTY(egg::String,name,Node,getName,setName);

protected:
	virtual void logicUpdateListeners(int dt);
	virtual void frameUpdateListeners(int dt);

	// Allocation items
	egg::PointerCounter *mPointerCounter;
	bool mManaged;

	// Engine items
	bool mCreated;
	Engine *mEngine;
	Scene *mScene;
	int mUniqueHandle;

	// Node items
	egg::Collection<NodeListener::ptr>::ptr mNodeListeners;

	Node::ptr mParent;
	void *mParentData;
	
	Node::ptr mDependsUpon;

	int mTransformUpdatedFrame;
	Transform::ptr mTransform;
	Bound::ptr mBound;
	Transform::ptr mWorldTransform;
	Bound::ptr mWorldBound;

	int mScope;
	egg::String mName;

	bool mActive;
	int mDeactivateCount;
	int mLastLogicFrame;
	int mLastFrame;

	friend class ParentNode;
};

}
}
}

#endif
