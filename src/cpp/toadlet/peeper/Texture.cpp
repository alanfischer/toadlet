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

#include <toadlet/peeper/Texture.h>
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

Texture::Texture():
	mType(Type_NORMAL),

	mDimension(Dimension_UNKNOWN),
	mFormat(Format_UNKNOWN),
	mWidth(0),
	mHeight(0),
	mDepth(0),

	mAutoGenerateMipMaps(true),

	mSAddressMode(AddressMode_REPEAT),
	mTAddressMode(AddressMode_REPEAT),
	mRAddressMode(AddressMode_REPEAT),

	mMinFilter(Filter_NEAREST),
	mMipFilter(Filter_NONE),
	mMagFilter(Filter_NEAREST),

	//mName,

	//mImage,

	mTexturePeer(NULL),
	mOwnsTexturePeer(false)
{
}

Texture::Texture(Image::ptr image):
	mType(Type_NORMAL),

	mDimension(Dimension_UNKNOWN),
	mFormat(Format_UNKNOWN),
	mWidth(0),
	mHeight(0),
	mDepth(0),

	mAutoGenerateMipMaps(true),

	mSAddressMode(AddressMode_REPEAT),
	mTAddressMode(AddressMode_REPEAT),
	mRAddressMode(AddressMode_REPEAT),

	mMinFilter(Filter_NEAREST),
	mMipFilter(Filter_NONE),
	mMagFilter(Filter_NEAREST),

	//mName,

	//mImage,

	mTexturePeer(NULL),
	mOwnsTexturePeer(false)
{
	setImage(image);
}

Texture::Texture(Renderer *renderer,Image::ptr image):
	mType(Type_NORMAL),

	mDimension(Dimension_UNKNOWN),
	mFormat(Format_UNKNOWN),
	mWidth(0),
	mHeight(0),
	mDepth(0),

	mAutoGenerateMipMaps(true),

	mSAddressMode(AddressMode_REPEAT),
	mTAddressMode(AddressMode_REPEAT),
	mRAddressMode(AddressMode_REPEAT),

	mMinFilter(Filter_NEAREST),
	mMipFilter(Filter_NONE),
	mMagFilter(Filter_NEAREST),

	//mName,

	//mImage,

	mTexturePeer(NULL),
	mOwnsTexturePeer(false)
{
	setImage(image);

	mTexturePeer=renderer->createTexturePeer(this);
	mOwnsTexturePeer=true;
}

Texture::Texture(Dimension dimension,int format,int width,int height,int depth):
	mType(Type_NORMAL),

	mDimension(Dimension_UNKNOWN),
	mFormat(Format_UNKNOWN),
	mWidth(0),
	mHeight(0),
	mDepth(0),

	mAutoGenerateMipMaps(true),

	mSAddressMode(AddressMode_REPEAT),
	mTAddressMode(AddressMode_REPEAT),
	mRAddressMode(AddressMode_REPEAT),

	mMinFilter(Filter_NEAREST),
	mMipFilter(Filter_NONE),
	mMagFilter(Filter_NEAREST),

	//mName,

	//mImage,

	mTexturePeer(NULL),
	mOwnsTexturePeer(false)
{
	mDimension=dimension;
	mFormat=format;
	mWidth=width;
	mHeight=height;
	mDepth=depth;
}

Texture::Texture(Renderer *renderer,Dimension dimension,int format,int width,int height,int depth):
	mType(Type_NORMAL),

	mDimension(Dimension_UNKNOWN),
	mFormat(Format_UNKNOWN),
	mWidth(0),
	mHeight(0),
	mDepth(0),

	mAutoGenerateMipMaps(true),

	mSAddressMode(AddressMode_REPEAT),
	mTAddressMode(AddressMode_REPEAT),
	mRAddressMode(AddressMode_REPEAT),

	mMinFilter(Filter_NEAREST),
	mMipFilter(Filter_NONE),
	mMagFilter(Filter_NEAREST),

	//mName,

	//mImage,

	mTexturePeer(NULL),
	mOwnsTexturePeer(false)
{
	mDimension=dimension;
	mFormat=format;
	mWidth=width;
	mHeight=height;
	mDepth=depth;

	mTexturePeer=renderer->createTexturePeer(this);
	mOwnsTexturePeer=true;
}

Texture::~Texture(){
	if(mOwnsTexturePeer && mTexturePeer!=NULL){
		delete mTexturePeer;
	}
}

void Texture::setAutoGenerateMipMaps(bool mipmaps){
	mAutoGenerateMipMaps=mipmaps;
}

void Texture::setSAddressMode(AddressMode addressMode){
	mSAddressMode=addressMode;
}

void Texture::setTAddressMode(AddressMode addressMode){
	mTAddressMode=addressMode;
}

void Texture::setRAddressMode(AddressMode addressMode){
	mRAddressMode=addressMode;
}

void Texture::setMinFilter(Filter minFilter){
	mMinFilter=minFilter;
}

void Texture::setMipFilter(Filter mipFilter){
	mMipFilter=mipFilter;
}

void Texture::setMagFilter(Filter magFilter){
	mMagFilter=magFilter;
}

void Texture::setName(const String &name){
	mName=name;
}

void Texture::setImage(Image::ptr image){
	if(mTexturePeer!=NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"Cannot change images after upload");
		return;
	}

	mDimension=image->getDimension();
	mFormat=image->getFormat();
	mWidth=image->getWidth();
	mHeight=image->getHeight();
	mDepth=image->getDepth();
	mImage=image;
}

void Texture::internal_setTexturePeer(TexturePeer *texturePeer,bool owns){
	if(mOwnsTexturePeer && mTexturePeer!=NULL){
		delete mTexturePeer;
	}
	mTexturePeer=texturePeer;
	mOwnsTexturePeer=owns;
}

}
}
