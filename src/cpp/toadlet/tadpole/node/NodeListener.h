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

#ifndef TOADLET_TADPOLE_NODE_NODELISTENER_H
#define TOADLET_TADPOLE_NODE_NODELISTENER_H

#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{
namespace node{

class Node;

/// @todo: This class should be changed into a NodeBehavior class.
///  it would be able to listen to most methods in the Node, and either override them or compliment them.
//   Then instead of extending classes, it would be easy to customize a node with Behaviors.
//   This would also make it easier to customize Nodes in a scripting system.
//   Also, a ParticleNode could get it's simulation by a Behavior.
class NodeListener{
public:
	TOADLET_SHARED_POINTERS(NodeListener);

	virtual ~NodeListener(){}

	virtual void nodeDestroyed(Node *node)=0;
	virtual void transformUpdated(Node *node,int tu)=0;
	virtual void logicUpdated(Node *node,int dt)=0;
	virtual void frameUpdated(Node *node,int dt)=0;
};

template<class Type>
class NodeDestroyedFunctor:public NodeListener{
public:
	NodeDestroyedFunctor(Type *obj,void (Type::*func)(void)):mObject(obj),mFunction(func){}

	virtual void nodeDestroyed(Node *node){(mObject->*mFunction)();}
	virtual void transformUpdated(Node *node,int tu){}
	virtual void logicUpdated(Node *node,int dt){}
	virtual void frameUpdated(Node *node,int dt){}

protected:
	Type *mObject;
	void (Type::*mFunction)(void);
};

template<class Type>
class NodeUpdateFunctor:public NodeListener{
public:
	NodeUpdateFunctor(Type *obj,void (Type::*logicFunc)(int),void (Type::*frameFunc)(int)):mObject(obj),mLogicFunction(logicFunc),mFrameFunction(frameFunc){}

	virtual void nodeDestroyed(Node *node){}
	virtual void transformUpdated(Node *node,int tu){}
	virtual void logicUpdated(Node *node,int dt){(mObject->*mLogicFunction)(dt);}
	virtual void frameUpdated(Node *node,int dt){(mObject->*mFrameFunction)(dt);}

protected:
	Type *mObject;
	void (Type::*mLogicFunction)(int);
	void (Type::*mFrameFunction)(int);
};

}
}
}

#endif
