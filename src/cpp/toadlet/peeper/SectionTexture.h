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

#ifndef TOADLET_PEEPER_SECTIONTEXTURE_H
#define TOADLET_PEEPER_SECTIONTEXTURE_H

#include <toadlet/egg/BaseResource.h>
#include <toadlet/egg/Error.h>
#include <toadlet/peeper/Texture.h>

namespace toadlet{
namespace peeper{

class TOADLET_API SectionTexture:protected egg::BaseResource,public Texture{
	TOADLET_BASERESOURCE_PASSTHROUGH(Texture);
public:
	TOADLET_SHARED_POINTERS(SectionTexture);

	SectionTexture(int widthFrames,int frameOffset,int numFrames,Texture::ptr texture);
	SectionTexture(int widthFrames,int heightFrames,int frameOffset,int numFrames,Texture::ptr texture);
	SectionTexture(int widthFrames,int heightFrames,int depthFrames,int frameOffset,int numFrames,Texture::ptr texture);
	virtual ~SectionTexture();

	Texture *getRootTexture(int frame);
	bool getRootTransform(int frame,Matrix4x4 &transform);

	bool create(int usage,Dimension dimension,int format,int width,int height,int depth,int mipLevels,int rowPitches[],int slicePitches[],tbyte *mipDatas[]){
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

	PixelBuffer::ptr getMipPixelBuffer(int i,int cubeSide){return NULL;}
	// These two methods could be implemented, but we would need to basically find which frame we are talking about, and then call load/read on that
	bool load(int width,int height,int depth,int mipLevel,int rowPitch,int slicePitch,tbyte *mipData){return false;}
	bool read(int width,int height,int depth,int mipLevel,int rowPitch,int slicePitch,tbyte *mipData){return false;}

	inline int getWidthFrames() const{return mWidthFrames;}
	inline int getHeightFrames() const{return mHeightFrames;}
	inline int getDepthFrames() const{return mDepthFrames;}
	inline int getFrameOffset() const{return mFrameOffset;}
	int getTotalNumTextures() const;
	int getNumTextures() const;
	inline const Texture::ptr &getTexture() const{return mTexture;}

protected:
	Dimension mDimension;
	int mWidthFrames;
	int mHeightFrames;
	int mDepthFrames;
	int mFrameOffset;
	int mNumFrames;
	Texture::ptr mTexture;
};

}
}

#endif
