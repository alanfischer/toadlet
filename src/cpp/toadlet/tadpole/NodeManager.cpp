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

#include <toadlet/tadpole/NodeManager.h>
#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{

NodeManager::NodeManager(Engine *engine):ResourceManager(engine->getArchiveManager()){
	mEngine=engine;
}

Node::ptr NodeManager::createNode(NodeResource *resource,Scene *scene){
	if(resource!=NULL){
		return resource->getNode()->clone(scene);
	}
	else{
		return NULL;
	}
}

NodeResource::ptr NodeManager::createNodeResource(Node *node,const String &name){
	return shared_static_cast<NodeResource>(manage(NodeResource::ptr(new NodeResource(node)),name));
}

void NodeManager::setNodeHandler(NodeHandler::ptr handler,const String &extension){
	ExtensionNodeHandlerMap::iterator it=mExtensionNodeHandlerMap.find(extension);
	if(it!=mExtensionNodeHandlerMap.end()){
		Logger::debug(Categories::TOADLET_TADPOLE,
			"Removing handler for extension "+extension);

		it->second=NULL;
	}

	if(handler!=NULL){
		Logger::debug(Categories::TOADLET_TADPOLE,
			"Adding handler for extension "+extension);
		mExtensionNodeHandlerMap.add(extension,handler);
	}
}

NodeHandler::ptr NodeManager::getNodeHandler(const String &extension){
	ExtensionNodeHandlerMap::iterator it=mExtensionNodeHandlerMap.find(extension);
	if(it!=mExtensionNodeHandlerMap.end()){
		return it->second;
	}
	else{
		return NULL;
	}
}

Resource::ptr NodeManager::findFromFile(const String &name,const ResourceHandlerData *handlerData){
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

	for(i=0;i<mResourceArchives.size();++i){
		Resource::ptr resource=mResourceArchives[i]->openResource(filename);
		if(resource!=NULL){
			return resource;
		}
	}

	if(extension!=(char*)NULL){
		NodeHandler *handler=getNodeHandler(extension);
		if(handler!=NULL){
			return Resource::ptr(handler->load(name,handlerData));
		}
	}

	return NULL;
}

}
}

