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

#ifndef TOADLET_PEEPER_TEXTURE_H
#define TOADLET_PEEPER_TEXTURE_H

#include <toadlet/egg/Resource.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/image/Image.h>
#include <toadlet/peeper/Surface.h>

namespace toadlet{
namespace peeper{

class Texture:public egg::Resource,public egg::image::ImageDefinitions{
public:
	TOADLET_SHARED_POINTERS(Texture);

	enum UsageFlags{
		UsageFlags_NONE=			0,
		UsageFlags_NPOT_RESTRICTED=	1<<0,	// Texture is a size thats non-power-of-two, but restricted in usage
		UsageFlags_RENDERTARGET=	1<<1,	// Texture will have its surfaces used by a SurfaceRenderTarget
		UsageFlags_AUTOGEN_MIPMAPS=	1<<2,	// Texture autogenerates its mipmap levels
	};

	virtual ~Texture(){}

	virtual Texture *getRootTexture()=0;

	virtual bool create(int usageFlags,Dimension dimension,int format,int width,int height,int depth,int mipLevels)=0;
	virtual void destroy()=0;

	virtual bool createContext()=0;
	virtual void destroyContext(bool backData)=0;
	virtual bool contextNeedsReset()=0;

	virtual int getUsageFlags() const=0;
	virtual Dimension getDimension() const=0;
	virtual int getFormat() const=0;
	virtual int getWidth() const=0;
	virtual int getHeight() const=0;
	virtual int getDepth() const=0;
	virtual int getNumMipLevels() const=0;

	virtual Surface::ptr getMipSuface(int i)=0;
	virtual bool load(int format,int width,int height,int depth,int mipLevel,uint8 *data)=0;
	virtual bool read(int format,int width,int height,int depth,int mipLevel,uint8 *data)=0;
};

}
}

#endif

