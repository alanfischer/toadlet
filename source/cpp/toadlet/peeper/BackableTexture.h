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
public:
	TOADLET_RESOURCE(BackableTexture,Texture);

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
	virtual bool load(TextureFormat *format,tbyte *data);
	virtual bool read(TextureFormat *format,tbyte *data);

	virtual void setBack(Texture::ptr back,RenderDevice *renderDevice);
	virtual Texture::ptr getBack(){return mBack;}

	static bool convertLoad(Texture::ptr texture,TextureFormat *format,tbyte *data);
	static bool convertRead(Texture::ptr texture,TextureFormat *format,tbyte *data);
	static bool convertCreate(Texture::ptr texture,RenderDevice *renderDevice,int usage,TextureFormat::ptr format,tbyte *mipDatas[]);

protected:
	int mUsage;
	TextureFormat::ptr mFormat;

	tbyte *mData;
	Texture::ptr mBack;
	Collection<PixelBuffer::ptr> mBuffers;
};

}
}

#endif
