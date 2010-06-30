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
#include <toadlet/peeper/Types.h>
#include <toadlet/peeper/Surface.h>

namespace toadlet{
namespace peeper{

class Texture:public egg::Resource,public egg::image::ImageDefinitions{
public:
	TOADLET_SHARED_POINTERS(Texture);

	enum Usage{
		Usage_NONE=			0,
		Usage_BIT_STATIC=	1<<0,	// Data is never changed
		Usage_BIT_STREAM=	1<<1,	// Data changes once per frame
		Usage_BIT_DYNAMIC=	1<<2,	// Data changes frequently
		Usage_BIT_NPOT_RESTRICTED=	1<<3,	// Texture is a size thats non-power-of-two, but restricted in usage
		Usage_BIT_RENDERTARGET=		1<<4,	// Texture will have its surfaces used by a SurfaceRenderTarget
		Usage_BIT_AUTOGEN_MIPMAPS=	1<<5,	// Texture autogenerates its mipmap levels
	};

	virtual ~Texture(){}

	virtual Texture *getRootTexture(scalar time)=0;
	virtual bool getRootTransform(scalar time,Matrix4x4 &transform)=0;

	virtual bool create(int usage,Dimension dimension,int format,int width,int height,int depth,int mipLevels,tbyte *mipDatas[])=0;
	virtual void destroy()=0;

	virtual void resetCreate()=0;
	virtual void resetDestroy()=0;

	virtual int getUsage() const=0;
	virtual Dimension getDimension() const=0;
	virtual int getFormat() const=0;
	virtual int getWidth() const=0;
	virtual int getHeight() const=0;
	virtual int getDepth() const=0;
	virtual int getNumMipLevels() const=0;
	virtual scalar getLength() const=0;

	/// @todo: Perhaps the whole frame portion of getRootTexture could be combined with this mipsurface thing somehow
	//  Where Textures would just have SubTextures, which would be the surfaces perhaps?
	virtual Surface::ptr getMipSurface(int i,int cubeSide)=0;
	virtual bool load(int width,int height,int depth,int mipLevel,tbyte *mipData)=0;
	virtual bool read(int width,int height,int depth,int mipLevel,tbyte *mipData)=0;
};

}
}

#endif

