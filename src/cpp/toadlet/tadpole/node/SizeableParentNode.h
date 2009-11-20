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

#ifndef TOADLET_TADPOLE_NODE_SIZEABLEPARENTNODE_H
#define TOADLET_TADPOLE_NODE_SIZEABLEPARENTNODE_H

#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/node/Sizeable.h>

namespace toadlet{
namespace tadpole{
namespace node{

class TOADLET_API SizeableParentNode:public ParentNode,public Sizeable{
public:
	TOADLET_NODE(SizeableParentNode,ParentNode);

	enum Layout{
		Layout_NONE=				0,
		Layout_FILL=				1<<0,
		Layout_SHRINK=				1<<1,
		Layout_CHILDREN_VERTICAL=	1<<2,
	};

	SizeableParentNode();
	virtual Node *create(Engine *engine);
	virtual void destroy();

	Sizeable *isSizeable(){return this;}

	void setLayout(int layout);
	int getLayout() const{return mLayout;}

	bool attach(Node *node);
	bool remove(Node *node);

	void setSize(scalar x,scalar y,scalar z);
	void setSize(const Vector3 &size);
	const Vector3 &getSize() const{return mSize;}
	const Vector3 &getDesiredSize();

protected:
	void updateShadowChildren();

	int mLayout;
	Vector3 mSize;
	egg::Collection<Node*> mSizeableChildren;

	friend class Scene;
};

}
}
}

#endif
