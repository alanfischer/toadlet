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

#ifndef TOADLET_PEEPER_D3D9PIXELBUFFER_H
#define TOADLET_PEEPER_D3D9PIXELBUFFER_H

#include "D3D9Includes.h"
#include <toadlet/peeper/PixelBuffer.h>

namespace toadlet{
namespace peeper{

class D3D9Renderer;

class TOADLET_API D3D9PixelBuffer:public PixelBuffer{
public:
	TOADLET_SHARED_POINTERS(D3D9PixelBuffer);

	D3D9PixelBuffer(D3D9Renderer *renderer,bool renderTarget);
	virtual ~D3D9PixelBuffer();

	PixelBuffer *getRootPixelBuffer(){return this;}

	void setBufferDestroyedListener(BufferDestroyedListener *listener){mListener=listener;}

	bool create(int usage,int access,int pixelFormat,int width,int height,int depth);
	void destroy();

	void resetCreate();
	void resetDestroy();

	int getUsage() const{return mUsage;}
	int getAccess() const{return mAccess;}
	int getDataSize() const{return mDataSize;}

	int getPixelFormat() const{return mPixelFormat;}
	int getWidth() const{return mWidth;}
	int getHeight() const{return mHeight;}
	int getDepth() const{return mDepth;}

	uint8 *lock(int lockAccess);
	bool unlock();

	bool needsReset();

	inline IDirect3DSurface9 *getSurface() const{return mSurface;}
	inline IDirect3DVolume9 *getVolume() const{return mVolume;}

protected:
	bool createContext(bool restore);
	bool destroyContext(bool backup);

	D3D9Renderer *mRenderer;

	BufferDestroyedListener *mListener;
	bool mRenderTarget;
	IDirect3DSurface9 *mSurface;
	IDirect3DVolume9 *mVolume;
	int mUsage;
	int mAccess;
	int mDataSize;
	int mPixelFormat;
	int mWidth,mHeight,mDepth;
	D3DPOOL mD3DPool;

	friend D3D9Renderer;
};

}
}

#endif
