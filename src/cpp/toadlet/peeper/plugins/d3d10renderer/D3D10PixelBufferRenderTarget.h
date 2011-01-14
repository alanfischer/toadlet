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
#if 0
#ifndef TOADLET_PEEPER_D3D10SURFACERENDERTARGET_H
#define TOADLET_PEEPER_D3D10SURFACERENDERTARGET_H

#include "D3D10RenderTarget.h"
#include "D3D10Surface.h"
#include <toadlet/peeper/SurfaceRenderTarget.h>

namespace toadlet{
namespace peeper{

// TODO: Make the D3D9SurfaceRenderTarget & the GLFBOSurfaceRenderTarget more strict about Surface sizes,
//  and smarter about destroying their temporary depth buffer when not needed
class TOADLET_API D3D10SurfaceRenderTarget:public D3D10RenderTarget,public SurfaceRenderTarget{
public:
	D3D10SurfaceRenderTarget(D3D10Renderer *renderer);
	virtual ~D3D10SurfaceRenderTarget();

	virtual RenderTarget *getRootRenderTarget(){return (D3D10RenderTarget*)this;}

	virtual bool create();
	virtual void destroy();

	virtual bool activate()=0;
	virtual bool deactivate(){return false;}
	virtual void reset(){}

	virtual bool attach(Surface::ptr surface,Attachment attachment);
	virtual bool remove(Surface::ptr surface);
	virtual bool compile();

	virtual bool isPrimary() const{return false;}
	virtual bool isValid() const{return true;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}

	virtual ID3D10Device *getID3D10Device() const{return NULL;}

protected:
	Surface::ptr createBufferSurface(int format,int width,int height);

	D3D10Renderer *mRenderer;
	int mWidth;
	int mHeight;
	bool mNeedsCompile;
	egg::Collection<Surface::ptr> mSurfaces;
	egg::Collection<Attachment> mSurfaceAttachments;
	egg::Collection<D3D10Surface::ptr> mOwnedSurfaces;
};

}
}

#endif
#endif