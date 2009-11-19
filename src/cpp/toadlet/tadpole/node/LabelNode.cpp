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
#include <toadlet/tadpole/node/LabelNode.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(LabelNode,"toadlet::tadpole::node::LabelNode");

LabelNode::LabelNode():super(),
	TOADLET_GIB_IMPLEMENT()

	mFont(NULL),
	mText(NULL),
	mPerspective(false),
	mAlignment(0),
	mPixelSpace(false),

	mMaterial(NULL),
	mVertexData(NULL),
	mIndexData(NULL)
{}

Node *LabelNode::create(Engine *engine){
	super::create(engine);

	setPerspective(true);
	setAlignment(Font::Alignment_BIT_HCENTER|Font::Alignment_BIT_VCENTER);
	setPixelSpace(false);

	mMaterial=engine->getMaterialManager()->createMaterial();
	mMaterial->retain();
	mMaterial->setFaceCulling(Renderer::FaceCulling_NONE);
	mMaterial->setDepthWrite(false);

	return this;
}

void LabelNode::destroy(){
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

	if(mFont!=NULL){
		mFont->release();
		mFont=NULL;
	}

	super::destroy();
}

void LabelNode::setFont(const Font::ptr &font){
	if(mFont!=NULL){
		mFont->release();
	}

	mFont=font;

	if(mFont!=NULL){
		mFont->retain();
	}

	updateLabel();
}

void LabelNode::setText(const String &text){
	mText=text;

	updateLabel();
}

void LabelNode::setPerspective(bool perspective){
	mPerspective=perspective;

	updateBound();
}

void LabelNode::setAlignment(int alignment){
	mAlignment=alignment;

	updateLabel();
}

void LabelNode::setPixelSpace(bool pixelSpace){
	mPixelSpace=pixelSpace;

	updateLabel();
}

void LabelNode::queueRenderable(Scene *scene){
	if(mVertexData==NULL || mIndexData==NULL){
		return;
	}

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

	Math::postMul(mVisualWorldTransform,scale);

#if defined(TOADLET_GCC_INHERITANCE_BUG)
	scene->queueRenderable(&renderable);
#else
	scene->queueRenderable(this);
#endif
}

void LabelNode::render(Renderer *renderer) const{
	renderer->renderPrimitive(mVertexData,mIndexData);
}

void LabelNode::updateLabel(){
	if(mFont==NULL){
		return;
	}

	String text=mText;
//	if(mWordWrap){
//		text=wordWrap(mFont,mWidth,text);
//	}

	int length=text.length();

	if(mVertexData==NULL || mVertexData->getVertexBuffer(0)->getSize()/4<length){
		int i,ix;

		VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::UsageFlags_DYNAMIC,Buffer::AccessType_WRITE_ONLY,mEngine->getVertexFormats().POSITION_TEX_COORD,length*4);
		mVertexData=VertexData::ptr(new VertexData(vertexBuffer));

		IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::UsageFlags_DYNAMIC,Buffer::AccessType_WRITE_ONLY,IndexBuffer::IndexFormat_UINT_16,length*6);
		mIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,length*6));

		{
			uint16 *data=(uint16*)indexBuffer->lock(Buffer::AccessType_WRITE_ONLY);
			for(i=0;i<length;i++){
				ix=i*6;
				data[ix+0]=i*4+0;
				data[ix+1]=i*4+1;
				data[ix+2]=i*4+3;
				data[ix+3]=i*4+3;
				data[ix+4]=i*4+1;
				data[ix+5]=i*4+2;
			}
			indexBuffer->unlock();
		}
	}

	mFont->updateVertexBufferForString(mVertexData->getVertexBuffer(0),text,Colors::WHITE,mAlignment,mPixelSpace);
	mIndexData->setCount(length*6);

	Texture::ptr texture=mFont->getTexture();
	mMaterial->setTextureStage(0,TextureStage::ptr(new TextureStage(texture)));
	if((texture->getFormat()&Texture::Format_BIT_A)>0){
		mMaterial->setBlend(Blend::Combination_ALPHA);
	}
	else{
		mMaterial->setBlend(Blend::Combination_COLOR);
	}

	updateBound();
}

void LabelNode::updateBound(){
	if(mFont==NULL){
		mBoundingRadius=0;
	}
	else if(mPerspective){
		int iw=mFont->getStringWidth(mText);
		int ih=mFont->getStringHeight(mText);

		scalar width,height;
		if(mPixelSpace){
			width=Math::fromInt(iw);
			height=Math::fromInt(ih);
		}
		else{
			scalar pointSize=
			#if defined(TOADLET_FIXED_POINT)
				Math::fromFloat(mFont->getPointSize();
			#else
				mFont->getPointSize();
			#endif

			width=Math::div(Math::fromInt(iw),pointSize);
			height=Math::div(Math::fromInt(ih),pointSize);
		}

		if(mAlignment==(Font::Alignment_BIT_HCENTER|Font::Alignment_BIT_VCENTER)){
			mBoundingRadius=Math::sqrt(Math::square(width/2) + Math::square(height/2));
		}
		else{
			mBoundingRadius=Math::sqrt(Math::square(width) + Math::square(height));
		}
	}
	else{
		mBoundingRadius=-Math::ONE;
	}
}

}
}
}
