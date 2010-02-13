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
#include <toadlet/tadpole/node/NodeDestroyedListener.h>

#ifndef TOADLET_NODE
	#define TOADLET_NODE(Class,SuperClass) \
		typedef SuperClass super; \
		typedef toadlet::egg::Type<Class,toadlet::tadpole::node::Node> ThisType; \
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

namespace node{

class ParentNode;
class Renderable;
class Scene;
class Sizeable;

class TOADLET_API Node{
public:
	TOADLET_NODE(Node,Node);

	Node();
	virtual ~Node();
	virtual Node *create(Engine *engine);
	inline bool created() const{return mCreated;}
	virtual void destroy();

	virtual ParentNode *isParent(){return NULL;}
	virtual Renderable *isRenderable(){return NULL;}
	// TODO: I am still not 100% sure on using the Sizeable interface.
	//  Not only does it seem to conflict with the idea of Scalable nodes,
	//  it also seems like it would be better handled using OrientedBoundingBoxes.
	//  That way the size of a node could be gotten just by getting its OBB, and perhaps nodes would also
	//  offer a method to set the OBB, and just return some error condition if that was not possible.
	virtual Sizeable *isSizeable(){return NULL;}

	virtual void setNodeDestroyedListener(NodeDestroyedListener *listener,bool owns);
	inline NodeDestroyedListener *getNodeDestroyedListener() const{return mNodeDestroyedListener;}
	virtual void removeAllNodeDestroyedListeners();

	virtual void parentChanged(ParentNode *parent);
	ParentNode *getParent() const;

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
	inline const Vector3 &getScale() const{return mScale;}
	inline const Vector3 &getWorldScale() const{return mWorldScale;}

	virtual void setTransform(const Matrix4x4 &transform);

	inline bool isIdentityTransform() const{return mIdentityTransform;}

	virtual void findTransformTo(Matrix4x4 &result,Node *node);

	virtual void handleEvent(const egg::Event::ptr &event){}
	
	virtual void setScope(int scope);
	inline int getScope() const{return mScope;}

	virtual void setName(const egg::String &name);
	inline const egg::String &getName() const{return mName;}
	virtual Node *findNodeByName(const egg::String &name);

	virtual void setCameraAligned(bool aligned);
	inline bool getCameraAligned() const{return mCameraAligned;}

	virtual void setBoundingRadius(scalar boundingRadius);
	inline scalar getBoundingRadius() const{return mBoundingRadius;}

	virtual void setReceiveUpdates(bool receiveUpdates);
	inline bool getReceiveUpdates() const{return mReceiveUpdates;}

	/// Only called if the Node registers itself with the Scene in registerUpdateNode.
	/// Dont forget to call unregisterUpdateNode in its destroy.
	virtual void logicUpdate(int dt){}
	virtual void renderUpdate(int dt){}

	void modified();
	void awake();
	void asleep();

	inline Engine *getEngine() const{return mEngine;}

	inline egg::PointerCounter *getCounter() const{return mCounter;}
	inline const Matrix4x4 &getRenderTransform() const{return mRenderTransform;}
	inline const Matrix4x4 &getWorldRenderTransform() const{return mWorldRenderTransform;}

	inline void internal_setManaged(bool managed){mManaged=managed;}
	inline bool internal_getManaged() const{return mManaged;}

protected:
	void setRenderTransformTranslate(const Vector3 &translate);
	void setRenderTransformRotateScale(const Quaternion &rotate,const Vector3 &scale);

	// Allocation items
	egg::PointerCounter *mCounter;
	bool mManaged;

	// Engine items
	bool mCreated;
	Engine *mEngine;

	// Node items
	NodeDestroyedListener *mNodeDestroyedListener;
	bool mOwnsNodeDestroyedListener;

	Node::ptr mParent;

	bool mIdentityTransform;
	Vector3 mTranslate;
	Quaternion mRotate;
	Vector3 mScale;
	Vector3 mWorldTranslate;
	Quaternion mWorldRotate;
	Vector3 mWorldScale;
	int mScope;
	egg::String mName;
	bool mCameraAligned;
	scalar mBoundingRadius;
	bool mReceiveUpdates;
	int mAwakeCount; // 2=Initially Awoken, 1=Awake, 0=Asleep

	Sphere mLogicWorldBound;
	Sphere mRenderWorldBound;
	Matrix4x4 mRenderTransform;
	Matrix4x4 mWorldRenderTransform;

	Vector3 cache_setRotate_vector;
	Matrix3x3 cache_setTransform_matrix;

	friend class SceneNode;
};

}
}
}

#endif
