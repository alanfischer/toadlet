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
		mNodeQueue.push_back(node);
	}
}

void RenderableSet::queueRenderable(Renderable *renderable){
	Material *material=renderable->getRenderMaterial();
	Bound *bound=renderable->getRenderBound();

	/// @todo: Add a flag to skip this, for shadow calculations
	Vector4 ambient;
	if(mRoot->findAmbientForBound(ambient,bound)==false){
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
			{
				IndexCollection::iterator it;
				for(it=mLayeredDepthQueueIndexes.begin();it!=mLayeredDepthQueueIndexes.end();++it){
					int index=*it;
					Material *material=mRenderableQueues[index][0].material;
					int queueLayer=material!=NULL?material->getLayer():0;
					if(layer<queueLayer) break;
				}
				mLayeredDepthQueueIndexes.insert(it,queueIndex);
			}
		}

		/// @todo: Real sorting algorithm, clean this up
		scalar depth=Math::lengthSquared(bound->getSphere().origin,mCamera->getPosition());
		RenderableQueue &queue=mRenderableQueues[queueIndex];
		{
			RenderableQueue::iterator it;
			for(it=queue.begin();it!=queue.end();++it){
				if(it->material->getLayer()<layer) continue;
				if(it+1!=queue.end() && (it+1)->material->getLayer()>layer) break;
				if(it->depth<depth) break;
			}
			queue.insert(it,RenderableQueueItem(renderable,material,ambient,depth));
		}
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
			{
				IndexCollection::iterator it;
				for(it=mLayeredMaterialQueueIndexes.begin();it!=mLayeredMaterialQueueIndexes.end();++it){
					int index=*it;
					Material *material=mRenderableQueues[index][0].material;
					int queueLayer=material!=NULL?material->getLayer():0;
					if(layer<queueLayer) break;
				}
				mLayeredMaterialQueueIndexes.insert(it,queueIndex);
			}
		}

		mRenderableQueues[queueIndex].push_back(RenderableQueueItem(renderable,material,ambient,0));
	}
}

void RenderableSet::queueLight(LightComponent *light){
	scalar depth=Math::lengthSquared(light->getParent()->getWorldTranslate(),mCamera->getPosition());

	LightQueue::iterator it;
	for(it=mLightQueue.begin();it!=mLightQueue.end();++it){
		if(it->depth<depth) break;
	}
	mLightQueue.insert(it,LightQueueItem(light,depth));
}

}
}
