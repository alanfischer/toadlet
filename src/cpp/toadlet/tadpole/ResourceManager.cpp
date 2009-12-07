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

ResourceManager::ResourceManager(InputStreamFactory *inputStreamFactory){
	mInputStreamFactory=inputStreamFactory;
}

ResourceManager::~ResourceManager(){
	destroy();
}

void ResourceManager::destroy(){
	int i;
	for(i=0;i<mResources.size();++i){
		Resource::ptr resource=mResources[i];
		mResources[i]->setFullyReleasedListener(NULL);
		mResources[i]->destroy();
	}
	mResources.clear();
	mNameResourceMap.clear();
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
		if(Logger::getInstance()->getMasterCategoryReportingLevel(Categories::TOADLET_TADPOLE)>=Logger::Level_EXCESSIVE){
			Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_EXCESSIVE,
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

Resource::ptr ResourceManager::manage(const Resource::ptr &resource){
	if(mResources.contains(resource)==false){
		mResources.add(resource);
		resource->setFullyReleasedListener(this);
	}

	String name=resource->getName();
	if(name!=(char*)NULL){
		mNameResourceMap[name]=resource;
	}

	return resource;
}

void ResourceManager::unmanage(Resource *resource){
	if(mResources.remove(resource)==false){
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_WARNING,"Error unmaning resource, check inheritance heiarchy");
	}

	String name=resource->getName();
	if(name!=(char*)NULL){
		NameResourceMap::iterator it=mNameResourceMap.find(name);
		if(it!=mNameResourceMap.end()){
			mNameResourceMap.erase(it);
		}
	}

	resource->destroy();
}

void ResourceManager::setHandler(ResourceHandler::ptr handler,const String &extension){
	ExtensionHandlerMap::iterator it=mExtensionHandlerMap.find(extension);
	if(it!=mExtensionHandlerMap.end()){
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_DEBUG,
			"Removing handler for extension "+extension);

		it->second=NULL;
	}

	if(handler!=NULL){
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_DEBUG,
			"Adding handler for extension "+extension);
		mExtensionHandlerMap[extension]=handler;
	}
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
		ExtensionHandlerMap::iterator it=mExtensionHandlerMap.find(extension);
		if(it!=mExtensionHandlerMap.end()){
			InputStream::ptr in=mInputStreamFactory->makeInputStream(filename);
			if(in!=NULL){
				return Resource::ptr(it->second->load(in,handlerData));
			}
			else{
				Error::unknown(Categories::TOADLET_TADPOLE,
					"File "+filename+" not found");
				return NULL;
			}
		}
		else if(mDefaultHandler!=NULL){
			InputStream::ptr in=mInputStreamFactory->makeInputStream(filename);
			if(in!=NULL){
				return Resource::ptr(mDefaultHandler->load(in,handlerData));
			}
			else{
				Error::unknown(Categories::TOADLET_TADPOLE,
					"File "+filename+" not found");
				return NULL;
			}
		}
		else{
			return Resource::ptr(unableToFindHandler(name,handlerData));
		}
	}
	else{
		Error::unknown(Categories::TOADLET_TADPOLE,
			"Extension not found on file");
		return NULL;
	}
}

}
}

