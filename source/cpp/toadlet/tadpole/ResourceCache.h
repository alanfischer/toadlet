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

#ifndef TOADLET_TADPOLE_RESOURCECACHE_H
#define TOADLET_TADPOLE_RESOURCECACHE_H

#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{

class ResourceCache;

class ResourceCacheListener{
public:
	virtual void resourceCacheReady(ResourceCache *cache)=0;
};

class TOADLET_API ResourceCache:public Object,public ResourceRequest{
public:
	TOADLET_IOBJECT(ResourceCache);

	ResourceCache();

	void setListener(ResourceCacheListener *listener){mListener=listener;}
	
	void cacheResource(const String &name,ResourceManager *manager,ResourceData *data=NULL);
	
	void startCaching();
	
	void resourceReady(Resource *resource);
	void resourceException(const Exception &ex);
	void resourceProgress(float progress){}

protected:
	class CacheInfo{
	public:
		CacheInfo(){}
		CacheInfo(const String &name1,ResourceManager::ptr manager1,ResourceData::ptr data1):name(name1),manager(manager1),data(data1){}

		String name;
		ResourceManager::ptr manager;
		ResourceData::ptr data;
	};

	ResourceCacheListener *mListener;
	Collection<CacheInfo> mCacheInfos;
	Collection<Resource::ptr> mResources;
};

}
}

#endif
