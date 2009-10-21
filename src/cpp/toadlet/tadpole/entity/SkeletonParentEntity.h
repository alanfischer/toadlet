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

#ifndef TOADLET_TADPOLE_ENTITY_SKELETONPARENTENTITY_H
#define TOADLET_TADPOLE_ENTITY_SKELETONPARENTENTITY_H

#include <toadlet/tadpole/entity/ParentEntity.h>
#include <toadlet/tadpole/entity/MeshEntitySkeleton.h>

namespace toadlet{
namespace tadpole{
namespace entity{

/// WARNING: This class is not currently logic-safe.  Avoid using the locations of these nodes in logic calculations.
class TOADLET_API SkeletonParentEntity:public ParentEntity{
public:
	TOADLET_ENTITY(SkeletonParentEntity,ParentEntity);

	SkeletonParentEntity();

	virtual void setSkeleton(MeshEntitySkeleton::ptr skeleton);

	virtual bool attach(Entity *entity,int bone);
	virtual bool remove(Entity *entity);

	virtual void visualUpdate(int dt);

protected:
	MeshEntitySkeleton::ptr mSkeleton;

	egg::Collection<int> mChildrenBones;

	friend class Scene;
};

}
}
}

#endif
