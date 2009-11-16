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
	mScaled(false),

	mMaterial(NULL),
	mVertexData(NULL),
	mIndexData(NULL)
{}

Node *LabelNode::create(Engine *engine){
	super::create(engine);

	mFont=NULL;
	mMaterial=NULL;
	mVertexData=NULL;
	mIndexData=NULL;
	
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

	super::destroy();
}

void LabelNode::setFont(const Font::ptr &font){
	mFont=font;
	rebuild();
}

void LabelNode::setText(const String &text){
	mText=text;
	rebuild();
}

void LabelNode::setScaled(bool scaled){
	mScaled=scaled;
}

void LabelNode::queueRenderables(Scene *scene){
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
		scale.setAt(0,0,Math::ONE);
		scale.setAt(1,1,Math::ONE);
		scale.setAt(2,2,Math::ONE);
	}
	else{
		point.set(mVisualWorldTransform.at(0,3),mVisualWorldTransform.at(1,3),mVisualWorldTransform.at(2,3),Math::ONE);
		Math::mul(point,viewTransform);
		Math::mul(point,projectionTransform);
		scale.setAt(0,0,point.w);
		scale.setAt(1,1,point.w);
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

void LabelNode::render(Renderer *renderer) const{
	renderer->renderPrimitive(mVertexData,mIndexData);
}

void LabelNode::rebuild(){
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

	int alignment=Font::Alignment_BIT_VCENTER|Font::Alignment_BIT_HCENTER;
	mFont->updateVertexBufferForString(mVertexData->getVertexBuffer(0),text,Colors::WHITE,alignment);
	mIndexData->setCount(length*6);

	Texture::ptr texture=mFont->getTexture();
	mMaterial->setTextureStage(0,TextureStage::ptr(new TextureStage(texture)));
	if((texture->getFormat()&Texture::Format_BIT_A)>0){
		mMaterial->setBlend(Blend::Combination_ALPHA);
	}
	else{
		mMaterial->setBlend(Blend::Combination_COLOR);
	}
/*
	if(scaled){
		scalar hw=width/2;
		scalar hh=height/2;
		mBoundingRadius=Math::sqrt(Math::square(hw) + Math::square(hh));
	}
	else{
		mBoundingRadius=-Math::ONE;
	}
*/
}

}
}
}
