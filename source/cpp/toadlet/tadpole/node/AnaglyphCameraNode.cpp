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
#include <toadlet/tadpole/node/AnaglyphCameraNode.h>

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
	int format=TextureFormat::Format_L_8;
	TextureFormat::ptr textureFormat(new TextureFormat(TextureFormat::Dimension_D2,format,size,size,1,1));

	mLeftTexture=mEngine->getTextureManager()->createTexture(flags,textureFormat);
	mLeftRenderTarget=mEngine->getTextureManager()->createPixelBufferRenderTarget();
	mLeftRenderTarget->attach(mLeftTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

	mRightTexture=mEngine->getTextureManager()->createTexture(flags,textureFormat);
	mRightRenderTarget=mEngine->getTextureManager()->createPixelBufferRenderTarget();
	mRightRenderTarget->attach(mRightTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

	mLeftMaterial=mEngine->createDiffuseMaterial(mLeftTexture);
	mLeftMaterial->getRenderState()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	mLeftMaterial->getRenderState()->setMaterialState(MaterialState(Colors::RED));

	mRightMaterial=mEngine->createDiffuseMaterial(mRightTexture);
	mRightMaterial->getRenderState()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	mRightMaterial->getRenderState()->setMaterialState(MaterialState(Colors::CYAN));
	mRightMaterial->getRenderState()->setBlendState(BlendState::Combination_COLOR_ADDITIVE);

	return this;
}

void AnaglyphCameraNode::destroy(){
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

	super::destroy();
}

Node *AnaglyphCameraNode::set(Node *node){
	super::set(node);
	
	AnaglyphCameraNode *cameraNode=(AnaglyphCameraNode*)node;
	setSeparation(cameraNode->getSeparation());
	setLeftColor(cameraNode->getLeftColor());
	setRightColor(cameraNode->getRightColor());
	
	return this;
}

void AnaglyphCameraNode::setLeftColor(const Vector4 &color){
	mLeftColor.set(color);
	mLeftMaterial->getPass()->setMaterialState(MaterialState(mLeftColor));
}

void AnaglyphCameraNode::setRightColor(const Vector4 &color){
	mRightColor.set(color);
	mRightMaterial->getPass()->setMaterialState(MaterialState(mRightColor));
}

void AnaglyphCameraNode::render(RenderDevice *device,Node *node){
	Vector3 translate=getTranslate();

	device->setRenderTarget(mLeftRenderTarget);
	translate.x-=mSeparation;
	setTranslate(translate);
	updateWorldTransform();
		mScene->render(device,this,NULL);
	device->swap();

	device->setRenderTarget(mRightRenderTarget);
	translate.x+=2*mSeparation;
	setTranslate(translate);
	updateWorldTransform();
		mScene->render(device,this,NULL);
	device->swap();

	device->setRenderTarget(device->getPrimaryRenderTarget());
	translate.x-=mSeparation;
	setTranslate(translate);
	updateWorldTransform();

	device->setViewport(getViewport());

	device->setMatrix(RenderDevice::MatrixType_PROJECTION,mOverlayMatrix);
	device->setMatrix(RenderDevice::MatrixType_VIEW,Math::IDENTITY_MATRIX4X4);
	device->setMatrix(RenderDevice::MatrixType_MODEL,Math::IDENTITY_MATRIX4X4);
/// @todo: Fix me
//	mLeftMaterial->setupRenderDevice(device);
	device->renderPrimitive(mOverlayVertexData,mOverlayIndexData);
//	mRightMaterial->setupRenderDevice(device);
	device->renderPrimitive(mOverlayVertexData,mOverlayIndexData);

//	renderOverlayGamma(device);
}

}
}
}
