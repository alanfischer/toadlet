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

#include <toadlet/tadpole/ShadowMappedSceneRenderer.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>

using namespace toadlet;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::node;

namespace toadlet{
namespace tadpole{

ShadowMappedSceneRenderer::ShadowMappedSceneRenderer(Scene *scene):
	SceneRenderer(scene)
{
	Engine *engine=scene->getEngine();

	mShadowTexture=engine->getTextureManager()->createTexture(Texture::Usage_BIT_RENDERTARGET,Texture::Dimension_D2,Texture::Format_DEPTH_24,1024,1024,1,1);
	mShadowTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
	mShadowTarget->attach(mShadowTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_DEPTH_STENCIL);

	mLightCamera=engine->createNodeType(CameraNode::type(),scene);
	mLightCamera->setProjectionFovX(Math::PI/4,1,30,60);
	scene->getRoot()->attach(mLightCamera);

	mShadowStates=engine->getMaterialManager()->createRenderStateSet();
	mShadowStates->setRasterizerState(RasterizerState(RasterizerState::CullType_FRONT,RasterizerState::FillType_SOLID,0,0.1));

	mLightStates=engine->getMaterialManager()->createRenderStateSet();
}

ShadowMappedSceneRenderer::~ShadowMappedSceneRenderer(){
}

void ShadowMappedSceneRenderer::renderScene(Renderer *renderer,Node *node,CameraNode *camera){
	if(mLight==NULL){
		SceneRenderer::renderScene(renderer,node,camera);
		return;
	}

	mLightCamera->setTransform(mLight->getTransform());

	gatherRenderables(mRenderableSet,node,mLightCamera);

	RenderTarget *oldRenderTarget=renderer->getRenderTarget();

	renderer->setRenderTarget(mShadowTarget);
	{
		renderer->setRenderStateSet(mShadowStates);

		renderRenderables(mRenderableSet,renderer,mLightCamera,false);
	}
	renderer->swap();


	gatherRenderables(mRenderableSet,node,camera);

	renderer->setRenderTarget(oldRenderTarget);
	{
		// Calculate texture matrix for projection
		// This matrix takes us from eye space to the light's clip space
		Matrix4x4 biasMatrix;
		renderer->getShadowBiasMatrix(mShadowTexture,biasMatrix);
		Matrix4x4 textureMatrix;
		Math::mul(textureMatrix,biasMatrix,mLightCamera->getProjectionMatrix());
		Math::postMul(textureMatrix,mLightCamera->getViewMatrix());
		Math::postMul(textureMatrix,camera->calculateInverseViewMatrix());

		TextureState lightTextureState;
		lightTextureState.calculation=TextureState::CalculationType_CAMERASPACE;
		lightTextureState.matrix.set(textureMatrix);
		mLightStates->setTextureState(0,lightTextureState);
		renderer->setRenderStateSet(mLightStates);
		renderer->setTexture(0,mShadowTexture);

		renderRenderables(mRenderableSet,renderer,camera,false);
	}
}

}
}
