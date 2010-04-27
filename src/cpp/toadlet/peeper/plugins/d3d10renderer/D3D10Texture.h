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

#ifndef TOADLET_PEEPER_D3D10TEXTURE_H
#define TOADLET_PEEPER_D3D10TEXTURE_H

#include "D3D10Includes.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/TextureBlend.h>

namespace toadlet{
namespace peeper{

class D3D10Renderer;

class TOADLET_API D3D10Texture:protected egg::BaseResource,public Texture{
	TOADLET_BASERESOURCE_PASSTHROUGH(Texture);
public:
	D3D10Texture(D3D10Renderer *renderer);

	virtual ~D3D10Texture();

	virtual Texture *getRootTexture(scalar time){return this;}
	virtual bool getRootTransform(scalar time,Matrix4x4 &transform){return true;}

	virtual bool create(int usageFlags,Dimension dimension,int format,int width,int height,int depth,int mipLevels,byte *mipDatas[]);
	virtual void destroy();

	virtual void resetCreate(){}
	virtual void resetDestroy(){}

	virtual int getUsage() const{return mUsage;}
	virtual Dimension getDimension() const{return mDimension;}
	virtual int getFormat() const{return mFormat;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}
	virtual int getDepth() const{return mDepth;}
	virtual int getNumMipLevels() const{return mMipLevels;}
	virtual scalar getLength() const{return 0;}

	virtual Surface::ptr getMipSurface(int level,int cubeSide);
	virtual bool load(int width,int height,int depth,int mipLevel,byte *mipData);
	virtual bool read(int width,int height,int depth,int mipLevel,byte *mipData);

protected:
	bool createContext(int mipLevels,byte *mipDatas[]);
	bool destroyContext();

	D3D10Renderer *mRenderer;

	int mUsage;
	Dimension mDimension;
	int mFormat;
	int mWidth;
	int mHeight;
	int mDepth;
	int mMipLevels;

	ID3D10Resource *mTexture;
	ID3D10ShaderResourceView *mShaderResourceView;

	friend D3D10Renderer;
};

}
}

#endif
