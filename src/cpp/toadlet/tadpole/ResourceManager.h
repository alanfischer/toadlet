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

#ifndef TOADLET_TADPOLE_RESOURCEMANAGER_H
#define TOADLET_TADPOLE_RESOURCEMANAGER_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/egg/Map.h>
#include <toadlet/egg/io/InputStream.h>
#include <toadlet/egg/io/OutputStream.h>
#include <toadlet/egg/io/InputStreamFactory.h>
#include <toadlet/tadpole/ResourceHandler.h>

namespace toadlet{
namespace tadpole{

//  There are 2 basic ways of using resource management.
//   1. cacheResource, and use it as you see fit, and when you are finished, uncacheResource
//   2. loadResource, and when the sharedPointer goes out of scope, it will be destroyed.
//  Also, context resources have Peers, and these resources use a mapping of Resource<->Peer, combined
//   with a PointerQueue callback for the WeakPointer associated with the Resource, so the Peer can
//   be inserted into a queue to unload from the context
class TOADLET_API ResourceManager{
public:
	class ResourceCache{
	public:
		TOADLET_SHARED_POINTERS(ResourceCache);

		ResourceCache(egg::Resource::wptr resource,const egg::String &name){
			this->resource=resource;
			this->name=name;
			count=1;
		}

		egg::Resource::wptr resource;
		egg::Resource::ptr cachedResource;
		egg::String name;
		int count;
	};

	typedef egg::Map<egg::Resource*,ResourceCache::ptr> ResourcePtrMap;
	typedef egg::Map<egg::String,ResourceCache::ptr> ResourceNameMap;

	ResourceManager(egg::io::InputStreamFactory *inputStreamFactory);
	virtual ~ResourceManager();

	void setInputStreamFactory(egg::io::InputStreamFactory *inputStreamFactory);

	egg::Resource::ptr load(const egg::String &name);
	egg::Resource::ptr load(const egg::String &name,const egg::String &file);
	egg::Resource::ptr load(const egg::String &name,ResourceHandlerData::ptr handlerData);
	egg::Resource::ptr load(const egg::String &name,const egg::String &file,ResourceHandlerData::ptr handlerData);
	egg::Resource::ptr load(const egg::String &name,const egg::Resource::ptr &resource);
	egg::Resource::ptr load(const egg::Resource::ptr &resource);

	egg::Resource::ptr cache(const egg::String &name);
	egg::Resource::ptr cache(const egg::String &name,const egg::String &file);
	egg::Resource::ptr cache(const egg::String &name,ResourceHandlerData::ptr handlerData);
	egg::Resource::ptr cache(const egg::String &name,const egg::String &file,ResourceHandlerData::ptr handlerData);
	egg::Resource::ptr cache(const egg::String &name,const egg::Resource::ptr &resource);
	egg::Resource::ptr cache(const egg::Resource::ptr &resource);

	bool uncache(const egg::String &name);
	bool uncache(const egg::Resource::ptr &resource);

	egg::Resource::ptr get(const egg::String &name);

	virtual void resourceLoaded(const egg::Resource::ptr &resource);

	virtual void resourceUnloaded(egg::Resource *resource);

	virtual void addToPointerQueue(egg::Resource *resource);

	inline ResourcePtrMap &getResourcePtrMap(){return mResourcePtrMap;}

	static egg::String cleanFilename(const egg::String &name);

	void addHandler(ResourceHandler::ptr handler,const egg::String &extension);

	void removeHandler(const egg::String &extension);

	ResourceHandler::ptr getHandler(const egg::String &extension);

	void setDefaultHandler(ResourceHandler::ptr handler);
	ResourceHandler::ptr getDefaultHandler();

	void setDefaultExtension(const egg::String &extension);

protected:
	typedef egg::Map<egg::String,ResourceHandler::ptr> ExtensionHandlerMap;

	virtual egg::Resource::ptr load(bool cache,const egg::String &name,const egg::String &file,egg::Resource::ptr resource,ResourceHandlerData::ptr handlerData);

	virtual egg::Resource::ptr loadFromFile(const egg::String &name,const ResourceHandlerData *handlerData);

	virtual egg::Resource *unableToFindHandler(const egg::String &name,const ResourceHandlerData *handlerData);
	
	void uncache(ResourceCache *resCache);

	ResourcePtrMap mResourcePtrMap;
	ResourceNameMap mResourceNameMap;

	egg::io::InputStreamFactory *mInputStreamFactory;
	ExtensionHandlerMap mExtensionHandlerMap;
	ResourceHandler::ptr mDefaultHandler;
	egg::String mDefaultExtension;
};

}
}

#endif
