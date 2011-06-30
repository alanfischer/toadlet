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

#ifndef TOADLET_PEEPER_D3D10PIXELBUFFERRENDERTARGET_H
#define TOADLET_PEEPER_D3D10PIXELBUFFERRENDERTARGET_H

#include "D3D10RenderTarget.h"
#include "D3D10TextureMipPixelBuffer.h"
#include <toadlet/peeper/PixelBufferRenderTarget.h>

namespace toadlet{
namespace peeper{

/// @todo: Make the D3D10PixelBufferRenderTarget & the GLFBOSurfaceRenderTarget more strict about Surface sizes
class TOADLET_API D3D10PixelBufferRenderTarget:public D3D10RenderTarget,public PixelBufferRenderTarget{
public:
	D3D10PixelBufferRenderTarget(D3D10RenderDevice *renderDevice);
	virtual ~D3D10PixelBufferRenderTarget();

	RenderTarget *getRootRenderTarget(){return (D3D10RenderTarget*)this;}
	PixelBufferRenderTarget *getRootPixelBufferRenderTarget(){return this;}

	void setRenderTargetDestroyedListener(RenderTargetDestroyedListener *listener){mListener=listener;}

	bool create();
	void destroy();

	void resetCreate(){}
	void resetDestroy(){}

	bool activate();
	void swap();
	void reset(){}

	bool attach(PixelBuffer::ptr buffer,Attachment attachment);
	bool remove(PixelBuffer::ptr buffer);
	bool compile();

	bool isPrimary() const{return false;}
	bool isValid() const{return true;}
	int getWidth() const{return mWidth;}
	int getHeight() const{return mHeight;}

protected:
	D3D10RenderDevice *mDevice;
	RenderTargetDestroyedListener *mListener;
	int mWidth;
	int mHeight;
	bool mNeedsCompile;
	Collection<PixelBuffer::ptr> mBuffers;
	Collection<Attachment> mBufferAttachments;
	PixelBuffer::ptr mDepthBuffer;
};

}
}

#endif
