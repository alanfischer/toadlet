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
	mScene(scene),
	mDevice(NULL),
	//mSceneParameters,
	//mRenderableSet,
	mLastPass(NULL),
	mLastRenderState(NULL),
	mLastShaderState(NULL)
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

	RenderTarget *target=camera->getRenderTarget();
	device->setRenderTarget(target!=NULL?target:device->getPrimaryRenderTarget());

	setupViewport(camera,device);

	renderRenderables(mRenderableSet,device,camera);

	mSceneParameters->setCamera(NULL);

	mLastPass=NULL;
	mLastRenderState=NULL;
	mLastShaderState=NULL;
	mDevice=NULL;
}

void SceneRenderer::setupPass(RenderPass *pass){
	mSceneParameters->setRenderPass(pass);

	pass->updateVariables(Material::Scope_MATERIAL,mSceneParameters);

	if(mLastShaderState==NULL || mLastShaderState!=pass->getShaderState()){
		mLastShaderState=pass->getShaderState();
		mDevice->setShaderState(mLastShaderState);
	}
	if(mLastRenderState==NULL || mLastRenderState!=pass->getRenderState()){
		mLastRenderState=pass->getRenderState();
		mDevice->setRenderState(mLastRenderState);
		mSceneParameters->setRenderState(mLastRenderState);
	}

	setupTextures(pass,Material::Scope_MATERIAL,mDevice);
	setupVariableBuffers(pass,Material::Scope_MATERIAL,mDevice);

	mLastPass=pass;
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
			mSceneParameters->setRenderState(camera->getDefaultState());
		}
	}

	setupLights(set->getLightQueue(),device);

	const RenderableSet::IndexCollection &materialQueueIndexes=set->getLayeredMaterialQueueIndexes();
	const RenderableSet::IndexCollection &depthQueueIndexes=set->getLayeredDepthQueueIndexes();
	int depthIndex=0;
	int i;
	for(i=0;i<materialQueueIndexes.size();++i){
		const RenderableSet::RenderableQueue &queue=set->getRenderableQueue(materialQueueIndexes[i]);
		Material *material=queue[0].material;
		int layer=material!=NULL?material->getLayer():0;

		while(depthIndex<depthQueueIndexes.size()){
			const RenderableSet::RenderableQueue &depthQueue=set->getRenderableQueue(depthQueueIndexes[depthIndex]);
			Material *depthMaterial=depthQueue[0].material;
			int depthLayer=depthMaterial!=NULL?depthMaterial->getLayer():0;
			if(depthLayer<layer){
				renderDepthSortedRenderables(depthQueue,useMaterials);
				depthIndex++;
			}
			else{
				break;
			}
		}

		renderQueueItems((useMaterials && material!=NULL)?material:NULL,&queue[0],queue.size());
	}

	while(depthIndex<depthQueueIndexes.size()){
		const RenderableSet::RenderableQueue &depthQueue=set->getRenderableQueue(depthQueueIndexes[depthIndex]);
		renderDepthSortedRenderables(depthQueue,useMaterials);
		depthIndex++;
	}

	if(listener!=NULL){
		listener->postRenderRenderables(set,device,camera);
	}
}

void SceneRenderer::renderDepthSortedRenderables(const RenderableSet::RenderableQueue &queue,bool useMaterials){
	int i;
	for(i=0;i<queue.size();++i){
		const RenderableSet::RenderableQueueItem &item=queue[i];
		Material *material=item.material;
		renderQueueItems((useMaterials && material!=NULL)?material:NULL,&item,1);
	}
}

