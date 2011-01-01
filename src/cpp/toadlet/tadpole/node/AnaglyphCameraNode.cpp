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
#include <toadlet/tadpole/node/AnaglyphCameraNode.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(AnaglyphCameraNode,Categories::TOADLET_TADPOLE_NODE+".AnaglyphCameraNode");

AnaglyphCameraNode::AnaglyphCameraNode():super(),mSeparation(Math::ONE){
}

Node *AnaglyphCameraNode::create(Scene *scene){
	super::create(scene);

	int flags=Texture::Usage_BIT_RENDERTARGET;
	int size=1024;//Math::nextPowerOf2((getWidth()<getHeight()?getWidth():getHeight())/2);
	int format=Texture::Format_L_8;

	mLeftTexture=mEngine->getTextureManager()->createTexture(flags,Texture::Dimension_D2,format,size,size,0,1);
	mLeftRenderTarget=mEngine->getTextureManager()->createPixelBufferRenderTarget();
	mLeftRenderTarget->attach(mLeftTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

	mRightTexture=mEngine->getTextureManager()->createTexture(flags,Texture::Dimension_D2,format,size,size,0,1);
	mRightRenderTarget=mEngine->getTextureManager()->createPixelBufferRenderTarget();
	mRightRenderTarget->attach(mRightTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

	mLeftMaterial=mEngine->getMaterialManager()->createMaterial();
	mLeftMaterial->setDepthTest(Renderer::DepthTest_NONE);
	mLeftMaterial->setLighting(true);
	mLeftMaterial->setLightEffect(LightEffect(Colors::RED));
	mLeftMaterial->setTextureStage(0,TextureStage::ptr(new TextureStage(mLeftTexture)));
	mLeftMaterial->retain();

	mRightMaterial=mEngine->getMaterialManager()->createMaterial();
	mRightMaterial->setDepthTest(Renderer::DepthTest_NONE);
	mRightMaterial->setLighting(true);
	mRightMaterial->setLightEffect(LightEffect(Colors::CYAN));
	mRightMaterial->setTextureStage(0,TextureStage::ptr(new TextureStage(mRightTexture)));
	mRightMaterial->setBlend(Blend::Combination_COLOR_ADDITIVE);
	mRightMaterial->retain();

	return this;
}

void AnaglyphCameraNode::destroy(){
	mLeftRenderTarget->destroy();
	mRightRenderTarget->destroy();

	mLeftMaterial->release();
	mRightMaterial->release();

	super::destroy();
}

void AnaglyphCameraNode::setLeftColor(const Color &color){
	mLeftColor.set(color);
	mLeftMaterial->setLightEffect(LightEffect(mLeftColor));
}

void AnaglyphCameraNode::setRightColor(const Color &color){
	mRightColor.set(color);
	mRightMaterial->setLightEffect(LightEffect(mRightColor));
}

void AnaglyphCameraNode::render(Renderer *renderer,Node *node){
	Vector3 translate=getTranslate();

	renderer->setRenderTarget(mLeftRenderTarget);
	translate.x-=mSeparation;
	setTranslate(translate);
	updateWorldTransform();
		mScene->render(renderer,this,NULL);
	renderer->swap();

	renderer->setRenderTarget(mRightRenderTarget);
	translate.x+=2*mSeparation;
	setTranslate(translate);
	updateWorldTransform();
		mScene->render(renderer,this,NULL);
	renderer->swap();

	renderer->setRenderTarget(renderer->getPrimaryRenderTarget());
	translate.x-=mSeparation;
	setTranslate(translate);
	updateWorldTransform();

	if(getViewportSet()){
		renderer->setViewport(getViewport());
	}
	else{
		RenderTarget *renderTarget=renderer->getRenderTarget();
		Viewport viewport;
		renderer->setViewport(viewport.set(0,0,renderTarget->getWidth(),renderTarget->getHeight()));
	}

	renderer->setProjectionMatrix(mOverlayMatrix);
	renderer->setViewMatrix(Math::IDENTITY_MATRIX4X4);
	renderer->setModelMatrix(Math::IDENTITY_MATRIX4X4);
	mLeftMaterial->setupRenderer(renderer);
	renderer->renderPrimitive(mOverlayVertexData,mOverlayIndexData);
	mRightMaterial->setupRenderer(renderer);
	renderer->renderPrimitive(mOverlayVertexData,mOverlayIndexData);

	renderOverlayGamma(renderer);
}

}
}
}
