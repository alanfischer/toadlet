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

class TOADLET_API NodeManager:public ResourceManager{
public:
	NodeManager(Engine *engine);

	NodeResource::ptr getNodeResource(int handle){return shared_static_cast<NodeResource>(ResourceManager::get(handle));}
	NodeResource::ptr findNodeResource(const String &name){return shared_static_cast<NodeResource>(ResourceManager::find(name));}
	NodeResource::ptr createNodeResource(Node *node,const String &name=(char*)NULL);

	Node::ptr createNode(int handle,Scene *scene){return createNode(getNodeResource(handle),scene);}
	Node::ptr createNode(const String &name,Scene *scene){return createNode(findNodeResource(name),scene);}
	Node::ptr createNode(NodeResource *resource,Scene *scene);

protected:
	Engine *mEngine;
};

}
}

#endif
