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
#include <toadlet/tadpole/node/SpriteNode.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(SpriteNode,"toadlet::tadpole::node::SpriteNode");

SpriteNode::SpriteNode():super(),
	TOADLET_GIB_IMPLEMENT()

	mPerspective(false),
	mAlignment(0),
	mPixelSpace(false),

	mMaterial(NULL),
	mVertexData(NULL),
	mIndexData(NULL)
	//mAlignmentTransform
{}

Node *SpriteNode::create(Engine *engine){
	super::create(engine);

	setPerspective(true);
	setAlignment(Font::Alignment_BIT_HCENTER|Font::Alignment_BIT_VCENTER);
	setPixelSpace(false);

	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::UsageFlags_STATIC,Buffer::AccessType_WRITE_ONLY,mEngine->getVertexFormats().POSITION_TEX_COORD,4);
	mVertexData=VertexData::ptr(new VertexData(vertexBuffer));
	{
		vba.lock(vertexBuffer,Buffer::AccessType_WRITE_ONLY);

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

	return this;
}

void SpriteNode::destroy(){
	if(mVertexData!=NULL){
		mVertexData->destroy();
		mVertexData=NULL;
	}
	
	if(mIndexData!=NULL){
		mIndexData->destroy();
		mIndexData=NULL;
	}
	
	if(mMaterial!=NULL){
		mMaterial->release();
		mMaterial=NULL;
	}

	super::destroy();
}

void SpriteNode::setMaterial(const egg::String &name){
	setMaterial(mEngine->getMaterialManager()->findMaterial(name));
}

void SpriteNode::setMaterial(Material::ptr material){
	if(mMaterial!=NULL){
		mMaterial->release();
	}

	mMaterial=material;

	if(mMaterial!=NULL){
		mMaterial->retain();
	}

	// TODO: Move these so they are set externally, perhaps in a material manager loadspritematerial or something
	//  Then we'd just have a debug check here to look for faceculling perhaps...?
	mMaterial->setFaceCulling(Renderer::FaceCulling_NONE);
	mMaterial->setDepthWrite(false);
	int i;
	for(i=0;i<mMaterial->getNumTextureStages();++i){
		mMaterial->getTextureStage(i)->setUAddressMode(TextureStage::AddressMode_CLAMP_TO_EDGE);
		mMaterial->getTextureStage(i)->setVAddressMode(TextureStage::AddressMode_CLAMP_TO_EDGE);
		mMaterial->getTextureStage(i)->setWAddressMode(TextureStage::AddressMode_CLAMP_TO_EDGE);
	}

	updateSprite();
}

void SpriteNode::setPerspective(bool perspective){
	mPerspective=perspective;

	updateBound();
}

void SpriteNode::setAlignment(int alignment){
	mAlignment=alignment;

	updateSprite();
}

void SpriteNode::setPixelSpace(bool pixelSpace){
	mPixelSpace=pixelSpace;

	updateSprite();
}

void SpriteNode::queueRenderable(Scene *scene){
	Matrix4x4 &scale=cache_queueRenderable_scale.reset();
	Vector4 &point=cache_queueRenderable_point.reset();
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
	if(mPerspective){
		scale.setAt(0,0,mScale.x);
		scale.setAt(1,1,mScale.y);
		scale.setAt(2,2,mScale.z);
	}
	else{
		point.set(mVisualWorldTransform.at(0,3),mVisualWorldTransform.at(1,3),mVisualWorldTransform.at(2,3),Math::ONE);
		Math::mul(point,viewTransform);
		Math::mul(point,projectionTransform);
		scale.setAt(0,0,Math::mul(point.w,mScale.x));
		scale.setAt(1,1,Math::mul(point.w,mScale.y));
		scale.setAt(2,2,Math::mul(point.w,mScale.z));
	}

	if(mAlignment!=(Font::Alignment_BIT_HCENTER|Font::Alignment_BIT_VCENTER) || mPixelSpace){
		Math::postMul(mVisualWorldTransform,mAlignmentTransform);
	}
	Math::postMul(mVisualWorldTransform,scale);

#if defined(TOADLET_GCC_INHERITANCE_BUG)
	scene->queueRenderable(&renderable);
#else
	scene->queueRenderable(this);
#endif
}

void SpriteNode::render(Renderer *renderer) const{
	renderer->renderPrimitive(mVertexData,mIndexData);
}

void SpriteNode::updateSprite(){
	scalar x=0,y=0;
	if((mAlignment&Font::Alignment_BIT_LEFT)>0){
		x=Math::HALF;
	}
	else if((mAlignment&Font::Alignment_BIT_RIGHT)>0){
		x=-Math::HALF;
	}
	if((mAlignment&Font::Alignment_BIT_BOTTOM)>0){
		y=Math::HALF;
	}
	else if((mAlignment&Font::Alignment_BIT_TOP)>0){
		y=-Math::HALF;
	}
	Math::setMatrix4x4FromTranslate(mAlignmentTransform,x,y,0);

	if(mPixelSpace && mMaterial!=NULL){
		TextureStage::ptr textureStage=mMaterial->getTextureStage(0);
		if(textureStage!=NULL){
			Texture::ptr texture=textureStage->getTexture();
			if(texture!=NULL){
				scalar width=texture->getWidth();
				scalar height=texture->getHeight();

				Matrix4x4 pixelScale;
				Math::setMatrix4x4FromScale(pixelScale,width,height,Math::ONE);
				Math::postMul(mAlignmentTransform,pixelScale);
			}
		}
	}

	updateBound();
}

void SpriteNode::updateBound(){
	if(mPerspective){
		if(mAlignment==(Font::Alignment_BIT_HCENTER|Font::Alignment_BIT_VCENTER)){
			mBoundingRadius=Math::sqrt(Math::square(mScale.x/2) + Math::square(mScale.y/2));
		}
		else{
			mBoundingRadius=Math::sqrt(Math::square(mScale.x) + Math::square(mScale.y));
		}
	}
	else{
		mBoundingRadius=-Math::ONE;
	}
}

}
}
}
