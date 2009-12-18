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
#include <toadlet/egg/io/InputStream.h>
#include <toadlet/tadpole/ResourceHandler.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API ResourceManager:public egg::ResourceFullyReleasedListener{
public:
	ResourceManager(egg::io::Archive *archive);
	virtual ~ResourceManager();

	virtual void destroy();

	virtual void setArchive(egg::io::Archive *archive){mArchive=archive;}
	virtual egg::io::Archive *getArchive() const{return mArchive;}

	virtual void addResourceArchive(egg::io::Archive::ptr archive);
	virtual void removeResourceArchive(egg::io::Archive::ptr archive);

	virtual egg::Resource::ptr get(const egg::String &name);
	virtual egg::Resource::ptr find(const egg::String &name,ResourceHandlerData::ptr handlerData=(int)NULL);
	virtual egg::Resource::ptr manage(const egg::Resource::ptr &resource);

	virtual void unmanage(egg::Resource *resource);

	virtual void setHandler(ResourceHandler::ptr handler,const egg::String &extension);
	virtual ResourceHandler::ptr getHandler(const egg::String &extension);

	virtual void setDefaultHandler(ResourceHandler::ptr handler){mDefaultHandler=handler;}
	virtual ResourceHandler::ptr getDefaultHandler(){return mDefaultHandler;}

	virtual void setDefaultExtension(const egg::String &extension){mDefaultExtension=extension;}
	virtual const egg::String &getDefaultExtension(){return mDefaultExtension;}

	virtual void resourceFullyReleased(egg::Resource *resource);

	static egg::String cleanFilename(const egg::String &name);

protected:
	typedef egg::Map<egg::String,egg::Resource::ptr> NameResourceMap;
	typedef egg::Map<egg::String,ResourceHandler::ptr> ExtensionHandlerMap;

	virtual egg::Resource::ptr unableToFindHandler(const egg::String &name,const ResourceHandlerData *handlerData);
	virtual egg::Resource::ptr findFromFile(const egg::String &name,const ResourceHandlerData *handlerData);

	virtual void resourceLoaded(const egg::Resource::ptr &resource){}
	virtual void resourceUnloaded(const egg::Resource::ptr &resource){}

	egg::io::Archive *mArchive;
	egg::Collection<egg::io::Archive::ptr> mResourceArchives;

	egg::Collection<egg::Resource::ptr> mResources;
	NameResourceMap mNameResourceMap;

	ExtensionHandlerMap mExtensionHandlerMap;
	ResourceHandler::ptr mDefaultHandler;
	egg::String mDefaultExtension;
};

}
}

#endif
