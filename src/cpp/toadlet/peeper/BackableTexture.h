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

#ifndef TOADLET_PEEPER_BACKABLETEXTURE_H
#define TOADLET_PEEPER_BACKABLETEXTURE_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/peeper/RenderDevice.h>

namespace toadlet{
namespace peeper{

class TOADLET_API BackableTexture:public BaseResource,public Texture{
	TOADLET_BASERESOURCE_PASSTHROUGH(Texture);
public:
	TOADLET_SHARED_POINTERS(BackableTexture);

	BackableTexture();
	virtual ~BackableTexture();

	virtual Texture *getRootTexture(){return mBack!=NULL?mBack->getRootTexture():NULL;}

	virtual bool create(int usage,TextureFormat::ptr format,tbyte *mipDatas[]);
	virtual void destroy();

	virtual void resetCreate();
	virtual void resetDestroy();

	virtual int getUsage() const{return mUsage;}
	virtual TextureFormat::ptr getFormat() const{return mFormat;}

	virtual PixelBuffer::ptr getMipPixelBuffer(int level,int cubeSide);
	virtual bool load(int width,int height,int depth,int mipLevel,tbyte *mipData);
	virtual bool read(int width,int height,int depth,int mipLevel,tbyte *mipData);

	virtual void setBack(Texture::ptr back,RenderDevice *renderDevice);
	virtual Texture::ptr getBack(){return mBack;}

	static bool convertLoad(Texture::ptr texture,int pixelFormat,int width,int height,int depth,int mipLevel,tbyte *mipData);
	static bool convertRead(Texture::ptr texture,int pixelFormat,int width,int height,int depth,int mipLevel,tbyte *mipData);
	static bool convertCreate(Texture::ptr texture,RenderDevice *renderDevice,int usage,TextureFormat::ptr format,tbyte *mipDatas[]);

protected:
	static void convertAndScale(tbyte *src,int srcFormat,int srcWidth,int srcHeight,int srcDepth,tbyte *dst,int dstFormat,int dstWidth,int dstHeight,int dstDepth);

	int mUsage;
	TextureFormat::ptr mFormat;

	int mRowPitch;
	int mSlicePitch;
	int mDataSize;
	tbyte *mData;
	Texture::ptr mBack;
	Collection<PixelBuffer::ptr> mBuffers;
};

}
}

#endif
