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

#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <string.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace tadpole{

ResourceManager::ResourceManager(Archive *archive){
	mArchive=archive;
	mResources.resize(1); // Handle 0 is always NULL
}

ResourceManager::~ResourceManager(){
	destroy();
}

void ResourceManager::destroy(){
	int i;
	for(i=0;i<mResources.size();++i){
		Resource::ptr resource=mResources[i];
		if(resource!=NULL){
			resource->setFullyReleasedListener(NULL);
			resource->destroy();
		}
	}
	mResources.clear();
	mNameResourceMap.clear();
}

void ResourceManager::addResourceArchive(egg::io::Archive::ptr archive){
	mResourceArchives.add(archive);
}

void ResourceManager::removeResourceArchive(egg::io::Archive::ptr archive){
	mResourceArchives.remove(archive);
}

Resource::ptr ResourceManager::get(const String &name){
	NameResourceMap::iterator it=mNameResourceMap.find(name);
	if(it!=mNameResourceMap.end()){
		return it->second;
	}
	else{
		return NULL;
	}
}

Resource::ptr ResourceManager::find(const egg::String &name,ResourceHandlerData::ptr handlerData){
	Resource::ptr resource=get(name);
	if(resource==NULL){
		if(Logger::getInstance()->getMasterCategoryReportingLevel(Categories::TOADLET_TADPOLE)>=Logger::Level_EXCESS){
			Logger::excess(Categories::TOADLET_TADPOLE,
				String("Finding ")+name);
		}

		TOADLET_TRY
			resource=findFromFile(name,handlerData);
		TOADLET_CATCH(const Exception &){resource=NULL;}
		if(resource!=NULL){
			resource->setName(name);
			manage(resource);
		}
	}

	return resource;
}

Resource::ptr ResourceManager::manage(const Resource::ptr &resource,const String &name){
	if(mResources.contains(resource)==false){
		int handle=-1;
		int size=mFreeHandles.size();
		if(size>0){
			handle=mFreeHandles.at(size-1);
			mFreeHandles.removeAt(size-1);
		}
		else{
			handle=mResources.size();
			mResources.resize(handle+1);
		}

		mResources[handle]=resource;
		resource->internal_setUniqueHandle(handle);
		resource->setFullyReleasedListener(this);
	}
	else{
		if(resource->getName()!=(char*)NULL){
			NameResourceMap::iterator it=mNameResourceMap.find(resource->getName());
			if(it!=mNameResourceMap.end()){
				mNameResourceMap.erase(it);
			}
		}
	}

	if(name!=(char*)NULL){
		resource->setName(name);
	}

	if(resource->getName()!=(char*)NULL){
		mNameResourceMap.add(resource->getName(),resource);
	}

	return resource;
}

void ResourceManager::unmanage(Resource *resource){
	if(mResources.contains(resource)==false){
		Logger::alert(Categories::TOADLET_TADPOLE,
			"Error unmanaging resource, check inheritance heiarchy");
	}
	else{
		int handle=resource->getUniqueHandle();
		if(handle>=0){
			mResources[handle]=NULL;
			mFreeHandles.add(handle);
			resource->internal_setUniqueHandle(0);
		}
	}

	if(resource->getName()!=(char*)NULL){
		NameResourceMap::iterator it=mNameResourceMap.find(resource->getName());
		if(it!=mNameResourceMap.end()){
			mNameResourceMap.erase(it);
		}
	}

	resource->destroy();
}

Resource::ptr ResourceManager::getByHandle(int handle){
	if(handle>=0 && handle<mResources.size()){
		return mResources[handle];
	}
	else{
		return NULL;
	}
}

void ResourceManager::setHandler(ResourceHandler::ptr handler,const String &extension){
	ExtensionHandlerMap::iterator it=mExtensionHandlerMap.find(extension);
	if(it!=mExtensionHandlerMap.end()){
		Logger::debug(Categories::TOADLET_TADPOLE,
			"Removing handler for extension "+extension);

		it->second=NULL;
	}

	if(handler!=NULL){
		Logger::debug(Categories::TOADLET_TADPOLE,
			"Adding handler for extension "+extension);
		mExtensionHandlerMap.add(extension,handler);
	}
}

