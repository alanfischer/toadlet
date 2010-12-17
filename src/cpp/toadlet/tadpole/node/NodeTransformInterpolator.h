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

#ifndef TOADLET_TADPOLE_NODE_NODETRANSLATIONINTERPOLATOR_H
#define TOADLET_TADPOLE_NODE_NODETRANSLATIONINTERPOLATOR_H

#include <toadlet/egg/Logger.h>
#include <toadlet/tadpole/node/NodeInterpolator.h>

namespace toadlet{
namespace tadpole{
namespace node{

class TOADLET_API NodeTransformInterpolator:public NodeInterpolator{
public:
	TOADLET_SHARED_POINTERS(NodeTransformInterpolator);

	NodeTransformInterpolator(){
		mTransform=Transform::ptr(new Transform());
		mLastTransform=Transform::ptr(new Transform());
		mLerpedTransform=Transform::ptr(new Transform());
	}

	virtual void transformUpdated(Node *node){
		mLastTransform->setTranslate(node->getTranslate());
		mLastTransform->setRotate(node->getRotate());
		mLastTransform->setScale(node->getScale());

		mTransform->set(mLastTransform);
	}

	virtual void logicUpdate(Node *node,int dt){
		mLastTransform->set(mTransform);

		mTransform->setTranslate(node->getTranslate());
		mTransform->setRotate(node->getRotate());
		mTransform->setScale(node->getScale());
	}

	virtual void interpolate(Node *node,scalar value){
		Math::lerp(mTranslateLerp,mLastTransform->getTranslate(),mTransform->getTranslate(),value);
		Math::slerp(mRotateLerp,mLastTransform->getRotate(),mTransform->getRotate(),value);
		Math::lerp(mScaleLerp,mLastTransform->getScale(),mTransform->getScale(),value);

		mLerpedTransform->set(mTranslateLerp,mRotateLerp,mScaleLerp);

		node->setTransform(mLerpedTransform,Node::TransformUpdate_INTERPOLATOR);
	}
	
protected:
	Transform::ptr mTransform;
	Transform::ptr mLastTransform;

	Vector3 mTranslateLerp;
	Quaternion mRotateLerp;
	Vector3 mScaleLerp;
	Transform::ptr mLerpedTransform;
};

}
}
}

#endif
