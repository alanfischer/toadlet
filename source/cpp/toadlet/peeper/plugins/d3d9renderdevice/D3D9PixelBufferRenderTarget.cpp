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

#include "D3D9RenderDevice.h"
#include "D3D9PixelBufferRenderTarget.h"

namespace toadlet{
namespace peeper{

D3D9PixelBufferRenderTarget::D3D9PixelBufferRenderTarget(D3D9RenderDevice *renderDevice):D3D9RenderTarget(),
	mDevice(NULL),
	mWidth(0),
	mHeight(0),
	mNeedsCompile(false)
	//mBuffers,
	//mBufferAttachments,
	//mDepthBuffer
{
	mDevice=renderDevice;
}

bool D3D9PixelBufferRenderTarget::create(){
	mWidth=0;
	mHeight=0;
	mNeedsCompile=true;

	return true;
}

void D3D9PixelBufferRenderTarget::destroy(){
	if(mDepthBuffer!=NULL){
		mDepthBuffer->destroy();
		mDepthBuffer=NULL;
	}

	BaseResource::destroy();
}

void D3D9PixelBufferRenderTarget::resetCreate(){
	if(mDepthBuffer!=NULL){
		mDepthBuffer->resetCreate();
	}
	if(mColorBuffer!=NULL){
		mColorBuffer->resetCreate();
	}
}

void D3D9PixelBufferRenderTarget::resetDestroy(){
	if(mDepthBuffer!=NULL){
		mDepthBuffer->resetDestroy();
	}
	if(mColorBuffer!=NULL){
		mColorBuffer->resetDestroy();
	}
}

bool D3D9PixelBufferRenderTarget::activate(){
	if(mNeedsCompile){
		compile();
	}

	IDirect3DDevice9 *device=mDevice->getDirect3DDevice9();

	bool result=false;
	#if defined(TOADLET_SET_D3DM)
		if(mBuffers.size()>=2 && mBuffers[0]!=NULL && mBuffers[1]!=NULL){
			D3D9PixelBuffer *renderBuffer=(D3D9PixelBuffer*)mBuffers[0]->getRootPixelBuffer();
			D3D9PixelBuffer *depthBuffer=(D3D9PixelBuffer*)mBuffers[1]->getRootPixelBuffer();
			device->SetRenderTarget(renderBuffer->getSurface(),depthBuffer->getSurface());
			result=true;
		}
	#else
		int i;
		for(i=0;i<mBuffers.size();++i){
			D3D9PixelBuffer *buffer=(D3D9PixelBuffer*)mBuffers[i]->getRootPixelBuffer();
			Attachment attachment=mBufferAttachments[i];
			if(attachment==Attachment_DEPTH_STENCIL){
				device->SetDepthStencilSurface(buffer->getSurface());
			}
			else{
				device->SetRenderTarget(attachment-Attachment_COLOR_0,buffer->getSurface());
			}
		}
		result=true;
	#endif

	return result;
}

bool D3D9PixelBufferRenderTarget::deactivate(){
	IDirect3DDevice9 *device=mDevice->getDirect3DDevice9();

	bool result=false;
	#if defined(TOADLET_SET_D3DM)
		if(mBuffers.size()>=2 && mBuffers[0]!=NULL && mBuffers[1]!=NULL){
			device->SetRenderTarget(NULL,NULL);
			result=true;
		}
	#else
		int i;
		for(i=0;i<mBuffers.size();++i){
			D3D9PixelBuffer *buffer=(D3D9PixelBuffer*)mBuffers[i]->getRootPixelBuffer();
			Attachment attachment=mBufferAttachments[i];
			if(attachment==Attachment_DEPTH_STENCIL){
				device->SetDepthStencilSurface(NULL);
			}
			else if(attachment!=Attachment_COLOR_0){ // Can't assign NULL to COLOR_0
				device->SetRenderTarget(attachment-Attachment_COLOR_0,NULL);
			}
		}
		result=true;
	#endif

	return result;
}

bool D3D9PixelBufferRenderTarget::attach(PixelBuffer::ptr buffer,Attachment attachment){
	#if defined(TOADLET_SET_D3DM)
		mBuffers.resize(2);
		mBufferAttachments.resize(2);
		if(attachment==Attachment_DEPTH_STENCIL){
			mBuffers.setAt(1,buffer);
			mBufferAttachments.setAt(1,attachment);
		}
		else{
			mBuffers.setAt(0,buffer);
			mBufferAttachments.setAt(0,attachment);
		}
	#else
		mBuffers.add(buffer);
		mBufferAttachments.add(attachment);
	#endif

	mWidth=buffer->getTextureFormat()->getWidth();
	mHeight=buffer->getTextureFormat()->getHeight();
	mNeedsCompile=true;

	return true;
}

bool D3D9PixelBufferRenderTarget::remove(PixelBuffer::ptr buffer){
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

bool D3D9PixelBufferRenderTarget::compile(){
	PixelBuffer::ptr depth;
	PixelBuffer::ptr color;

	if(mDepthBuffer!=NULL){
		remove(mDepthBuffer);
		mDepthBuffer->destroy();
		mDepthBuffer=NULL;
	}

	if(mColorBuffer!=NULL){
		remove(mColorBuffer);
		mColorBuffer->destroy();
		mColorBuffer=NULL;
	}

	int i;
	for(i=0;i<mBufferAttachments.size();++i){
		if(mBufferAttachments[i]==Attachment_DEPTH_STENCIL){
			depth=mBuffers[i];
		}
		else{
			color=mBuffers[i];
		}
	}

	if(color!=NULL && depth==NULL){
		// No Depth-Stencil surface, so add one
		D3D9PixelBuffer::ptr buffer=new D3D9PixelBuffer(mDevice,true);
		TextureFormat::ptr format=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_DEPTH_16,mWidth,mHeight,1,1);
		if(buffer->create(Texture::Usage_BIT_RENDERTARGET | Buffer::Usage_BIT_STATIC,Buffer::Access_NONE,format)){
			attach(buffer,Attachment_DEPTH_STENCIL);
			mDepthBuffer=buffer;
		}
	}
	else if(color==NULL && depth!=NULL){
		// No Color surface, so add one
		/// @todo: Is there a way to have D3D9 not care about the color buffer, if it isn't needed?
		///  Without the below code we get an error if the size of the depth buffer is greater than the color buffer
		D3D9PixelBuffer::ptr buffer=new D3D9PixelBuffer(mDevice,true);
		TextureFormat::ptr format=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_RGB_5_6_5,mWidth,mHeight,1,1);
		if(buffer->create(Texture::Usage_BIT_RENDERTARGET | Buffer::Usage_BIT_STATIC,Buffer::Access_NONE,format)){
			attach(buffer,Attachment_COLOR_0);
			mColorBuffer=buffer;
		}
	}

	mNeedsCompile=false;

	return true;
}

}
}
