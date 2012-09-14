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

#ifndef TOADLET_PEEPER_D3D9PIXELBUFFERRENDERTARGET_H
#define TOADLET_PEEPER_D3D9PIXELBUFFERRENDERTARGET_H

#include "D3D9RenderTarget.h"
#include "D3D9PixelBuffer.h"
#include <toadlet/peeper/PixelBufferRenderTarget.h>

namespace toadlet{
namespace peeper{

/// @todo: Make the D3D9SurfaceRenderTarget & the GLFBOSurfaceRenderTarget more strict about Surface sizes
class TOADLET_API D3D9PixelBufferRenderTarget:public BaseResource,public D3D9RenderTarget,public PixelBufferRenderTarget{
public:
	TOADLET_RESOURCE(D3D9PixelBufferRenderTarget,PixelBufferRenderTarget);

	D3D9PixelBufferRenderTarget(D3D9RenderDevice *renderDevice);

	RenderTarget *getRootRenderTarget(){return (D3D9RenderTarget*)this;}
	PixelBufferRenderTarget *getRootPixelBufferRenderTarget(){return this;}

	bool create();
	void destroy();

	void resetCreate();
	void resetDestroy();

	bool activate();
	bool deactivate();
	void swap(){}
	void reset(){}

	bool attach(PixelBuffer::ptr buffer,Attachment attachment);
	bool remove(PixelBuffer::ptr buffer);
	bool compile();

	bool isPrimary() const{return false;}
	bool isValid() const{return true;}
	int getWidth() const{return mWidth;}
	int getHeight() const{return mHeight;}

	IDirect3D9 *getDirect3D9() const{return NULL;}
	IDirect3DDevice9 *getDirect3DDevice9() const{return NULL;}

protected:
	D3D9RenderDevice *mDevice;
	int mWidth;
	int mHeight;
	bool mNeedsCompile;
	Collection<PixelBuffer::ptr> mBuffers;
	Collection<Attachment> mBufferAttachments;
	PixelBuffer::ptr mDepthBuffer,mColorBuffer;
};

}
}

#endif
