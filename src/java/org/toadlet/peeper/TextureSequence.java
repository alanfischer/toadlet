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

package org.toadlet.peeper;

#include <org/toadlet/tadpole/Types.h>

import java.util.Vector;

public class TextureSequence extends AnimatedTexture{
	public TextureSequence(){}

	public void setNumFrames(int numFrames){mFrames.setSize(numFrames);}
	public int getNumFrames(){return mFrames.size();}

	public void setFrame(int frame,Texture texture){
		mFrames.set(frame,texture);

		// Takes the attributes from the 0th frame
		if(frame==0){
			mDimension=texture.getDimension();
			mFormat=texture.getFormat();
			mWidth=texture.getWidth();
			mHeight=texture.getHeight();
			mDepth=texture.getDepth();
		}
	}

	public Texture getTextureForFrame(int frame){return mFrames.get(frame);}
	public boolean getMatrix4x4ForFrame(int frame,Matrix4x4 matrix){return true;}

	protected Vector<Texture> mFrames=new Vector<Texture>();
	protected int mCurrentFrame;
}
