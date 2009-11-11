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
	TOADLET_SHARED_POINTERS(TextureStage);

	enum AddressMode{
		AddressMode_REPEAT,
		AddressMode_CLAMP_TO_EDGE,
		AddressMode_CLAMP_TO_BORDER,
		AddressMode_MIRRORED_REPEAT,
	};

	enum Filter{
		Filter_NONE,
		Filter_NEAREST,
		Filter_LINEAR,
	};

	enum Calculation{
		Calculation_DISABLED,
		Calculation_NORMAL,
		Calculation_OBJECTSPACE,
		Calculation_CAMERASPACE,
	};

	TextureStage();
	TextureStage(const Texture::ptr &texture1);
	virtual ~TextureStage(){}

	void setTexture(const Texture::ptr &texture1){texture=texture1;}
	inline const Texture::ptr &getTexture() const{return texture;}
	void setTextureName(const egg::String &name){textureName=name;}
	inline const egg::String &getTextureName() const{return textureName;}

	void setBlend(const TextureBlend &blend1){blend.set(blend1);}
	inline const TextureBlend &getBlend() const{return blend;}

	void setSAddressMode(AddressMode addressMode){sAddressMode=addressMode;}
	inline AddressMode getSAddressMode() const{return sAddressMode;}
	void setTAddressMode(AddressMode addressMode){tAddressMode=addressMode;}
	inline AddressMode getTAddressMode() const{return tAddressMode;}
	void setRAddressMode(AddressMode addressMode){rAddressMode=addressMode;}
	inline AddressMode getRAddressMode() const{return rAddressMode;}

	void setMinFilter(Filter minf){minFilter=minf;}
	inline Filter getMinFilter() const{return minFilter;}
	void setMipFilter(Filter mipf){mipFilter=mipf;}
	inline Filter getMipFilter() const{return mipFilter;}
	void setMagFilter(Filter magf){magFilter=magf;}
	inline Filter getMagFilter() const{return magFilter;}

	void setTexCoordIndex(int index){texCoordIndex=index;}
	inline int getTexCoordIndex() const{return texCoordIndex;}

	void setCalculation(Calculation calculation1,const Matrix4x4 &matrix1);
	inline Calculation getCalculation() const{return calculation;}
	inline const Matrix4x4 &getMatrix() const{return matrix;}

	Texture::ptr texture;
	egg::String textureName;

	TextureBlend blend;

	AddressMode sAddressMode;
	AddressMode tAddressMode;
	AddressMode rAddressMode;

	Filter minFilter;
	Filter mipFilter;
	Filter magFilter;

	int texCoordIndex;

	Calculation calculation;
	Matrix4x4 matrix;
};

}
}

#endif
