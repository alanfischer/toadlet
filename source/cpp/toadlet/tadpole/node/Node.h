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

#include <toadlet/tadpole/Types.h>
#include <toadlet/egg/Type.h>
#include <toadlet/tadpole/Bound.h>
#include <toadlet/tadpole/BaseComponent.h>
#include <toadlet/tadpole/Transformable.h>

#ifndef TOADLET_NODE
	#define TOADLET_NODE(Class,SuperClass) \
		typedef SuperClass super; \
		typedef toadlet::egg::Type<Class,toadlet::tadpole::node::Node> ThisType; \
		static ThisType *type(); \
		virtual toadlet::egg::BaseType<toadlet::tadpole::node::Node> *getType(){return Class::type();} \
		TOADLET_IPTR(Class)
#endif

#ifndef TOADLET_NONINSTANCIABLENODE
	#define TOADLET_NONINSTANCIABLENODE(Class,SuperClass) \
		typedef SuperClass super; \
		typedef toadlet::egg::NonInstantiableType<Class,toadlet::tadpole::node::Node> ThisType; \
		static ThisType *type(); \
		virtual toadlet::egg::BaseType<toadlet::tadpole::node::Node> *getType(){return Class::type();} \
		TOADLET_IPTR(Class)
#endif

#ifndef TOADLET_NODE_IMPLEMENT
	#define TOADLET_NODE_IMPLEMENT(Class,FullName) \
		Class::ThisType *Class::type(){static ThisType t(FullName);return &t;}
#endif

namespace toadlet{
namespace tadpole{

class Engine;
class RenderableSet;
class Scene;

namespace node{

class CameraNode;

class TOADLET_API Node:public BaseComponent,public Transformable{
public:
	TOADLET_NODE(Node,Node);

	static const Transform &identityTransform(){static Transform transform;return transform;}
	static const Bound &zeroBound(){static Bound bound;return bound;}

	enum InterfaceType{
		InterfaceType_ATTACHABLE,
		InterfaceType_DETAILTRACEABLE,
		InterfaceType_MATERIALABLE,
		InterfaceType_RENDERABLE,
		InterfaceType_TRACEABLE,
		InterfaceType_TRANSFORMABLE,
		InterfaceType_VISIBLE,
	};

	TOADLET_ALIGNED_NEW;

	Node();
	virtual ~Node();
	inline Node *create(Engine *engine){mEngine=engine;return create((Scene*)NULL);}
	virtual Node *create(Scene *scene);
	inline bool created() const{return mCreated;}
	virtual void destroy();
	inline bool destroyed() const{return !mCreated;}
	virtual Node *set(Node *node);
	Node *clone(Scene *scene);

	virtual Node *isNode(){return this;}
	virtual Node *isEntity(){return NULL;}
	virtual void *hasInterface(int type);

	inline int getUniqueHandle() const{return mUniqueHandle;}

	void destroyAllChildren();

	Node *getParent() const{return mParent;}
	void *getParentData() const{return mParentData;}
	virtual bool parentChanged(Node *node);
	virtual void parentDataChanged(void *parentData){mParentData=parentData;}
	Node *getPrevious() const{return mPrevious;}
	virtual void previousChanged(Node *previous){mPrevious=previous;}
	Node *getNext() const{return mNext;}
	virtual void nextChanged(Node *next){mNext=next;}

	virtual bool attach(Component *component);
	virtual void nodeAttached(Node *node){}
	virtual bool remove(Component *component);
	virtual void nodeRemoved(Node *node){}
	Node *getFirstChild(){return mFirstChild;}
	Node *getLastChild(){return mLastChild;}

	virtual void setDependsUpon(Node *node){mDependsUpon=node;}
	inline Node *getDependsUpon() const{return mDependsUpon;}

	virtual void mergeWorldBound(Node *child,bool justAttached);

	virtual void setTranslate(const Vector3 &translate);
	virtual void setTranslate(scalar x,scalar y,scalar z);
	inline const Vector3 &getTranslate() const{return mTransform.getTranslate();}

	virtual void setRotate(const Quaternion &rotate);
	virtual void setRotate(const Matrix3x3 &rotate);
	virtual void setRotate(const Vector3 &axis,scalar angle);
	inline const Quaternion &getRotate() const{return mTransform.getRotate();}

	virtual void setScale(const Vector3 &scale);
	virtual void setScale(scalar x,scalar y,scalar z);
	virtual void setScale(scalar s){setScale(s,s,s);}
	inline const Vector3 &getScale() const{return mTransform.getScale();}

	inline const Vector3 &getWorldTranslate() const{return mWorldTransform.getTranslate();}
	inline const Quaternion &getWorldRotate() const{return mWorldTransform.getRotate();}
	inline const Vector3 &getWorldScale() const{return mWorldTransform.getScale();}

	virtual void setMatrix4x4(const Matrix4x4 &matrix);

	virtual void setTransform(const Transform &transform);
	virtual const Transform &getTransform() const{return mTransform;}
	inline const Transform &getWorldTransform() const{return mWorldTransform;}

	virtual void setBound(const Bound &bound);
	inline const Bound &getBound() const{return mBound;}
	inline const Bound &getWorldBound() const{return mWorldBound;}

	virtual void setScope(int scope){mScope=scope;}
	inline int getScope() const{return mScope;}

	virtual void setName(const String &name){mName=name;}
	inline const String &getName() const{return mName;}

	virtual void logicUpdate(int dt,int scope);
	virtual void frameUpdate(int dt,int scope);

	virtual void setStayActive(bool active);
	inline bool getStayActive() const{return mDeactivateCount<0;}
	virtual void activate();
	virtual void deactivate();
	virtual void tryDeactivate();
	inline bool active() const{return mActive;}
	bool getTransformUpdated();

	virtual void updateWorldTransform();
	virtual void updateAllWorldTransforms();
	virtual void spacialUpdated();
	virtual void gatherRenderables(CameraNode *camera,RenderableSet *set);

	inline Engine *getEngine() const{return mEngine;}
	inline Scene *getScene() const{return mScene;}

protected:
	// Engine items
	bool mCreated;
	Engine *mEngine;
	Scene *mScene;
	int mUniqueHandle;

	Node::ptr mParent,mPrevious,mNext;
	void *mParentData;

	Collection<Component::ptr> mComponents;
	Node::ptr mFirstChild,mLastChild;
	bool mChildrenActive;
	bool mActivateChildren;

	Node::ptr mDependsUpon;
	bool mActive;
	int mDeactivateCount;
	int mLastLogicFrame;
	int mLastFrame;
	int mTransformUpdatedFrame;

	Transform mTransform;
	Bound mBound;
	Transform mWorldTransform;
	Bound mWorldBound;
	int mScope;
	String mName;
};

}
}
}

#endif
