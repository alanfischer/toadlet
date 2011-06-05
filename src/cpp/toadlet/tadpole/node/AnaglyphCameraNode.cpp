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
	mLeftMaterial->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	mLeftMaterial->setMaterialState(MaterialState(Colors::RED));
	mLeftMaterial->setTexture(0,mLeftTexture);
	mLeftMaterial->retain();

	mRightMaterial=mEngine->getMaterialManager()->createMaterial();
	mRightMaterial->setDepthState(DepthState(DepthState::DepthTest_NEVER,false));
	mRightMaterial->setMaterialState(MaterialState(Colors::CYAN));
	mRightMaterial->setBlendState(BlendState::Combination_COLOR_ADDITIVE);
	mRightMaterial->setTexture(0,mRightTexture);
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
	mLeftMaterial->setMaterialState(MaterialState(mLeftColor));
}

void AnaglyphCameraNode::setRightColor(const Vector4 &color){
	mRightColor.set(color);
	mRightMaterial->setMaterialState(MaterialState(mRightColor));
}

void AnaglyphCameraNode::render(RenderDevice *renderDevice,Node *node){
	Vector3 translate=getTranslate();

	renderDevice->setRenderTarget(mLeftRenderTarget);
	translate.x-=mSeparation;
	setTranslate(translate);
	updateWorldTransform();
		mScene->render(renderDevice,this,NULL);
	renderDevice->swap();

	renderDevice->setRenderTarget(mRightRenderTarget);
	translate.x+=2*mSeparation;
	setTranslate(translate);
	updateWorldTransform();
		mScene->render(renderDevice,this,NULL);
	renderDevice->swap();

	renderDevice->setRenderTarget(renderDevice->getPrimaryRenderTarget());
	translate.x-=mSeparation;
	setTranslate(translate);
	updateWorldTransform();

	if(getViewportSet()){
		renderDevice->setViewport(getViewport());
	}
	else{
		RenderTarget *renderTarget=renderDevice->getRenderTarget();
		Viewport viewport;
		renderDevice->setViewport(viewport.set(0,0,renderTarget->getWidth(),renderTarget->getHeight()));
	}

	renderDevice->setProjectionMatrix(mOverlayMatrix);
	renderDevice->setViewMatrix(Math::IDENTITY_MATRIX4X4);
	renderDevice->setModelMatrix(Math::IDENTITY_MATRIX4X4);
	mLeftMaterial->setupRenderDevice(renderDevice);
	renderDevice->renderPrimitive(mOverlayVertexData,mOverlayIndexData);
	mRightMaterial->setupRenderDevice(renderDevice);
	renderDevice->renderPrimitive(mOverlayVertexData,mOverlayIndexData);

	renderOverlayGamma(renderDevice);
}

}
}
}
