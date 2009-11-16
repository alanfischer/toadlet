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

#ifndef TOADLET_TADPOLE_NODE_RENDERABLENODE_H
#define TOADLET_TADPOLE_NODE_RENDERABLENODE_H

#include <toadlet/tadpole/node/Node.h>

namespace toadlet{
namespace tadpole{
namespace node{

class Scene;

class TOADLET_API RenderableNode:public Node{
public:
	TOADLET_NODE(RenderableNode,Node);

	RenderableNode();
	Node *create(Engine *engine);

	virtual bool isRenderable() const{return true;}

	virtual void setVisible(bool visible);
	inline bool getVisible() const{return mVisible;}

	virtual void queueRenderables(Scene *scene){}

	#if defined(TOADLET_GCC_INHERITANCE_BUG)
		template<typename RenderableNodeType>
		class RenderableWorkaround:public Renderable{
		public:
			RenderableWorkaround(RenderableNodeType *node):renderableNode(node){}
			RenderableNodeType *renderableNode;
			const Material::ptr &getRenderMaterial() const{return renderableNode->getRenderMaterial();}
			const Matrix4x4 &getRenderTransform() const{return renderableNode->getRenderTransform();}
			void render(peeper::Renderer *renderer) const{renderableNode->render(renderer);}
		};
		#define TOADLET_GIB_DEFINE(type) toadlet::tadpole::node::RenderableNode::RenderableWorkaround<type> renderable;
		#define TOADLET_GIB_IMPLEMENT() renderable(this),
	#else
		#define TOADLET_GIB_DEFINE(type)
		#define TOADLET_GIB_IMPLEMENT()
	#endif

protected:
	bool mVisible;

	friend class Scene;
};

}
}
}

#endif
