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
	mSceneParameters=SceneParameters::ptr(new SceneParameters());
	mSceneParameters->setScene(scene);
}

SceneRenderer::~SceneRenderer(){
}

void SceneRenderer::renderScene(RenderDevice *device,Node *node,CameraNode *camera){
	mDevice=device;

	mSceneParameters->setCamera(camera);
	gatherRenderables(mRenderableSet,node,camera);
	renderRenderables(mRenderableSet,device,camera);
	mSceneParameters->setCamera(NULL);

	mDevice=NULL;
}

/// @todo: Remember the previous pass to avoid setting duplicate states
void SceneRenderer::setupPass(RenderPass *pass){
	mDevice->setRenderState(pass->getRenderState());
	mDevice->setShaderState(pass->getShaderState());

	MaterialState materialState;
	pass->getRenderState()->getMaterialState(materialState);
	mSceneParameters->setMaterialState(materialState);
	pass->setupRenderVariables(mDevice,Material::Scope_MATERIAL,mSceneParameters);

	int i;
	for(i=0;i<pass->getNumTextures();++i){
		mDevice->setTexture(i,pass->getTexture(i));
	}
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
	int i;
	for(i=0;i<sortedIndexes.size();++i){
		const RenderableSet::RenderableQueue &renderableQueue=set->getRenderableQueue(sortedIndexes[i]);
		Material *material=renderableQueue[0].material;

		if(renderedDepthSorted==false && (material!=NULL && material->getLayer()!=0)){
			renderedDepthSorted=true;
			renderDepthSortedRenderables(set,device,camera,useMaterials);
		}

		renderQueueItems((useMaterials && material!=NULL)?material:NULL,&renderableQueue[0],renderableQueue.size());
	}

	if(renderedDepthSorted==false){
		renderDepthSortedRenderables(set,device,camera,useMaterials);
	}

	if(listener!=NULL){
		listener->postRenderRenderables(set,device,camera);
	}
}

void SceneRenderer::renderDepthSortedRenderables(RenderableSet *set,RenderDevice *device,CameraNode *camera,bool useMaterials){
	int i;
	const RenderableSet::RenderableQueue &renderableQueue=set->getDepthSortedQueue();
	for(i=0;i<renderableQueue.size();++i){
		const RenderableSet::RenderableQueueItem &item=renderableQueue[i];
		Material *material=item.material;
		renderQueueItems((useMaterials && material!=NULL)?material:NULL,&item,1);
	}
}

/// @todo: This needs to be cleaned up, it should set the camera default states after each pass
///  And we should see if the Pass is a Fixed or Shader pass, in which case we either set Fixed states, or setupRenderVariables
///  And then maybe set Fixed states should be moved the pass, like setupRenderVariables
///  And we no longer need to pass device & camera to the above functions
void SceneRenderer::renderQueueItems(Material *material,const RenderableSet::RenderableQueueItem *items,int numItems){
	Matrix4x4 matrix;
	RenderPath *path=(material!=NULL)?material->getBestPath():NULL;
	int numPasses=(path!=NULL?path->getNumPasses():1);
	int i;
	for(i=0;i<numPasses;++i){
		RenderPass *pass=(path!=NULL)?path->getPass(i):NULL;
		if(pass!=NULL){
			setupPass(pass);
		}

		for(i=0;i<numItems;++i){
			const RenderableSet::RenderableQueueItem &item=items[i];
			Renderable *renderable=item.renderable;
			renderable->getRenderTransform().getMatrix(matrix);

			// Fixed states
			mDevice->setAmbientColor(item.ambient);
			mDevice->setMatrix(RenderDevice::MatrixType_MODEL,matrix);

			// Shader states
			if(pass!=NULL){
				mSceneParameters->setRenderable(renderable);
				mSceneParameters->setAmbient(item.ambient);
				pass->setupRenderVariables(mDevice,Material::Scope_RENDERABLE,mSceneParameters);
				mSceneParameters->setRenderable(NULL);
			}

			renderable->render(this);
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

/// @todo: Do this only if the material is fixed function
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
