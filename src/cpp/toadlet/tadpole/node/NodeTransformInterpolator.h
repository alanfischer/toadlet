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

	NodeTransformInterpolator(){}

	virtual void transformUpdated(Node *node){
		mLastTranslate.set(node->getTranslate());
		mLastRotate.set(node->getRotate());

		mTranslate.set(mLastTranslate);
		mRotate.set(mLastRotate);
	}

	virtual void logicUpdate(Node *node,int dt){
		mLastTranslate.set(mTranslate);
		mLastRotate.set(mRotate);

		mTranslate.set(node->getTranslate());
		mRotate.set(node->getRotate());
	}

	virtual void interpolate(Node *node,scalar value){
		Math::lerp(mTranslateLerp,mLastTranslate,mTranslate,value);
		Math::slerp(mRotateLerp,mLastRotate,mRotate,value);

		// Only call the base function, so Physics implementations don't get all flustered and continually reactivate & reset their positions
		node->Node::setTranslate(mTranslateLerp);
		node->Node::setRotate(mRotateLerp);
	}
	
protected:
	Vector3 mTranslate;
	Quaternion mRotate;
	Vector3 mLastTranslate;
	Quaternion mLastRotate;

	Vector3 mTranslateLerp;
	Quaternion mRotateLerp;
};

}
}
}

#endif
