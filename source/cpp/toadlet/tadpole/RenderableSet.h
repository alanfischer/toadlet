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
#include <toadlet/tadpole/Camera.h>
#include <toadlet/tadpole/LightComponent.h>
#include <toadlet/tadpole/PartitionNode.h>

namespace toadlet{
namespace tadpole{

class Scene;

class TOADLET_API RenderableSet{
public:
	TOADLET_SPTR(RenderableSet);

	class RenderableQueueItem{
	public:
		RenderableQueueItem():
			renderable(NULL),
			material(NULL),
			//ambient,
			depth(0){}
	
		RenderableQueueItem(Renderable *r,material::Material *m,const Vector4 &a,scalar d):
			renderable(r),
			material(m),
			ambient(a),
			depth(d){}

		Renderable *renderable;
		material::Material *material;
		Vector4 ambient;
		scalar depth;
	};

	class LightQueueItem{
	public:
		LightQueueItem():
			light(NULL),
			depth(0){}
	
		LightQueueItem(LightComponent *l,scalar d):
			light(l),
			depth(d){}

		LightComponent *light;
		scalar depth;
	};

	typedef Collection<RenderableQueueItem> RenderableQueue;
	typedef Collection<LightQueueItem> LightQueue;
	typedef Collection<Node*> NodeQueue;
	typedef Map<Material*,int> MaterialToQueueIndexMap;
	typedef Map<int,int> LayerToQueueIndexMap;
	typedef Collection<int> IndexCollection;

	RenderableSet(Scene *scene);
	virtual ~RenderableSet();

	virtual void setCamera(Camera *camera){mCamera=camera;}
	virtual void startQueuing();
	virtual void endQueuing();
	virtual void queueNode(Node *node);
	virtual void queueRenderable(Renderable *renderable);
	virtual void queueLight(LightComponent *light);

	inline const RenderableQueue &getRenderableQueue(int i) const{return mRenderableQueues[i];}
	inline int getNumRenderableQueues() const{return mRenderableQueues.size();}
	inline const MaterialToQueueIndexMap &getMaterialToQueueIndexMap() const{return mMaterialToQueueIndexMap;}
	inline const IndexCollection &getLayeredMaterialQueueIndexes() const{return mLayeredMaterialQueueIndexes;}
	inline const IndexCollection &getLayeredDepthQueueIndexes() const{return mLayeredDepthQueueIndexes;}
	inline const LightQueue &getLightQueue(){return mLightQueue;}
	inline const NodeQueue &getNodeQueue(){return mNodeQueue;}
	inline void setGatherNodes(bool gather){mGatherNodes=gather;}

protected:
	Scene *mScene;
	PartitionNode *mRoot;
	Camera *mCamera;
	Collection<RenderableQueue> mRenderableQueues;
	int mRenderableQueueCount;
	MaterialToQueueIndexMap mMaterialToQueueIndexMap;
	LayerToQueueIndexMap mLayerToQueueIndexMap;
	IndexCollection mLayeredMaterialQueueIndexes;
	IndexCollection mLayeredDepthQueueIndexes;
	LightQueue mLightQueue;
	NodeQueue mNodeQueue;
	bool mGatherNodes;
};

}
}

#endif
