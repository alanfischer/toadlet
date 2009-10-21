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

#include <toadlet/peeper/RenderTexture.h>
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

RenderTexture::RenderTexture(int format,int width,int height):
	Texture(Dimension_D2,format,width,height),
	RenderTarget()
{
	mType=Type_RENDER;

	mSAddressMode=AddressMode_CLAMP_TO_EDGE;
	mTAddressMode=AddressMode_CLAMP_TO_EDGE;
	mRAddressMode=AddressMode_CLAMP_TO_EDGE;

	mMipFilter=Filter_NONE;
}

RenderTexture::RenderTexture(Renderer *renderer,int format,int width,int height):
	Texture(Dimension_D2,format,width,height),
	RenderTarget()
{
	mType=Type_RENDER;

	mSAddressMode=AddressMode_CLAMP_TO_EDGE;
	mTAddressMode=AddressMode_CLAMP_TO_EDGE;
	mRAddressMode=AddressMode_CLAMP_TO_EDGE;

	mMipFilter=Filter_NONE;

	mTexturePeer=renderer->createTexturePeer(this);
	mOwnsTexturePeer=true;
	if(mTexturePeer!=NULL){
		mRenderTargetPeer=mTexturePeer->castToRenderTargetPeer();
		mOwnsRenderTargetPeer=false;
	}
}

RenderTexture::~RenderTexture(){
}

int RenderTexture::getWidth() const{
	if(mRenderTargetPeer!=NULL){
		return RenderTarget::getWidth();
	}
	else{
		return mWidth;
	}
}

int RenderTexture::getHeight() const{
	if(mRenderTargetPeer!=NULL){
		return RenderTarget::getHeight();
	}
	else{
		return mHeight;
	}
}

void RenderTexture::internal_setTexturePeer(TexturePeer *texturePeer,bool own){
	RenderTargetPeer *renderTargetPeer=texturePeer->castToRenderTargetPeer();
	if(texturePeer!=NULL && renderTargetPeer==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"Non RenderTargetPeer used in RenderTexture");
		return;
	}

	RenderTarget::internal_setRenderTargetPeer(renderTargetPeer,false);
	// The ownership semantics are kept to the TexturePeer
	Texture::internal_setTexturePeer(texturePeer,own);
}

void RenderTexture::internal_setRenderTargetPeer(RenderTargetPeer *renderTargetPeer,bool own){
	TexturePeer *texturePeer=renderTargetPeer->castToTexturePeer();
	if(renderTargetPeer!=NULL && texturePeer==NULL){
		Error::unknown(Categories::TOADLET_PEEPER,
			"Non TexturePeer used in RenderTexture");
		return;
	}

	RenderTarget::internal_setRenderTargetPeer(renderTargetPeer,false);
	// The ownership semantics are kept to the TexturePeer
	Texture::internal_setTexturePeer(texturePeer,own);
}

}
}

