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

#ifndef TOADLET_TADPOLE_RENDERABLESET_H
#define TOADLET_TADPOLE_RENDERABLESET_H

#include <toadlet/tadpole/Renderable.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/LightNode.h>
#include <toadlet/tadpole/node/PartitionNode.h>

namespace toadlet{
namespace tadpole{

class Scene;

class TOADLET_API RenderableSet{
public:
	TOADLET_SHARED_POINTERS(RenderableSet);

	class RenderableQueueItem{
	public:
		RenderableQueueItem():
			renderable(NULL),
			material(NULL),
			//ambient,
			depth(0){}
	
		RenderableQueueItem(Renderable *r,Material *m,const Vector4 &a,scalar d):
			renderable(r),
			material(m),
			ambient(a),
			depth(d){}

		Renderable *renderable;
		Material *material;
		Vector4 ambient;
		scalar depth;
	};

	class LightQueueItem{
	public:
		LightQueueItem():
			light(NULL),
			depth(0){}
	
		LightQueueItem(node::LightNode *l,scalar d):
			light(l),
			depth(d){}

		node::LightNode *light;
		scalar depth;
	};

	typedef egg::Collection<RenderableQueueItem> RenderableQueue;
	typedef egg::Collection<LightQueueItem> LightQueue;
	typedef egg::Map<Material*,int> MaterialToQueueIndexMap;

	RenderableSet(Scene *scene);
	virtual ~RenderableSet();

	virtual void setCamera(node::CameraNode *camera){mCamera=camera;}
	virtual void startQueuing();
	virtual void endQueuing();
	virtual void queueRenderable(Renderable *renderable);
	virtual void queueLight(node::LightNode *light);

	inline const RenderableQueue &getDepthSortedQueue() const{return mRenderableQueues[0];}
	inline const RenderableQueue &getRenderableQueue(int i) const{return mRenderableQueues[i];}
	inline const MaterialToQueueIndexMap &getMaterialToQueueIndexMap() const{return mMaterialToQueueIndexMap;}
	inline const LightQueue &getLightQueue(){return mLightQueue;}

protected:
	Scene *mScene;
	node::PartitionNode *mRoot;
	node::CameraNode::ptr mCamera;
	egg::Collection<RenderableQueue> mRenderableQueues;
	int mRenderableQueueCount;
	MaterialToQueueIndexMap mMaterialToQueueIndexMap;
	LightQueue mLightQueue;
};

}
}

#endif
