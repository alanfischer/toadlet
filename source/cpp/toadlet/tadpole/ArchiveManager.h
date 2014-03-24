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

#ifndef TOADLET_TADPOLE_ARCHIVEMANAGER_H
#define TOADLET_TADPOLE_ARCHIVEMANAGER_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/io/Archive.h>
#include <toadlet/tadpole/ResourceManager.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API ArchiveManager:public ResourceManager{
public:
	TOADLET_OBJECT(ArchiveManager);

	ArchiveManager(Engine *engine);

	Resource::ptr manage(Resource *resource,const String &name=(char*)NULL);
	void unmanage(Resource *resource);

	Archive::ptr getArchive(const String &name){return shared_static_cast<Archive>(get(name));}

	void addDirectory(const String &directory);
	void removeDirectory(const String &directory);
	int getNumDirectories() const{return mDirectories.size();}
	String getDirectory(int i) const{return mDirectories[i];}

	// Searches through all archives for the file
	bool openStream(const String &name,StreamRequest *request);
	bool openResource(const String &name,ResourceRequest *request){return false;}

	const Collection<String> &getEntries(){return mEntries;}

protected:
	class ArchiveStreamRequest:public Object,public StreamRequest{
	public:
		TOADLET_IOBJECT(ArchiveStreamRequest);

		ArchiveStreamRequest(ArchiveManager *manager,const String &name,StreamRequest *request);

		void request();
		void notFound();

		void streamReady(Stream *stream);
		void streamException(const Exception &ex);
		void streamProgress(float progress){}

	protected:
		ArchiveManager *mManager;
		String mName;
		StreamRequest::ptr mRequest;
		Collection<Resource*>::iterator mIt;
	};

	Collection<String> mDirectories;
	Collection<Archive::ptr> mArchives;
	Collection<String> mEntries;
};

}
}

#endif
