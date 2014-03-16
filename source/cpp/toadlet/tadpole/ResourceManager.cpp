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

#include <toadlet/egg/Extents.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/ResourceManager.h>

namespace toadlet{
namespace tadpole{

ResourceManager::ResourceManager(Engine *engine){
	mEngine=engine;
	mResources.add(NULL); // Handle 0 is always NULL
	#if defined(TOADLET_PLATFORM_ANDROID)
		mMaxStreamLength=256*256*4*2; // Twice the size of a 256x256 32bpp image
	#else
		mMaxStreamLength=Extents::MAX_INT; // No real limit otherwise
	#endif
}

void ResourceManager::destroy(){
	int i;
	for(i=0;i<mResources.size();++i){
		Resource::ptr resource=mResources[i];
		if(resource!=NULL){
			resource->setDestroyedListener(NULL);
			resource->destroy();
		}
	}
	mResources.clear();
	mNameResourceMap.clear();
}

void ResourceManager::addResourceArchive(Archive *archive){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	mResourceArchives.add(archive);

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

void ResourceManager::removeResourceArchive(Archive  *archive){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	mResourceArchives.remove(archive);

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

Resource::ptr ResourceManager::get(int handle){
	Resource::ptr resource;

	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	if(handle>=0 && handle<mResources.size()){
		resource=mResources[handle];
	}

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif

	return resource;
}

Resource::ptr ResourceManager::get(const String &name){
	Resource::ptr resource;

	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	NameResourceMap::iterator it=mNameResourceMap.find(name);
	if(it!=mNameResourceMap.end()){
		resource=it->second;
	}

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif

	return resource;
}

bool ResourceManager::find(const String &name,ResourceRequest *request,ResourceData *data){
	Resource::ptr resource=get(name);
	if(resource!=NULL){
		request->resourceReady(resource);
		return true;
	}
	else{
		ArchiveResourceRequest::ptr resourceRequest=new ArchiveResourceRequest(this,name,data,request);
		resourceRequest->request();
		return true;
	}
}

Resource::ptr ResourceManager::manage(Resource  *resource,const String &name){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

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
		resource->setDestroyedListener(this);
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

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif

	return resource;
}

void ResourceManager::unmanage(Resource *resource){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	if(mResources.contains(resource)==false){
		#if defined(TOADLET_THREADSAFE)
			mMutex.unlock();
		#endif

		Error::unknown(Categories::TOADLET_TADPOLE,
			"error unmanaging resource, check that resource is managed, or check inheritance heiarchy");
		return;
	}
	else{
		int handle=resource->getUniqueHandle();
		if(handle>=0 && mResources[handle]==resource){
			mResources[handle]=NULL;
			mFreeHandles.add(handle);
		}
	}

	if(resource->getName()!=(char*)NULL){
		NameResourceMap::iterator it=mNameResourceMap.find(resource->getName());
		if(it!=mNameResourceMap.end()){
			mNameResourceMap.erase(it);
		}
	}

	resource->setDestroyedListener(NULL);

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

void ResourceManager::setStreamer(ResourceStreamer  *streamer,const String &extension){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	ExtensionStreamerMap::iterator it=mExtensionStreamerMap.find(extension);
	if(it!=mExtensionStreamerMap.end()){
		Log::debug(Categories::TOADLET_TADPOLE,
			"removing streamer for extension "+extension);

		it->second=NULL;
	}

	if(streamer!=NULL){
		Log::debug(Categories::TOADLET_TADPOLE,
			"adding streamer for extension "+extension);
		mExtensionStreamerMap.add(extension,streamer);
	}

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

void ResourceManager::setDefaultStreamer(ResourceStreamer  *streamer){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	Log::debug(Categories::TOADLET_TADPOLE,
		"adding default streamer");

	mDefaultStreamer=streamer;

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

ResourceStreamer *ResourceManager::getStreamer(const String &extension){
	ResourceStreamer *streamer=NULL;

	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	ExtensionStreamerMap::iterator it=mExtensionStreamerMap.find(extension);
	if(it!=mExtensionStreamerMap.end()){
		streamer=it->second;
	}

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif

	return streamer;
}

void ResourceManager::resourceDestroyed(Resource *resource){
	unmanage(resource);
}

void ResourceManager::logAllResources(){
	#if defined(TOADLET_THREADSAFE)
		mMutex.lock();
	#endif

	int i;
	for(i=0;i<mResources.size();++i){
		Log::alert(String("Resource:")+i+" name:"+(mResources[i]!=NULL?mResources[i]->getName():""));
	}

	#if defined(TOADLET_THREADSAFE)
		mMutex.unlock();
	#endif
}

String ResourceManager::checkDefaultExtension(const String &path){
	if(path.rfind('.')==String::npos && mDefaultExtension.length()>0){
		return path+"."+mDefaultExtension;
	}
	else{
		return path;
	}
}

String ResourceManager::cleanPath(const String &path,bool directory){
	char *temp=new char[path.length()+2];
	memcpy(temp,path.c_str(),path.length()+2);

	// Eliminate all ".." from the path
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

	if(directory){
		int len=strlen(temp);
		if(len>0 && temp[len-1]!='/'){
			strcat(temp,"/");
		}
	}

	String cleanPath=temp;

	delete[] temp;

	return cleanPath;
}

String ResourceManager::findExtension(const String &path){
	int i=path.rfind('.');
	if(i!=String::npos){
		return path.substr(i+1,path.length()).toLower();
	}
	else{
		return String();
	}
}

ResourceManager::ArchiveResourceRequest::ArchiveResourceRequest(ResourceManager *manager,const String &name,ResourceData *data,ResourceRequest *request){
	mManager=manager;
	mName=name;
	mData=data;
	if(mData==NULL){
		mData=manager->createResourceData();
	}
	mData->setName(mName);
	mRequest=request;
	mIt=manager->mResourceArchives.begin();
}

void ResourceManager::ArchiveResourceRequest::request(){
	if(mIt!=mManager->mResourceArchives.end()){
		((Archive*)(*mIt))->openResource(mName,this);
	}
	else{
		notFound();
	}
}

void ResourceManager::ArchiveResourceRequest::notFound(){
	mFilename=mManager->checkDefaultExtension(cleanPath(mName));
	String extension=findExtension(mFilename);

	Engine *engine=mManager->mEngine;
	if(extension!=(char*)NULL){
		mStreamer=mManager->getStreamer(extension);
		if(mStreamer==NULL){
			mStreamer=mManager->mDefaultStreamer;
		}
		if(mStreamer!=NULL){
			engine->getArchiveManager()->openStream(mFilename,this);
		}
		else{
			mRequest->resourceException(Error::unknown(Categories::TOADLET_TADPOLE,
				"unable to find streamer for:"+mName,Error::Throw_NO));
			return;
		}
	}
	else{
		mRequest->resourceException(Error::unknown(Categories::TOADLET_TADPOLE,
			"extension not found on file",Error::Throw_NO));
		return;
	}
}

void ResourceManager::ArchiveResourceRequest::resourceReady(Resource *resource){
	resource->setName(mName);
	mManager->manage(resource);

	mRequest->resourceReady(resource);
}

void ResourceManager::ArchiveResourceRequest::resourceException(const Exception &ex){
	if(mIt!=mManager->mResourceArchives.end()){
		// Still archive searching
		mIt++;
		request();
	}
	else{
		// Failed streamer load
		mRequest->resourceException(ex);
	}
}

void ResourceManager::ArchiveResourceRequest::streamReady(Stream *stream){
	if(stream->length()>mManager->mMaxStreamLength){
		stream->close();

		mRequest->resourceException(Error::insufficientMemory(Categories::TOADLET_TADPOLE,
			"stream length too large, increase ResourceManager max stream length or reduce resource size",Error::Throw_NO));
		return;
	}

	Engine *engine=mManager->mEngine;
	String tempPath;
	int slash=mFilename.rfind('/');
	if(slash>0){
		tempPath=mFilename.substr(0,slash);
		if(engine->getArchiveManager()->getNumDirectories()>0){
			tempPath=engine->getArchiveManager()->getDirectory(0)+tempPath;
		}
	}

	if(tempPath.length()>0){
		Log::warning("no longer adding temp paths, needs to be cleaned");
//		engine->getArchiveManager()->addDirectory(tempPath);
	}

	mStreamer->load(stream,mData,this);

	if(tempPath.length()>0){
//		engine->getArchiveManager()->removeDirectory(tempPath);
	}
}

void ArchiveManager::ArchiveResourceRequest::streamException(const Exception &ex){
	mRequest->resourceException(ex);
}

}
}

