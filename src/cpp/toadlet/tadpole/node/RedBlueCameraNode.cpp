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
#include <toadlet/tadpole/node/RedBlueCameraNode.h>

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(RedBlueCameraNode,Categories::TOADLET_TADPOLE_NODE+".RedBlueCameraNode");

RedBlueCameraNode::RedBlueCameraNode():super(),mSeparation(Math::ONE){
}

Node *RedBlueCameraNode::create(Scene *scene){
	super::create(scene);

	int flags=Texture::Usage_BIT_RENDERTARGET;
	int size=1024;//Math::nextPowerOf2((getWidth()<getHeight()?getWidth():getHeight())/2);
	int format=Texture::Format_L_8;

	mRedTexture=mEngine->getTextureManager()->createTexture(flags,Texture::Dimension_D2,format,size,size,0,1);
	mRedRenderTarget=mEngine->getTextureManager()->createSurfaceRenderTarget();
	mRedRenderTarget->attach(mRedTexture->getMipSurface(0,0),SurfaceRenderTarget::Attachment_COLOR_0);

	mBlueTexture=mEngine->getTextureManager()->createTexture(flags,Texture::Dimension_D2,format,size,size,0,1);
	mBlueRenderTarget=mEngine->getTextureManager()->createSurfaceRenderTarget();
	mBlueRenderTarget->attach(mBlueTexture->getMipSurface(0,0),SurfaceRenderTarget::Attachment_COLOR_0);

	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,4);
	VertexBufferAccessor vba(vertexBuffer);
	vba.set3(0,0,0,0,0);
	vba.set3(0,1,0,0,0);
	vba.set2(0,2,0,1);
	vba.set3(1,0,1,0,0);
	vba.set3(1,1,0,0,0);
	vba.set2(1,2,1,1);
	vba.set3(2,0,0,1,0);
	vba.set3(2,1,0,0,0);
	vba.set2(2,2,0,0);
	vba.set3(3,0,1,1,0);
	vba.set3(3,1,0,0,0);
	vba.set2(3,2,1,0);
	vba.unlock();
	mVertexData=VertexData::ptr(new VertexData(vertexBuffer));
	mIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,vertexBuffer->getSize()));

	mRedMaterial=mEngine->getMaterialManager()->createMaterial();
	mRedMaterial->setDepthTest(Renderer::DepthTest_NONE);
	mRedMaterial->setLighting(true);
	mRedMaterial->setLightEffect(LightEffect(Color(2.0,0,0,1)));
	mRedMaterial->setTextureStage(0,TextureStage::ptr(new TextureStage(mRedTexture)));
	mRedMaterial->retain();

	mBlueMaterial=mEngine->getMaterialManager()->createMaterial();
	mBlueMaterial->setDepthTest(Renderer::DepthTest_NONE);
	mBlueMaterial->setLighting(true);
	mBlueMaterial->setLightEffect(LightEffect(Color(0,0,2.0,1)));
	mBlueMaterial->setBlend(Blend::Combination_COLOR_ADDITIVE);
	mBlueMaterial->setTextureStage(0,TextureStage::ptr(new TextureStage(mBlueTexture)));
	mBlueMaterial->retain();

	return this;
}

void RedBlueCameraNode::destroy(){
	mRedRenderTarget->destroy();
	mBlueRenderTarget->destroy();

	mRedMaterial->release();
	mBlueMaterial->release();

	super::destroy();
}

void RedBlueCameraNode::render(Renderer *renderer,Node *node){
	Vector3 translate=getTranslate();

	renderer->setRenderTarget(mRedRenderTarget);
	translate.x-=mSeparation;
	setTranslate(translate);
	updateWorldTransform();
		mScene->render(renderer,this,NULL);
	renderer->swap();

	renderer->setRenderTarget(mBlueRenderTarget);
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

	Matrix4x4 matrix;
	Math::setMatrix4x4FromOrtho(matrix,0,1,0,1,-1,1);
	renderer->setProjectionMatrix(matrix);
	renderer->setViewMatrix(Math::IDENTITY_MATRIX4X4);
	renderer->setModelMatrix(Math::IDENTITY_MATRIX4X4);
	mRedMaterial->setupRenderer(renderer);
	renderer->renderPrimitive(mVertexData,mIndexData);
	mBlueMaterial->setupRenderer(renderer);
	renderer->renderPrimitive(mVertexData,mIndexData);
}

}
}
}
