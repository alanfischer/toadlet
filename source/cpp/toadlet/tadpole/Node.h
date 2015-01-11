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

#ifndef TOADLET_TADPOLE_NODE_H
#define TOADLET_TADPOLE_NODE_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/egg/Type.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/egg/IteratorRange.h>
#include <toadlet/tadpole/Bound.h>
#include <toadlet/tadpole/BaseComponent.h>

#define TOADLET_NODE(Class)\
	TOADLET_OBJECT(Class);\
	static toadlet::egg::Type<toadlet::tadpole::Component> *type(){static toadlet::egg::InstantiableType<Class,toadlet::tadpole::Component> thisType(#Class);return &thisType;} \
	virtual toadlet::egg::Type<toadlet::tadpole::Component> *getType() const{return type();}

#define TOADLET_NODE_NAME(Class,Name)\
	TOADLET_OBJECT(Class);\
	static toadlet::egg::Type<toadlet::tadpole::Component> *type(){static toadlet::egg::InstantiableType<Class,toadlet::tadpole::Component> thisType(Name);return &thisType;} \
	virtual toadlet::egg::Type<toadlet::tadpole::Component> *getType() const{return type();}

#define TOADLET_INODE(Class)\
	TOADLET_IOBJECT(Class);\
	static toadlet::egg::Type<toadlet::tadpole::Component> *type(){static toadlet::egg::InstantiableType<Class,toadlet::tadpole::Component> thisType(#Class);return &thisType;} \
	virtual toadlet::egg::Type<toadlet::tadpole::Component> *getType() const{return type();}

#define TOADLET_INODE_NAME(Class,Name)\
	TOADLET_IOBJECT(Class);\
	static toadlet::egg::Type<toadlet::tadpole::Component> *type(){static toadlet::egg::InstantiableType<Class,toadlet::tadpole::Component> thisType(Name);return &thisType;} \
	virtual toadlet::egg::Type<toadlet::tadpole::Component> *getType() const{return type();}

namespace toadlet{
namespace tadpole{

class Engine;
class RenderableSet;
class Scene;
class Camera;
class Visible;
class Spacial;
class Animatable;
class ActionComponent;
class LightComponent;
class PhysicsComponent;

namespace action{
class Action;
}

typedef PointerIteratorRange<Component> ComponentRange;
typedef PointerIteratorRange<Node> NodeRange;
typedef PointerIteratorRange<Spacial> SpacialRange;
typedef PointerIteratorRange<Visible> VisibleRange;
typedef PointerIteratorRange<Animatable> AnimatableRange;
typedef PointerIteratorRange<LightComponent> LightRange;

class TOADLET_API Node:public BaseComponent,public TransformListener{
public:
	TOADLET_NODE(Node);

	Node(Scene *scene=NULL);
	virtual ~Node();
	virtual void destroy();

	virtual void create(Scene *scene);

	void internal_setUniqueHandle(int handle){mUniqueHandle=handle;}
	int getUniqueHandle() const{return mUniqueHandle;}

	void destroyAllChildren();

	virtual void rootChanged(Node *root);

	virtual void notifyParentAttached();
	virtual void notifyParentRemoved();

	virtual Node *getRoot() const{return mRoot;}

	void *getParentData() const{return mParentData;}
	void setParentData(void *parentData){mParentData=parentData;}

	virtual bool attach(Component *component);
	virtual bool remove(Component *component);
	virtual void componentAttached(Component *component);
	virtual void componentRemoved(Component *component);
	virtual ComponentRange getComponents() const{return ComponentRange::wrapCollection(mComponents);}
	virtual Component *getChild(const String &name) const;
	virtual Component *getChild(const Type<Component> *type) const;
	template<typename Type> Type *getChildType() const{return (Type*)getChild(Type::type());}

	virtual void nodeAttached(Node *node);
	virtual void nodeRemoved(Node *node);
	virtual NodeRange getNodes() const{return NodeRange::wrapCollection(mNodes);}
	virtual Node *getNode(const String &name) const;
	virtual Node *getNode(const Type<Component> *type) const;
	template<typename Type> Type *getNodeType() const{return (Type*)getNode(Type::type());}

	// Attached Interface methods
	virtual void actionAttached(ActionComponent *action);
	virtual void actionRemoved(ActionComponent *action);
	virtual Action *getAction(const String &name) const;
	virtual void startAction(const String &name);
	virtual void stopAction(const String &name);
	virtual void stopAllActions();
	virtual bool getActionActive(const String &name) const;

	virtual void spacialAttached(Spacial *spacial);
	virtual void spacialRemoved(Spacial *spacial);
	virtual SpacialRange getSpacials() const{return SpacialRange::wrapCollection(mSpacials);}

	virtual void visibleAttached(Visible *visible);
	virtual void visibleRemoved(Visible *visible);
	virtual VisibleRange getVisibles() const{return VisibleRange::wrapCollection(mVisibles);}
	
	virtual void animatableAttached(Animatable *animatable);
	virtual void animatableRemoved(Animatable *animatable);
	virtual AnimatableRange getAnimatables() const{return AnimatableRange::wrapCollection(mAnimatables);}

	virtual void lightAttached(LightComponent *light);
	virtual void lightRemoved(LightComponent *light);
	virtual LightRange getLights() const{return LightRange::wrapCollection(mLights);}
	
	virtual void physicsAttached(PhysicsComponent *physics);
	virtual void physicsRemoved(PhysicsComponent *physics);
	virtual PhysicsComponent *getPhysics() const{return mPhysics;}

	// Spacial methods
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

	virtual void setMatrix4x4(const Matrix4x4 &matrix);

	virtual void setTransform(Transform::ptr transform);
	inline Transform *getTransform() const{return mTransform;}
	inline Transform *getWorldTransform() const{return mWorldTransform;}

	virtual void setBound(Bound::ptr bound);
	inline Bound *getBound() const{return mBound;}
	inline Bound *getWorldBound() const{return mWorldBound;}

	virtual void setScope(int scope){mScope=scope;}
	inline int getScope() const{return mScope;}
	inline int getWorldScope() const{return mWorldScope;}

	virtual void logicUpdate(int dt,int scope);
	virtual void frameUpdate(int dt,int scope);

	virtual bool handleEvent(Event *event);

	// Activation methods
	virtual void setStayActive(bool active);
	inline bool getStayActive() const{return mDeactivateCount<0;}
	virtual void activate();
	virtual void deactivate();
	virtual void tryDeactivate();
	virtual bool getActive() const{return mActive;}

	virtual void transformChanged(Transform *transform);
	virtual void boundChanged();
	virtual void nodeBoundChanged(Node *node);
	virtual void gatherRenderables(Camera *camera,RenderableSet *set);

	inline Engine *getEngine() const{return (Engine*)mEngine;}
	inline Scene *getScene() const{return (Scene*)mScene;}

protected:
	typedef Collection<Component::ptr> ComponentCollection;
	typedef Collection<Node::ptr> NodeCollection;

	virtual void calculateBound();

	// Engine items
	IntrusivePointer<Engine,ObjectSemantics> mEngine;
	IntrusivePointer<Scene,ObjectSemantics> mScene;
	int mUniqueHandle;
	void *mParentData;

	ComponentCollection mComponents;
	NodeCollection mNodes;
	bool mChildrenActive;
	bool mActivateChildren;

	Collection<ActionComponent*> mActions;
	Collection<Visible*> mVisibles;
	Collection<Animatable*> mAnimatables;
	Collection<LightComponent*> mLights;
	Collection<Spacial*> mSpacials;
	PhysicsComponent *mPhysics;

	bool mActive;
	int mDeactivateCount;

	Transform::ptr mTransform;
	Bound::ptr mBound;
	Transform::ptr mWorldTransform;
	Bound::ptr mWorldBound;
	int mScope;
	int mWorldScope;
};

}
}

#endif
