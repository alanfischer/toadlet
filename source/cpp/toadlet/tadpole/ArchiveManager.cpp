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

#include <toadlet/egg/System.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/io/Stream.h>
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/tadpole/ArchiveManager.h>

namespace toadlet{
namespace tadpole{

ArchiveManager::ArchiveManager(Engine *engine):ResourceManager(engine){
}

ArchiveManager::~ArchiveManager(){
	destroy();
}

void ArchiveManager::addDirectory(const String &directory){
	mDirectories.add(cleanPath(directory,true));
}

void ArchiveManager::removeDirectory(const String &directory){
	mDirectories.remove(cleanPath(directory,true));
}

Stream::ptr ArchiveManager::openStream(const String &name){
	Stream::ptr stream;

	int i;
	for(i=0;i<mResources.size();++i){
		Archive *archive=(Archive*)mResources[i];
		if(archive!=NULL && (stream=archive->openStream(name))!=NULL){
			break;
		}
	}

	if(stream==NULL){
		FileStream::ptr fileStream;
		if(System::absolutePath(name)==false){
			for(i=0;i<mDirectories.size();++i){
				fileStream=new FileStream(mDirectories[i]+name,FileStream::Open_READ_BINARY);
				if(fileStream->closed()==false){
					break;
				}
			}
		}
		if(fileStream==NULL || fileStream->closed()==true){
			fileStream=new FileStream(name,FileStream::Open_READ_BINARY);
		}
		if(fileStream!=NULL && fileStream->closed()==false){
			stream=fileStream;
		}
	}

	return stream;
}

}
}
