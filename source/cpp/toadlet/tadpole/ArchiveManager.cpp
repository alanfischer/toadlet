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
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/io/Stream.h>
#include <toadlet/egg/io/FileStream.h>
#include <toadlet/tadpole/ArchiveManager.h>

namespace toadlet{
namespace tadpole{

ArchiveManager::ArchiveManager(Engine *engine):ResourceManager(engine){
	mMaxStreamLength=Extents::MAX_INT; // Archive streams are usually not loaded into memory
}

Resource::ptr ArchiveManager::manage(Resource *resource,const String &name){
	Archive *archive=(Archive*)resource;
	mArchives.add(archive);
	return ResourceManager::manage(resource,name);
}

void ArchiveManager::unmanage(Resource *resource){
	Archive *archive=(Archive*)resource;
	mArchives.remove(archive);
	ResourceManager::unmanage(resource);
}

void ArchiveManager::addDirectory(const String &directory){
	mDirectories.add(cleanPath(directory,true));
}

void ArchiveManager::removeDirectory(const String &directory){
	mDirectories.remove(cleanPath(directory,true));
}

void ArchiveManager::pushDirectory(const String &directory){
	mDirectories.add(cleanPath(directory,true));
}

void ArchiveManager::popDirectory(){
	mDirectories.removeAt(mDirectories.size()-1);
}

bool ArchiveManager::openStream(const String &name,StreamRequest *request){
	ArchiveStreamRequest::ptr streamRequest=new ArchiveStreamRequest(this,name,request);
	streamRequest->request();

	return true;
}

ArchiveManager::ArchiveStreamRequest::ArchiveStreamRequest(ArchiveManager *manager,const String &name,StreamRequest *request){
	mManager=manager;
	mName=name;
	mRequest=request;
	mIt=manager->mResources.begin();
}

void ArchiveManager::ArchiveStreamRequest::request(){
	if(mIt!=mManager->mResources.end()){
		Archive *archive=((Archive*)(*mIt));
		if(archive!=NULL){
			bool result=false;
			Exception exception;
			TOADLET_TRY
				result=archive->openStream(mName,this);
			TOADLET_CATCH(Exception ex){exception=ex;}
			if(result==false){
				streamException(exception);
			}
		}
		else{
			streamException(Exception());
		}
	}
	else{
		notFound();
	}
}

void ArchiveManager::ArchiveStreamRequest::notFound(){
	FileStream::ptr fileStream;
	if(System::absolutePath(mName)==false){
		for(int i=0;i<mManager->mDirectories.size();++i){
			fileStream=new FileStream(mManager->mDirectories[i]+mName,FileStream::Open_READ_BINARY);
			if(fileStream->closed()==false){
				break;
			}
		}
	}
	if(fileStream==NULL || fileStream->closed()==true){
		fileStream=new FileStream(mName,FileStream::Open_READ_BINARY);
	}

	if(fileStream!=NULL && fileStream->closed()==false){
		mRequest->streamReady(fileStream);
	}
	else{
		mRequest->streamException(Exception(Errorer::Type_FILE_NOT_FOUND));
	}
}

void ArchiveManager::ArchiveStreamRequest::streamReady(Stream *stream){
	mRequest->streamReady(stream);
}

void ArchiveManager::ArchiveStreamRequest::streamException(const Exception &ex){
	mIt++;
	request();
}

}
}
