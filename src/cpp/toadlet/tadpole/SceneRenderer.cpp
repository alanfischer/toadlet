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
#include <toadlet/peeper/RenderCaps.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/SceneRenderer.h>
#include <toadlet/tadpole/RenderListener.h>

namespace toadlet{
namespace tadpole{

SceneRenderer::SceneRenderer(Scene *scene):
	mScene(scene)
	//mRenderableSet
{
	mRenderableSet=RenderableSet::ptr(new RenderableSet(scene));
}

SceneRenderer::~SceneRenderer(){
}

void SceneRenderer::renderScene(RenderDevice *device,Node *node,CameraNode *camera){
	gatherRenderables(mRenderableSet,node,camera);
	renderRenderables(mRenderableSet,device,camera);
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

void SceneRenderer::renderRenderables(RenderableSet *set,RenderDevice *device,CameraNode *camera,bool useMaterials){
	Matrix4x4 matrix;
	int i,j;

	RenderListener *listener=mScene->getRenderListener();

	if(listener!=NULL){
		listener->preRenderRenderables(set,device,camera);
	}

	setupViewport(camera,device);

	int clearFlags=camera->getClearFlags();
	if(clearFlags!=0 && !camera->getSkipFirstClear()){
		device->clear(clearFlags,camera->getClearColor());
	}

	device->setMatrix(RenderDevice::MatrixType_PROJECTION,camera->getProjectionMatrix());
	device->setMatrix(RenderDevice::MatrixType_VIEW,camera->getViewMatrix());
	device->setMatrix(RenderDevice::MatrixType_MODEL,Math::IDENTITY_MATRIX4X4);

	if(useMaterials){
		device->setDefaultState();
		if(camera->getDefaultState()!=NULL){
			device->setRenderState(camera->getDefaultState());
		}
	}

	setupLights(set->getLightQueue(),device);

	bool renderedDepthSorted=false;
	const RenderableSet::IndexCollection &sortedIndexes=set->getLayerSortedQueueIndexes();
	for(j=0;j<sortedIndexes.size();++j){
		const RenderableSet::RenderableQueue &renderableQueue=set->getRenderableQueue(sortedIndexes[j]);
		Material *material=renderableQueue[0].material;

		if(renderedDepthSorted==false && (material!=NULL && material->getLayer()!=0)){
			renderedDepthSorted=true;
			renderDepthSortedRenderables(set,device,camera,useMaterials);
		}

		if(useMaterials && material!=NULL){
			material->setupRenderDevice(device);
		}

		for(i=0;i<renderableQueue.size();++i){
			const RenderableSet::RenderableQueueItem &item=renderableQueue[i];
			Renderable *renderable=item.renderable;
			renderable->getRenderTransform().getMatrix(matrix);

			device->setAmbientColor(item.ambient);
			device->setMatrix(RenderDevice::MatrixType_MODEL,matrix);
			renderable->render(device);
		}

		if(useMaterials){
			if(camera->getDefaultState()!=NULL){
				device->setRenderState(camera->getDefaultState());
			}
		}
	}

	if(renderedDepthSorted==false){
		renderDepthSortedRenderables(set,device,camera,useMaterials);
	}

	if(listener!=NULL){
		listener->postRenderRenderables(set,device,camera);
	}
}

void SceneRenderer::renderDepthSortedRenderables(RenderableSet *set,RenderDevice *device,CameraNode *camera,bool useMaterials){
	Matrix4x4 matrix;
	int i;
	const RenderableSet::RenderableQueue &renderableQueue=set->getDepthSortedQueue();
	for(i=0;i<renderableQueue.size();++i){
		const RenderableSet::RenderableQueueItem &item=renderableQueue[i];
		Material *material=item.material;
		Renderable *renderable=item.renderable;
		renderable->getRenderTransform().getMatrix(matrix);

		if(useMaterials && material!=NULL){
			material->setupRenderDevice(device);
		}

		device->setAmbientColor(item.ambient);
		device->setMatrix(RenderDevice::MatrixType_MODEL,matrix);
		renderable->render(device);

		if(useMaterials){
			if(camera->getDefaultState()!=NULL){
				device->setRenderState(camera->getDefaultState());
			}
		}
	}
}

void SceneRenderer::setupViewport(CameraNode *camera,RenderDevice *device){
	if(camera->getViewportSet()){
		device->setViewport(camera->getViewport());
	}
	else{
		RenderTarget *renderTarget=device->getRenderTarget();
		device->setViewport(Viewport(0,0,renderTarget->getWidth(),renderTarget->getHeight()));
	}
}

void SceneRenderer::setupLights(const RenderableSet::LightQueue &lightQueue,RenderDevice *device){
	int i;
	int maxLights=mScene->getEngine()->getRenderCaps().maxLights;
	for(i=0;i<maxLights;++i){
		if(i<lightQueue.size()){
			LightNode *light=lightQueue[i].light;
			device->setLightState(i,light->getLightState());
			device->setLightEnabled(i,light->getEnabled());
		}
		else{
			device->setLightEnabled(i,false);
		}
	}
}

}
}
