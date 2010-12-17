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

#ifndef TOADLET_TADPOLE_RENDERABLE_H
#define TOADLET_TADPOLE_RENDERABLE_H

#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace peeper{

class Renderer;

}
namespace tadpole{

class Material;
class Transform;

class Renderable{
public:
	virtual ~Renderable(){}

	virtual Material *getRenderMaterial() const=0;
	virtual Transform *getRenderTransform() const=0;
	virtual void render(peeper::Renderer *renderer) const=0;
};
	
#if defined(TOADLET_GCC_INHERITANCE_BUG)
template<typename RenderableType>
class RenderableWorkaround:public Renderable{
public:
	RenderableType *renderable;
	RenderableWorkaround(RenderableType *type):renderable(type){}
	Material *getRenderMaterial() const{return renderable->getRenderMaterial();}
	Transform *getRenderTransform() const{return renderable->getRenderTransform();}
	void render(peeper::Renderer *renderer) const{renderable->render(renderer);}
};
#define TOADLET_GIB_DEFINE(type) toadlet::tadpole::RenderableWorkaround<type> renderable;
#define TOADLET_GIB_IMPLEMENT() renderable(this),
#else
#define TOADLET_GIB_DEFINE(type)
#define TOADLET_GIB_IMPLEMENT()
#endif	

}
}

#endif
