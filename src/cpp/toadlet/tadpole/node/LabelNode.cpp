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
#include <toadlet/tadpole/node/CameraNode.h>
#include <toadlet/tadpole/node/LabelNode.h>
#include <toadlet/tadpole/node/ParentNode.h>
#include <toadlet/tadpole/node/SceneNode.h>
#include <toadlet/tadpole/Engine.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace node{

TOADLET_NODE_IMPLEMENT(LabelNode,Categories::TOADLET_TADPOLE_NODE+".LabelNode");

LabelNode::LabelNode():super(),
	TOADLET_GIB_IMPLEMENT()

	//mFont,
	//mText,
	mPerspective(false),
	mAlignment(0),
	mNormalized(false),
	mWordWrap(false)
	//mSize,

	//mMaterial,
	//mVertexData,
	//mIndexData
{}

Node *LabelNode::create(Scene *scene){
	super::create(scene);

	setPerspective(true);
	setCameraAligned(true);
	setAlignment(Font::Alignment_BIT_HCENTER|Font::Alignment_BIT_VCENTER);
	setNormalized(true);
	setWordWrap(false);

	mMaterial=getEngine()->getMaterialManager()->createMaterial();
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

	updateLabel();
}

void LabelNode::setAlignment(int alignment){
	mAlignment=alignment;

	updateLabel();
}

void LabelNode::setNormalized(bool normalized){
	mNormalized=normalized;

	updateLabel();
}

void LabelNode::setWordWrap(bool wordWrap){
	mWordWrap=wordWrap;

	updateLabel();
}

void LabelNode::queueRenderable(SceneNode *scene,CameraNode *camera){
	if(mVertexData==NULL || mIndexData==NULL){
		return;
	}

	if(!mPerspective){
		Matrix4x4 &scale=cache_queueRenderable_scale.reset();
		Vector4 &point=cache_queueRenderable_point.reset();

		point.set(mWorldRenderTransform.at(0,3),mWorldRenderTransform.at(1,3),mWorldRenderTransform.at(2,3),Math::ONE);
		Math::mul(point,camera->getViewTransform());
		Math::mul(point,camera->getProjectionTransform());
		scale.setAt(0,0,point.w);
		scale.setAt(1,1,point.w);
		scale.setAt(2,2,point.w);

		Math::postMul(mWorldRenderTransform,scale);
	}

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

	// Update word wrap
	String text;
	if(mWordWrap){
		if(mNormalized){
			text=wordWrap(mFont,mFont->getPointSize(),mText);
		}
		else{
			text=wordWrap(mFont,Math::ONE,mText);
		}
	}
	else{
		text=mText;
	}

	// Update buffers
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
	mFont->updateVertexBufferForString(mVertexData->getVertexBuffer(0),text,Colors::WHITE,mAlignment,!mNormalized,true);
	mIndexData->setCount(length*6);

	// Update material
	Texture::ptr texture=mFont->getTexture();
	mMaterial->setTextureStage(0,TextureStage::ptr(new TextureStage(texture)));
	if((texture->getFormat()&Texture::Format_BIT_A)>0){
		mMaterial->setBlend(Blend::Combination_ALPHA);
	}
	else{
		mMaterial->setBlend(Blend::Combination_COLOR);
	}

	// Update bound
	if(mFont==NULL){
		mLocalBound.radius=0;
	}
	else if(mPerspective){
		int iw=mFont->getStringWidth(mText);
		int ih=mFont->getStringHeight(mText);

		scalar width,height;
		if(mNormalized){
			scalar pointSize=
			#if defined(TOADLET_FIXED_POINT)
				Math::fromFloat(mFont->getPointSize());
			#else
				mFont->getPointSize();
			#endif

			width=Math::div(Math::fromInt(iw),pointSize);
			height=Math::div(Math::fromInt(ih),pointSize);
		}
		else{
			width=Math::fromInt(iw);
			height=Math::fromInt(ih);
		}

		if(mAlignment==(Font::Alignment_BIT_HCENTER|Font::Alignment_BIT_VCENTER)){
			mLocalBound.radius=Math::sqrt(Math::square(width/2) + Math::square(height/2));
		}
		else{
			mLocalBound.radius=Math::sqrt(Math::square(width) + Math::square(height));
		}
	}
	else{
		mLocalBound.radius=-Math::ONE;
	}
}

String LabelNode::wordWrap(Font::ptr font,float width,const String &text){
	// Word wrapping algorithm
	String result=text;
	int spaceWidth=font->getStringWidth(" ");
	int spaceLeft=(int)width;
	int start=0;
	int end=result.find(' ');
	while(end!=String::npos){
		String word=result.substr(start,end-start);
		int wordWidth=font->getStringWidth(word);
		if(wordWidth>spaceLeft){
			result=result.substr(0,start)+(char)10+result.substr(start,result.length());
			spaceLeft=(int)width-wordWidth;
		}
		else{
			spaceLeft=spaceLeft-(wordWidth + spaceWidth);
		}

		if(end>=result.length()){
			break;
		}
		else{
			start=end+1;
			end=result.find(' ',start);
			if(end==String::npos){
				end=result.length();
			}
		}
	}

	return result;
}

}
}
}
