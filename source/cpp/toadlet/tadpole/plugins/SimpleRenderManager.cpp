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

#include <toadlet/peeper/RenderCaps.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include "SimpleRenderManager.h"

namespace toadlet{
namespace tadpole{

SimpleRenderManager::SimpleRenderManager(Scene *scene):
	mScene(scene),
	mDevice(NULL),
	mCamera(NULL),
	//mParams,
	//mRenderableSet,
	mLastPass(NULL),
	mLastRenderState(NULL),
	mLastShaderState(NULL)
{
	mRenderableSet=new RenderableSet(scene);
	mParams=new SceneParameters();
	mParams->setScene(scene);
	mDefaultMaterial=scene->getEngine()->createDiffuseMaterial(NULL);
	mDefaultMaterial->getRenderState()->setMaterialState(MaterialState(Vector4()));
}

SimpleRenderManager::~SimpleRenderManager(){
}

void SimpleRenderManager::renderScene(RenderDevice *device,Node *node,Camera *camera){
	TOADLET_PROFILE_AUTOSCOPE();

	mDevice=device;

	gatherRenderables(mRenderableSet,node,camera);

	setupCamera(camera,device);

	setupLights(mRenderableSet,device);

	renderRenderables(mRenderableSet,device,camera,true);

	mParams->setCamera(NULL);
	mLastPass=NULL;
	mLastRenderState=NULL;
	mLastShaderState=NULL;
	mDevice=NULL;
	mCamera=NULL;
}

void SimpleRenderManager::setupPass(RenderPass *pass,RenderDevice *device){
	mParams->setRenderPass(pass);

	if(mLastShaderState==NULL || mLastShaderState!=pass->getShaderState()){
		mLastShaderState=pass->getShaderState();
		device->setShaderState(mLastShaderState);
	}
	if(mLastRenderState==NULL || mLastRenderState!=pass->getRenderState()){
		RenderState *renderState=pass->getRenderState();
		RenderState *cameraState=mCamera->getDefaultState();

		if(mLastRenderState!=NULL && renderState!=NULL && cameraState!=NULL){
			int lastBits=mLastRenderState->getSetStates();
			int renderBits=renderState->getSetStates();
			int cameraBits=cameraState->getSetStates();
			int oldBits=((~renderBits) & lastBits);

			if(oldBits & cameraBits){
				device->setRenderState(mCamera->getDefaultState());
				mParams->setRenderState(mCamera->getDefaultState());
			}
		}

		mLastRenderState=renderState;
		device->setRenderState(mLastRenderState);
		mParams->setRenderState(mLastRenderState);
	}

	setupTextures(pass,Material::Scope_MATERIAL,device);
	setupVariableBuffers(pass,Material::Scope_MATERIAL,device);

	mLastPass=pass;
}

void SimpleRenderManager::setupPassForRenderable(RenderPass *pass,RenderDevice *device,Renderable *renderable,const Vector4 &ambient){
	Camera *camera=mCamera;

	Matrix4x4 matrix;
	renderable->getRenderTransform()->getMatrix(matrix);

	GeometryState state;
	if(pass!=NULL && pass->getRenderState()->getGeometryState(state) && state.matrixFlags!=0){
		Vector3 translate;
		Quaternion rotate;
		Vector3 scale;
		Math::setTranslateFromMatrix4x4(translate,matrix);
		Math::setQuaternionFromMatrix4x4(rotate,matrix);
		Math::setScaleFromMatrix4x4(scale,matrix);

		if((state.matrixFlags&GeometryState::MatrixFlag_CAMERA_ALIGNED)!=0){
			Vector3 axis=state.matrixAlignAxis;
			if(axis==Math::ZERO_VECTOR3 && camera->getAlignmentCalculationsUseOrigin()){
				axis=camera->getUp();
			}

			if(axis!=Math::ZERO_VECTOR3){
				Vector3 look=camera->getPosition()-translate;
				Vector3 up=axis;
				Vector3 right;
				Math::cross(right,up,look);
				Math::normalize(right);
				Math::cross(look,right,up);
				Math::setQuaternionFromAxes(rotate,right,up,look);
			}
			else{
				Math::setQuaternionFromMatrix4x4(rotate,camera->getViewMatrix());
				Math::invert(rotate);
			}
		}

		if((state.matrixFlags&GeometryState::MatrixFlag_NO_PERSPECTIVE)!=0){
			Vector4 point;
			point.set(translate,Math::ONE);
			Math::mul(point,camera->getViewMatrix());
			Math::mul(point,camera->getProjectionMatrix());
			Math::mul(scale,point.w);
		}

		if((state.matrixFlags&GeometryState::MatrixFlag_ASPECT_CORRECT)!=0){
			const Viewport &viewport=mParams->getViewport();
			scale.x=Math::mul(scale.x,Math::div(Math::fromInt(viewport.height),Math::fromInt(viewport.width)));
		}

		Math::setMatrix4x4FromTranslateRotateScale(matrix,translate,rotate,scale);
	}

	device->setMatrix(RenderDevice::MatrixType_MODEL,matrix);
	mParams->setMatrix(RenderDevice::MatrixType_MODEL,matrix);

	device->setAmbientColor(ambient);
	mParams->setAmbientColor(ambient);

	// Shader states
	if(pass!=NULL){
		mParams->setRenderable(renderable);

		setupVariableBuffers(pass,Material::Scope_RENDERABLE,device);
	
		mParams->setRenderable(NULL);
	}
}

void SimpleRenderManager::gatherRenderables(RenderableSet *set,Node *node,Camera *camera){
	TOADLET_PROFILE_AUTOSCOPE();

	set->setCamera(camera);

	/// @todo: We need a way to expose gatherRenderables so we can not only gather Renderables but also Nodes, which can be processed for shadows, since decal shadows would be node based, not renderable based
	set->startQueuing();
	if(node==mScene->getRoot() && mScene->getBackground()!=NULL){
		mScene->getBackground()->gatherRenderables(camera,set);
	}
	node->gatherRenderables(camera,set);
	set->endQueuing();
}

void SimpleRenderManager::renderRenderables(RenderableSet *set,RenderDevice *device,Camera *camera,bool useMaterials){
	TOADLET_PROFILE_AUTOSCOPE();

	if(useMaterials){
		if(camera->getDefaultState()!=NULL){
			device->setRenderState(camera->getDefaultState());
			mParams->setRenderState(camera->getDefaultState());
		}
	}

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

		renderQueueItems((useMaterials && material!=NULL)?material:NULL,&queue[0],queue.size(),useMaterials);
	}

