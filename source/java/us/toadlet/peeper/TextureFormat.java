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

package us.toadlet.peeper;

public class TextureFormat{
		public static final int Format_UNKNOWN=				0;

		// Format semantics
		public static final int Format_MASK_SEMANTICS=		0xFF;
		public static final int Format_SEMANTIC_L=			1;
		public static final int Format_SEMANTIC_A=			2;
		public static final int Format_SEMANTIC_LA=			3;
		public static final int Format_SEMANTIC_R=			4;
		public static final int Format_SEMANTIC_RG=			5;
		public static final int Format_SEMANTIC_RGB=		6;
		public static final int Format_SEMANTIC_BGR=		7;
		public static final int Format_SEMANTIC_RGBA=		8;
		public static final int Format_SEMANTIC_BGRA=		9;
		public static final int Format_SEMANTIC_DEPTH=		10;

		// Format types
		public static final int Format_SHIFT_TYPES=			8;
		public static final int Format_MASK_TYPES=			0xFF00;
		public static final int Format_TYPE_UINT_8=			1<<Format_SHIFT_TYPES;
		public static final int Format_TYPE_UINT_16=		2<<Format_SHIFT_TYPES;
		public static final int Format_TYPE_UINT_24=		3<<Format_SHIFT_TYPES;
		public static final int Format_TYPE_UINT_32=		4<<Format_SHIFT_TYPES;
		public static final int Format_TYPE_FLOAT_16=		5<<Format_SHIFT_TYPES;
		public static final int Format_TYPE_FLOAT_32=		6<<Format_SHIFT_TYPES;
		public static final int Format_TYPE_UINT_5_6_5=		7<<Format_SHIFT_TYPES;
		public static final int Format_TYPE_UINT_5_5_5_1=	8<<Format_SHIFT_TYPES;
		public static final int Format_TYPE_UINT_4_4_4_4=	9<<Format_SHIFT_TYPES;
		public static final int Format_TYPE_DXT1=			10<<Format_SHIFT_TYPES;
		public static final int Format_TYPE_DXT2=			11<<Format_SHIFT_TYPES;
		public static final int Format_TYPE_DXT3=			12<<Format_SHIFT_TYPES;
		public static final int Format_TYPE_DXT4=			13<<Format_SHIFT_TYPES;
		public static final int Format_TYPE_DXT5=			14<<Format_SHIFT_TYPES;

		public static final int Format_L_8=					Format_SEMANTIC_L|Format_TYPE_UINT_8;
		public static final int Format_R_8=					Format_SEMANTIC_R|Format_TYPE_UINT_8;
		public static final int Format_A_8=					Format_SEMANTIC_A|Format_TYPE_UINT_8;
		public static final int Format_LA_8=				Format_SEMANTIC_LA|Format_TYPE_UINT_8;
		public static final int Format_RG_8=				Format_SEMANTIC_RG|Format_TYPE_UINT_8;
		public static final int Format_RGB_8=				Format_SEMANTIC_RGB|Format_TYPE_UINT_8;
		public static final int Format_BGR_8=				Format_SEMANTIC_BGR|Format_TYPE_UINT_8;
		public static final int Format_RGBA_8=				Format_SEMANTIC_RGBA|Format_TYPE_UINT_8;
		public static final int Format_BGRA_8=				Format_SEMANTIC_BGRA|Format_TYPE_UINT_8;
		public static final int Format_DEPTH_8=				Format_SEMANTIC_DEPTH|Format_TYPE_UINT_8;
		public static final int Format_DEPTH_16=			Format_SEMANTIC_DEPTH|Format_TYPE_UINT_16;
		public static final int Format_DEPTH_24=			Format_SEMANTIC_DEPTH|Format_TYPE_UINT_24;
		public static final int Format_DEPTH_32=			Format_SEMANTIC_DEPTH|Format_TYPE_UINT_32;
		public static final int Format_L_F32=				Format_SEMANTIC_L|Format_TYPE_FLOAT_32;
		public static final int Format_A_F32=				Format_SEMANTIC_A|Format_TYPE_FLOAT_32;
		public static final int Format_LA_F32=				Format_SEMANTIC_LA|Format_TYPE_FLOAT_32;
		public static final int Format_RGB_F32=				Format_SEMANTIC_RGB|Format_TYPE_FLOAT_32;
		public static final int Format_BGR_F32=				Format_SEMANTIC_BGR|Format_TYPE_FLOAT_32;
		public static final int Format_RGBA_F32=			Format_SEMANTIC_RGBA|Format_TYPE_FLOAT_32;
		public static final int Format_BGRA_F32=			Format_SEMANTIC_BGRA|Format_TYPE_FLOAT_32;
		public static final int Format_DEPTH_F32=			Format_SEMANTIC_DEPTH|Format_TYPE_FLOAT_32;
		public static final int Format_RGB_5_6_5=			Format_SEMANTIC_RGB|Format_TYPE_UINT_5_6_5;
		public static final int Format_BGR_5_6_5=			Format_SEMANTIC_BGR|Format_TYPE_UINT_5_6_5;
		public static final int Format_RGBA_5_5_5_1=		Format_SEMANTIC_RGBA|Format_TYPE_UINT_5_5_5_1;
		public static final int Format_BGRA_5_5_5_1=		Format_SEMANTIC_BGRA|Format_TYPE_UINT_5_5_5_1;
		public static final int Format_RGBA_4_4_4_4=		Format_SEMANTIC_RGBA|Format_TYPE_UINT_4_4_4_4;
		public static final int Format_BGRA_4_4_4_4=		Format_SEMANTIC_BGRA|Format_TYPE_UINT_4_4_4_4;
		public static final int Format_RGB_DXT1=			Format_SEMANTIC_RGB|Format_TYPE_DXT1;
		public static final int Format_RGBA_DXT2=			Format_SEMANTIC_RGBA|Format_TYPE_DXT2;
		public static final int Format_RGBA_DXT3=			Format_SEMANTIC_RGBA|Format_TYPE_DXT3;
		public static final int Format_RGBA_DXT4=			Format_SEMANTIC_RGBA|Format_TYPE_DXT4;
		public static final int Format_RGBA_DXT5=			Format_SEMANTIC_RGBA|Format_TYPE_DXT5;

