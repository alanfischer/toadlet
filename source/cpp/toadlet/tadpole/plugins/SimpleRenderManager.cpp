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
#include <toadlet/peeper/RenderCaps.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include "SimpleRenderManager.h"

namespace toadlet{
namespace tadpole{

SimpleRenderManager::SimpleRenderManager(Scene *scene):
	mScene(scene),
	mDevice(NULL),
	//mSceneParameters,
	//mRenderableSet,
	mLastPass(NULL),
	mLastRenderState(NULL),
	mLastShaderState(NULL)
{
	mRenderableSet=RenderableSet::ptr(new RenderableSet(scene));
	mSceneParameters=new SceneParameters();
	mSceneParameters->setScene(scene);
}

SimpleRenderManager::~SimpleRenderManager(){
}

void SimpleRenderManager::renderScene(RenderDevice *device,Node *node,Camera *camera){
	TOADLET_PROFILE_AUTOSCOPE();

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

void SimpleRenderManager::setupPass(RenderPass *pass){
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

void SimpleRenderManager::setupPassForRenderable(RenderPass *pass,Renderable *renderable,const Vector4 &ambient){
	Matrix4x4 matrix;
	renderable->getRenderTransform().getMatrix(matrix);
	Camera *camera=mSceneParameters->getCamera();

	// This should exist as a RenderVariable, but to support fixed devices it is calculated here
	int flags=pass->getModelMatrixFlags();
	if(flags!=0){
		Vector3 translate;
		Quaternion rotate;
		Vector3 scale;
		Math::setTranslateFromMatrix4x4(translate,matrix);
		Math::setQuaternionFromMatrix4x4(rotate,matrix);
		Math::setScaleFromMatrix4x4(scale,matrix);

		if((flags&Material::MatrixFlag_CAMERA_ALIGNED)!=0){
			if(mSceneParameters->getCamera()->getAlignmentCalculationsUseOrigin()){
				Matrix4x4 lookAtCamera;
				Math::setMatrix4x4FromLookAt(lookAtCamera,camera->getPosition(),translate,Math::Z_UNIT_VECTOR3,false);
				Math::setQuaternionFromMatrix4x4(rotate,lookAtCamera);
			}
			else{
				Math::setQuaternionFromMatrix4x4(rotate,camera->getViewMatrix());
			}
			Math::invert(rotate);
		}

		if((flags&Material::MatrixFlag_NO_PERSPECTIVE)!=0){
			Vector4 point;
			point.set(translate,Math::ONE);
			Math::mul(point,camera->getViewMatrix());
			Math::mul(point,camera->getProjectionMatrix());
			Math::mul(scale,point.w);
		}

		Math::setMatrix4x4FromTranslateRotateScale(matrix,translate,rotate,scale);
	}

	// Fixed states
	mDevice->setMatrix(RenderDevice::MatrixType_MODEL,matrix);
	mDevice->setAmbientColor(ambient);

	// Shader states
	if(pass!=NULL){
		mSceneParameters->setModelMatrix(matrix);
		mSceneParameters->setRenderable(renderable);
		mSceneParameters->setAmbient(ambient);

		pass->updateVariables(Material::Scope_RENDERABLE,mSceneParameters);
		setupVariableBuffers(pass,Material::Scope_RENDERABLE,mDevice);
	
		mSceneParameters->setRenderable(NULL);
	}
}

void SimpleRenderManager::gatherRenderables(RenderableSet *set,Node *node,Camera *camera){
	TOADLET_PROFILE_AUTOSCOPE();

	set->setCamera(camera);

	set->startQueuing();
	if(node==mScene->getRoot()){
		mScene->getBackground()->gatherRenderables(camera,set);
	}
	node->gatherRenderables(camera,set);
	set->endQueuing();
}

void SimpleRenderManager::renderRenderables(RenderableSet *set,RenderDevice *device,Camera *camera,bool useMaterials){
	TOADLET_PROFILE_AUTOSCOPE();

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

	if(useMaterials){
		// This seems redundant, but it solves some issues with fixed function materials
		device->setDefaultState();
	}
}

void SimpleRenderManager::renderDepthSortedRenderables(const RenderableSet::RenderableQueue &queue,bool useMaterials){
	int i;
	for(i=0;i<queue.size();++i){
		const RenderableSet::RenderableQueueItem &item=queue[i];
		Material *material=item.material;
		renderQueueItems((useMaterials && material!=NULL)?material:NULL,&item,1);
	}
}

/// @todo: We should see if the Pass is a Fixed or Shader pass, in which case we either set Fixed states, or setupRenderVariables
///  And then maybe set Fixed states should be moved the pass, like setupRenderVariables
void SimpleRenderManager::renderQueueItems(Material *material,const RenderableSet::RenderableQueueItem *items,int numItems){
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
			if(pass!=NULL){
				setupPassForRenderable(pass,item.renderable,item.ambient);
			}
			item.renderable->render(this);
		}

		if(mSceneParameters->getCamera()->getDefaultState()!=NULL){
			mDevice->setRenderState(mSceneParameters->getCamera()->getDefaultState());
			mSceneParameters->setRenderState(mSceneParameters->getCamera()->getDefaultState());
			mLastRenderState=NULL;
		}
	}
}

void SimpleRenderManager::setupViewport(Camera *camera,RenderDevice *device){
	Viewport viewport=camera->getViewport();
	if(viewport.x==0 && viewport.y==0 && viewport.width==0 && viewport.height==0){
		viewport.width=device->getRenderTarget()->getWidth();
		viewport.height=device->getRenderTarget()->getHeight();
	}
	device->setViewport(viewport);
	mSceneParameters->setViewport(viewport);
}

/// @todo: Clean this up to handle multiple lights in shader passes
void SimpleRenderManager::setupLights(const RenderableSet::LightQueue &lightQueue,RenderDevice *device){
	if(lightQueue.size()>0){
		LightComponent *light=lightQueue[0].light;

		LightState state;
		light->getLightState(state);
		mSceneParameters->setLightState(state);
	}

	int i;
	int maxLights=mScene->getEngine()->getRenderCaps().maxLights;
	for(i=0;i<maxLights;++i){
		if(i<lightQueue.size()){
			LightComponent *light=lightQueue[i].light;

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

void SimpleRenderManager::setupTextures(RenderPass *pass,int scope,RenderDevice *device){
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

void SimpleRenderManager::setupVariableBuffers(RenderPass *pass,int scope,RenderDevice *device){
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
