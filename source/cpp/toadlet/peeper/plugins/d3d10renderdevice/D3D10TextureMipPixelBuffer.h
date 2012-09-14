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

#ifndef TOADLET_PEEPER_D3D10TEXTUREMIPPIXELBUFFER_H
#define TOADLET_PEEPER_D3D10TEXTUREMIPPIXELBUFFER_H

#include <toadlet/peeper/PixelBuffer.h>
#include "D3D10Texture.h"

namespace toadlet{
namespace peeper{

class D3D10RenderDevice;

class TOADLET_API D3D10TextureMipPixelBuffer:public BaseResource,public PixelBuffer{
public:
	TOADLET_RESOURCE(D3D10TextureMipPixelBuffer,PixelBuffer);

	D3D10TextureMipPixelBuffer(D3D10Texture *texture,int level,int cubeSide);
	D3D10TextureMipPixelBuffer(D3D10RenderDevice *renderDevice);

	PixelBuffer *getRootPixelBuffer(){return this;}

	bool create(int usage,int access,TextureFormat::ptr textureFormat);
	void destroy();

	void resetCreate(){}
	void resetDestroy(){}

	int getUsage() const{return 0;}
	int getAccess() const{return 0;}
	TextureFormat::ptr getTextureFormat() const{return mFormat;}
	int getDataSize() const{return mDataSize;}
	int getSize() const{return mFormat->getSize();}

	tbyte *lock(int lockAccess);
	bool unlock();
	bool update(tbyte *data,int start,int size){return false;}

	inline D3D10Texture *getTexture() const{return mTexture;}
	inline ID3D10RenderTargetView *getD3D10RenderTargetView() const{return mD3DRenderTargetView;}
	inline ID3D10DepthStencilView *getD3D10DepthStencilView() const{return mD3DDepthStencilView;}

protected:
	bool createViews(int dimension,int pixelFormat,int level);

	D3D10RenderDevice *mDevice;
	D3D10Texture::ptr mTexture;
	ID3D10Resource *mD3DTexture;
	ID3D10RenderTargetView *mD3DRenderTargetView;
	ID3D10DepthStencilView *mD3DDepthStencilView;
	Texture::ptr mBufferTexture;
	int mLevel;
	int mCubeSide;
	int mDataSize;
	TextureFormat::ptr mFormat;

	friend class D3D10RenderDevice;
	friend class D3D10Texture;
};

}
}

#endif
