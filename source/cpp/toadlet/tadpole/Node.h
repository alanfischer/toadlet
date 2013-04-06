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
#include <toadlet/tadpole/Bound.h>
#include <toadlet/tadpole/BaseComponent.h>
#include <toadlet/tadpole/Transformable.h>

#define TOADLET_NODE(Class,TypeName)\
	TOADLET_OBJECT(Class); \
	static toadlet::egg::Type<toadlet::tadpole::Node> *type(){static toadlet::egg::InstantiableType<Class,toadlet::tadpole::Node> thisType(TypeName);return &thisType;} \
	virtual toadlet::egg::Type<toadlet::tadpole::Node> *getType() const{return type();}

namespace toadlet{
namespace tadpole{

class Engine;
class RenderableSet;
class Scene;
class Camera;
class Visible;
class Animatable;
class ActionComponent;
class LightComponent;
class PhysicsComponent;

class TOADLET_API Node:public BaseComponent,public Transformable{
public:
	TOADLET_NODE(Node,"toadlet.tadpole.Node");

	Node(Scene *scene=NULL);
	virtual ~Node();
	virtual void destroy();

	virtual void create(Scene *scene);

	void internal_setUniqueHandle(int handle){mUniqueHandle=handle;}
	int getUniqueHandle() const{return mUniqueHandle;}

	void destroyAllChildren();

	virtual void parentChanged(Node *node);
	virtual void rootChanged(Node *root);

	void *getParentData() const{return mParentData;}
	void setParentData(void *parentData){mParentData=parentData;}

	virtual bool attach(Component *component);
	virtual bool remove(Component *component);
	virtual Component *getChild(const String &name);

	virtual void nodeAttached(Node *node);
	virtual void nodeRemoved(Node *node);
	virtual int getNumNodes() const{return mNodes.size();}
	virtual Node *getNode(int i) const{return mNodes[i];}
	virtual Node *getNode(const String &name);

	// Attached Interface methods
	/// @todo: Replace these with finding Components by Interface, even if that Interface has to extend Component
	virtual void actionAttached(ActionComponent *action);
	virtual void actionRemoved(ActionComponent *action);
	virtual ActionComponent *getAction(const String &name);
	virtual void startAction(const String &name);
	virtual void stopAction(const String &name);
	virtual void stopAllActions();
	virtual bool getActionActive(const String &name);

	virtual void visibleAttached(Visible *visible);
	virtual void visibleRemoved(Visible *visible);
	virtual int getNumVisibles() const{return mVisibles.size();}
	virtual Visible *getVisible(int i){return mVisibles[i];}
	
	virtual void animatableAttached(Animatable *animatable);
	virtual void animatableRemoved(Animatable *animatable);
	virtual int getNumAnimatables() const{return mAnimatables.size();}
	virtual Animatable *getAnimatable(int i){return mAnimatables[i];}

	virtual void lightAttached(LightComponent *light);
	virtual void lightRemoved(LightComponent *light);
	virtual int getNumLights() const{return mLights.size();}
	virtual LightComponent *getLight(int i){return mLights[i];}
	
	virtual void physicsAttached(PhysicsComponent *physics);
	virtual void physicsRemoved(PhysicsComponent *physics);
	virtual PhysicsComponent *getPhysics(){return mPhysics;}

	// Spacial methods
	virtual void mergeWorldBound(Node *child,bool justAttached);

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

	virtual void setTransform(Transform *transform);
	inline Transform *getTransform() const{return mTransform;}
	inline Transform *getWorldTransform() const{return mWorldTransform;}

	virtual void setBound(Bound *bound);
	inline Bound *getBound() const{return mBound;}
	inline Bound *getWorldBound() const{return mWorldBound;}

	virtual void setScope(int scope){mScope=scope;}
	inline int getScope() const{return mScope;}

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
	bool getTransformUpdated();

	virtual void updateTransform();
	virtual void updateComponentBound();
	virtual void updateNodeBound();

	virtual void updateWorldSpacial();

	virtual void spacialUpdated();
	virtual void gatherRenderables(Camera *camera,RenderableSet *set);

	inline Engine *getEngine() const{return (Engine*)mEngine;}
	inline Scene *getScene() const{return (Scene*)mScene;}

protected:
	// Engine items
	IntrusivePointer<Engine,ObjectSemantics> mEngine;
	IntrusivePointer<Scene,ObjectSemantics> mScene;
	int mUniqueHandle;
	void *mParentData;

	Collection<Component::ptr> mComponents;
	Collection<Node::ptr> mNodes;
	bool mChildrenActive;
	bool mActivateChildren;

	Collection<ActionComponent*> mActions;
	Collection<Visible*> mVisibles;
	Collection<Animatable*> mAnimatables;
	Collection<LightComponent*> mLights;
	PhysicsComponent *mPhysics;

	bool mActive;
	int mDeactivateCount;
	int mTransformUpdatedFrame;

	Transform::ptr mTransform;
	Bound::ptr mBound;
	Transform::ptr mWorldTransform;
	Bound::ptr mWorldBound;
	Bound::ptr mComponentBound;
	Bound::ptr mComponentWorldBound;
	int mScope;
};

}
}

#endif
