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

#include <toadlet/tadpole/node/NodeInterpolator.h>

namespace toadlet{
namespace tadpole{
namespace node{

class TOADLET_API NodeTranslationInterpolator:public NodeInterpolator{
public:
	TOADLET_SHARED_POINTERS(NodeTranslationInterpolator);

	NodeTranslationInterpolator():
		mActive(false)
	{}

	virtual void transformUpdated(Node *node,int transformBits){
		if(mInterpolating==false && transformBits==Node::Transform_BIT_TRANSLATE){
			mLastTranslate.set(node->getTranslate());
			mTranslate.set(node->getTranslate());
		}
	}

	virtual void logicFrame(Node *node,int frame){
		mActive=true;
		mLastTranslate=mTranslate;
		mTranslate=node->getTranslate();
	}

	virtual void interpolate(Node *node,scalar value){
		if(mActive){
			mInterpolating=true;

			Vector3 result;
			Math::lerp(result,mLastTranslate,mTranslate,value);
			// Only call the base setTranslate, so Physics implementations don't get all flustered and continually reactivate & reset their positions
			node->Node::setTranslate(result);

			mInterpolating=false;
		}
	}
	
protected:
	bool mActive;
	bool mInterpolating;
	Vector3 mTranslate;
	Vector3 mLastTranslate;
};

}
}
}

#endif
