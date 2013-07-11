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

	Archive::ptr findArchive(const String &name){return shared_static_cast<Archive>(ResourceManager::find(name));}

	Resource::ptr manage(Resource *resource,const String &name=(char*)NULL);
	void unmanage(Resource *resource);

	void addDirectory(const String &directory);
	void removeDirectory(const String &directory);
	int getNumDirectories() const{return mDirectories.size();}
	String getDirectory(int i) const{return mDirectories[i];}

	// Searches through all archives for the file
	Stream::ptr openStream(const String &name);
	Resource::ptr openResource(const String &name){return NULL;}

	const Collection<String> &getEntries(){return mEntries;}

protected:
	Collection<String> mDirectories;
	Collection<Archive::ptr> mArchives;
	Collection<String> mEntries;
};

}
}

#endif
