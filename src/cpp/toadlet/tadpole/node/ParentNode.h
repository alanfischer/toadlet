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

#ifndef TOADLET_TADPOLE_NODE_PARENTNODE_H
#define TOADLET_TADPOLE_NODE_PARENTNODE_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/Map.h>
#include <toadlet/tadpole/node/Node.h>

namespace toadlet{
namespace tadpole{
namespace node{

class TOADLET_API ParentNode:public Node{
public:
	TOADLET_NODE(ParentNode,Node);

	ParentNode();
	virtual Node *create(Scene *scene);
	virtual void destroy();
	virtual Node *set(Node *node);

	virtual ParentNode *isParent(){return this;}

	virtual void destroyAllChildren();
	virtual void removeAllNodeListeners();

	virtual bool attach(Node *node);
	virtual void nodeAttached(Node *node){}
	virtual bool remove(Node *node);
	virtual void nodeRemoved(Node *node){}

	inline int getNumChildren() const{return mChildren.size();}
	inline Node *getChild(int i) const{return mChildren[i];}

	virtual void handleEvent(const egg::Event::ptr &event);

	virtual void logicUpdate(int dt,int scope);
	virtual void frameUpdate(int dt,int scope);
	virtual void gatherRenderables(CameraNode *camera,RenderableSet *set);
	virtual void mergeWorldBound(Node *child,bool justAttached);

	virtual void activate();
	virtual void deactivate();
	virtual void tryDeactivate();

protected:
	virtual void updateShadowChildren();

	egg::Collection<Node::ptr> mChildren;
	bool mChildrenActive;
	bool mActivateChildren;

	bool mShadowChildrenDirty;
	egg::Collection<Node::ptr> mShadowChildren;
};

}
}
}

#endif
