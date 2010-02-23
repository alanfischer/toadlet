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

namespace toadlet{
namespace peeper{

class TOADLET_API BackableTexture:public egg::BaseResource,public Texture{
	TOADLET_BASERESOURCE_PASSTHROUGH(Texture);
public:
	TOADLET_SHARED_POINTERS(BackableTexture);

	BackableTexture();
	virtual ~BackableTexture();

	virtual Texture *getRootTexture(scalar time){return mBack!=NULL?mBack->getRootTexture(time):NULL;}
	virtual bool getRootTransform(scalar time,Matrix4x4 &transform){return mBack!=NULL?mBack->getRootTransform(time,transform):true;}

	virtual bool create(int usageFlags,Dimension dimension,int format,int width,int height,int depth,int mipLevels);
	virtual void destroy();

	virtual bool createContext(){return mBack->createContext();}
	virtual void destroyContext(bool backData){mBack->destroyContext(backData);}
	virtual bool contextNeedsReset(){return mBack->contextNeedsReset();}

	virtual int getUsageFlags() const{return mUsageFlags;}
	virtual Dimension getDimension() const{return mDimension;}
	virtual int getFormat() const{return mFormat;}
	virtual int getWidth() const{return mWidth;}
	virtual int getHeight() const{return mHeight;}
	virtual int getDepth() const{return mDepth;}
	virtual int getNumMipLevels() const{return mMipLevels;}
	virtual scalar getLength() const{return 0;}

	virtual Surface::ptr getMipSurface(int level,int cubeSide);
	virtual bool load(int format,int width,int height,int depth,int mipLevel,uint8 *data);
	virtual bool read(int format,int width,int height,int depth,int mipLevel,uint8 *data);

	virtual void setBack(Texture::ptr back,bool initial=false);
	virtual Texture::ptr getBack(){return mBack;}

protected:
	int mUsageFlags;
	Dimension mDimension;
	int mFormat;
	int mWidth;
	int mHeight;
	int mDepth;
	int mMipLevels;

	int mDataSize;
	uint8 *mData;
	Texture::ptr mBack;
};

}
}

#endif
