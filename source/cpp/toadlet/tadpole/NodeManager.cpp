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

NodeManager::NodeManager(Engine *engine):ResourceManager(engine){
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
	return shared_static_cast<NodeResource>(manage(new NodeResource(node),name));
}

}
}

