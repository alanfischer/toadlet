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

#ifndef TOADLET_PEEPER_SEQUENCETEXTURE_H
#define TOADLET_PEEPER_SEQUENCETEXTURE_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/Collection.h>
#include <toadlet/peeper/Texture.h>

namespace toadlet{
namespace peeper{

class TOADLET_API SequenceTexture:protected egg::BaseResource,public Texture{
	TOADLET_BASERESOURCE_PASSTHROUGH(Texture);
public:
	TOADLET_SHARED_POINTERS(SequenceTexture);

	SequenceTexture(Dimension dimension,int frames);
	virtual ~SequenceTexture();

	Texture *getRootTexture(scalar time);
	bool getRootTransform(scalar time,Matrix4x4 &transform);

	bool create(int usage,Dimension dimension,int format,int width,int height,int depth,int mipLevels,tbyte *mipDatas[]){
		egg::Error::unimplemented("create not implemented for SectionTexture");
		return false;
	}
	void destroy();

	void resetCreate(){}
	void resetDestroy(){}

	int getUsage() const{return 0;}
	Dimension getDimension() const{return mDimension;}
	int getFormat() const{return Format_UNKNOWN;}
	int getWidth() const;
	int getHeight() const;
	int getDepth() const;
	int getNumMipLevels() const{return 0;}
	scalar getLength() const;

	Surface::ptr getMipSurface(int i,int cubeSide){return NULL;}
	// These two methods could be implemented, but we would need to basically find which frame we are talking about, and then call load/read on that
	bool load(int width,int height,int depth,int mipLevel,tbyte *mipData){return false;}
	bool read(int width,int height,int depth,int mipLevel,tbyte *mipData){return false;}

	int getNumTextures() const;
	Texture::ptr getTexture(int frame);
	void setTexture(int frame,peeper::Texture::ptr texture,scalar delay);

protected:
	Dimension mDimension;
	egg::Collection<peeper::Texture::ptr> mTextures;
	egg::Collection<scalar> mDelays;
};

}
}

#endif

