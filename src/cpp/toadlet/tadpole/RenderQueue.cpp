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
}

RenderQueue::~RenderQueue(){
	int i;
	for(i=0;i<mRenderLayers.size();++i){
		if(mRenderLayers[i]!=NULL){
			delete mRenderLayers[i];
		}
	}
}

void RenderQueue::queueRenderable(Renderable *renderable){
	Material *material=renderable->getRenderMaterial();
	int layer=(material==NULL)?0:material->getLayer();
	getRenderLayer(layer)->renderables.add(renderable);
}

void RenderQueue::queueLight(node::LightNode *light){
	// TODO: Find best light
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
