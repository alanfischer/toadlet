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
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/PixelBuffer.h>

namespace toadlet{
namespace peeper{

class D3D9RenderDevice;

class TOADLET_API D3D9PixelBuffer:public BaseResource,public PixelBuffer{
public:
	TOADLET_RESOURCE(D3D9PixelBuffer,PixelBuffer);

	D3D9PixelBuffer(D3D9RenderDevice *renderDevice,bool renderTarget);
	virtual ~D3D9PixelBuffer();

	PixelBuffer *getRootPixelBuffer(){return this;}

	bool create(int usage,int access,TextureFormat::ptr format);
	void destroy();

	void resetCreate();
	void resetDestroy();

	int getUsage() const{return mUsage;}
	int getAccess() const{return mAccess;}
	int getDataSize() const{return mDataSize;}
	int getSize() const{return mFormat->getSize();}

	TextureFormat::ptr getTextureFormat() const{return mFormat;}

	tbyte *lock(int lockAccess);
	bool unlock();
	bool update(tbyte *data,int start,int size){return false;}

	bool needsReset();

	inline IDirect3DSurface9 *getSurface() const{return mSurface;}
	inline IDirect3DVolume9 *getVolume() const{return mVolume;}

protected:
	bool createContext(bool restore);
	bool destroyContext(bool backup);

	D3D9RenderDevice *mDevice;

	bool mRenderTarget;
	IDirect3DSurface9 *mSurface;
	IDirect3DVolume9 *mVolume;
	int mUsage;
	int mAccess;
	TextureFormat::ptr mFormat;
	int mDataSize;
	D3DPOOL mD3DPool;

	friend D3D9RenderDevice;
};

}
}

#endif
