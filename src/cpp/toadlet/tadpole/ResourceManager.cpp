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

Resource::ptr ResourceManager::find(const egg::String &name,ResourceHandlerData::ptr handlerData=NULL){
}

Resource::ptr ResourceManager::manage(const egg::Resource::ptr &resource){
	mResources.add(resource);

	String name=resource->getName();
	if(name!=(char*)NULL){
		mNameResourceMap[name]=mResources;
	}

	return resource;
}

void ResourceManager::addHandler(ResourceHandler::ptr handler,const String &extension){
	ExtensionHandlerMap::iterator it=mExtensionHandlerMap.find(extension);
	if(it!=mExtensionHandlerMap.end()){
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_DEBUG,
			"Removing handler for extension "+extension);

		it->second=NULL;
	}

	Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_DEBUG,
		"Adding handler for extension "+extension);
	mExtensionHandlerMap[extension]=handler;
}

void ResourceManager::removeHandler(const String &extension){
	ExtensionHandlerMap::iterator it=mExtensionHandlerMap.find(extension);
	if(it!=mExtensionHandlerMap.end()){
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_DEBUG,
			"Removing handler for extension "+extension);

		it->second=NULL;
		mExtensionHandlerMap.remove(*it);
	}
	else{
		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_DEBUG,
			"Handler not found for extension "+extension);
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

Resource::ptr ResourceManager::load(bool cache,const String &name,const String &file,Resource::ptr resource,ResourceHandlerData::ptr handlerData){
	if(Logger::getInstance()->getMasterCategoryReportingLevel(Categories::TOADLET_TADPOLE)>=Logger::Level_EXCESSIVE){
		String description=cache?"Caching":"Loading";
		description=description+" resource ";
		if(file!=(char*)NULL){
			description=description+file+" ";
		}
		if(name!=(char*)NULL){
			description=description+"as "+name;
		}
		else{
			description=description+"anonymously";
		}

		Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_EXCESSIVE,
			description);
	}

	ResourceCache::ptr resCache;
	if(name!=(char*)NULL){
		ResourceNameMap::iterator it=mResourceNameMap.find(name);
		if(it!=mResourceNameMap.end()){
			resCache=it->second;
		}
	}

	if(resCache!=NULL){
		if(resource!=NULL && resCache->resource.get()!=resource){
			Logger::log(Categories::TOADLET_TADPOLE,Logger::Level_WARNING,
				"Replacing previous resource of same name, previous resource is becoming anonymous");

			resCache->name=String();
			ResourceNameMap::iterator it=mResourceNameMap.find(name);
			if(it!=mResourceNameMap.end()){
				mResourceNameMap.erase(it);
			}
			resCache=NULL;
		}
		else if(cache){
			resCache->count++;
		}
	}

	if(resCache==NULL){
		if(resource==NULL){
			if(file!=(char*)NULL){
				TOADLET_TRY
					resource=loadFromFile(file,handlerData);
				TOADLET_CATCH(const Exception &){
					resource=NULL;
				}
				if(resource==NULL){
					return resource;
				}
			}
			else{
				return resource;
			}
		}
		resCache=ResourceCache::ptr(new ResourceCache(resource,name));
		if(cache){
			resCache->cachedResource=resource;
		}
		if(name!=(char*)NULL){
			mResourceNameMap[name]=resCache;
		}
		mResourcePtrMap[resource]=resCache;

		resourceLoaded(resCache->resource);
	}

	return resCache->resource;
}

Resource::ptr ResourceManager::loadFromFile(const String &name,const ResourceHandlerData *handlerData){
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

Resource *ResourceManager::unableToFindHandler(const String &name,const ResourceHandlerData *handlerData){
	String extension;
	int i=name.rfind('.');
	if(i!=String::npos){
		extension=name.substr(i+1,name.length()).toLower();
	}

	Error::unknown(Categories::TOADLET_TADPOLE,
		"Handler for extension \""+extension+"\" not found");
	return NULL;
}


void ResourceManager::uncache(ResourceCache *resCache){
	if(resCache!=NULL){
		resCache->count--;
		if(resCache->count<=0){
			resCache->cachedResource=NULL;
		}
	}
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

}
}

