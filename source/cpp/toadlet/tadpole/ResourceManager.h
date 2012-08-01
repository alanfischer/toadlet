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
#include <toadlet/egg/ResourceDestroyedListener.h>
#include <toadlet/egg/io/Archive.h>
#include <toadlet/egg/io/Stream.h>
#include <toadlet/tadpole/ResourceCreator.h>
#include <toadlet/tadpole/ResourceStreamer.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API ResourceManager:public Object,public ResourceDestroyedListener{
public:
	TOADLET_OBJECT(ResourceManager);

	ResourceManager(Engine *engine);
	virtual ~ResourceManager();

	virtual void destroy();

	virtual void addResourceArchive(Archive::ptr archive);
	virtual void removeResourceArchive(Archive::ptr archive);

	virtual Resource::ptr get(int handle);
	virtual Resource::ptr get(const String &name);
	virtual Resource::ptr find(const String &name,ResourceData *data=NULL);
	virtual Resource::ptr manage(const Resource::ptr &resource,const String &name=(char*)NULL);
	virtual void unmanage(Resource *resource);

	virtual void setStreamer(ResourceStreamer::ptr streamer,const String &extension);
	virtual ResourceStreamer::ptr getStreamer(const String &extension);

	virtual void setDefaultStreamer(ResourceStreamer::ptr streamer);
	virtual ResourceStreamer::ptr getDefaultStreamer(){return mDefaultStreamer;}

	virtual void setDefaultExtension(const String &extension){mDefaultExtension=extension;}
	virtual const String &getDefaultExtension(){return mDefaultExtension;}

	virtual void resourceDestroyed(Resource *resource);

	virtual void logAllResources();

	inline Engine *getEngine(){return mEngine;}

	static String cleanPath(const String &name,bool directory=false);

protected:
	typedef Map<String,Resource*> NameResourceMap;
	typedef Map<String,ResourceStreamer::ptr> ExtensionStreamerMap;

	virtual Resource::ptr unableToFindStreamer(const String &name,ResourceData *data);
	virtual Resource::ptr findFromFile(const String &name,ResourceData *data);

	virtual void resourceLoaded(const Resource::ptr &resource){}
	virtual void resourceUnloaded(const Resource::ptr &resource){}

	Engine *mEngine;
	Collection<Archive::ptr> mResourceArchives;
	int mMaxStreamLength;

	Collection<int> mFreeHandles;
	Collection<Resource*> mResources;
	NameResourceMap mNameResourceMap;

	ExtensionStreamerMap mExtensionStreamerMap;
	ResourceStreamer::ptr mDefaultStreamer;
	String mDefaultExtension;
};

}
}

#endif
