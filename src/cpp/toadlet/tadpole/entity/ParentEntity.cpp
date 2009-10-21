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

#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/entity/ParentEntity.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{
namespace entity{

ParentEntity::ParentEntity():Entity(),
	mShadowChildrenDirty(false)
{
}

Entity *ParentEntity::create(Engine *engine){
	super::create(engine);

	mChildren.clear();
	mShadowChildren.clear();
	mShadowChildrenDirty=false;

	return this;
}

void ParentEntity::destroy(){
	while(mChildren.size()>0){
		mChildren[0]->destroy();
	}

	mShadowChildren.clear();

	super::destroy();
}

void ParentEntity::removeAllEntityDestroyedListeners(){
	super::removeAllEntityDestroyedListeners();

	int i;
	for(i=mChildren.size()-1;i>=0;--i){
		mChildren[i]->removeAllEntityDestroyedListeners();
	}
}

bool ParentEntity::attach(Entity *entity){
	if(entity->destroyed()){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"cannot attach a destroyed entity");
		return false;
	}

	ParentEntity *parent=entity->getParent();
	if(parent!=NULL){
		parent->remove(entity);
	}

	mChildren.add(entity);

	entity->parentChanged(this);

	mShadowChildrenDirty=true;

	return true;
}

bool ParentEntity::remove(Entity *entity){
	Entity::ptr reference(entity); // To make sure that the object is not deleted until we can call parentChanged
	int i;
	for(i=mChildren.size()-1;i>=0;--i){
		if(mChildren[i]==entity)break;
	}
	if(i>=0){
		mChildren.removeAt(i);

		entity->parentChanged(NULL);

		mShadowChildrenDirty=true;

		return true;
	}
	else{
		return false;
	}
}

void ParentEntity::updateShadowChildren(){
	mShadowChildrenDirty=false;

	mShadowChildren.resize(mChildren.size());
	int i;
	for(i=mChildren.size()-1;i>=0;--i){
		mShadowChildren[i]=mChildren[i];
	}
}

}
}
}
