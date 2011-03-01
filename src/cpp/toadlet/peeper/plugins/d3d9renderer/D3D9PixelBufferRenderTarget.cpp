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

#include "D3D9Renderer.h"
#include "D3D9PixelBufferRenderTarget.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

D3D9PixelBufferRenderTarget::D3D9PixelBufferRenderTarget(D3D9Renderer *renderer):D3D9RenderTarget(),
	mRenderer(NULL),
	
	mListener(NULL),
	mWidth(0),
	mHeight(0),
	mNeedsCompile(false)
	//mSurfaces,
	//mSurfaceAttachments,
	//mDepthBuffer
{
	mRenderer=renderer;
}

D3D9PixelBufferRenderTarget::~D3D9PixelBufferRenderTarget(){
	destroy();
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

	if(mListener!=NULL){
		mListener->renderTargetDestroyed((PixelBufferRenderTarget*)this);
		mListener=NULL;
	}
}

bool D3D9PixelBufferRenderTarget::activate(){
	if(mNeedsCompile){
		compile();
	}

	IDirect3DDevice9 *device=mRenderer->getDirect3DDevice9();

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

	mWidth=buffer->getWidth();
	mHeight=buffer->getHeight();
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
		D3D9PixelBuffer::ptr buffer(new D3D9PixelBuffer(mRenderer,true));
		if(buffer->create(Buffer::Usage_NONE,Buffer::Access_NONE,Texture::Format_DEPTH_16,mWidth,mHeight,1)){
			attach(buffer,Attachment_DEPTH_STENCIL);
			mDepthBuffer=buffer;
		}
	}

	mNeedsCompile=false;

	return true;
}

}
}
