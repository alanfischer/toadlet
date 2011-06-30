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
namespace image{};
using namespace image;

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
		Dimension_CUBE,
	};

	enum CubeSide{
		CubeSide_POSITIVE_X=0,
		CubeSide_NEGATIVE_X=1,
		CubeSide_POSITIVE_Y=2,
		CubeSide_NEGATIVE_Y=3,
		CubeSide_POSITIVE_Z=4,
		CubeSide_NEGATIVE_Z=5,
		CubeSide_MAX=6,
	};

	/// @todo: Clean up this Format and merge it with the VertexFormat
	enum Format{
		Format_UNKNOWN=				0,

		Format_BIT_L=				1<<0,
		Format_BIT_A=				1<<1,
		Format_BIT_LA=				1<<2,
		Format_BIT_R=				1<<3,
		Format_BIT_RG=				1<<4,
		Format_BIT_RGB=				1<<5,
		Format_BIT_BGR=				1<<6,
		Format_BIT_RGBA=			1<<7,
		Format_BIT_BGRA=			1<<8,
		Format_BIT_DEPTH=			1<<9,
		Format_ALL_COLOR=			0xFFFF,

		// These formats are stored sequentially in memory
		Format_BIT_UINT_8=			1<<11,
		Format_BIT_UINT_16=			1<<12,
		Format_BIT_UINT_24=			1<<13,
		Format_BIT_UINT_32=			1<<14,
		Format_BIT_FLOAT_16=		1<<15,
		Format_BIT_FLOAT_32=		1<<16,
		// These formats are stored high to low byte in memory
		Format_BIT_UINT_5_6_5=		1<<17,
		Format_BIT_UINT_5_5_5_1=	1<<18,
		Format_BIT_UINT_4_4_4_4=	1<<19,
		// These formats are compressed
		Format_BIT_DXT1=			1<<20,
		Format_BIT_DXT2=			1<<21,
		Format_BIT_DXT3=			1<<22,
		Format_BIT_DXT4=			1<<23,
		Format_BIT_DXT5=			1<<24,
		Format_ALL_SIZE=			0xFFFF0000,

		Format_L_8=					Format_BIT_L|Format_BIT_UINT_8,
		Format_R_8=					Format_BIT_R|Format_BIT_UINT_8,
		Format_A_8=					Format_BIT_A|Format_BIT_UINT_8,
		Format_LA_8=				Format_BIT_LA|Format_BIT_UINT_8,
		Format_RG_8=				Format_BIT_RG|Format_BIT_UINT_8,
		Format_RGB_8=				Format_BIT_RGB|Format_BIT_UINT_8,
		Format_BGR_8=				Format_BIT_BGR|Format_BIT_UINT_8,
		Format_RGBA_8=				Format_BIT_RGBA|Format_BIT_UINT_8,
		Format_BGRA_8=				Format_BIT_BGRA|Format_BIT_UINT_8,
		Format_DEPTH_8=				Format_BIT_DEPTH|Format_BIT_UINT_8,
		Format_DEPTH_16=			Format_BIT_DEPTH|Format_BIT_UINT_16,
		Format_DEPTH_24=			Format_BIT_DEPTH|Format_BIT_UINT_24,
		Format_DEPTH_32=			Format_BIT_DEPTH|Format_BIT_UINT_32,
		Format_L_F32=				Format_BIT_L|Format_BIT_FLOAT_32,
		Format_A_F32=				Format_BIT_A|Format_BIT_FLOAT_32,
		Format_LA_F32=				Format_BIT_LA|Format_BIT_FLOAT_32,
		Format_RGB_F32=				Format_BIT_RGB|Format_BIT_FLOAT_32,
		Format_BGR_F32=				Format_BIT_BGR|Format_BIT_FLOAT_32,
		Format_RGBA_F32=			Format_BIT_RGBA|Format_BIT_FLOAT_32,
		Format_BGRA_F32=			Format_BIT_BGRA|Format_BIT_FLOAT_32,
		Format_DEPTH_F32=			Format_BIT_DEPTH|Format_BIT_FLOAT_32,
		Format_RGB_5_6_5=			Format_BIT_RGB|Format_BIT_UINT_5_6_5,
		Format_BGR_5_6_5=			Format_BIT_BGR|Format_BIT_UINT_5_6_5,
		Format_RGBA_5_5_5_1=		Format_BIT_RGBA|Format_BIT_UINT_5_5_5_1,
		Format_BGRA_5_5_5_1=		Format_BIT_BGRA|Format_BIT_UINT_5_5_5_1,
		Format_RGBA_4_4_4_4=		Format_BIT_RGBA|Format_BIT_UINT_4_4_4_4,
		Format_BGRA_4_4_4_4=		Format_BIT_BGRA|Format_BIT_UINT_4_4_4_4,
		Format_RGBA_DXT1=			Format_BIT_DXT1,
		Format_RGBA_DXT2=			Format_BIT_DXT2,
		Format_RGBA_DXT3=			Format_BIT_DXT3,
		Format_RGBA_DXT4=			Format_BIT_DXT4,
		Format_RGBA_DXT5=			Format_BIT_DXT5,
	};
};

}
}
}

#endif
