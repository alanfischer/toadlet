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

#ifndef TOADLET_TADPOLE_NODEMANAGER_H
#define TOADLET_TADPOLE_NODEMANAGER_H

#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/node/NodeResource.h>

namespace toadlet{
namespace tadpole{

class Engine;

class NodeHandler{
public:
	TOADLET_SHARED_POINTERS(NodeHandler);

	virtual node::NodeResource *load(const egg::String &name,const ResourceHandlerData *handlerData)=0;
};

class TOADLET_API NodeManager:public ResourceManager{
public:
	NodeManager(Engine *engine);

	node::NodeResource::ptr getNodeResource(int handle){return egg::shared_static_cast<node::NodeResource>(ResourceManager::get(handle));}
	node::NodeResource::ptr findNodeResource(const egg::String &name){return egg::shared_static_cast<node::NodeResource>(ResourceManager::find(name));}

	node::Node::ptr createNode(int handle,Scene *scene){return createNode(getNodeResource(handle),scene);}
	node::Node::ptr createNode(const egg::String &name,Scene *scene){return createNode(findNodeResource(name),scene);}
	node::Node::ptr createNode(node::NodeResource *resource,Scene *scene);
	
	void setNodeHandler(NodeHandler::ptr handler,const egg::String &extension);
	NodeHandler::ptr getNodeHandler(const egg::String &extension);

protected:
	typedef egg::Map<egg::String,NodeHandler::ptr> ExtensionNodeHandlerMap;

	egg::Resource::ptr findFromFile(const egg::String &name,const ResourceHandlerData *handlerData);
	NodeHandler::ptr findNodeHandler(const egg::String &extension);

	Engine *mEngine;

	ExtensionNodeHandlerMap mExtensionNodeHandlerMap;
};

}
}

#endif
