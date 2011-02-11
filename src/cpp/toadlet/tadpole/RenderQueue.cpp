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

#include <toadlet/tadpole/RenderQueue.h>
#include <toadlet/tadpole/Material.h>

namespace toadlet{
namespace tadpole{

RenderQueue::RenderQueue(){
	getRenderLayer(0)->forceRender=true; // Always render layer 0
}

RenderQueue::~RenderQueue(){
	int i;
	for(i=0;i<mRenderLayers.size();++i){
		if(mRenderLayers[i]!=NULL){
			delete mRenderLayers[i];
		}
	}
}

void RenderQueue::startQueuing(){
	mLight=NULL;
}

void RenderQueue::endQueuing(){
}

void RenderQueue::queueRenderable(Renderable *renderable){
	Material *material=renderable->getRenderMaterial();
	RenderLayer *layer=getRenderLayer((material==NULL)?0:material->getLayer());
	if(material!=NULL && material->getDepthSorted()){
		/// @todo: Real sorting algorithm, clean this up
		scalar depth=0;
		Bound *bound=renderable->getRenderBound();
		if(bound!=NULL){
			depth=Math::lengthSquared(bound->getSphere().origin,mCamera->getWorldTranslate());
		}
		else{
			depth=Math::lengthSquared(mCamera->getWorldTranslate());
		}
		int numRenderables=layer->depthSortedRenderables.size();
		int i;
		for(i=0;i<numRenderables;++i){
			if(layer->depthSortedRenderables[i].depth<depth) break;
		}
		layer->depthSortedRenderables.insert(i,DepthRenderable(renderable,depth));
	}
	else{
		Material *material=renderable->getRenderMaterial();
		int numRenderables=layer->materialSortedRenderables.size();
		int i;
		for(i=0;i<numRenderables;++i){
			if(layer->materialSortedRenderables[i].material==material) break;
		}
		if(i<numRenderables){
			layer->materialSortedRenderables[i].renderables.add(renderable);
		}
		else{
			layer->materialSortedRenderables.add(MaterialRenderable(renderable,material));
		}
	}
}

void RenderQueue::queueLight(node::LightNode *light){
	/// @todo: Find best light
	mLight=light;
}

RenderQueue::RenderLayer *RenderQueue::getRenderLayer(int layer){
	layer-=Material::MIN_LAYER;
	if(mRenderLayers.size()<=layer){
		mRenderLayers.resize(layer+1,NULL);
	}

	RenderLayer *renderLayer=mRenderLayers[layer];
	if(renderLayer==NULL){
		renderLayer=new RenderLayer();
		mRenderLayers[layer]=renderLayer;
	}
	return renderLayer;
}

}
}
