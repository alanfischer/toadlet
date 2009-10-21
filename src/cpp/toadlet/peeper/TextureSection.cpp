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

#include <toadlet/peeper/TextureSection.h>

namespace toadlet{
namespace peeper{

TextureSection::TextureSection(int widthFrames,int frameOffset,int numFrames,Texture::ptr texture):AnimatedTexture(){
	mWidthFrames=widthFrames;
	mHeightFrames=0;
	mDepthFrames=0;
	mFrameOffset=frameOffset;
	mNumFrames=numFrames;
	mTexture=texture;
}

TextureSection::TextureSection(int widthFrames,int heightFrames,int frameOffset,int numFrames,Texture::ptr texture):AnimatedTexture(){
	mWidthFrames=widthFrames;
	mHeightFrames=heightFrames;
	mDepthFrames=0;
	mFrameOffset=frameOffset;
	mNumFrames=numFrames;
	mTexture=texture;
}

TextureSection::TextureSection(int widthFrames,int heightFrames,int depthFrames,int frameOffset,int numFrames,Texture::ptr texture):AnimatedTexture(){
	mWidthFrames=widthFrames;
	mHeightFrames=heightFrames;
	mDepthFrames=depthFrames;
	mFrameOffset=frameOffset;
	mNumFrames=numFrames;
	mTexture=texture;
}

TextureSection::~TextureSection(){
}

bool TextureSection::getMatrix4x4ForFrame(int frame,Matrix4x4 &matrix) const{
	int v;
	scalar scale;
	scalar offset;

	if(mWidthFrames>0){
		v=(frame+mFrameOffset)%mWidthFrames;
		scale=Math::div(Math::ONE,Math::fromInt(mWidthFrames));
		offset=Math::mul(Math::fromInt(v),scale);
		matrix.setAt(0,0,scale);
		matrix.setAt(0,3,offset);
	}

	if(mHeightFrames>0){
		v=((frame+mFrameOffset)/mWidthFrames)%mHeightFrames;
		scale=Math::div(Math::ONE,Math::fromInt(mHeightFrames));
		offset=Math::mul(Math::fromInt(v),scale);
		matrix.setAt(1,1,scale);
		matrix.setAt(1,3,offset);
	}

	if(mDepthFrames>0){
		v=((frame+mFrameOffset)/(mWidthFrames*mHeightFrames))%mDepthFrames;
		scale=Math::div(Math::ONE,Math::fromInt(mDepthFrames));
		offset=Math::mul(Math::fromInt(v),scale);
		matrix.setAt(2,2,scale);
		matrix.setAt(2,3,offset);
	}

	return false;
}

}
}
