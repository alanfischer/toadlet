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
	};

	enum Source{
		Source_UNSPECIFIED,
		Source_PREVIOUS,
		Source_TEXTURE,
		Source_PRIMARY_COLOR,
	};

	TextureBlend(Operation op=Operation_UNSPECIFIED,Source src1=Source_UNSPECIFIED,Source src2=Source_UNSPECIFIED){
		set(op,src1,src2);
	}

	TextureBlend &set(const TextureBlend &blend){
		operation=blend.operation;
		source1=blend.source1;
		source2=blend.source2;

		return *this;
	}

	TextureBlend &set(Operation op,Source src1,Source src2){
		operation=op;
		source1=src1;
		source2=src2;

		return *this;
	}

	inline bool operator==(const TextureBlend &blend) const{
		return (operation==blend.operation && source1==blend.source1 && source2==blend.source2);
	}

	inline bool operator!=(const TextureBlend &blend) const{
		return !(operation==blend.operation && source1==blend.source1 && source2==blend.source2);
	}

	Operation operation;
	Source source1;
	Source source2;
};

}
}

#endif
