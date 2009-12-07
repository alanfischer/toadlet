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
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/TextureBlend.h>

namespace toadlet{
namespace peeper{

class D3D9Renderer;

class TOADLET_API D3D9Texture:protected egg::BaseResource,public Texture{
	TOADLET_BASERESOURCE_PASSTHROUGH(Texture);
public:
	D3D9Texture(D3D9Renderer *renderer);

	virtual ~D3D9Texture();

	virtual Texture *getRootTexture(){return this;}

	virtual bool create(int usageFlags,Dimension dimension,int format,int width,int height,int depth,int mipLevels);
	virtual void destroy();

	virtual bool createContext();
	virtual void destroyContext(bool backData);
	virtual bool contextNeedsReset();

	virtual int getUsageFlags() const{return mUsageFlags;}
	virtual Dimension getDimension() const{return mDimension;}
	virtual int getFormat() const{return mFormat;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}
	virtual int getDepth() const{return mDepth;}
	virtual int getNumMipLevels() const{return mTexture==NULL?0:mTexture->GetLevelCount();}

	virtual Surface::ptr getMipSurface(int level,int cubeSide);
	virtual bool load(int format,int width,int height,int depth,int mipLevel,uint8 *data);
	virtual bool read(int format,int width,int height,int depth,int mipLevel,uint8 *data);

protected:
	static bool isD3DFORMATValid(IDirect3D9 *d3d,D3DFORMAT adapterFormat,D3DFORMAT textureFormat,DWORD usage);

	static int getClosestTextureFormat(int textureFormat);
	static D3DFORMAT getD3DFORMAT(int textureFormat);
	static DWORD getD3DTADDRESS(TextureStage::AddressMode addressMode);
	static DWORD getD3DTEXF(TextureStage::Filter filter);

	D3D9Renderer *mRenderer;

	int mUsageFlags;
	Dimension mDimension;
	int mFormat;
	int mWidth;
	int mHeight;
	int mDepth;
	int mMipLevels;

	int mInternalFormat;
	D3DFORMAT mD3DFormat;
	DWORD mD3DUsage;
	D3DPOOL mD3DPool;
	IDirect3DBaseTexture9 *mTexture;
	bool mManuallyGenerateMipLevels;

	friend D3D9Renderer;
};

}
}

#endif
