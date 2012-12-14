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

#include <toadlet/tadpole/Colors.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/Scene.h>
#include <toadlet/tadpole/AnaglyphCamera.h>
#include <toadlet/tadpole/SpriteComponent.h>

namespace toadlet{
namespace tadpole{

AnaglyphCamera::AnaglyphCamera(Engine *engine):
	mSeparation(Math::ONE)
{
	int flags=Texture::Usage_BIT_RENDERTARGET;
	int size=1024;//Math::nextPowerOf2((getWidth()<getHeight()?getWidth():getHeight())/2);
	int format=TextureFormat::Format_L_8;
	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,format,size,size,1,1);

	mLeftTexture=engine->getTextureManager()->createTexture(flags,textureFormat);
	mLeftRenderTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
	mLeftRenderTarget->attach(mLeftTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

	mRightTexture=engine->getTextureManager()->createTexture(flags,textureFormat);
	mRightRenderTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
	mRightRenderTarget->attach(mRightTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

	Material::ptr leftMaterial=engine->createDiffuseMaterial(mLeftTexture);
	Material::ptr rightMaterial=engine->createDiffuseMaterial(mRightTexture);

	mMaterial=engine->getMaterialManager()->createMaterial();
	for(int i=0;i<leftMaterial->getNumPaths();++i){
		RenderPath::ptr leftPath=leftMaterial->getPath(i);
		RenderPath::ptr rightPath=leftMaterial->getPath(i);
		RenderPath::ptr path=mMaterial->addPath();

		RenderPass::ptr leftPass=leftPath->takePass(0);
		leftPass->getRenderState()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
		path->addPass(leftPass);

		RenderPass::ptr rightPass=rightPath->takePass(0);
		rightPass->getRenderState()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
		rightPass->getRenderState()->setBlendState(BlendState::Combination_COLOR_ADDITIVE);
		path->addPass(rightPass);
	}

	SpriteComponent::ptr sprite=new SpriteComponent(engine);
//	sprite->setMaterial(
	/// @todo: Fix me
//	device->setMatrix(RenderDevice::MatrixType_PROJECTION,mOverlayMatrix);
//	device->setMatrix(RenderDevice::MatrixType_VIEW,Math::IDENTITY_MATRIX4X4);
//	device->setMatrix(RenderDevice::MatrixType_MODEL,Math::IDENTITY_MATRIX4X4);
//	mLeftMaterial->setupRenderDevice(device);
//	device->renderPrimitive(mOverlayVertexData,mOverlayIndexData);
//	mRightMaterial->setupRenderDevice(device);
//	device->renderPrimitive(mOverlayVertexData,mOverlayIndexData);


	setLeftColor(Colors::RED);
	setRightColor(Colors::CYAN);
}

void AnaglyphCamera::destroy(){
	if(mLeftRenderTarget!=NULL){
		mLeftRenderTarget->destroy();
		mLeftRenderTarget=NULL;
	}

	if(mRightRenderTarget!=NULL){
		mRightRenderTarget->destroy();
		mRightRenderTarget=NULL;
	}

	mMaterial=NULL;
}

void AnaglyphCamera::setLeftColor(const Vector4 &color){
	mLeftColor.set(color);
	for(int i=0;i<mMaterial->getNumPaths();++i){
		mMaterial->getPath(i)->getPass(0)->getRenderState()->setMaterialState(MaterialState(mLeftColor));
	}
}

void AnaglyphCamera::setRightColor(const Vector4 &color){
	mRightColor.set(color);
	for(int i=0;i<mMaterial->getNumPaths();++i){
		mMaterial->getPath(i)->getPass(1)->getRenderState()->setMaterialState(MaterialState(mLeftColor));
	}
}

void AnaglyphCamera::render(RenderDevice *device,Scene *scene){
	Vector3 position=getPosition();

	device->setRenderTarget(mLeftRenderTarget);
	Math::msub(position,mRight,mSeparation,position);
	setPosition(position);
	scene->render(device,this,NULL);

	device->setRenderTarget(mRightRenderTarget);
	Math::madd(position,mRight,2*mSeparation,position);
	setPosition(position);
	scene->render(device,this,NULL);

	device->setRenderTarget(device->getPrimaryRenderTarget());
	Math::msub(position,mRight,mSeparation,position);
	setPosition(position);

//	scene->render(device,this,overlay);
}

}
}
