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

#ifndef TOADLET_TADPOLE_NODE_NODEATTACHER_H
#define TOADLET_TADPOLE_NODE_NODEATTACHER_H

#include <toadlet/tadpole/node/Node.h>
#include <toadlet/tadpole/Attachable.h>

namespace toadlet{
namespace tadpole{
namespace node{

// This currently requires that the attached Node have the same parent as the Attachable,
//  due to it just using the LocalTransform of the Attachable Node
class NodeAttacher:public NodeListener{
public:
	TOADLET_SHARED_POINTERS(NodeAttacher);

	NodeAttacher(Node::ptr node,Attachable *attachable,int index){
		mNode=node;
		mAttachable=attachable;
		mIndex=index;
		mTransform=Transform::ptr(new Transform());
	}
	
	void nodeDestroyed(Node *node){}
	void logicUpdate(Node *node,int dt){updateAttachment(node);}
	void frameUpdate(Node *node,int dt){updateAttachment(node);}
	
protected:
	void updateAttachment(Node *node){
		mAttachable->getAttachmentTransform(mTransform,mIndex);
		node->setTransform(mTransform,0);
	}

	Node::ptr mNode;
	Attachable *mAttachable;
	int mIndex;
	Transform::ptr mTransform;
};

}
}
}

#endif
