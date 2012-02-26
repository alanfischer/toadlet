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

#ifndef TOADLET_TADPOLE_ANIMATION_NODEPATHANIMATION_H
#define TOADLET_TADPOLE_ANIMATION_NODEPATHANIMATION_H

#include <toadlet/tadpole/TransformTrack.h>
#include <toadlet/tadpole/animation/Animatable.h>
#include <toadlet/tadpole/node/Node.h>

namespace toadlet{
namespace tadpole{
namespace animation{

class TOADLET_API NodePathAnimation:public Animatable{
public:
	TOADLET_SPTR(NodePathAnimation);

	NodePathAnimation(Node::ptr target);
	virtual ~NodePathAnimation(){}

	void setTarget(Node::ptr target);
	inline Node::ptr getTarget() const{return mTarget;}

	void setTrack(TransformTrack::ptr track);
	inline TransformTrack::ptr getTrack() const{return mTrack;}

	void setLookAt(const Vector3 &lookAt);
	inline const Vector3 &getLookAt() const{return mLookAt;}

	void set(scalar value);

	scalar getMin() const;
	scalar getMax() const;

protected:
	Node::ptr mTarget;
	TransformTrack::ptr mTrack;
	bool mUseLookAt;
	Vector3 mLookAt;
	int mHint;

	Vector3 cache_set_translate;
	Quaternion cache_set_rotate;
	Matrix3x3 cache_set_rotateMatrix;
};

}
}
}

#endif

