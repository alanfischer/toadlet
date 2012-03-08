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
	virtual ~ArchiveManager();

	Archive::ptr findArchive(const String &name){return shared_static_cast<Archive>(ResourceManager::find(name));}

	// A slight hack, we need to store an iptr to the archive so it isn't automatically released
	Archive::ptr manageArchive(const Archive::ptr &resource,const String &name=(char*)NULL){mArchives.add(resource);return shared_static_cast<Archive>(ResourceManager::manage(resource,name));}
	void unmanageArchive(const Archive::ptr &resource){mArchives.remove(resource);ResourceManager::unmanage(resource);}

	void setDirectory(const String &directory);
	const String &getDirectory() const{return mDirectory;}

	// Searches through all archives for the file
	Stream::ptr openStream(const String &name);
	Resource::ptr openResource(const String &name){return NULL;}

	Collection<String>::ptr getEntries(){return NULL;}

protected:
	String mDirectory;
	Collection<Archive::ptr> mArchives;
};

}
}

#endif
