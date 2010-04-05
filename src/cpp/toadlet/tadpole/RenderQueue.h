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

#ifndef TOADLET_TADPOLE_RENDERQUEUE_H
#define TOADLET_TADPOLE_RENDERQUEUE_H

#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/LightNode.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API RenderQueue{
public:
	TOADLET_SHARED_POINTERS(RenderQueue);

	class DepthRenderable{
	public:
		DepthRenderable():
			renderable(NULL),
			depth(0){}
	
		DepthRenderable(Renderable *r,scalar d):
			renderable(r),
			depth(d){}

		Renderable *renderable;
		scalar depth;
	};

	class RenderLayer{
	public:
		RenderLayer():
			forceRender(false),
			clearLayer(true){}

		egg::Collection<DepthRenderable> depthSortedRenderables;
		egg::Collection<Renderable*> renderables;

		bool forceRender;
		bool clearLayer;
	};
	
	RenderQueue();
	virtual ~RenderQueue();

	virtual void setCamera(node::CameraNode *camera){mCamera=camera;}
	virtual void queueRenderable(Renderable *renderable);
	virtual void queueLight(node::LightNode *light);
	inline RenderLayer *getRenderLayer(int layer);
	inline const egg::Collection<RenderLayer*> getRenderLayers(){return mRenderLayers;}
	inline node::LightNode *getLight(){return mLight;}

protected:
	node::CameraNode::ptr mCamera;
	egg::Collection<RenderLayer*> mRenderLayers;
	node::LightNode::ptr mLight;
};

}
}

#endif
