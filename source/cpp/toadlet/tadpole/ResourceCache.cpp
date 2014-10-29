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

#include <toadlet/tadpole/ResourceCache.h>

namespace toadlet{
namespace tadpole{

ResourceCache::ResourceCache():
	mListener(NULL)
{}

void ResourceCache::cacheResource(const String &name,ResourceManager *manager,ResourceData *data){
	mCacheInfos.push_back(CacheInfo(name,manager,data));
}

void ResourceCache::startCaching(){
	if(!mCacheInfos.empty()){
		const CacheInfo &info=mCacheInfos.back();
		info.manager->find(info.name,this,info.data);
	}
	else if(mListener!=NULL){
		mListener->resourceCacheReady(this);
	}
}
	
void ResourceCache::resourceReady(Resource *resource){
	mResources.push_back(resource);
	mCacheInfos.pop_back();
	startCaching();
}

void ResourceCache::resourceException(const Exception &ex){
	resourceReady(NULL);
}

}
}