/// @todo: We should see if the Pass is a Fixed or Shader pass, in which case we either set Fixed states, or setupRenderVariables
///  And then maybe set Fixed states should be moved the pass, like setupRenderVariables
void SceneRenderer::renderQueueItems(Material *material,const RenderableSet::RenderableQueueItem *items,int numItems){
	Matrix4x4 matrix;
	RenderPath *path=(material!=NULL)?material->getBestPath():NULL;
	int numPasses=(path!=NULL?path->getNumPasses():1);
	int i,j;
	for(i=0;i<numPasses;++i){
		RenderPass *pass=(path!=NULL)?path->getPass(i):NULL;
		if(pass!=NULL){
			setupPass(pass);
		}

		for(j=0;j<numItems;++j){
			const RenderableSet::RenderableQueueItem &item=items[j];
			Renderable *renderable=item.renderable;
			renderable->getRenderTransform().getMatrix(matrix);

			// Fixed states
			mDevice->setAmbientColor(item.ambient);
			mDevice->setMatrix(RenderDevice::MatrixType_MODEL,matrix);

			// Shader states
			if(pass!=NULL){
				mSceneParameters->setRenderable(renderable);
				mSceneParameters->setAmbient(item.ambient);

				pass->updateVariables(Material::Scope_RENDERABLE,mSceneParameters);
				setupVariableBuffers(pass,Material::Scope_RENDERABLE,mDevice);
	
				mSceneParameters->setRenderable(NULL);
			}

			renderable->render(this);
		}

		if(mSceneParameters->getCamera()->getDefaultState()!=NULL){
			mDevice->setRenderState(mSceneParameters->getCamera()->getDefaultState());
			mSceneParameters->setRenderState(mSceneParameters->getCamera()->getDefaultState());
			mLastRenderState=NULL;
		}
	}
}

void SceneRenderer::setupViewport(CameraNode *camera,RenderDevice *device){
	Viewport viewport=camera->getViewport();
	if(viewport.x==0 && viewport.y==0 && viewport.width==0 && viewport.height==0){
		viewport.width=device->getRenderTarget()->getWidth();
		viewport.height=device->getRenderTarget()->getHeight();
	}
	device->setViewport(viewport);
	mSceneParameters->setViewport(viewport);
}

/// @todo: Clean this up to handle multiple lights in shader passes
void SceneRenderer::setupLights(const RenderableSet::LightQueue &lightQueue,RenderDevice *device){
	if(lightQueue.size()>0){
		LightNode *light=lightQueue[0].light;

		LightState state;
		light->getLightState(state);
		mSceneParameters->setLightState(state);
	}

	int i;
	int maxLights=mScene->getEngine()->getRenderCaps().maxLights;
	for(i=0;i<maxLights;++i){
		if(i<lightQueue.size()){
			LightNode *light=lightQueue[i].light;

			LightState state;
			light->getLightState(state);
			device->setLightState(i,state);
			device->setLightEnabled(i,light->getEnabled());
		}
		else{
			device->setLightEnabled(i,false);
		}
	}
}

void SceneRenderer::setupTextures(RenderPass *pass,int scope,RenderDevice *device){
	int i,j;
	for(j=0;j<Shader::ShaderType_MAX;++j){
		for(i=0;i<pass->getNumTextures((Shader::ShaderType)j);++i){
			device->setTexture((Shader::ShaderType)j,i,pass->getTexture((Shader::ShaderType)j,i));
		}
		if(mLastPass!=NULL){ /// @todo: Only do this if we're rendering a fixed function pass
			for(;i<mLastPass->getNumTextures((Shader::ShaderType)j);++i){
				device->setTexture((Shader::ShaderType)j,i,NULL);
			}
		}
	}
}

void SceneRenderer::setupVariableBuffers(RenderPass *pass,int scope,RenderDevice *device){
	RenderVariableSet::ptr variables=pass->getVariables();
	if(variables!=NULL){
		int i;
		for(i=0;i<variables->getNumBuffers();++i){
			int bufferScope=variables->getBufferScope(i);
			// If the buffer applies to this scope, and it is the highest bit scope in the buffer
			if((bufferScope&scope)!=0 && (bufferScope&~scope)<=scope){
				device->setBuffer(variables->getBufferShaderType(i),variables->getBufferIndex(i),variables->getBuffer(i));
			}
		}
	}
}

}
}
