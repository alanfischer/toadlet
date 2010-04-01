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

#ifndef TOADLET_PEEPER_D3D9SURFACERENDERTARGET_H
#define TOADLET_PEEPER_D3D9SURFACERENDERTARGET_H

#include "D3D9RenderTarget.h"
#include "D3D9Surface.h"
#include <toadlet/peeper/SurfaceRenderTarget.h>

namespace toadlet{
namespace peeper{

/// @todo: Make the D3D9SurfaceRenderTarget & the GLFBOSurfaceRenderTarget more strict about Surface sizes,
//  and smarter about destroying their temporary depth buffer when not needed
class TOADLET_API D3D9SurfaceRenderTarget:public D3D9RenderTarget,public SurfaceRenderTarget{
public:
	D3D9SurfaceRenderTarget(D3D9Renderer *renderer);
	virtual ~D3D9SurfaceRenderTarget();

	virtual RenderTarget *getRootRenderTarget(){return (D3D9RenderTarget*)this;}

	virtual bool create();
	virtual bool destroy();

	virtual bool makeCurrent(IDirect3DDevice9 *device);
	virtual void reset(){}

	virtual bool attach(Surface::ptr surface,Attachment attachment);
	virtual bool remove(Surface::ptr surface);
	virtual bool compile();

	virtual bool isPrimary() const{return false;}
	virtual bool isValid() const{return true;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}

	virtual IDirect3D9 *getDirect3D9() const{return NULL;}
	virtual IDirect3DDevice9 *getDirect3DDevice9() const{return NULL;}

protected:
	Surface::ptr createBufferSurface(int format,int width,int height);

	D3D9Renderer *mRenderer;
	int mWidth;
	int mHeight;
	bool mNeedsCompile;
	egg::Collection<Surface::ptr> mSurfaces;
	egg::Collection<Attachment> mSurfaceAttachments;
	egg::Collection<D3D9Surface::ptr> mOwnedSurfaces;
};

}
}

#endif
