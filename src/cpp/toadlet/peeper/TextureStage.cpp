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

#include <toadlet/peeper/TextureStage.h>

namespace toadlet{
namespace peeper{

TextureStage::TextureStage():
	//texture,
	//textureName

	//blend,

	sAddressMode(AddressMode_REPEAT),
	tAddressMode(AddressMode_REPEAT),
	rAddressMode(AddressMode_REPEAT),

	minFilter(Filter_NEAREST),
	mipFilter(Filter_NONE),
	magFilter(Filter_NEAREST),

	texCoordIndex(0),

	calculation(Calculation_DISABLED)
	//matrix,
{
}

TextureStage::TextureStage(const Texture::ptr &texture1):
	//texture,
	//textureName

	//blend,

	sAddressMode(AddressMode_REPEAT),
	tAddressMode(AddressMode_REPEAT),
	rAddressMode(AddressMode_REPEAT),

	minFilter(Filter_NEAREST),
	mipFilter(Filter_NONE),
	magFilter(Filter_NEAREST),

	texCoordIndex(0),

	calculation(Calculation_DISABLED)
	//mMatrix,
{
	texture=texture1;
}
	
void TextureStage::setCalculation(Calculation calculation1,const Matrix4x4 &matrix1){
	calculation=calculation1;
	matrix.set(matrix1);
}

}
}

