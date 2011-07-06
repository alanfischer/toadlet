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
	int format=Texture::Format_L_8;

	mLeftTexture=mEngine->getTextureManager()->createTexture(flags,Texture::Dimension_D2,format,size,size,0,1);
	mLeftRenderTarget=mEngine->getTextureManager()->createPixelBufferRenderTarget();
	mLeftRenderTarget->attach(mLeftTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

	mRightTexture=mEngine->getTextureManager()->createTexture(flags,Texture::Dimension_D2,format,size,size,0,1);
	mRightRenderTarget=mEngine->getTextureManager()->createPixelBufferRenderTarget();
	mRightRenderTarget->attach(mRightTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

	mLeftMaterial=mEngine->getMaterialManager()->createMaterial();
	mLeftMaterial->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	mLeftMaterial->getPass()->setMaterialState(MaterialState(Colors::RED));
	mLeftMaterial->getPass()->setTexture(0,mLeftTexture);
	mLeftMaterial->retain();

	mRightMaterial=mEngine->getMaterialManager()->createMaterial();
	mRightMaterial->getPass()->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	mRightMaterial->getPass()->setMaterialState(MaterialState(Colors::CYAN));
	mRightMaterial->getPass()->setBlendState(BlendState::Combination_COLOR_ADDITIVE);
	mRightMaterial->getPass()->setTexture(0,mRightTexture);
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

	if(getViewportSet()){
		device->setViewport(getViewport());
	}
	else{
		RenderTarget *renderTarget=device->getRenderTarget();
		Viewport viewport;
		device->setViewport(viewport.set(0,0,renderTarget->getWidth(),renderTarget->getHeight()));
	}

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
