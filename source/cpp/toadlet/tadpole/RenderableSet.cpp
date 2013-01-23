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

#include <toadlet/egg/Log.h>
#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/material/Material.h>

namespace toadlet{
namespace tadpole{

RenderableSet::RenderableSet(Scene *scene):
	mScene(NULL),
	mRoot(NULL),
	//mCamera,
	//mRenderableQueues,
	mRenderableQueueCount(0),
	//mMaterialToQueueIndexMap,
	//mLightQueue,
	//mNodeQueue,
	mGatherNodes(false)
{
	mScene=scene;
}

RenderableSet::~RenderableSet(){
}

void RenderableSet::startQueuing(){
	int i;
	for(i=0;i<mRenderableQueues.size();++i){
		mRenderableQueues[i].clear();
	}
	mRenderableQueueCount=0;
	mMaterialToQueueIndexMap.clear();
	mLayerToQueueIndexMap.clear();
	mLayeredMaterialQueueIndexes.clear();
	mLayeredDepthQueueIndexes.clear();
	mLightQueue.clear();
	mNodeQueue.clear();

	mRoot=mScene->getRoot();
}

void RenderableSet::endQueuing(){
	mRoot=NULL;
}

void RenderableSet::queueNode(Node *node){
	if(mGatherNodes){
		mNodeQueue.add(node);
	}
}

void RenderableSet::queueRenderable(Renderable *renderable){
	Material *material=renderable->getRenderMaterial();
	Transform *transform=renderable->getRenderTransform();
	Bound *bound=renderable->getRenderBound();

	/// @todo: Add a flag to skip this, for shadow calculations
	Vector4 ambient;
	if(mRoot->findAmbientForPoint(ambient,transform->getTranslate())==false){
		ambient.set(mScene->getAmbientColor());
	}

	int layer=material!=NULL?material->getLayer():0;

	if(material!=NULL && material->isDepthSorted()){
		LayerToQueueIndexMap::iterator it=mLayerToQueueIndexMap.find(layer);
		int queueIndex=0;
		if(it!=mLayerToQueueIndexMap.end()){
			queueIndex=it->second;
		}
		else{
			queueIndex=mRenderableQueueCount;
			if(mRenderableQueues.size()<=queueIndex){
				mRenderableQueues.resize(queueIndex+1);
			}
			mRenderableQueueCount++;
			mLayerToQueueIndexMap[layer]=queueIndex;
			int i;
			for(i=0;i<mLayeredDepthQueueIndexes.size();++i){
				int index=mLayeredDepthQueueIndexes[i];
				const RenderableQueue &queue=mRenderableQueues[index];
				int queueLayer=queue[0].material!=NULL?queue[0].material->getLayer():0;
				if(layer<queueLayer) break;
			}
			mLayeredDepthQueueIndexes.insert(i,queueIndex);
		}

		/// @todo: Real sorting algorithm, clean this up
		scalar depth=Math::lengthSquared(bound->getSphere().origin,mCamera->getPosition());
		RenderableQueue &queue=mRenderableQueues[queueIndex];
		int numRenderables=queue.size();
		int i;
		for(i=0;i<numRenderables;++i){
			if(queue[i].material->getLayer()<layer) continue;
			if(i<numRenderables-1 && queue[i+1].material->getLayer()>layer) break;
			if(queue[i].depth<depth) break;
		}
		queue.insert(i,RenderableQueueItem(renderable,material,ambient,depth));
	}
	else{
		MaterialToQueueIndexMap::iterator it=mMaterialToQueueIndexMap.find(material);
		int queueIndex=0;
		if(it!=mMaterialToQueueIndexMap.end()){
			queueIndex=it->second;
		}
		else{
			queueIndex=mRenderableQueueCount;
			if(mRenderableQueues.size()<=queueIndex){
				mRenderableQueues.resize(queueIndex+1);
			}
			mRenderableQueueCount++;
			mMaterialToQueueIndexMap[material]=queueIndex;
			int i;
			for(i=0;i<mLayeredMaterialQueueIndexes.size();++i){
				int index=mLayeredMaterialQueueIndexes[i];
				const RenderableQueue &queue=mRenderableQueues[index];
				int queueLayer=queue[0].material!=NULL?queue[0].material->getLayer():0;
				if(layer<queueLayer) break;
			}
			mLayeredMaterialQueueIndexes.insert(i,queueIndex);
		}

		mRenderableQueues[queueIndex].add(RenderableQueueItem(renderable,material,ambient,0));
	}
}

void RenderableSet::queueLight(LightComponent *light){
	scalar depth=Math::lengthSquared(light->getParent()->getWorldTranslate(),mCamera->getPosition());

	int numLights=mLightQueue.size();
	int i;
	for(i=0;i<numLights;++i){
		if(mLightQueue[i].depth<depth) break;
	}
	mLightQueue.insert(i,LightQueueItem(light,depth));
}

}
}