void ResourceManager::setDefaultHandler(ResourceHandler::ptr handler){
	Logger::debug(Categories::TOADLET_TADPOLE,
		"Adding default handler");

	mDefaultHandler=handler;
}

ResourceHandler::ptr ResourceManager::getHandler(const String &extension){
	ExtensionHandlerMap::iterator it=mExtensionHandlerMap.find(extension);
	if(it!=mExtensionHandlerMap.end()){
		return it->second;
	}
	else{
		return NULL;
	}
}

ResourceHandler::ptr ResourceManager::findHandler(const String &extension){
	ExtensionHandlerMap::iterator it=mExtensionHandlerMap.find(extension);
	if(it!=mExtensionHandlerMap.end()){
		return it->second;
	}
	else{
		return mDefaultHandler;
	}
}

void ResourceManager::resourceFullyReleased(Resource *resource){
	unmanage(resource);
}

String ResourceManager::cleanFilename(const String &name){
	char *temp=new char[name.length()+1];
	memcpy(temp,name.c_str(),name.length()+1);

	// Eliminate all ".." from the name
	char *p=0;
	while((p=strstr(temp,".."))!=0){
		char *p2=p;
		while(*p2!='/' && p2>=temp){
			p2--;
		}

		char *p3=p2-1;
		while(*p3!='/' && p3>=temp){
			p3--;
		}

		if(p2>=temp && p3>=temp){
			*p3=0;
			strcpy(p3,p+2);
		}
		else{
			break;
		}
	}

	// Remove any starting ./'s
	if(strncmp(temp,"./",2)==0 || strncmp(temp,".\\",2)==0){
		int len=strlen(temp+2)+1;
		memmove(temp,temp+2,len);
	}

	// Replace any \ with /
	for(p=temp;(*p)!=0;p++){
		if((*p)=='\\'){
			(*p)='/';
		}
	}

	String cleanName=temp;

	delete[] temp;

	return cleanName;
}

Resource::ptr ResourceManager::unableToFindHandler(const String &name,const ResourceHandlerData *handlerData){
	Error::unknown(Categories::TOADLET_TADPOLE,
		"Handler for \""+name+"\" not found");
	return NULL;
}

Resource::ptr ResourceManager::findFromFile(const String &name,const ResourceHandlerData *handlerData){
	String filename=cleanFilename(name);

	String extension;
	int i=filename.rfind('.');
	if(i!=String::npos){
		extension=filename.substr(i+1,filename.length()).toLower();
	}
	else if(mDefaultExtension.length()>0){
		extension=mDefaultExtension;
		filename+="."+extension;
	}
	if(extension!=(char*)NULL){
		int i;
		for(i=0;i<mResourceArchives.size();++i){
			Resource::ptr resource=mResourceArchives[i]->openResource(filename);
			if(resource!=NULL){
				return resource;
			}
		}

		ResourceHandler *handler=findHandler(extension);
		if(handler!=NULL){
			Stream::ptr stream=mArchive->openStream(filename);
			if(stream!=NULL){
				return Resource::ptr(handler->load(stream,handlerData));
			}
			else{
				Error::unknown(Categories::TOADLET_TADPOLE,
					"File "+filename+" not found");
				return NULL;
			}
		}

		return Resource::ptr(unableToFindHandler(name,handlerData));
	}
	else{
		int i;
		for(i=0;i<mResourceArchives.size();++i){
			Resource::ptr resource=mResourceArchives[i]->openResource(filename);
			if(resource!=NULL){
				return resource;
			}
		}

		Error::unknown(Categories::TOADLET_TADPOLE,
			"Extension not found on file");
		return NULL;
	}
}

}
}

