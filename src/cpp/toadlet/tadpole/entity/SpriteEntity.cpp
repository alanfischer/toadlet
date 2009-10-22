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

#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/entity/SpriteEntity.h>
#include <toadlet/tadpole/entity/ParentEntity.h>
#include <toadlet/tadpole/entity/CameraEntity.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace entity{

SpriteEntity::SpriteEntity():RenderableEntity(),
#if defined(TOADLET_GCC_INHERITANCE_BUG)
	renderable(this),
#endif
	
	mFrame(0),
	mWidth(0),
	mHeight(0),
	mScaled(false)
{}

Entity *SpriteEntity::create(Engine *engine){
	super::create(engine);

	mTexture=NULL;
//	mAnimatedTexture=NULL;
	mTextureMatrix.reset();

	mFrame=0;
	mWidth=0;
	mHeight=0;
	mScaled=true;

	mMaterial=NULL;
	mVertexData=NULL;
	mIndexData=NULL;

	VertexBuffer::ptr vertexBuffer=engine->loadVertexBuffer(VertexBuffer::ptr(new VertexBuffer(Buffer::UsageType_STATIC,Buffer::AccessType_WRITE_ONLY,mEngine->getVertexFormats().POSITION_TEX_COORD,4)));
	mVertexData=VertexData::ptr(new VertexData(vertexBuffer));
	{
		vba.lock(vertexBuffer,Buffer::LockType_WRITE_ONLY);

		vba.set3(0,0, -Math::HALF,Math::HALF,0);
		vba.set2(0,1, 0,0);

		vba.set3(1,0, Math::HALF,Math::HALF,0);
		vba.set2(1,1, Math::ONE,0);

		vba.set3(2,0, -Math::HALF,-Math::HALF,0);
		vba.set2(2,1, 0,Math::ONE);

		vba.set3(3,0, Math::HALF,-Math::HALF,0);
		vba.set2(3,1, Math::ONE,Math::ONE);

		vba.unlock();
	}

	mIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,4));

	mMaterial=Material::ptr(new Material());
	mMaterial->setFaceCulling(Renderer::FaceCulling_NONE);
	mMaterial->setDepthWrite(false);

	return this;
}

void SpriteEntity::destroy(){
	if(mVertexData!=NULL){
//		mVertexData->destroy();
		mVertexData=NULL;
	}
	
	if(mIndexData!=NULL){
//		mIndexData->destroy();
		mIndexData=NULL;
	}
	
	if(mMaterial!=NULL){
		mMaterial=NULL;
	}

	super::destroy();
}

void SpriteEntity::load(scalar width,scalar height,bool scaled,const String &name){
	load(width,height,scaled,mEngine->getTexture(name));
}

void SpriteEntity::load(scalar width,scalar height,bool scaled,Texture::ptr texture){
	mTexture=texture;

	if(mTexture==NULL){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"Invalid Texture");
		return;
	}

//	if(mTexture->getType()==Texture::Type_ANIMATED){
//		mAnimatedTexture=shared_static_cast<AnimatedTexture>(texture);
//	}
//	else{
//		mAnimatedTexture=NULL;
		mTextureMatrix.set(Math::IDENTITY_MATRIX4X4);
		TextureStage::ptr textureStage(new TextureStage(mTexture));
		textureStage->setSAddressMode(TextureStage::AddressMode_CLAMP_TO_EDGE);
		textureStage->setTAddressMode(TextureStage::AddressMode_CLAMP_TO_EDGE);
		textureStage->setTextureMatrix(mTextureMatrix);
		mMaterial->setTextureStage(0,textureStage);
//	}

	if(scaled){
		scalar hw=width/2;
		scalar hh=height/2;
		mBoundingRadius=Math::sqrt(Math::square(hw) + Math::square(hh));
	}
	else{
		mBoundingRadius=-Math::ONE;
	}

	mWidth=width;
	mHeight=height;
	mScaled=scaled;

	updateFrame();
}

void SpriteEntity::setFrame(int frame){
	mFrame=frame;
	updateFrame();
}

int SpriteEntity::getNumFrames() const{
	int numFrames=1;
//	if(mAnimatedTexture!=NULL){
//		numFrames=mAnimatedTexture->getNumFrames();
//	}
	return numFrames;
}

void SpriteEntity::queueRenderables(Scene *scene){
	Matrix4x4 &scale=cache_queueRenderables_scale.reset();
	Vector4 &point=cache_queueRenderables_point.reset();
	const Matrix4x4 &viewTransform=scene->getCamera()->getViewTransform();
	const Matrix4x4 &projectionTransform=scene->getCamera()->getProjectionTransform();

	if(mIdentityTransform){
		mVisualWorldTransform.set(mParent->getVisualWorldTransform());
	}
	else{
		Math::mul(mVisualWorldTransform,mParent->getVisualWorldTransform(),mVisualTransform);
	}

	mVisualWorldTransform.setAt(0,0,viewTransform.at(0,0));
	mVisualWorldTransform.setAt(1,0,viewTransform.at(0,1));
	mVisualWorldTransform.setAt(2,0,viewTransform.at(0,2));
	mVisualWorldTransform.setAt(0,1,viewTransform.at(1,0));
	mVisualWorldTransform.setAt(1,1,viewTransform.at(1,1));
	mVisualWorldTransform.setAt(2,1,viewTransform.at(1,2));
	mVisualWorldTransform.setAt(0,2,viewTransform.at(2,0));
	mVisualWorldTransform.setAt(1,2,viewTransform.at(2,1));
	mVisualWorldTransform.setAt(2,2,viewTransform.at(2,2));

	scale.reset();
	if(mScaled){
		scale.setAt(0,0,mWidth);
		scale.setAt(1,1,mHeight);
		scale.setAt(2,2,Math::ONE);
	}
	else{
		point.set(mVisualWorldTransform.at(0,3),mVisualWorldTransform.at(1,3),mVisualWorldTransform.at(2,3),Math::ONE);
		Math::mul(point,viewTransform);
		Math::mul(point,projectionTransform);
		scale.setAt(0,0,Math::mul(point.w,mWidth));
		scale.setAt(1,1,Math::mul(point.w,mHeight));
		scale.setAt(2,2,Math::ONE);
	}

	/// @todo  Modify this so we can have sprites of arbitrary orientations, and also have the scale passed by the
	///  transform matrix, and not having to just grab the entities scale
	scale.setAt(0,0,Math::mul(scale.at(0,0),mScale.x));
	scale.setAt(1,1,Math::mul(scale.at(1,1),mScale.y));

	Math::postMul(mVisualWorldTransform,scale);

#if defined(TOADLET_GCC_INHERITANCE_BUG)
	scene->queueRenderable(&renderable);
#else
	scene->queueRenderable(this);
#endif
}

void SpriteEntity::render(Renderer *renderer) const{
	renderer->renderPrimitive(mVertexData,mIndexData);
}

void SpriteEntity::updateFrame(){
//	if(mAnimatedTexture!=NULL){
//		mAnimatedTexture->getMatrix4x4ForFrame(mFrame,mTextureMatrix);
//		TextureStage::ptr textureStage(new TextureStage(mAnimatedTexture->getTextureForFrame(mFrame)));
//		textureStage->setSAddressMode(Texture::AddressMode_CLAMP_TO_EDGE);
//		textureStage->setTAddressMode(Texture::AddressMode_CLAMP_TO_EDGE);
//		textureStage->setTextureMatrix(mTextureMatrix);
//		mMaterial->setTextureStage(0,textureStage);
//	}
}

}
}
}
