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

#include <toadlet/egg/Logger.h>
#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/Material.h>
#include <toadlet/tadpole/Scene.h>

using namespace toadlet::egg;

namespace toadlet{
namespace tadpole{

RenderableSet::RenderableSet(Scene *scene):
	mScene(NULL),
	mRoot(NULL),
	//mCamera,
	//mRenderableQueues,
	mRenderableQueueCount(0)
	//mMaterialToQueueIndexMap,
	//mLightQueue
{
	mScene=scene;
}

RenderableSet::~RenderableSet(){
}

void RenderableSet::startQueuing(){
	if(mRenderableQueues.size()==0){
		mRenderableQueues.resize(1);
	}
	mRenderableQueueCount=1;

	int i;
	for(i=0;i<mRenderableQueues.size();++i){
		mRenderableQueues[i].clear();
	}
	mMaterialToQueueIndexMap.clear();
	mLayerSortedQueueIndexes.clear();
	mLightQueue.clear();

	mRoot=mScene->getRoot();
}

void RenderableSet::endQueuing(){
	mRoot=NULL;
}

void RenderableSet::queueRenderable(Renderable *renderable){
	Material *material=renderable->getRenderMaterial();
	const Transform &transform=renderable->getRenderTransform();
	const Bound &bound=renderable->getRenderBound();

	/// @todo: Add a flag to skip this, for shadow calculations
	Vector4 ambient;
	if(mRoot->findAmbientForPoint(ambient,transform.getTranslate())==false){
		ambient.set(mScene->getAmbientColor());
	}

	if(material!=NULL && material->getDepthSorted()){
		/// @todo: Real sorting algorithm, clean this up
		scalar depth=Math::lengthSquared(bound.getSphere().origin,mCamera->getWorldTranslate());

		RenderableQueue &depthQueue=mRenderableQueues[0];
		int numRenderables=depthQueue.size();
		int i;
		for(i=0;i<numRenderables;++i){
			if(depthQueue[i].depth<depth) break;
		}
		depthQueue.insert(i,RenderableQueueItem(renderable,material,ambient,depth));
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
			int layer=material!=NULL?material->getLayer():0;
			int i;
			for(i=0;i<mLayerSortedQueueIndexes.size();++i){
				int index=mLayerSortedQueueIndexes[i];
				const RenderableQueue &queue=mRenderableQueues[index];
				int queueLayer=queue[0].material!=NULL?queue[0].material->getLayer():0;
				if(layer<queueLayer) break;
			}
			mLayerSortedQueueIndexes.insert(i,queueIndex);
		}
		mRenderableQueues[queueIndex].add(RenderableQueueItem(renderable,material,ambient,0));
	}
}

void RenderableSet::queueLight(node::LightNode *light){
	scalar depth=Math::lengthSquared(light->getWorldTranslate(),mCamera->getWorldTranslate());

	int numLights=mLightQueue.size();
	int i;
	for(i=0;i<numLights;++i){
		if(mLightQueue[i].depth<depth) break;
	}
	mLightQueue.insert(i,LightQueueItem(light,depth));
}

}
}
