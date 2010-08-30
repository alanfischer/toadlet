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

#ifndef TOADLET_PEEPER_TEXTUREBLEND_H
#define TOADLET_PEEPER_TEXTUREBLEND_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TextureBlend{
public:
	enum Operation{
		Operation_UNSPECIFIED,
		Operation_REPLACE,
		Operation_ADD,
		Operation_MODULATE,
		Operation_MODULATE_2X,
		Operation_MODULATE_4X,
		Operation_DOTPRODUCT,
		Operation_ALPHABLEND,
	};

	enum Source{
		Source_UNSPECIFIED,
		Source_PREVIOUS,
		Source_TEXTURE,
		Source_PRIMARY_COLOR,
	};

	TextureBlend(Operation colorOp=Operation_UNSPECIFIED,Source colorSrc1=Source_UNSPECIFIED,Source colorSrc2=Source_UNSPECIFIED,Source colorSrc3=Source_UNSPECIFIED,Operation alphaOp=Operation_UNSPECIFIED,Source alphaSrc1=Source_UNSPECIFIED,Source alphaSrc2=Source_UNSPECIFIED,Source alphaSrc3=Source_UNSPECIFIED){
		set(colorOp,colorSrc1,colorSrc2,colorSrc3,alphaOp,alphaSrc1,alphaSrc2,alphaSrc3);
	}

	TextureBlend &set(const TextureBlend &blend){
		colorOperation=blend.colorOperation;
		colorSource1=blend.colorSource1;
		colorSource2=blend.colorSource2;
		colorSource3=blend.colorSource3;
		alphaOperation=blend.alphaOperation;
		alphaSource1=blend.alphaSource1;
		alphaSource2=blend.alphaSource2;
		alphaSource3=blend.alphaSource3;

		return *this;
	}

	TextureBlend &set(Operation colorOp,Source colorSrc1,Source colorSrc2,Source colorSrc3,Operation alphaOp,Source alphaSrc1,Source alphaSrc2,Source alphaSrc3){
		setColor(colorOp,colorSrc1,colorSrc2,colorSrc3);
		setAlpha(alphaOp,alphaSrc1,alphaSrc2,colorSrc3);

		return *this;
	}

	TextureBlend &setColor(Operation colorOp,Source colorSrc1,Source colorSrc2,Source colorSrc3){
		colorOperation=colorOp;
		colorSource1=colorSrc1;
		colorSource2=colorSrc2;
		colorSource3=colorSrc3;

		return *this;
	}

	TextureBlend &setAlpha(Operation alphaOp,Source alphaSrc1,Source alphaSrc2,Source alphaSrc3){
		alphaOperation=alphaOp;
		alphaSource1=alphaSrc1;
		alphaSource2=alphaSrc2;
		alphaSource3=alphaSrc3;

		return *this;
	}

	inline bool operator==(const TextureBlend &blend) const{
		return (colorOperation==blend.colorOperation&& colorSource1==blend.colorSource1 && colorSource2==blend.colorSource2 && colorSource3==blend.colorSource3 &&
			alphaOperation==blend.alphaOperation && alphaSource1==blend.alphaSource1 && alphaSource2==blend.alphaSource2 && alphaSource3==blend.alphaSource3);
	}

	inline bool operator!=(const TextureBlend &blend) const{
		return !(colorOperation==blend.colorOperation&& colorSource1==blend.colorSource1 && colorSource2==blend.colorSource2 && colorSource3==blend.colorSource3 &&
			alphaOperation==blend.alphaOperation && alphaSource1==blend.alphaSource1 && alphaSource2==blend.alphaSource2 && alphaSource3==blend.alphaSource3);
	}

	Operation colorOperation;
	Source colorSource1;
	Source colorSource2;
	Source colorSource3;
	Operation alphaOperation;
	Source alphaSource1;
	Source alphaSource2;
	Source alphaSource3;
};

}
}

#endif
