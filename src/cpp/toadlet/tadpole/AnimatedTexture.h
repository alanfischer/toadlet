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

#ifndef TOADLET_TADPOLE_ANIMATEDTEXTURE_H
#define TOADLET_TADPOLE_ANIMATEDTEXTURE_H

#include <toadlet/peeper/Texture.h>

namespace toadlet{
namespace tadpole{

class AnimatedTexture:public peeper::Texture{
public:
	TOADLET_SHARED_POINTERS(AnimatedTexture);

	virtual ~AnimatedTexture(){}

	virtual 
	virtual Texture *getRootTexture(){return mTextures.size()>0?mTextures[0]:NULL;}

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
	virtual int getNumMipLevels() const{return mMipLevels;}

	virtual Surface::ptr getMipSurface(int i,int cubeSide);
	virtual bool load(int format,int width,int height,int depth,int mipLevel,uint8 *data);
	virtual bool read(int format,int width,int height,int depth,int mipLevel,uint8 *data);
};

}
}

#endif

