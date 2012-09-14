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

namespace toadlet{
namespace tadpole{

AnaglyphCamera::AnaglyphCamera(Engine *engine):
	mSeparation(Math::ONE)
{
	int flags=Texture::Usage_BIT_RENDERTARGET;
	int size=1024;//Math::nextPowerOf2((getWidth()<getHeight()?getWidth():getHeight())/2);
	int format=TextureFormat::Format_L_8;
	TextureFormat::ptr textureFormat(new TextureFormat(TextureFormat::Dimension_D2,format,size,size,1,1));

	mLeftTexture=engine->getTextureManager()->createTexture(flags,textureFormat);
	mLeftRenderTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
	mLeftRenderTarget->attach(mLeftTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

	mRightTexture=engine->getTextureManager()->createTexture(flags,textureFormat);
	mRightRenderTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
	mRightRenderTarget->attach(mRightTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

	mLeftMaterial=engine->createDiffuseMaterial(mLeftTexture);
	mLeftMaterial->getRenderState()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	mLeftMaterial->getRenderState()->setMaterialState(MaterialState(Colors::RED));

	mRightMaterial=engine->createDiffuseMaterial(mRightTexture);
	mRightMaterial->getRenderState()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	mRightMaterial->getRenderState()->setMaterialState(MaterialState(Colors::CYAN));
	mRightMaterial->getRenderState()->setBlendState(BlendState::Combination_COLOR_ADDITIVE);
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

	mLeftMaterial=NULL;
	mRightMaterial=NULL;
}

void AnaglyphCamera::setLeftColor(const Vector4 &color){
	mLeftColor.set(color);
	mLeftMaterial->getPass()->setMaterialState(MaterialState(mLeftColor));
}

void AnaglyphCamera::setRightColor(const Vector4 &color){
	mRightColor.set(color);
	mRightMaterial->getPass()->setMaterialState(MaterialState(mRightColor));
}

void AnaglyphCamera::render(RenderDevice *device,Scene *scene){
	Vector3 position=getPosition();

	device->setRenderTarget(mLeftRenderTarget);
	Math::msub(position,mRight,mSeparation,position);
	setPosition(position);
		scene->render(device,this,NULL);
	device->swap();

	device->setRenderTarget(mRightRenderTarget);
	Math::madd(position,mRight,2*mSeparation,position);
	setPosition(position);
		scene->render(device,this,NULL);
	device->swap();

	device->setRenderTarget(device->getPrimaryRenderTarget());
	Math::msub(position,mRight,mSeparation,position);
	setPosition(position);

	device->setViewport(getViewport());

/// @todo: Fix me
//	device->setMatrix(RenderDevice::MatrixType_PROJECTION,mOverlayMatrix);
//	device->setMatrix(RenderDevice::MatrixType_VIEW,Math::IDENTITY_MATRIX4X4);
//	device->setMatrix(RenderDevice::MatrixType_MODEL,Math::IDENTITY_MATRIX4X4);
//	mLeftMaterial->setupRenderDevice(device);
//	device->renderPrimitive(mOverlayVertexData,mOverlayIndexData);
//	mRightMaterial->setupRenderDevice(device);
//	device->renderPrimitive(mOverlayVertexData,mOverlayIndexData);
}

}
}