	interRenderRenderables(set,device,camera,useMaterials);

	while(depthIndex<depthQueueIndexes.size()){
		const RenderableSet::RenderableQueue &depthQueue=set->getRenderableQueue(depthQueueIndexes[depthIndex]);
		renderDepthSortedRenderables(depthQueue,useMaterials);
		depthIndex++;
	}
}

void SimpleRenderManager::renderDepthSortedRenderables(const RenderableSet::RenderableQueue &queue,bool useMaterials){
	int i;
	for(i=0;i<queue.size();++i){
		const RenderableSet::RenderableQueueItem &item=queue[i];
		Material *material=item.material;
		renderQueueItems((useMaterials && material!=NULL)?material:NULL,&item,1,useMaterials);
	}
}

/// @todo: We should see if the Pass is a Fixed or Shader pass, in which case we either set Fixed states, or setupRenderVariables
///  And then maybe set Fixed states should be moved the pass, like setupRenderVariables
void SimpleRenderManager::renderQueueItems(Material *material,const RenderableSet::RenderableQueueItem *items,int numItems,bool useMaterials){
	int j;
	RenderPath *path=(material!=NULL)?material->getBestPath():NULL;
	if(path!=NULL){
		tforeach(RenderPath::PassCollection::const_iterator,it,path->getPasses()){
			RenderPass *pass=*it;
			setupPass(pass,mDevice);
			for(j=0;j<numItems;++j){
				const RenderableSet::RenderableQueueItem &item=items[j];
				setupPassForRenderable(pass,mDevice,item.renderable,item.ambient);
				item.renderable->render(this);
			}
		}
	}
	else{
		RenderPass *pass=NULL;
		if(useMaterials){
			pass=mDefaultMaterial->getPass();
			setupPass(pass,mDevice);
		}
		for(j=0;j<numItems;++j){
			const RenderableSet::RenderableQueueItem &item=items[j];
			setupPassForRenderable(pass,mDevice,item.renderable,item.ambient);
			item.renderable->render(this);
		}
	}
}

void SimpleRenderManager::setupCamera(Camera *camera,RenderDevice *device){
	mCamera=camera;
	mParams->setCamera(camera);

	RenderTarget *target=camera->getRenderTarget();
	device->setRenderTarget(target!=NULL?target:device->getPrimaryRenderTarget());

	Viewport viewport=camera->getViewport();
	if(viewport.empty){
		viewport.set(0,0,device->getRenderTarget()->getWidth(),device->getRenderTarget()->getHeight());
	}
	if(mParams->getViewport()!=viewport){
		device->setViewport(viewport);
		mParams->setViewport(viewport);
	}

	device->setMatrix(RenderDevice::MatrixType_VIEW,camera->getViewMatrix());
	mParams->setMatrix(RenderDevice::MatrixType_VIEW,camera->getViewMatrix());

	device->setMatrix(RenderDevice::MatrixType_PROJECTION,camera->getProjectionMatrix());
	mParams->setMatrix(RenderDevice::MatrixType_PROJECTION,camera->getProjectionMatrix());

	device->setMatrix(RenderDevice::MatrixType_MODEL,Math::IDENTITY_MATRIX4X4);
	mParams->setMatrix(RenderDevice::MatrixType_MODEL,Math::IDENTITY_MATRIX4X4);

	int clearFlags=camera->getClearFlags();
	if(clearFlags!=0){
		device->clear(clearFlags,camera->getClearColor());
	}
}

/// @todo: Clean this up to handle multiple lights in shader passes
void SimpleRenderManager::setupLights(RenderableSet *set,RenderDevice *device){
	const RenderableSet::LightQueue &lightQueue=set->getLightQueue();

	if(lightQueue.size()>0){
		LightComponent *light=lightQueue[0].light;

		LightState state;
		light->getLightState(state);
		mParams->setLightState(state);
	}

	RenderCaps caps;
	device->getRenderCaps(caps);
	int maxLights=caps.maxLights;
	int i;
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
		if(pass->getShaderState()==NULL && mLastPass!=NULL){
			for(;i<mLastPass->getNumTextures((Shader::ShaderType)j);++i){
				device->setTexture((Shader::ShaderType)j,i,NULL);
			}
		}
	}
}

void SimpleRenderManager::setupVariableBuffers(RenderPass *pass,int scope,RenderDevice *device){
	pass->updateVariables(scope,mParams);

	int i,j;
	for(j=0;j<Shader::ShaderType_MAX;++j){
		for(i=0;i<pass->getNumBuffers((Shader::ShaderType)j);++i){
			int bufferScope=pass->getBufferScope((Shader::ShaderType)j,i);
			// If the buffer applies to this scope, and it is the highest bit scope in the buffer
			if((bufferScope&scope)!=0 && (bufferScope&~scope)<=scope){
				device->setBuffer((Shader::ShaderType)j,i,pass->getBuffer((Shader::ShaderType)j,i));
			}
		}
	}
}

}
}
