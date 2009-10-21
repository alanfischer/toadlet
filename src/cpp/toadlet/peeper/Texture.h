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

namespace toadlet{
namespace peeper{

// TODO: Make Texture also extend Buffer, and inherit its Usage functions, and use those in create(), plus it will let us specify RenderTexture
// TODO: Also add a NPOT_RESTRICTED flag to Usage, so I dont have to have the D2_RESTRICTED thing
class Texture:public egg::Resource,public egg::image::ImageDefinitions{
public:
	TOADLET_SHARED_POINTERS(Texture,egg::Resource);

	virtual ~Texture(){}

	virtual Texture *getRootTexture() const=0; // Return the base texture in the texture decorator chain

	virtual void create(Dimension dimension,int format,int width,int height,int depth)=0;
	virtual void destroy()=0;

	virtual Dimension getDimension() const=0;
	virtual int getFormat() const=0;
	virtual int getWidth() const=0;
	virtual int getHeight() const=0;
	virtual int getDepth() const=0;

	virtual void load(int format,int width,int height,int depth,uint8 *data)=0;
	virtual bool read(int format,int width,int height,int depth,uint8 *data)=0;

	virtual void setAutoGenerateMipMaps(bool mipmaps)=0;
	virtual bool getAutoGenerateMipMaps() const=0;

	virtual void setName(const egg::String &name)=0;
	virtual const egg::String &getName() const=0;
};

}
}

#endif

