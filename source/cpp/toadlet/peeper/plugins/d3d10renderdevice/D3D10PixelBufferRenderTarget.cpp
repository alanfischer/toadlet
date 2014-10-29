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

#include "D3D10RenderDevice.h"
#include "D3D10PixelBufferRenderTarget.h"

namespace toadlet{
namespace peeper{

D3D10PixelBufferRenderTarget::D3D10PixelBufferRenderTarget(D3D10RenderDevice *renderDevice):D3D10RenderTarget(),
	mDevice(NULL),
	mWidth(0),
	mHeight(0),
	mNeedsCompile(false)
	//mSurfaces,
	//mSurfaceAttachments,
	//mOwnedSurfaces
{
	mDevice=renderDevice;
	mD3DDevice=mDevice->getD3D10Device();
}

bool D3D10PixelBufferRenderTarget::create(){
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

	BaseResource::destroy();
}

bool D3D10PixelBufferRenderTarget::activate(){
	if(mNeedsCompile){
		compile();
	}

	return D3D10RenderTarget::activate();
}

bool D3D10PixelBufferRenderTarget::deactivate(){
	int i;
	for(i=0;i<mBufferAttachments.size();++i){
		if(mBufferAttachments[i]!=Attachment_DEPTH_STENCIL){
			D3D10TextureMipPixelBuffer *buffer=(D3D10TextureMipPixelBuffer*)(mBuffers[i]->getRootPixelBuffer());
			buffer->getTexture()->generateMipLevels();
		}
	}

	return D3D10RenderTarget::deactivate();
}

bool D3D10PixelBufferRenderTarget::attach(PixelBuffer::ptr buffer,Attachment attachment){
	mBuffers.push_back(buffer);
	mBufferAttachments.push_back(attachment);

	mWidth=buffer->getTextureFormat()->getWidth();
	mHeight=buffer->getTextureFormat()->getHeight();
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

	mBuffers.erase(mBuffers.begin()+i);
	mBufferAttachments.erase(mBufferAttachments.begin()+i);
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
		D3D10TextureMipPixelBuffer::ptr buffer=new D3D10TextureMipPixelBuffer(mDevice);
		TextureFormat::ptr format=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_DEPTH_16,mWidth,mHeight,1,1);
		if(buffer->create(Buffer::Usage_BIT_STREAM,Buffer::Access_NONE,format)){
			attach(buffer,Attachment_DEPTH_STENCIL);
			mDepthBuffer=buffer;
		}
	}

	mRenderTargetViews.clear();
	for(i=0;i<mBufferAttachments.size();++i){
		D3D10TextureMipPixelBuffer *buffer=(D3D10TextureMipPixelBuffer*)(mBuffers[i]->getRootPixelBuffer());
		if(mBufferAttachments[i]==Attachment_DEPTH_STENCIL){
			mDepthStencilView=buffer->getD3D10DepthStencilView();
		}
		else{
			mRenderTargetViews.push_back(buffer->getD3D10RenderTargetView());
		}
	}

	mNeedsCompile=false;

	return true;
}

}
}
