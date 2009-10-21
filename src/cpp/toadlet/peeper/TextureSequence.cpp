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

#include <toadlet/peeper/TextureSequence.h>

namespace toadlet{
namespace peeper{

TextureSequence::TextureSequence():AnimatedTexture(){
}

TextureSequence::~TextureSequence(){
}

void TextureSequence::setNumFrames(int numFrames){
	mFrames.resize(numFrames);
}

void TextureSequence::setFrame(int frame,const Texture::ptr &texture){
	mFrames[frame]=texture;

	// Takes the attributes from the 0th frame
	if(frame==0){
		mDimension=texture->getDimension();
		mFormat=texture->getFormat();
		mWidth=texture->getWidth();
		mHeight=texture->getHeight();
		mDepth=texture->getDepth();
	}
}

}
}
