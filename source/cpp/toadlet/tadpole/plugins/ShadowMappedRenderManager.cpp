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

	mShadowTexture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_RENDERTARGET,TextureFormat::ptr(new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_DEPTH_24,1024,1024,1,1)));
	mShadowTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
	mShadowTarget->attach(mShadowTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_DEPTH_STENCIL);

	mLightCamera=engine->createNodeType(CameraNode::type(),scene);
	mLightCamera->setProjectionFovX(Math::PI/4,1,30,60);
	scene->getRoot()->attach(mLightCamera);

	mShadowState=engine->getMaterialManager()->createRenderState();
	mShadowState->setRasterizerState(RasterizerState(RasterizerState::CullType_FRONT,RasterizerState::FillType_SOLID,0,0.1));

	mLightState=engine->getMaterialManager()->createRenderState();
}

ShadowMappedRenderManager::~ShadowMappedRenderManager(){
}

void ShadowMappedRenderManager::renderScene(RenderDevice *renderDevice,Node *node,CameraNode *camera){
	if(mLight==NULL){
		SimpleRenderManager::renderScene(renderDevice,node,camera);
		return;
	}

	mLightCamera->setTransform(mLight->getTransform());

	gatherRenderables(mRenderableSet,node,mLightCamera);

	RenderTarget *oldRenderTarget=renderDevice->getRenderTarget();

	renderDevice->setRenderTarget(mShadowTarget);
	{
		renderDevice->setRenderState(mShadowState);

		renderRenderables(mRenderableSet,renderDevice,mLightCamera,false);
	}
	renderDevice->swap();


	gatherRenderables(mRenderableSet,node,camera);

	renderDevice->setRenderTarget(oldRenderTarget);
	{
		// Calculate texture matrix for projection
		// This matrix takes us from eye space to the light's clip space
		Matrix4x4 biasMatrix;
		renderDevice->getShadowBiasMatrix(mShadowTexture,biasMatrix);
		Matrix4x4 textureMatrix;
		Math::mul(textureMatrix,biasMatrix,mLightCamera->getProjectionMatrix());
		Math::postMul(textureMatrix,mLightCamera->getViewMatrix());
		Math::postMul(textureMatrix,camera->calculateInverseViewMatrix());

		TextureState lightTextureState;
		lightTextureState.calculation=TextureState::CalculationType_CAMERASPACE;
		lightTextureState.matrix.set(textureMatrix);
TOADLET_ASSERT(false && "BROKEN!");
	//	mLightState->setTextureState(0,lightTextureState);
//		renderDevice->setRenderState(mLightState);
//		renderDevice->setTexture(0,mShadowTexture);

		renderRenderables(mRenderableSet,renderDevice,camera,false);
	}
}

}
}
