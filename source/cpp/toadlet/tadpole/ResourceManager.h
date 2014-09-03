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
#include <toadlet/egg/Mutex.h>
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

	class ImmediateFindRequest:public Object,public ResourceRequest{
	public:
		TOADLET_IOBJECT(ImmediateFindRequest);

		ImmediateFindRequest():mHasException(false){}

		Resource::ptr get() const{return mResource;}
		bool hasException() const{return mHasException;}
		const Exception &getException() const{return mException;}

		void resourceReady(Resource *resource){mResource=resource;}
		void resourceException(const Exception &ex){mHasException=true;mException=ex;}
		void resourceProgress(float progress){}

	protected:
		Resource::ptr mResource;
		bool mHasException;
		Exception mException;
	};

	ResourceManager(Engine *engine);

	virtual void destroy();

	virtual void addResourceArchive(Archive *archive);
	virtual void removeResourceArchive(Archive *archive);

	virtual Resource::ptr get(int handle);
	virtual Resource::ptr get(const String &name);

	virtual bool find(const String &name,ResourceRequest *request,ResourceData *data=NULL);
	// Only returns a resource if it was loaded immediately
	virtual Resource::ptr find(const String &name,ResourceData *data=NULL);

	virtual Resource::ptr manage(Resource *resource,const String &name=(char*)NULL);
	virtual void unmanage(Resource *resource);

	virtual void setStreamer(ResourceStreamer *streamer,const String &extension);
	virtual ResourceStreamer *getStreamer(const String &extension);

	virtual void setDefaultStreamer(ResourceStreamer *streamer);
	virtual ResourceStreamer *getDefaultStreamer(){return mDefaultStreamer;}

	virtual void setDefaultExtension(const String &extension){mDefaultExtension=extension;}
	virtual const String &getDefaultExtension(){return mDefaultExtension;}

	virtual void resourceDestroyed(Resource *resource);

	virtual void logAllResources();

	virtual ResourceData *createResourceData(){return new ResourceData();}

	inline Engine *getEngine(){return mEngine;}

	String checkDefaultExtension(const String &path);
	static String cleanPath(const String &path,bool directory=false);
	static String findExtension(const String &path);

protected:
	class ArchiveResourceRequest:public Object,public ResourceRequest,public StreamRequest{
	public:
		TOADLET_IOBJECT(ArchiveResourceRequest);

		ArchiveResourceRequest(ResourceManager *manager,const String &name,ResourceData *data,ResourceRequest *request);

		void request();
		void notFound();

		void resourceReady(Resource *resource);
		void resourceException(const Exception &ex);
		void resourceProgress(float progress){mRequest->resourceProgress(progress);}

		void streamReady(Stream *stream);
		void streamException(const Exception &ex);
		void streamProgress(float progress){}

	protected:
		ResourceManager::ptr mManager;
		String mName;
		String mFilename;
		ResourceStreamer *mStreamer;
		ResourceRequest::ptr mRequest;
		ResourceData::ptr mData;
		Collection<Archive::ptr>::iterator mIt;
		bool mSearchingArchives;
		bool mPushedTemp;
	};

	typedef Map<String,Resource*> NameResourceMap;
	typedef Map<String,ResourceStreamer::ptr> ExtensionStreamerMap;

	Engine *mEngine;
	Collection<Archive::ptr> mResourceArchives;
	int mMaxStreamLength;

	Collection<int> mFreeHandles;
	Collection<Resource*> mResources;
	NameResourceMap mNameResourceMap;

	ExtensionStreamerMap mExtensionStreamerMap;
	ResourceStreamer::ptr mDefaultStreamer;
	String mDefaultExtension;

	#if defined(TOADLET_THREADSAFE)
		Mutex mMutex;
	#endif
};

}
}

#endif
