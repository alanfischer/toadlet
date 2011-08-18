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

	enum Format{
		Format_UNKNOWN=				0,

		// Format semantics
		Format_MASK_SEMANTICS=		0xFF,
		Format_SEMANTIC_L=			1,
		Format_SEMANTIC_A=			2,
		Format_SEMANTIC_LA=			3,
		Format_SEMANTIC_R=			4,
		Format_SEMANTIC_RG=			5,
		Format_SEMANTIC_RGB=		6,
		Format_SEMANTIC_BGR=		7,
		Format_SEMANTIC_RGBA=		8,
		Format_SEMANTIC_BGRA=		9,
		Format_SEMANTIC_DEPTH=		10,

		// Format types
		Format_SHIFT_TYPES=			8,
		Format_MASK_TYPES=			0xFF00,
		Format_TYPE_UINT_8=			1<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_16=		2<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_24=		3<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_32=		4<<Format_SHIFT_TYPES,
		Format_TYPE_FLOAT_16=		5<<Format_SHIFT_TYPES,
		Format_TYPE_FLOAT_32=		6<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_5_6_5=		7<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_5_5_5_1=	8<<Format_SHIFT_TYPES,
		Format_TYPE_UINT_4_4_4_4=	9<<Format_SHIFT_TYPES,
		Format_TYPE_DXT1=			10<<Format_SHIFT_TYPES,
		Format_TYPE_DXT2=			11<<Format_SHIFT_TYPES,
		Format_TYPE_DXT3=			12<<Format_SHIFT_TYPES,
		Format_TYPE_DXT4=			13<<Format_SHIFT_TYPES,
		Format_TYPE_DXT5=			14<<Format_SHIFT_TYPES,

		Format_L_8=					Format_SEMANTIC_L|Format_TYPE_UINT_8,
		Format_R_8=					Format_SEMANTIC_R|Format_TYPE_UINT_8,
		Format_A_8=					Format_SEMANTIC_A|Format_TYPE_UINT_8,
		Format_LA_8=				Format_SEMANTIC_LA|Format_TYPE_UINT_8,
		Format_RG_8=				Format_SEMANTIC_RG|Format_TYPE_UINT_8,
		Format_RGB_8=				Format_SEMANTIC_RGB|Format_TYPE_UINT_8,
		Format_BGR_8=				Format_SEMANTIC_BGR|Format_TYPE_UINT_8,
		Format_RGBA_8=				Format_SEMANTIC_RGBA|Format_TYPE_UINT_8,
		Format_BGRA_8=				Format_SEMANTIC_BGRA|Format_TYPE_UINT_8,
		Format_DEPTH_8=				Format_SEMANTIC_DEPTH|Format_TYPE_UINT_8,
		Format_DEPTH_16=			Format_SEMANTIC_DEPTH|Format_TYPE_UINT_16,
		Format_DEPTH_24=			Format_SEMANTIC_DEPTH|Format_TYPE_UINT_24,
		Format_DEPTH_32=			Format_SEMANTIC_DEPTH|Format_TYPE_UINT_32,
		Format_L_F32=				Format_SEMANTIC_L|Format_TYPE_FLOAT_32,
		Format_A_F32=				Format_SEMANTIC_A|Format_TYPE_FLOAT_32,
		Format_LA_F32=				Format_SEMANTIC_LA|Format_TYPE_FLOAT_32,
		Format_RGB_F32=				Format_SEMANTIC_RGB|Format_TYPE_FLOAT_32,
		Format_BGR_F32=				Format_SEMANTIC_BGR|Format_TYPE_FLOAT_32,
		Format_RGBA_F32=			Format_SEMANTIC_RGBA|Format_TYPE_FLOAT_32,
		Format_BGRA_F32=			Format_SEMANTIC_BGRA|Format_TYPE_FLOAT_32,
		Format_DEPTH_F32=			Format_SEMANTIC_DEPTH|Format_TYPE_FLOAT_32,
		Format_RGB_5_6_5=			Format_SEMANTIC_RGB|Format_TYPE_UINT_5_6_5,
		Format_BGR_5_6_5=			Format_SEMANTIC_BGR|Format_TYPE_UINT_5_6_5,
		Format_RGBA_5_5_5_1=		Format_SEMANTIC_RGBA|Format_TYPE_UINT_5_5_5_1,
		Format_BGRA_5_5_5_1=		Format_SEMANTIC_BGRA|Format_TYPE_UINT_5_5_5_1,
		Format_RGBA_4_4_4_4=		Format_SEMANTIC_RGBA|Format_TYPE_UINT_4_4_4_4,
		Format_BGRA_4_4_4_4=		Format_SEMANTIC_BGRA|Format_TYPE_UINT_4_4_4_4,
		Format_RGB_DXT1=			Format_SEMANTIC_RGB|Format_TYPE_DXT1,
		Format_RGBA_DXT2=			Format_SEMANTIC_RGBA|Format_TYPE_DXT2,
		Format_RGBA_DXT3=			Format_SEMANTIC_RGBA|Format_TYPE_DXT3,
		Format_RGBA_DXT4=			Format_SEMANTIC_RGBA|Format_TYPE_DXT4,
		Format_RGBA_DXT5=			Format_SEMANTIC_RGBA|Format_TYPE_DXT5,
	};
};

}
}
}

#endif
