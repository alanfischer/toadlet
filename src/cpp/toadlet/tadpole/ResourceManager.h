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
#include <toadlet/egg/Resource.h>
#include <toadlet/egg/ResourceFullyReleasedListener.h>
#include <toadlet/egg/io/Archive.h>
#include <toadlet/egg/io/Stream.h>
#include <toadlet/tadpole/ResourceHandler.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API ResourceManager:public ResourceFullyReleasedListener{
public:
	ResourceManager(Archive *archive);
	virtual ~ResourceManager();

	virtual void destroy();

	virtual void setArchive(Archive *archive){mArchive=archive;}
	virtual Archive *getArchive() const{return mArchive;}

	virtual void addResourceArchive(Archive::ptr archive);
	virtual void removeResourceArchive(Archive::ptr archive);

	virtual Resource::ptr get(int handle);
	virtual Resource::ptr get(const String &name);
	virtual Resource::ptr find(const String &name,ResourceHandlerData::ptr handlerData=ResourceHandlerData::ptr());
	virtual Resource::ptr manage(const Resource::ptr &resource,const String &name=(char*)NULL);
	virtual void unmanage(Resource *resource);

	virtual void setHandler(ResourceHandler::ptr handler,const String &extension);
	virtual ResourceHandler::ptr getHandler(const String &extension);

	virtual void setDefaultHandler(ResourceHandler::ptr handler);
	virtual ResourceHandler::ptr getDefaultHandler(){return mDefaultHandler;}

	virtual void setDefaultExtension(const String &extension){mDefaultExtension=extension;}
	virtual const String &getDefaultExtension(){return mDefaultExtension;}

	virtual void resourceFullyReleased(Resource *resource);

	static String cleanFilename(const String &name);

protected:
	typedef Map<String,Resource::ptr> NameResourceMap;
	typedef Map<String,ResourceHandler::ptr> ExtensionHandlerMap;

	virtual Resource::ptr unableToFindHandler(const String &name,const ResourceHandlerData *handlerData);
	virtual Resource::ptr findFromFile(const String &name,const ResourceHandlerData *handlerData);

	virtual void resourceLoaded(const Resource::ptr &resource){}
	virtual void resourceUnloaded(const Resource::ptr &resource){}

	Archive *mArchive;
	Collection<Archive::ptr> mResourceArchives;

	Collection<int> mFreeHandles;
	Collection<Resource::ptr> mResources;
	NameResourceMap mNameResourceMap;

	ExtensionHandlerMap mExtensionHandlerMap;
	ResourceHandler::ptr mDefaultHandler;
	String mDefaultExtension;
};

}
}

#endif