	public static int getRedBits(int format){
		switch(format){
			case Format_RGB_8:
			case Format_RGBA_8:
			case Format_BGR_8:
			case Format_BGRA_8:
				return 8;
			case Format_RGB_F32:
			case Format_RGBA_F32:
			case Format_BGR_F32:
			case Format_BGRA_F32:
				return 32;
			case Format_RGB_5_6_5:
			case Format_RGBA_5_5_5_1:
			case Format_BGR_5_6_5:
			case Format_BGRA_5_5_5_1:
				return 5;
			case Format_RGBA_4_4_4_4:
			case Format_BGRA_4_4_4_4:
				return 4;
			default:
				return 0;
		}
	}		

	public static int getGreenBits(int format){
		switch(format){
			case Format_RGB_8:
			case Format_RGBA_8:
			case Format_BGR_8:
			case Format_BGRA_8:
				return 8;
			case Format_RGB_F32:
			case Format_RGBA_F32:
			case Format_BGR_F32:
			case Format_BGRA_F32:
				return 32;
			case Format_RGB_5_6_5:
			case Format_BGR_5_6_5:
				return 6;
			case Format_RGBA_5_5_5_1:
			case Format_BGRA_5_5_5_1:
				return 5;
			case Format_RGBA_4_4_4_4:
			case Format_BGRA_4_4_4_4:
				return 4;
			default:
				return 0;
		}
	}		

	public static int getBlueBits(int format){
		switch(format){
			case Format_RGB_8:
			case Format_RGBA_8:
			case Format_BGR_8:
			case Format_BGRA_8:
				return 8;
			case Format_RGB_F32:
			case Format_RGBA_F32:
			case Format_BGR_F32:
			case Format_BGRA_F32:
				return 32;
			case Format_RGB_5_6_5:
			case Format_RGBA_5_5_5_1:
			case Format_BGR_5_6_5:
			case Format_BGRA_5_5_5_1:
				return 5;
			case Format_RGBA_4_4_4_4:
			case Format_BGRA_4_4_4_4:
				return 4;
			default:
				return 0;
		}
	}		

	public static int getAlphaBits(int format){
		switch(format){
			case Format_A_8:
			case Format_LA_8:
			case Format_RGBA_8:
			case Format_BGRA_8:
				return 8;
			case Format_A_F32:
			case Format_LA_F32:
			case Format_RGBA_F32:
			case Format_BGRA_F32:
				return 32;
			case Format_RGBA_5_5_5_1:
			case Format_BGRA_5_5_5_1:
				return 1;
			case Format_RGBA_4_4_4_4:
			case Format_BGRA_4_4_4_4:
				return 4;
			default:
				return 0;
		}
	}
}
