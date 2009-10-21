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
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/peeper/Colors.h>
#include <toadlet/peeper/TextureStage.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/widget/SpriteWidget.h>
#include <toadlet/tadpole/widget/ParentWidget.h>
#include <toadlet/tadpole/widget/Window.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace widget{

SpriteWidget::SpriteWidget(Engine *engine):RenderableWidget(engine),
	//mTexture,
	//mAnimatedTexture,
	//mTextureMatrix,

	mFrame(0)

	//mVertexData,
	//mIndexData,
	//mTextureStage
{
	mType|=Type_SPRITE;

	VertexBuffer::ptr vertexBuffer=engine->loadVertexBuffer(VertexBuffer::ptr(new VertexBuffer(Buffer::UsageType_STATIC,Buffer::AccessType_WRITE_ONLY,mEngine->getVertexFormats().POSITION_TEX_COORD,4)));
	mVertexData=VertexData::ptr(new VertexData(vertexBuffer));
	{
		VertexBufferAccessor vba;
		vba.lock(vertexBuffer,Buffer::LockType_WRITE_ONLY);

		vba.set3(	0,0, 0,Math::ONE,0);
		vba.set2(	0,1, 0,0);

		vba.set3(	1,0, Math::ONE,Math::ONE,0);
		vba.set2(	1,1, Math::ONE,0);

		vba.set3(	2,0, 0,0,0);
		vba.set2(	2,1, 0,Math::ONE);

		vba.set3(	3,0, Math::ONE,0,0);
		vba.set2(	3,1, Math::ONE,Math::ONE);

		vba.unlock();
	}

	mIndexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,0,4));
}

SpriteWidget::~SpriteWidget(){
}

void SpriteWidget::load(scalar fractionalWidth,scalar fractionalHeight,const String &name){
	load(fractionalWidth,fractionalHeight,mEngine->getTexture(name));
}

void SpriteWidget::load(scalar fractionalWidth,scalar fractionalHeight,const Texture::ptr &texture){
	mTexture=texture;

	if(mTexture==NULL){
		Error::invalidParameters(Categories::TOADLET_TADPOLE,
			"Invalid Texture");
		return;
	}

	if(mTexture->getType()==Texture::Type_ANIMATED){
		mAnimatedTexture=shared_static_cast<AnimatedTexture>(texture);
	}
	else{
		mAnimatedTexture=NULL;
		mTextureMatrix.set(Math::IDENTITY_MATRIX4X4);
		TextureStage::ptr textureStage(new TextureStage(mTexture));
		textureStage->setSAddressMode(Texture::AddressMode_CLAMP_TO_EDGE);
		textureStage->setTAddressMode(Texture::AddressMode_CLAMP_TO_EDGE);
		textureStage->setTextureMatrix(mTextureMatrix);
		mTextureStage=textureStage;
	}

	setFractionalSize(fractionalWidth,fractionalHeight,false,true);

	updateFrame();
}

void SpriteWidget::setFrame(int frame){
	mFrame=frame;
	updateFrame();
}

int SpriteWidget::getNumFrames() const{
	int numFrames=1;
	if(mAnimatedTexture!=NULL){
		numFrames=mAnimatedTexture->getNumFrames();
	}
	return numFrames;
}

void SpriteWidget::render(Renderer *renderer){
	renderer->setTextureStage(0,mTextureStage);

	renderer->renderPrimitive(mVertexData,mIndexData);
}

void SpriteWidget::updateFrame(){
	if(mAnimatedTexture!=NULL){
		mAnimatedTexture->getMatrix4x4ForFrame(mFrame,mTextureMatrix);
		TextureStage::ptr textureStage(new TextureStage(mAnimatedTexture->getTextureForFrame(mFrame)));
		textureStage->setSAddressMode(Texture::AddressMode_CLAMP_TO_EDGE);
		textureStage->setTAddressMode(Texture::AddressMode_CLAMP_TO_EDGE);
		textureStage->setTextureMatrix(mTextureMatrix);
		mTextureStage=textureStage;
	}
}

}
}
}

