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

#ifndef TOADLET_PEEPER_TEXTURESECTION_H
#define TOADLET_PEEPER_TEXTURESECTION_H

#include <toadlet/peeper/AnimatedTexture.h>
#include <toadlet/egg/Collection.h>

namespace toadlet{
namespace peeper{

class TOADLET_API TextureSection:public AnimatedTexture{
public:
	TOADLET_SHARED_POINTERS(TextureSection,egg::Resource);

	TextureSection(int widthFrames,int frameOffset,int numFrames,Texture::ptr texture);
	TextureSection(int widthFrames,int heightFrames,int frameOffset,int numFrames,Texture::ptr texture);
	TextureSection(int widthFrames,int heightFrames,int depthFrames,int frameOffset,int numFrames,Texture::ptr texture);
	virtual ~TextureSection();

	inline int getWidthFrames() const{return mWidthFrames;}
	inline int getHeightFrames() const{return mHeightFrames;}
	inline int getDepthFrames() const{return mDepthFrames;}
	inline int getFrameOffset() const{return mFrameOffset;}
	inline const Texture::ptr &getTexture() const{return mTexture;}

	int getNumFrames() const{return mNumFrames;}
	const Texture::ptr &getTextureForFrame(int frame) const{return mTexture;}
	bool getMatrix4x4ForFrame(int frame,Matrix4x4 &matrix) const;

protected:
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
