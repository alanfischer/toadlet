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

#ifndef TOADLET_PEEPER_TEXTURESTAGE_H
#define TOADLET_PEEPER_TEXTURESTAGE_H

#include <toadlet/peeper/TextureBlend.h>
#include <toadlet/peeper/Texture.h>

namespace toadlet{
namespace peeper{

class TOADLET_API TextureStage{
public:
	TOADLET_SHARED_POINTERS(TextureStage,TextureStage);

	TextureStage();
	TextureStage(const Texture::ptr &texture);
	virtual ~TextureStage(){}

	void setTexture(const Texture::ptr &texture){mTexture=texture;}
	inline const Texture::ptr &getTexture() const{return mTexture;}

	void setTextureName(const egg::String &textureName){mTextureName=textureName;}
	inline const egg::String &getTextureName() const{return mTextureName;}

	void setBlend(const TextureBlend &blend){mBlend.set(blend);}
	inline const TextureBlend &getBlend() const{return mBlend;}

	void setSAddressMode(Texture::AddressMode addressMode);
	inline Texture::AddressMode getSAddressMode() const{return mSAddressMode;}
	void setTAddressMode(Texture::AddressMode addressMode);
	inline Texture::AddressMode getTAddressMode() const{return mTAddressMode;}
	void setRAddressMode(Texture::AddressMode addressMode);
	inline Texture::AddressMode getRAddressMode() const{return mRAddressMode;}
	inline bool getAddressModeSpecified() const{return mAddressModeSpecified;}

	void setMinFilter(Texture::Filter minFilter);
	inline Texture::Filter getMinFilter() const{return mMinFilter;}
	void setMipFilter(Texture::Filter mipFilter);
	inline Texture::Filter getMipFilter() const{return mMipFilter;}
	void setMagFilter(Texture::Filter magFilter);
	inline Texture::Filter getMagFilter() const{return mMagFilter;}
	inline bool getFilterSpecified() const{return mFilterSpecified;}

	void setTexCoordIndex(int index){mTexCoordIndex=index;}
	inline int getTexCoordIndex() const{return mTexCoordIndex;}

	void setTextureMatrix(const Matrix4x4 &matrix);
	inline const Matrix4x4 &getTextureMatrix() const{return mTextureMatrix;}

	inline bool getTextureMatrixIdentity() const{return mTextureMatrixIdentity;}

protected:
	Texture::ptr mTexture;
	egg::String mTextureName;

	TextureBlend mBlend;

	Texture::AddressMode mSAddressMode;
	Texture::AddressMode mTAddressMode;
	Texture::AddressMode mRAddressMode;
	bool mAddressModeSpecified;

	Texture::Filter mMinFilter;
	Texture::Filter mMipFilter;
	Texture::Filter mMagFilter;
	bool mFilterSpecified;

	int mTexCoordIndex;

	///@todo  Change this so it could be on the heap, and NULL would mean identity.  Would save mem & translate to java fine
	Matrix4x4 mTextureMatrix;
	bool mTextureMatrixIdentity;
};

}
}

#endif
