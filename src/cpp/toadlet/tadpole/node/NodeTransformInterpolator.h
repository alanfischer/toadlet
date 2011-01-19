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

	NodeTransformInterpolator():
		mInterpolate(0),
		mForceInterpolate(0)
	{
		mInterpolate=Node::TransformUpdate_BIT_TRANSLATE|Node::TransformUpdate_BIT_ROTATE|Node::TransformUpdate_BIT_SCALE;

		mTransform=Transform::ptr(new Transform());
		mLastTransform=Transform::ptr(new Transform());
		mLerpedTransform=Transform::ptr(new Transform());
	}

	virtual void transformUpdated(Node *node,int tu){
		if((~mForceInterpolate&tu&Node::TransformUpdate_BIT_TRANSLATE)>0){
			const Vector3 &translate=node->getTranslate();
			mLastTransform->setTranslate(translate);
			mTransform->setTranslate(translate);
		}
		if((~mForceInterpolate&tu&Node::TransformUpdate_BIT_ROTATE)>0){
			const Quaternion &rotate=node->getRotate();
			mLastTransform->setRotate(rotate);
			mTransform->setRotate(rotate);
		}
		if((~mForceInterpolate&tu&Node::TransformUpdate_BIT_SCALE)>0){
			const Vector3 &scale=node->getScale();
			mLastTransform->setScale(scale);
			mTransform->setScale(scale);
		}
	}

	virtual void logicUpdate(Node *node,int dt){
		mLastTransform->set(mTransform);

		mTransform->setTranslate(node->getTranslate());
		mTransform->setRotate(node->getRotate());
		mTransform->setScale(node->getScale());
	}

	virtual void interpolate(Node *node,scalar value){
		if((mInterpolate&Node::TransformUpdate_BIT_TRANSLATE)>0){
			Math::lerp(mTranslateLerp,mLastTransform->getTranslate(),mTransform->getTranslate(),value);
			mLerpedTransform->setTranslate(mTranslateLerp);
		}
		else{
			mLerpedTransform->setTranslate(mLastTransform->getTranslate());
		}

		if((mInterpolate&Node::TransformUpdate_BIT_ROTATE)>0){
			Math::slerp(mRotateLerp,mLastTransform->getRotate(),mTransform->getRotate(),value);
			mLerpedTransform->setRotate(mRotateLerp);
		}
		else{
			mLerpedTransform->setRotate(mLastTransform->getRotate());
		}

		if((mInterpolate&Node::TransformUpdate_BIT_SCALE)>0){
			Math::lerp(mScaleLerp,mLastTransform->getScale(),mTransform->getScale(),value);
			mLerpedTransform->setScale(mScaleLerp);
		}
		else{
			mLerpedTransform->setScale(mLastTransform->getScale());
		}

		node->setTransform(mLerpedTransform,Node::TransformUpdate_BIT_INTERPOLATOR);
	}

	void setBitsToInterpolate(int interpolate){mInterpolate=interpolate;}
	int getBitsToInterpolate() const{return mInterpolate;}

	// These will force interpolation, even when transform parameters are explicity set
	void setBitsToForceInterpolate(int interpolate){mForceInterpolate=interpolate;}
	int getBitsToForceInterpolate() const{return mForceInterpolate;}

protected:
	Transform::ptr mTransform;
	Transform::ptr mLastTransform;

	int mInterpolate;
	int mForceInterpolate;

	Vector3 mTranslateLerp;
	Quaternion mRotateLerp;
	Vector3 mScaleLerp;
	Transform::ptr mLerpedTransform;
};

}
}
}

#endif
