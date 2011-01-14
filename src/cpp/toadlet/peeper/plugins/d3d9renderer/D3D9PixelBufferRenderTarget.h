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
class TOADLET_API D3D9PixelBufferRenderTarget:public D3D9RenderTarget,public PixelBufferRenderTarget{
public:
	D3D9PixelBufferRenderTarget(D3D9Renderer *renderer);
	virtual ~D3D9PixelBufferRenderTarget();

	virtual RenderTarget *getRootRenderTarget(){return (D3D9RenderTarget*)this;}

	virtual bool create();
	virtual void destroy();

	virtual bool activate();
	virtual bool deactivate(){return true;}
	virtual void reset(){}

	virtual bool attach(PixelBuffer::ptr buffer,Attachment attachment);
	virtual bool remove(PixelBuffer::ptr buffer);
	virtual bool compile();

	virtual bool isPrimary() const{return false;}
	virtual bool isValid() const{return true;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}

	virtual IDirect3D9 *getDirect3D9() const{return NULL;}
	virtual IDirect3DDevice9 *getDirect3DDevice9() const{return NULL;}

protected:
	D3D9Renderer *mRenderer;
	int mWidth;
	int mHeight;
	bool mNeedsCompile;
	egg::Collection<PixelBuffer::ptr> mBuffers;
	egg::Collection<Attachment> mBufferAttachments;
	PixelBuffer::ptr mDepthBuffer;
};

}
}

#endif
