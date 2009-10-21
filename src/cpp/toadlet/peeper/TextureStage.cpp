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

#include <toadlet/peeper/TextureStage.h>

namespace toadlet{
namespace peeper{

TextureStage::TextureStage():
	//mTexture,

	//mBlend,

	mSAddressMode(Texture::AddressMode_REPEAT),
	mTAddressMode(Texture::AddressMode_REPEAT),
	mRAddressMode(Texture::AddressMode_REPEAT),
	mAddressModeSpecified(false),

	mMinFilter(Texture::Filter_NEAREST),
	mMipFilter(Texture::Filter_NONE),
	mMagFilter(Texture::Filter_NEAREST),
	mFilterSpecified(false),

	mTexCoordIndex(0),

	//mTextureMatrix,
	mTextureMatrixIdentity(true)
{
}

TextureStage::TextureStage(const Texture::ptr &texture):
	//mTexture,

	//mBlend,

	mSAddressMode(Texture::AddressMode_REPEAT),
	mTAddressMode(Texture::AddressMode_REPEAT),
	mRAddressMode(Texture::AddressMode_REPEAT),
	mAddressModeSpecified(false),

	mMinFilter(Texture::Filter_NEAREST),
	mMipFilter(Texture::Filter_NONE),
	mMagFilter(Texture::Filter_NEAREST),
	mFilterSpecified(false),

	mTexCoordIndex(0),

	//mTextureMatrix,
	mTextureMatrixIdentity(true)
{
	mTexture=texture;
}
	
void TextureStage::setSAddressMode(Texture::AddressMode addressMode){
	mSAddressMode=addressMode;
	mAddressModeSpecified=true;
}

void TextureStage::setTAddressMode(Texture::AddressMode addressMode){
	mTAddressMode=addressMode;
	mAddressModeSpecified=true;
}

void TextureStage::setRAddressMode(Texture::AddressMode addressMode){
	mRAddressMode=addressMode;
	mAddressModeSpecified=true;
}

void TextureStage::setMinFilter(Texture::Filter minFilter){
	mMinFilter=minFilter;
	mFilterSpecified=true;
}

void TextureStage::setMipFilter(Texture::Filter mipFilter){
	mMipFilter=mipFilter;
	mFilterSpecified=true;
}

void TextureStage::setMagFilter(Texture::Filter magFilter){
	mMagFilter=magFilter;
	mFilterSpecified=true;
}

void TextureStage::setTextureMatrix(const Matrix4x4 &matrix){
	mTextureMatrix.set(matrix);
	mTextureMatrixIdentity=false;
}

}
}
