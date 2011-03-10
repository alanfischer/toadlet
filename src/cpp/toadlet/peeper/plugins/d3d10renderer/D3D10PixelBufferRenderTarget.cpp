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

#include "D3D10Renderer.h"
#include "D3D10PixelBufferRenderTarget.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

D3D10PixelBufferRenderTarget::D3D10PixelBufferRenderTarget(D3D10Renderer *renderer):D3D10RenderTarget(),
	mRenderer(NULL),
	mListener(NULL),
	mWidth(0),
	mHeight(0),
	mNeedsCompile(false)
	//mSurfaces,
	//mSurfaceAttachments,
	//mOwnedSurfaces
{
	mRenderer=renderer;
	mD3DDevice=mRenderer->getD3D10Device();
}

D3D10PixelBufferRenderTarget::~D3D10PixelBufferRenderTarget(){
	destroy();
}

bool D3D10PixelBufferRenderTarget::create(){
	destroy();

	mWidth=0;
	mHeight=0;
	mNeedsCompile=true;

	D3D10RenderTarget::create();

	return true;
}

void D3D10PixelBufferRenderTarget::destroy(){
	D3D10RenderTarget::destroy();

	if(mDepthBuffer!=NULL){
		mDepthBuffer->destroy();
		mDepthBuffer=NULL;
	}

	if(mListener!=NULL){
		mListener->renderTargetDestroyed((PixelBufferRenderTarget*)this);
		mListener=NULL;
	}
}

bool D3D10PixelBufferRenderTarget::activate(){
	if(mNeedsCompile){
		compile();
	}

	return D3D10RenderTarget::activate();
}

void D3D10PixelBufferRenderTarget::swap(){
	int i;
	for(i=0;i<mBufferAttachments.size();++i){
		if(mBufferAttachments[i]!=Attachment_DEPTH_STENCIL){
			D3D10TextureMipPixelBuffer::ptr buffer=shared_static_cast<D3D10TextureMipPixelBuffer>(mBuffers[i]);
			buffer->getTexture()->generateMipLevels();
		}
	}
}

bool D3D10PixelBufferRenderTarget::attach(PixelBuffer::ptr buffer,Attachment attachment){
	mBuffers.add(buffer);
	mBufferAttachments.add(attachment);

	mWidth=buffer->getWidth();
	mHeight=buffer->getHeight();
	mNeedsCompile=true;

	return true;
}

bool D3D10PixelBufferRenderTarget::remove(PixelBuffer::ptr buffer){
	int i;
	for(i=0;i<mBuffers.size();++i){
		if(mBuffers[i]==buffer){
			break;
		}
	}
	if(i==mBuffers.size()){
		return false;
	}

	mBuffers.removeAt(i);
	mBufferAttachments.removeAt(i);
	mNeedsCompile=true;

	return true;
}

bool D3D10PixelBufferRenderTarget::compile(){
	PixelBuffer::ptr depth;
	PixelBuffer::ptr color;

	int i;
	for(i=0;i<mBufferAttachments.size();++i){
		if(mBufferAttachments[i]==Attachment_DEPTH_STENCIL){
			depth=mBuffers[i];
		}
		else{
			color=mBuffers[i];
		}
	}

	if(mDepthBuffer!=NULL){
		remove(mDepthBuffer);
		mDepthBuffer->destroy();
		mDepthBuffer=NULL;
	}

	if(color!=NULL && depth==NULL){
		// No Depth-Stencil surface, so add one
		D3D10TextureMipPixelBuffer::ptr buffer(new D3D10TextureMipPixelBuffer(mRenderer));
		if(buffer->create(Buffer::Usage_BIT_STATIC,Buffer::Access_NONE,Texture::Format_DEPTH_16,mWidth,mHeight,1)){
			attach(buffer,Attachment_DEPTH_STENCIL);
			mDepthBuffer=buffer;
		}
	}

	mRenderTargetViews.clear();
	for(i=0;i<mBufferAttachments.size();++i){
		D3D10TextureMipPixelBuffer::ptr buffer=shared_static_cast<D3D10TextureMipPixelBuffer>(mBuffers[i]);
		if(mBufferAttachments[i]==Attachment_DEPTH_STENCIL){
			mDepthStencilView=buffer->getD3D10DepthStencilView();
		}
		else{
			mRenderTargetViews.add(buffer->getD3D10RenderTargetView());
		}
	}

	mNeedsCompile=false;

	return true;
}

}
}
