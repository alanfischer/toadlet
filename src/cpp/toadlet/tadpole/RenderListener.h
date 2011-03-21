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

#ifndef TOADLET_TADPOLE_RENDERLISTENER_H
#define TOADLET_TADPOLE_RENDERLISTENER_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/node/CameraNode.h>

namespace toadlet{
namespace tadpole{

class RenderableSet;

class RenderListener{
public:
	virtual ~RenderListener(){}

	virtual void preRender(peeper::Renderer *renderer,node::CameraNode *camera,node::Node *node)=0;
	virtual void postRender(peeper::Renderer *renderer,node::CameraNode *camera,node::Node *node)=0;
	virtual void preGatherRenderables(RenderableSet *set,node::Node *node,node::CameraNode *camera)=0;
	virtual void postGatherRenderables(RenderableSet *set,node::Node *node,node::CameraNode *camera)=0;
	virtual void preRenderRenderables(RenderableSet *set,peeper::Renderer *renderer,node::CameraNode *camera)=0;
	virtual void postRenderRenderables(RenderableSet *set,peeper::Renderer *renderer,node::CameraNode *camera)=0;
};

}
}

#endif
