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

#ifndef TOADLET_PEEPER_TEXTURESTATE_H
#define TOADLET_PEEPER_TEXTURESTATE_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TOADLET_API TOADLET_ALIGNED TextureState{
public:
	TOADLET_ALIGNED_NEW;

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
		Source_CONSTANT_COLOR,
	};

	enum CalculationType{
		CalculationType_DISABLED,
		CalculationType_NORMAL,
		CalculationType_OBJECTSPACE,
		CalculationType_CAMERASPACE,
	};

	enum ShadowResult{
		ShadowResult_DISABLED, 
		ShadowResult_L,
		ShadowResult_A,
	};

	TextureState(Operation colorOp=Operation_UNSPECIFIED,Source colorSrc1=Source_UNSPECIFIED,Source colorSrc2=Source_UNSPECIFIED,Source colorSrc3=Source_UNSPECIFIED,Operation alphaOp=Operation_UNSPECIFIED,Source alphaSrc1=Source_UNSPECIFIED,Source alphaSrc2=Source_UNSPECIFIED,Source alphaSrc3=Source_UNSPECIFIED):
		colorOperation(colorOp),
		colorSource1(colorSrc1),
		colorSource2(colorSrc2),
		colorSource3(colorSrc3),

		alphaOperation(alphaOp),
		alphaSource1(alphaSrc1),
		alphaSource2(alphaSrc2),
		alphaSource3(alphaSrc3),

		//constantColor,
		
		shadowResult(ShadowResult_DISABLED),
		
		texCoordIndex(0),
		calculation(CalculationType_DISABLED)
		//matrix
	{}

	TextureState &set(const TextureState &state){
		colorOperation=state.colorOperation;
		colorSource1=state.colorSource1;
		colorSource2=state.colorSource2;
		colorSource3=state.colorSource3;

		alphaOperation=state.alphaOperation;
		alphaSource1=state.alphaSource1;
		alphaSource2=state.alphaSource2;
		alphaSource3=state.alphaSource3;

		constantColor.set(state.constantColor);

		shadowResult=(state.shadowResult);
		
		texCoordIndex=state.texCoordIndex;
		calculation=state.calculation;
		matrix.set(state.matrix);
	
		return *this;
	}

	inline bool equals(const TextureState &state) const{
		return (colorOperation==state.colorOperation && colorSource1==state.colorSource1 && colorSource2==state.colorSource2 && colorSource3==state.colorSource3 &&
			alphaOperation==state.alphaOperation && alphaSource1==state.alphaSource1 && alphaSource2==state.alphaSource2 && alphaSource3==state.alphaSource3 &&
			constantColor.equals(state.constantColor) && shadowResult==state.shadowResult &&
			texCoordIndex==state.texCoordIndex && calculation==state.calculation && matrix.equals(state.matrix));
	}

	inline bool operator==(const TextureState &state) const{
		return equals(state);
	}

	inline bool operator!=(const TextureState &state) const{
		return !equals(state);
	}

	Operation colorOperation;
	Source colorSource1;
	Source colorSource2;
	Source colorSource3;
	
	Operation alphaOperation;
	Source alphaSource1;
	Source alphaSource2;
	Source alphaSource3;

	Vector4 constantColor;

	// Only supported on GL
	ShadowResult shadowResult;

	int texCoordIndex;
	CalculationType calculation;
	Matrix4x4 matrix;
};

}
}

#endif
