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
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/TextureBlend.h>

namespace toadlet{
namespace peeper{

class D3D9Renderer;

class TOADLET_API D3D9Texture:public Texture{
public:
	D3D9Texture(D3D9Renderer *renderer);

	virtual ~D3D9Texture();

	virtual Texture *getRootTexture(){return this;}

	virtual bool create(int usageFlags,Dimension dimension,int format,int width,int height,int depth);
	virtual void destroy();

	virtual int getUsageFlags() const{return mUsageFlags;}
	virtual Dimension getDimension() const{return mDimension;}
	virtual int getFormat() const{return mFormat;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}
	virtual int getDepth() const{return mDepth;}

	virtual void load(int format,int width,int height,int depth,uint8 *data);
	virtual bool read(int format,int width,int height,int depth,uint8 *data);

	virtual void setAutoGenerateMipMaps(bool mipmaps);
	virtual bool getAutoGenerateMipMaps() const{return mAutoGenerateMipMaps;}

	virtual void setName(const egg::String &name){mName=name;}
	virtual const egg::String &getName() const{return mName;}

protected:
	static bool isD3DFORMATValid(IDirect3D9 *d3d,D3DFORMAT textureFormat,D3DFORMAT adapterFormat);

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

	egg::String mName;
	
	IDirect3DBaseTexture9 *mTexture;
	bool mAutoGenerateMipMaps;

	friend D3D9Renderer;
};

}
}

#endif
