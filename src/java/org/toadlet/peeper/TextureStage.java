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

#include <org/toadlet/peeper/Types.h>

public class TextureStage{
	public TextureStage(){}
	public TextureStage(Texture texture){mTexture=texture;}

	public void setTexture(Texture texture){mTexture=texture;}
	public Texture getTexture(){return mTexture;}

	public void setTextureName(String textureName){mTextureName=textureName;}
	public String getTextureName(){return mTextureName;}
	
	public void setBlend(TextureBlend blend){mBlend.set(blend);}
	public TextureBlend getBlend(){return mBlend;}

	public void setSAddressMode(Texture.AddressMode addressMode){
		mSAddressMode=addressMode;
		mAddressModeSpecified=true;
	}

	public Texture.AddressMode getSAddressMode(){return mSAddressMode;}

	public void setTAddressMode(Texture.AddressMode addressMode){
		mTAddressMode=addressMode;
		mAddressModeSpecified=true;
	}

	public Texture.AddressMode getTAddressMode(){return mTAddressMode;}

	public void setRAddressMode(Texture.AddressMode addressMode){
		mRAddressMode=addressMode;
		mAddressModeSpecified=true;
	}

	public Texture.AddressMode getRAddressMode(){return mRAddressMode;}

	public boolean getAddressModeSpecified(){return mAddressModeSpecified;}

	public void setMinFilter(Texture.Filter minFilter){
		mMinFilter=minFilter;
		mFilterSpecified=true;
	}

	public Texture.Filter getMinFilter(){return mMinFilter;}

	public void setMipFilter(Texture.Filter mipFilter){
		mMipFilter=mipFilter;
		mFilterSpecified=true;
	}

	public Texture.Filter getMipFilter(){return mMipFilter;}

	public void setMagFilter(Texture.Filter magFilter){
		mMagFilter=magFilter;
		mFilterSpecified=true;
	}

	public Texture.Filter getMagFilter(){return mMagFilter;}

	public boolean getFilterSpecified(){return mFilterSpecified;}

	public void setTexCoordIndex(int index){mTexCoordIndex=index;}
	public int getTexCoordIndex(){return mTexCoordIndex;}

	public void setTextureMatrix(Matrix4x4 matrix){
		mTextureMatrix.set(matrix);
		mTextureMatrixIdentity=false;
	}

	public Matrix4x4 getTextureMatrix(){return mTextureMatrix;}

	public boolean getTextureMatrixIdentity(){return mTextureMatrixIdentity;}

	Texture mTexture;
	String mTextureName;

	TextureBlend mBlend=new TextureBlend();

	Texture.AddressMode mSAddressMode=Texture.AddressMode.REPEAT;
	Texture.AddressMode mTAddressMode=Texture.AddressMode.REPEAT;
	Texture.AddressMode mRAddressMode=Texture.AddressMode.REPEAT;
	boolean mAddressModeSpecified=false;

	Texture.Filter mMinFilter=Texture.Filter.NEAREST;
	Texture.Filter mMipFilter=Texture.Filter.NONE;
	Texture.Filter mMagFilter=Texture.Filter.NEAREST;
	boolean mFilterSpecified=false;

	int mTexCoordIndex=0;

	Matrix4x4 mTextureMatrix=new Matrix4x4();
	boolean mTextureMatrixIdentity=true;
}
