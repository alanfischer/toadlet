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

#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/peeper/Colors.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/widget/LabelWidget.h>
#include <toadlet/tadpole/widget/ParentWidget.h>
#include <toadlet/tadpole/widget/Window.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace widget{

// TODO: The widget system needs to be reworked a lot.

//  It should set dirty flags for when things need to be rebuild.

//  Calling parent->layout() every time things change gets to be quite slow, so thats removed as of now.
LabelWidget::LabelWidget(Engine *engine):RenderableWidget(engine),
	//mText,
	//mFont,
	mColor(Colors::WHITE),
	mWordWrap(true)

	//mVertexData,
	//mIndexData
{
	mType|=Type_LABEL;

	mTextureStage=TextureStage::ptr(new TextureStage());
}

LabelWidget::~LabelWidget(){
}

void LabelWidget::setText(const String &text){
	mText=text;

	if(mParent!=NULL){
		mParent->layout();
	}

	rebuild();
}

void LabelWidget::setFont(Font::ptr font){
	mFont=font;

	if(mParent!=NULL){
		mParent->layout();
	}

	rebuild();
}

void LabelWidget::setWordWrap(bool wordWrap){
	mWordWrap=wordWrap;

	if(mParent!=NULL){
//		mParent->layout();
	}
}

void LabelWidget::parentChanged(Widget *widget,ParentWidget *oldParent,ParentWidget *newParent){
	if(oldParent==mWindow){
		mWindow=NULL;
	}

	if(newParent!=NULL && newParent->instanceOf(Type_WINDOW)){
		mWindow=shared_static_cast<Window>(ParentWidget::ptr(newParent));
	}

	Super::parentChanged(widget,oldParent,newParent);
}

void LabelWidget::setSize(int width,int height){
	Super::setSize(width,height);

	rebuild();
}

void LabelWidget::getDesiredSize(int size[]){
	Font::ptr font=mFont;
	if(font==NULL && mWindow!=NULL){
		font=mWindow->getDefaultFont();
	}
	if(font==NULL){
		size[0]=0;
		size[1]=0;
		return;
	}

	int width=0;
	int height=0;
	String text=mText;
	if(mWordWrap){
		text=wordWrap(font,size[0],text);
	}
	int start=0;
	int i;
	for(i=0;i<text.length();++i){
		if(text[i]==(char)10){
			int w=font->getStringWidth(text.substr(start,i-start));
			if(w>width) width=w;
			height+=font->getHeight();
			start=i+1;
		}
	}

	if(height==0){
		width=font->getStringWidth(text);
	}
	height+=font->getHeight();

	size[0]=width;
	size[1]=height;
}

void LabelWidget::render(Renderer *renderer){
	if(mVertexData==NULL || mIndexData==NULL){
		return;
	}

	renderer->setBlend(mBlend);
	renderer->setTextureStage(0,mTextureStage);

	renderer->renderPrimitive(mVertexData,mIndexData);
}

String LabelWidget::wordWrap(Font::ptr font,int width,String text){
	// Word wrapping algorithm
	int spaceWidth=font->getStringWidth(" ");
	int spaceLeft=width;
	int start=0;
	int end=text.find(' ');
	while(end!=String::npos){
		String word=text.substr(start,end-start);
		int wordWidth=font->getStringWidth(word);
		if(wordWidth>spaceLeft){
			text=text.substr(0,start)+(char)10+text.substr(start,text.length());
			spaceLeft=width-wordWidth;
		}
		else{
			spaceLeft=spaceLeft-(wordWidth + spaceWidth);
		}

		if(end>=text.length()){
			break;
		}
		else{
			start=end+1;
			end=text.find(' ',start);
			if(end==String::npos){
				end=text.length();
			}
		}
	}

	return text;
}

void LabelWidget::rebuild(){
	Font::ptr font=mFont;
	if(font==NULL && mWindow!=NULL){
		font=mWindow->getDefaultFont();
	}
	if(font==NULL){
		return;
	}

	String text=mText;
	if(mWordWrap){
		text=wordWrap(font,mWidth,text);
	}

	int length=text.length();

	if(mVertexData==NULL || mVertexData->getVertexBuffer(0)->getSize()/4<length){
		int i,ix;

		VertexBuffer::ptr vertexBuffer=mEngine->loadVertexBuffer(VertexBuffer::ptr(new VertexBuffer(Buffer::UsageType_DYNAMIC,Buffer::AccessType_WRITE_ONLY,mEngine->getVertexFormats().POSITION_COLOR_TEX_COORD,length*4)));
		// Any time our contents would be lost, should be followed up with a resize so they should get restored
		// realistically I should be able to ensure this in Window somehow, but for now we'll just trust its the truth
		// TODO: Why if I turn it off in BorderWidget does it not work?  Maybe i'm incorrect?
		vertexBuffer->setRememberContents(false);
		mVertexData=VertexData::ptr(new VertexData(vertexBuffer));

		IndexBuffer::ptr indexBuffer=mEngine->loadIndexBuffer(IndexBuffer::ptr(new IndexBuffer(Buffer::UsageType_STATIC,Buffer::AccessType_WRITE_ONLY,IndexBuffer::IndexFormat_UINT_16,length*6)));
		mIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,length*6));

		{
			uint16 *data=(uint16*)indexBuffer->lock(Buffer::LockType_WRITE_ONLY);
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

	font->updateVertexBufferForString(mVertexData->getVertexBuffer(0),text,mColor,Font::Alignment_BIT_LEFT|Font::Alignment_BIT_TOP);
	mIndexData->setCount(length*6);

	Texture::ptr texture=font->getTexture();
	mTextureStage->setTexture(texture);
	if((texture->getFormat()&Texture::Format_BIT_A)>0){
		mBlend=Blend::Combination_ALPHA;
	}
	else{
		mBlend=Blend::Combination_COLOR;
	}
}

}
}
}

