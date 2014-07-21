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

#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include "ShadowMappedRenderManager.h"

namespace toadlet{
namespace tadpole{

ShadowMappedRenderManager::ShadowMappedRenderManager(Scene *scene):
	SimpleRenderManager(scene)
{
	Engine *engine=scene->getEngine();

	// TODO: This is currently ONLY functional on a FixedFunction pipeline, fix that!
	TOADLET_ASSERT(!engine->hasShader(Shader::ShaderType_FRAGMENT));

	mShadowTexture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_DYNAMIC | Texture::Usage_BIT_RENDERTARGET,new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_DEPTH_24,1024,1024,1,1));
	mShadowTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
	mShadowTarget->attach(mShadowTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_DEPTH_STENCIL);

	mLightCamera=new Camera(engine);
	mLightCamera->setProjectionFovX(Math::PI/4,1,10,300);
	mLightCamera->setRenderTarget(mShadowTarget);

	mShadowState=engine->getMaterialManager()->createRenderState();
	mShadowState->setRasterizerState(RasterizerState(RasterizerState::CullType_FRONT,RasterizerState::FillType_SOLID));

	mLightState=engine->getMaterialManager()->createRenderState();
	TextureState lightTextureState;
	lightTextureState.calculation=TextureState::CalculationType_CAMERASPACE;
	lightTextureState.shadowResult=TextureState::ShadowResult_A;
	mLightState->setTextureState(Shader::ShaderType_VERTEX,0,lightTextureState);
	SamplerState lightSamplerState(
		SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,SamplerState::FilterType_LINEAR,
		SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE,SamplerState::AddressType_CLAMP_TO_EDGE);
	mLightState->setSamplerState(Shader::ShaderType_VERTEX,0,lightSamplerState);
	mLightState->setDepthState(DepthState(DepthState::DepthTest_LEQUAL,true));
	mLightState->setMaterialState(MaterialState(MaterialState::AlphaTest_GEQUAL,0.6f));
}

ShadowMappedRenderManager::~ShadowMappedRenderManager(){
}

void ShadowMappedRenderManager::renderScene(RenderDevice *device,Node *node,Camera *camera){
	if(mLight==NULL){
		SimpleRenderManager::renderScene(device,node,camera);
		return;
	}

	mDevice=device;

	
	mLightCamera->setWorldMatrix(mLight->getParent()->getWorldTransform()->getMatrix());

	// Calculate texture matrix for projection
	// This matrix takes us from eye space to the light's clip space
	Matrix4x4 biasMatrix;
	device->getShadowBiasMatrix(biasMatrix,mShadowTexture,0.02);
	Matrix4x4 textureMatrix;
	Math::mul(textureMatrix,biasMatrix,mLightCamera->getProjectionMatrix());
	Math::postMul(textureMatrix,mLightCamera->getViewMatrix());
	Matrix4x4 invViewMatrix;
	Math::invert(invViewMatrix,camera->getViewMatrix());
	Math::postMul(textureMatrix,invViewMatrix);

	TextureState lightTextureState;
	mLightState->getTextureState(Shader::ShaderType_VERTEX,0,lightTextureState);
	lightTextureState.matrix.set(textureMatrix);
	mLightState->setTextureState(Shader::ShaderType_VERTEX,0,lightTextureState);


	// First pass, render from light's view to get depth buffer into shadowTexture
	gatherRenderables(mRenderableSet,node,mLightCamera);

	setupCamera(mLightCamera,device);

	device->setRenderState(mShadowState);
	renderRenderables(mRenderableSet,device,mLightCamera,false);

	
	// Second pass, render from camera's view to show shadowed areas
	gatherRenderables(mRenderableSet,node,camera);

	setupCamera(camera,device);

	setupLights(mRenderableSet,device);

	renderRenderables(mRenderableSet,device,camera,true);


	// Third pass, render from camera's view to show lit areas
	device->setRenderState(mLightState);
	device->setTexture(Shader::ShaderType_FRAGMENT,0,mShadowTexture);

	renderRenderables(mRenderableSet,device,camera,false);


	mParams->setCamera(NULL);
	mCamera=NULL;
	mDevice=NULL;
	mLastPass=NULL;
	mLastRenderState=NULL;
	mLastShaderState=NULL;
}

}
}
