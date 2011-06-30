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

#ifndef TOADLET_TADPOLE_ANIMATION_SKELETONANIMATION_H
#define TOADLET_TADPOLE_ANIMATION_SKELETONANIMATION_H

#include <toadlet/tadpole/animation/Animatable.h>
#include <toadlet/tadpole/node/MeshNodeSkeleton.h>

namespace toadlet{
namespace tadpole{
namespace animation{

class Controller;

class TOADLET_API SkeletonAnimation:public Animatable{
public:
	TOADLET_SHARED_POINTERS(SkeletonAnimation);

	SkeletonAnimation();
	virtual ~SkeletonAnimation();

	void setTarget(MeshNodeSkeleton::ptr target);
	inline MeshNodeSkeleton::ptr getTarget() const{return mTarget;}

	void setSequenceIndex(int sequenceIndex);
	inline int getSequenceIndex() const{return mSequenceIndex;}

	void set(scalar value);

	scalar getMin() const;
	scalar getMax() const;

	void attached(Controller *controller);
	void removed(Controller *controller);

protected:
	Controller *mController;
	MeshNodeSkeleton::ptr mTarget;
	int mSequenceIndex;
};

}
}
}

#endif
