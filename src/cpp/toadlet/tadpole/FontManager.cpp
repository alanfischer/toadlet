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

#include <toadlet/tadpole/FontManager.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace tadpole{

FontManager::FontManager(InputStreamFactory *inputStreamFactory):ResourceManager(inputStreamFactory){}

Resource::ptr FontManager::manage(const Resource::ptr &resource){
	if(mResources.contains(resource)==false){
		mResources.add(resource);
		resource->setFullyReleasedListener(this);
	}

	String name=resource->getName();
	if(name!=(char*)NULL){
		mNameResourceMap[name+String(":")+shared_static_cast<Font>(resource)->getPointSize()]=resource;
	}

	return resource;
}

void FontManager::unmanage(Resource *resource){
	mResources.remove(resource);

	String name=resource->getName();
	if(name!=(char*)NULL){
		NameResourceMap::iterator it=mNameResourceMap.find(name+String(":")+((Font*)resource)->getPointSize());
		if(it!=mNameResourceMap.end()){
			mNameResourceMap.erase(it);
		}
	}

	resource->destroy();
}

}
}

