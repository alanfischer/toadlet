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
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/SceneRenderer.h>
#include <toadlet/tadpole/RenderListener.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{

SceneRenderer::SceneRenderer(Scene *scene):
	mScene(scene),
	//mRenderableSet,
	mPreviousMaterial(NULL)
{
	mRenderableSet=RenderableSet::ptr(new RenderableSet(scene));
}

SceneRenderer::~SceneRenderer(){
}

void SceneRenderer::renderScene(Renderer *renderer,Node *node,CameraNode *camera){
	gatherRenderables(mRenderableSet,node,camera);
	renderRenderables(mRenderableSet,renderer,camera);
}

void SceneRenderer::gatherRenderables(RenderableSet *set,Node *node,CameraNode *camera){
	RenderListener *listener=mScene->getRenderListener();

	if(listener!=NULL){
		listener->preGatherRenderables(set,node,camera);
	}

	set->setCamera(camera);

	set->startQueuing();
	if(node==mScene->getRoot()){
		mScene->getBackground()->gatherRenderables(camera,set);
	}
	node->gatherRenderables(camera,set);
	set->endQueuing();

	if(listener!=NULL){
		listener->postGatherRenderables(set,node,camera);
	}
}

void SceneRenderer::renderRenderables(RenderableSet *set,Renderer *renderer,CameraNode *camera){
	Matrix4x4 matrix;
	int i,j;

	RenderListener *listener=mScene->getRenderListener();

	if(listener!=NULL){
		listener->preRenderRenderables(set,renderer,camera);
	}

	mPreviousMaterial=NULL;

	setupViewport(camera,renderer);

	int clearFlags=camera->getClearFlags();
	if(clearFlags>0 && !camera->getSkipFirstClear()){
		renderer->setDepthWrite(true);
		renderer->clear(clearFlags,camera->getClearColor());
	}

	renderer->setDefaultStates();
	renderer->setProjectionMatrix(camera->getProjectionMatrix());
	renderer->setViewMatrix(camera->getViewMatrix());
	renderer->setModelMatrix(Math::IDENTITY_MATRIX4X4);

	renderer->setFogState(mScene->getFogState());

	setupLights(set->getLightQueue(),renderer);

	const RenderableSet::IndexCollection &sortedIndexes=set->getLayerSortedQueueIndexes();
	for(j=0;j<sortedIndexes.size();++j){
		const RenderableSet::RenderableQueue &renderableQueue=set->getRenderableQueue(sortedIndexes[j]);
		Material *material=renderableQueue[0].material;

		if(material!=NULL && mPreviousMaterial!=material){
			material->setupRenderer(renderer,mPreviousMaterial);
		}
		mPreviousMaterial=material;

		for(i=0;i<renderableQueue.size();++i){
			const RenderableSet::RenderableQueueItem &item=renderableQueue[i];
			Renderable *renderable=item.renderable;
			renderable->getRenderTransform().getMatrix(matrix);

			renderer->setAmbientColor(item.ambient);
			renderer->setModelMatrix(matrix);
			renderable->render(renderer);
		}

		/// @todo: Replace this specific state setting with a more generic Scene Default Material, that will be reset its specific states
		if(material!=NULL && (material->getStates()&Material::State_FOG)>0){
			renderer->setFogState(mScene->getFogState());
		}
	}

	const RenderableSet::RenderableQueue &renderableQueue=set->getDepthSortedQueue();
	for(i=0;i<renderableQueue.size();++i){
		const RenderableSet::RenderableQueueItem &item=renderableQueue[i];
		Material *material=item.material;
		Renderable *renderable=item.renderable;
		renderable->getRenderTransform().getMatrix(matrix);

		if(material!=NULL && mPreviousMaterial!=material){
			material->setupRenderer(renderer,mPreviousMaterial);
		}
		mPreviousMaterial=material;

		renderer->setAmbientColor(item.ambient);
		renderer->setModelMatrix(matrix);
		renderable->render(renderer);

		/// @todo: Replace this specific state setting with a more generic Scene Default Material, that will be reset its specific states
		if(material!=NULL && (material->getStates()&Material::State_FOG)>0){
			renderer->setFogState(mScene->getFogState());
		}
	}

	if(listener!=NULL){
		listener->postRenderRenderables(set,renderer,camera);
	}
}

void SceneRenderer::setupViewport(CameraNode *camera,Renderer *renderer){
	if(camera->getViewportSet()){
		renderer->setViewport(camera->getViewport());
	}
	else{
		RenderTarget *renderTarget=renderer->getRenderTarget();
		renderer->setViewport(cache_render_viewport.set(0,0,renderTarget->getWidth(),renderTarget->getHeight()));
	}
}

void SceneRenderer::setupLights(const RenderableSet::LightQueue &lightQueue,Renderer *renderer){
	int i;
	int maxLights=renderer->getCapabilitySet().maxLights;
	for(i=0;i<maxLights;++i){
		if(i<lightQueue.size()){
			LightNode *light=lightQueue[i].light;
			renderer->setLightState(i,light->getLightState());
			renderer->setLightEnabled(i,light->getEnabled());
		}
		else{
			renderer->setLightEnabled(i,false);
		}
	}
}

}
}
