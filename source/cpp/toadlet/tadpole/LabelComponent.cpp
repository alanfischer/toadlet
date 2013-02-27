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
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/RenderableSet.h>
#include <toadlet/tadpole/RenderManager.h>
#include <toadlet/tadpole/LabelComponent.h>

namespace toadlet{
namespace tadpole{

LabelComponent::LabelComponent(Engine *engine):
	//mFont,
	//mText,
	mAlignment(0),
	mNormalized(false),
	mWordWrap(false),
	mWidth(0),
	mRendered(false)

	//mMaterial,
	//mSharedRenderState,
	//mVertexData,
	//mIndexData
{
	mEngine=engine;
	mRendered=true;
	mBound=new Bound();

	setFont(mEngine->getFontManager()->getDefaultFont());
	setAlignment(Font::Alignment_BIT_HCENTER|Font::Alignment_BIT_VCENTER);
	setNormalized(true);
	setWordWrap(false);
}

void LabelComponent::destroy(){
	if(mVertexData!=NULL){
		mVertexData->destroy();
		mVertexData=NULL;
	}
	
	if(mIndexData!=NULL){
		mIndexData->destroy();
		mIndexData=NULL;
	}
	
	if(mSharedRenderState!=NULL){
		mSharedRenderState->destroy();
		mSharedRenderState=NULL;
	}

	mMaterial=NULL;
	mFont=NULL;

	BaseComponent::destroy();
}

void LabelComponent::parentChanged(Node *node){
	if(mParent!=NULL){
		mParent->visibleRemoved(this);
	}

	BaseComponent::parentChanged(node);
	
	if(mParent!=NULL){
		mParent->visibleAttached(this);
	}
}

void LabelComponent::setFont(Font *font){
	mFont=font;

	if(mFont!=NULL){
		mMaterial=mEngine->createFontMaterial(mFont);
		mMaterial->setModelMatrixFlags(Material::MatrixFlag_CAMERA_ALIGNED);

		if(mSharedRenderState!=NULL){
			mMaterial=mEngine->getMaterialManager()->createSharedMaterial(mMaterial,mSharedRenderState);
		}
	}

	updateLabel();
}

void LabelComponent::setText(const String &text){
	mText=text;

	updateLabel();
}

void LabelComponent::setAlignment(int alignment){
	mAlignment=alignment;

	updateLabel();
}

void LabelComponent::setNormalized(bool normalized){
	mNormalized=normalized;

	updateLabel();
}

void LabelComponent::setWordWrap(bool wordWrap){
	mWordWrap=wordWrap;

	updateLabel();
}

RenderState::ptr LabelComponent::getSharedRenderState(){
	if(mSharedRenderState==NULL){
		mSharedRenderState=mEngine->getMaterialManager()->createRenderState();
		if(mMaterial!=NULL){
			mEngine->getMaterialManager()->modifyRenderState(mSharedRenderState,mMaterial->getPass()->getRenderState());
			mMaterial=mEngine->getMaterialManager()->createSharedMaterial(mMaterial,mSharedRenderState);
		}
	}
	return mSharedRenderState;
}

void LabelComponent::gatherRenderables(Camera *camera,RenderableSet *set){
	if(mRendered==false || mVertexData==NULL || mIndexData==NULL || mMaterial==NULL){
		return;
	}

	set->queueRenderable(this);
}

void LabelComponent::render(RenderManager *manager) const{
	manager->getDevice()->renderPrimitive(mVertexData,mIndexData);
}

void LabelComponent::updateLabel(){
	int i,ix;

	if(mFont==NULL){
		return;
	}

	// Update word wrap
	String text;
	if(mWordWrap){
//		if(mNormalized){
//			text=wordWrap(mFont,mFont->getPointSize(),mText);
//		}
//		else{
			text=wordWrap(mFont,mWidth,mText);
//		}
	}
	else{
		text=mText;
	}

	// Update buffers
	if(mVertexData!=NULL){
		mVertexData->destroy();
		mVertexData=NULL;
	}

	int length=text.length();
	if(length>0){
		VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_COLOR_TEX_COORD,length*4);
		mVertexData=new VertexData(vertexBuffer);

		IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,length*6);
		mIndexData=new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,length*6);

		{
			uint16 *data=(uint16*)indexBuffer->lock(Buffer::Access_BIT_WRITE);
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

		mFont->updateVertexBufferForString(mVertexData->getVertexBuffer(0),text,Math::ONE_VECTOR4,mAlignment,!mNormalized,true);
		mIndexData->setCount(length*6);
	}

	// Update bound
	if(mFont==NULL){
		mBound->reset();
	}
	else{
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
			mBound->set(Math::sqrt(Math::square(width/2) + Math::square(height/2)));
		}
		else{
			mBound->set(Math::sqrt(Math::square(width) + Math::square(height)));
		}
	}
}

String LabelComponent::wordWrap(Font *font,float width,const String &text){
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
