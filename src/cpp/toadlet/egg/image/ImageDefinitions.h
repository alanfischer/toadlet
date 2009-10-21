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

#ifndef TOADLET_EGG_IMAGE_IMAGEDEFINITIONS_H
#define TOADLET_EGG_IMAGE_IMAGEDEFINITIONS_H

#include <toadlet/Types.h>

namespace toadlet{
namespace egg{
namespace image{

/*	This may be an instance of the constant interface antipattern, but it achieves the purpose of
	letting both Images and Textures have these attributes without placing them in an external class,
	which is the desired effect. */
class ImageDefinitions{
public:
	enum Dimension{
		Dimension_UNKNOWN=0,
		Dimension_D1,
		Dimension_D2,
		Dimension_D3,
		Dimension_CUBEMAP,
	};

	enum CubemapSide{
		CubemapSide_POSITIVE_X,
		CubemapSide_NEGATIVE_X,
		CubemapSide_POSITIVE_Y,
		CubemapSide_NEGATIVE_Y,
		CubemapSide_POSITIVE_Z,
		CubemapSide_NEGATIVE_Z,
		CubemapSide_MAX,
	};

	enum Format{
		Format_UNKNOWN=				0,

		Format_BIT_L=				1<<0,
		Format_BIT_A=				1<<1,
		Format_BIT_LA=				1<<2,
		Format_BIT_RGB=				1<<3,
		Format_BIT_RGBA=			1<<4,
		Format_BIT_DEPTH=			1<<5,

		// These formats are stored sequentially in memory
		Format_BIT_UINT_8=			1<<10,
		Format_BIT_FLOAT_32=		1<<11,
		// These formats are stored high to low byte in memory
		Format_BIT_UINT_5_6_5=		1<<12,
		Format_BIT_UINT_5_5_5_1=	1<<13,
		Format_BIT_UINT_4_4_4_4=	1<<14,

		Format_L_8=					Format_BIT_L|Format_BIT_UINT_8,
		Format_A_8=					Format_BIT_A|Format_BIT_UINT_8,
		Format_LA_8=				Format_BIT_LA|Format_BIT_UINT_8,
		Format_RGB_8=				Format_BIT_RGB|Format_BIT_UINT_8,
		Format_RGBA_8=				Format_BIT_RGBA|Format_BIT_UINT_8,
		Format_DEPTH_8=				Format_BIT_DEPTH|Format_BIT_UINT_8,
		Format_L_32=				Format_BIT_L|Format_BIT_FLOAT_32,
		Format_A_32=				Format_BIT_A|Format_BIT_FLOAT_32,
		Format_LA_32=				Format_BIT_LA|Format_BIT_FLOAT_32,
		Format_RGB_32=				Format_BIT_RGB|Format_BIT_FLOAT_32,
		Format_RGBA_32=				Format_BIT_RGBA|Format_BIT_FLOAT_32,
		Format_DEPTH_32=			Format_BIT_DEPTH|Format_BIT_FLOAT_32,
		Format_RGB_5_6_5=			Format_BIT_RGB|Format_BIT_UINT_5_6_5,
		Format_RGBA_5_5_5_1=		Format_BIT_RGBA|Format_BIT_UINT_5_5_5_1,
		Format_RGBA_4_4_4_4=		Format_BIT_RGBA|Format_BIT_UINT_4_4_4_4,
	};
};

}
}
}

#endif
