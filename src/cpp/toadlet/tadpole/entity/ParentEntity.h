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

#ifndef TOADLET_TADPOLE_ENTITY_PARENTENTITY_H
#define TOADLET_TADPOLE_ENTITY_PARENTENTITY_H

#include <toadlet/egg/Collection.h>
#include <toadlet/tadpole/entity/Entity.h>

namespace toadlet{
namespace tadpole{
namespace entity{

class TOADLET_API ParentEntity:public Entity{
public:
	TOADLET_ENTITY(ParentEntity,Entity);

	ParentEntity();
	virtual Entity *create(Engine *engine);
	virtual void destroy();

	virtual bool isParent() const{return true;}

	virtual void removeAllEntityDestroyedListeners();

	virtual bool attach(Entity *entity);
	virtual bool remove(Entity *entity);

	inline int getNumChildren() const{return mChildren.size();}
	inline Entity *getChild(int i) const{return mChildren[i];}

protected:
	void updateShadowChildren();

	egg::Collection<Entity::ptr> mChildren;

	bool mShadowChildrenDirty;
	egg::Collection<Entity::ptr> mShadowChildren;

	friend class Scene;
};

}
}
}

#endif
