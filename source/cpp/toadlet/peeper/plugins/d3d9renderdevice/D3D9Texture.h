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

#ifndef TOADLET_PEEPER_D3D9TEXTURE_H
#define TOADLET_PEEPER_D3D9TEXTURE_H

#include "D3D9Includes.h"
#include "D3D9PixelBuffer.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/Texture.h>

namespace toadlet{
namespace peeper{

class D3D9RenderDevice;
class D3D9TextureMipPixelBuffer;

class TOADLET_API D3D9Texture:public BaseResource,public Texture{
public:
	TOADLET_RESOURCE(D3D9Texture,Texture);

	D3D9Texture(D3D9RenderDevice *renderDevice);

	Texture *getRootTexture(){return this;}

	bool create(int usage,TextureFormat::ptr format,byte *mipDatas[]);
	void destroy();

	void resetCreate();
	void resetDestroy();

	int getUsage() const{return mUsage;}
	TextureFormat::ptr getFormat() const{return mFormat;}

	PixelBuffer::ptr getMipPixelBuffer(int level,int cubeSide);
	bool load(TextureFormat *format,byte *data);
	bool read(TextureFormat *format,byte *data);

	bool needsReset();

protected:
	bool createContext(bool restore);
	bool destroyContext(bool backup);
	static void setRECT(RECT &rect,TextureFormat *format);
	static void setD3DBOX(D3DBOX &box,TextureFormat *format);

	D3D9RenderDevice *mDevice;

	int mUsage;
	TextureFormat::ptr mFormat;

	D3DFORMAT mD3DFormat;
	DWORD mD3DUsage;
	D3DPOOL mD3DPool;
	IDirect3DBaseTexture9 *mTexture;
	bool mManuallyGenerateMipLevels;
	Collection<PixelBuffer::ptr> mBuffers;
	D3D9PixelBuffer::ptr mBackingBuffer;

	friend class D3D9RenderDevice;
	friend class D3D9TextureMipPixelBuffer;
};

}
}

#endif
