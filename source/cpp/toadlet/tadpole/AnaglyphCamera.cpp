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

AnaglyphCamera::AnaglyphCamera(Scene *scene):
	Camera(scene->getEngine()),
	mSeparation(0)
{
	Engine *engine=scene->getEngine();

	int flags=Texture::Usage_BIT_RENDERTARGET;
	int size=1024;//Math::nextPowerOf2((getWidth()<getHeight()?getWidth():getHeight())/2);
	int format=TextureFormat::Format_L_8;
	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,format,size,size,1,1);

	mLeftTexture=engine->getTextureManager()->createTexture(flags,textureFormat);
	mLeftRenderTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
	mLeftRenderTarget->attach(mLeftTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

	mLeftCamera=new Camera(engine);
	mLeftCamera->setRenderTarget(mLeftRenderTarget);

	mRightTexture=engine->getTextureManager()->createTexture(flags,textureFormat);
	mRightRenderTarget=engine->getTextureManager()->createPixelBufferRenderTarget();
	mRightRenderTarget->attach(mRightTexture->getMipPixelBuffer(0,0),PixelBufferRenderTarget::Attachment_COLOR_0);

	mRightCamera=new Camera(engine);
	mRightCamera->setRenderTarget(mRightRenderTarget);

	Material::ptr leftMaterial=engine->createDiffuseMaterial(mLeftTexture);
	Material::ptr rightMaterial=engine->createDiffuseMaterial(mRightTexture);

	mMaterial=engine->getMaterialManager()->createMaterial();
	for(int i=0;i<leftMaterial->getPaths().size();++i){
		RenderPath::ptr leftPath=leftMaterial->getPaths()[i];
		RenderPath::ptr rightPath=rightMaterial->getPaths()[i];
		RenderPath::ptr path=mMaterial->addPath();

		RenderPass::ptr leftPass=leftPath->takePass(0);
		leftPass->getRenderState()->setDepthState(DepthState(DepthState::DepthTest_ALWAYS,false));
		leftPass->getRenderState()->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
		path->addPass(leftPass);

		RenderPass::ptr rightPass=rightPath->takePass(0);
		rightPass->getRenderState()->setDepthState(DepthState(DepthState::DepthTest_ALWAYS,false));
		rightPass->getRenderState()->setRasterizerState(RasterizerState(RasterizerState::CullType_NONE));
		rightPass->getRenderState()->setBlendState(BlendState::Combination_COLOR_ADDITIVE);
		path->addPass(rightPass);
	}
	mMaterial->compile();

	mOverlay=new Node(scene);
	SpriteComponent::ptr sprite=new SpriteComponent(engine);
	sprite->setMaterial(mMaterial);
	mOverlay->attach(sprite);

	mOverlayCamera=new Camera(engine);
	mOverlayCamera->setProjectionOrtho(Math::HALF,-Math::HALF,Math::HALF,-Math::HALF,-Math::ONE,Math::ONE);
	mOverlayCamera->setLookAt(Math::NEG_Z_UNIT_VECTOR3,Math::ZERO_VECTOR3,Math::Y_UNIT_VECTOR3);

	setSeparation(Math::ONE);
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

void AnaglyphCamera::setScope(int scope){
	Camera::setScope(scope);

	mLeftCamera->setScope(scope);
	mRightCamera->setScope(scope);
}

void AnaglyphCamera::setClearFlags(int clearFlags){
	Camera::setClearFlags(clearFlags);

	mLeftCamera->setClearFlags(clearFlags);
	mRightCamera->setClearFlags(clearFlags);
}

void AnaglyphCamera::setClearColor(const Vector4 &clearColor){
	Camera::setClearColor(clearColor);

	mLeftCamera->setClearColor(clearColor);
	mRightCamera->setClearColor(clearColor);
}

void AnaglyphCamera::setDefaultState(RenderState *renderState){
	Camera::setDefaultState(renderState);

	mLeftCamera->setDefaultState(renderState);
	mRightCamera->setDefaultState(renderState);
}

void AnaglyphCamera::setRenderTarget(RenderTarget *target){
	Camera::setRenderTarget(target);

	mOverlayCamera->setRenderTarget(target);
}

void AnaglyphCamera::setSeparation(scalar separation){
	mSeparation=separation;

	updateWorldTransform();
}

void AnaglyphCamera::setLeftColor(const Vector4 &color){
	mLeftColor.set(color);
	tforeach(Material::PathCollection::const_iterator,it,mMaterial->getPaths()){
		RenderPath *path=*it;
		path->getPasses()[0]->getRenderState()->setMaterialState(MaterialState(mLeftColor));
	}
}

void AnaglyphCamera::setRightColor(const Vector4 &color){
	mRightColor.set(color);
	tforeach(Material::PathCollection::const_iterator,it,mMaterial->getPaths()){
		RenderPath *path=*it;
		path->getPasses()[1]->getRenderState()->setMaterialState(MaterialState(mRightColor));
	}
}

void AnaglyphCamera::render(RenderDevice *device,Scene *scene,Node *node){
	if(mProjectionType==ProjectionType_FOV){
		autoUpdateProjection(mRenderTarget!=NULL?mRenderTarget.get():device->getPrimaryRenderTarget());
	}

	mLeftCamera->render(device,scene,node);

	mRightCamera->render(device,scene,node);

	mOverlayCamera->render(device,scene,mOverlay);
}

void AnaglyphCamera::projectionUpdated(){
	Camera::projectionUpdated();

	mLeftCamera->setProjectionMatrix(mProjectionMatrix);
	mRightCamera->setProjectionMatrix(mProjectionMatrix);
}

void AnaglyphCamera::updateWorldTransform(){
	Camera::updateWorldTransform();

	scalar separation=mSeparation;

	mLeftCamera->setWorldMatrix(mWorldMatrix);
	{
		Vector3 position=mLeftCamera->getPosition();
		Math::madd(position,mRight,-separation,position);
		mLeftCamera->setPosition(position);
	}

	mRightCamera->setWorldMatrix(mWorldMatrix);
	{
		Vector3 position=mRightCamera->getPosition();
		Math::madd(position,mRight,separation,position);
		mRightCamera->setPosition(position);
	}
}

}
}
