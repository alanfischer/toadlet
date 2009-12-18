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

class TOADLET_API ArchiveManager:public ResourceManager,public egg::io::Archive,public egg::BaseResource{
public:
	ArchiveManager();
	virtual ~ArchiveManager();

	void destroy();

	bool isResourceArchive() const{return false;}

	bool open(egg::io::InputStream::ptr stream){return true;}

	egg::io::Archive::ptr findArchive(const egg::String &name){return egg::shared_static_cast<egg::io::Archive>(ResourceManager::find(name));}

	void setDirectory(const egg::String &directory);
	const egg::String &getDirectory() const{return mDirectory;}

	// Searches through all archives for the file
	egg::io::InputStream::ptr openStream(const egg::String &name);
	egg::Resource::ptr openResource(const egg::String &name){return NULL;}

	TOADLET_BASERESOURCE_PASSTHROUGH(egg::io::Archive);

protected:
	egg::String mDirectory;
};

}
}

#endif
