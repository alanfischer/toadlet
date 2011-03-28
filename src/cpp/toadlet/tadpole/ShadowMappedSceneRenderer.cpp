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
	mShadowTexture=scene->getEngine()->getTextureManager()->createTexture(Texture::Usage_BIT_RENDERTARGET,Texture::Dimension_D2,Texture::Format_DEPTH_24,1024,1024,1,1);
	mShadowTarget=scene->getEngine()->getTextureManager()->createPixelBufferRenderTarget();
	mShadowTarget->attach(mShadowTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_DEPTH_STENCIL);

	mLightCamera=scene->getEngine()->createNodeType(CameraNode::type(),scene);
	mLightCamera->setProjectionFovX(Math::PI/4,1,30,60);
	scene->getRoot()->attach(mLightCamera);
}

ShadowMappedSceneRenderer::~ShadowMappedSceneRenderer(){
}

#if 0
void ShadowMappedSceneRenderer::renderScene(Renderer *renderer,Node *node,CameraNode *camera){
//	renderer->set
}

/*
scene->DO_STATES=true;
	light->setEnabled(false);

	renderer->setRenderTarget(shadowTarget);
	renderer->beginScene();
scene->DO_STATES=false;
renderer->setFaceCulling(Renderer::FaceCulling_FRONT);
renderer->setDepthBias(0,.1);
		lightCamera->render(renderer);
renderer->setDepthBias(0,0);
renderer->setFaceCulling(Renderer::FaceCulling_BACK);
scene->DO_STATES=true;
	renderer->endScene();
	renderer->swap();

	renderer->setRenderTarget(detectorTarget);
	renderer->beginScene();
		gun->render(renderer);
	renderer->endScene();
	renderer->swap();

	light->setEnabled(true);

	renderer->setRenderTarget(correctorTarget);
	renderer->beginScene();
		correctedGun->render(renderer);
	renderer->endScene();
	renderer->swap();
*/
	renderer->setRenderTarget(this);
	renderer->beginScene();
LightState state=light->getLightState();
state.diffuseColor=Vector4(0,0,0,0);
state.specularColor=Vector4(0,0,0,0);
light->setLightState(state);
		camera->render(renderer);
/*
int cf=camera->getClearFlags();
camera->setClearFlags(0);
		
Matrix4x4 biasMatrix;
renderer->getShadowBiasMatrix(shadowTexture,biasMatrix);

// Calculate texture matrix for projection
// This matrix takes us from eye space to the light's clip space
Matrix4x4 textureMatrix=biasMatrix*
	lightCamera->getProjectionMatrix() *
	lightCamera->getViewMatrix() * camera->calculateInverseViewMatrix();
TextureStage::ptr shadowStage=mEngine->getMaterialManager()->createTextureStage(shadowTexture,true);
shadowStage->setCalculation(TextureStage::Calculation_CAMERASPACE,textureMatrix);
renderer->setTextureStage(0,shadowStage);

renderer->setShadowComparisonMethod(true);

state.diffuseColor=Vector4(1,1,1,0);
//state.specularColor=Vector4(1,1,1,0);
light->setLightState(state);

scene->DO_STATES=false;
renderer->setBlend(Blend::Combination_COLOR);
camera->setScope(Scope_SCENE);
camera->render(renderer);
camera->setScope(Scope_SCENE|Scope_HUD);
scene->DO_STATES=true;

renderer->setShadowComparisonMethod(false);

camera->setClearFlags(cf);
renderer->setDefaultStates();
*/	renderer->endScene();
	renderer->swap();





	mShadowMaterial->setupRenderer(renderer,NULL);

	int i,j;
	for(i=0;i<set->getNumRenderableQueues();++i){
		const RenderableSet::RenderableQueue &queue=set->getRenderableQueue(i);
		for(j=0;j<queue.size();++j){
			const RenderableSet::RenderableQueueItem &item=queue[j];
			Renderable *renderable=item.renderable;

			Matrix4x4 m;
			Plane p;
			p.normal=Vector3(0,0,1);
			p.distance=20;
			if(state.type==LightState::Type_DIRECTION){
				Math::setMatrix4x4FromObliquePlane(m,p,state.direction);
			}
			else{
				Math::setMatrix4x4FromPerspectivePlane(m,p,state.position);
			}
			
			Matrix4x4 m2;
			renderable->getRenderTransform().getMatrix(m2);
			Math::postMul(m,m2);
			
			renderer->setModelMatrix(m);
			renderable->render(renderer);
		}
	}
}
#endif
}
}
