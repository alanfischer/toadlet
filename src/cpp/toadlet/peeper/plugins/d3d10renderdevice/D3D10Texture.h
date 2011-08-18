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

namespace toadlet{
namespace peeper{

class D3D10RenderDevice;
class D3D10TextureMipPixelBuffer;

class TOADLET_API D3D10Texture:protected BaseResource,public Texture{
	TOADLET_BASERESOURCE_PASSTHROUGH(Texture);
public:
	D3D10Texture(D3D10RenderDevice *renderDevice);
	D3D10Texture(ID3D10Device *device);

	virtual ~D3D10Texture();

	Texture *getRootTexture(){return this;}

	bool create(int usageFlags,TextureFormat::ptr format,byte *mipDatas[]);
	void destroy();

	void resetCreate(){}
	void resetDestroy(){}

	int getUsage() const{return mUsage;}
	TextureFormat::ptr getFormat() const{return mFormat;}

	PixelBuffer::ptr getMipPixelBuffer(int level,int cubeSide);
	bool load(int width,int height,int depth,int mipLevel,byte *mipData);
	bool read(int width,int height,int depth,int mipLevel,byte *mipData);

	ID3D10Resource *getD3D10Resource(){return mTexture;}

	bool generateMipLevels();

protected:
	bool createContext(int numMipDatas,byte *mipDatas[]);
	bool destroyContext();

	D3D10RenderDevice *mDevice;
	ID3D10Device *mD3DDevice;

	int mUsage;
	TextureFormat::ptr mFormat;

	ID3D10Resource *mTexture;
	ID3D10ShaderResourceView *mShaderResourceView;
	Collection<PixelBuffer::ptr> mBuffers;

	friend class D3D10RenderDevice;
	friend class D3D10TextureMipPixelBuffer;
};

}
}

#endif
