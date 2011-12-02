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

package org.toadlet.egg.image;

/*	This may be an instance of the constant interface antipattern, but it achieves the purpose of
	letting both Images and Textures have these attributes without placing them in an external class,
	which is the desired effect. */
public interface ImageDefinitions{
	public enum Dimension{
		UNKNOWN,
		D1,
		D2,
		D3,
		CUBEMAP,
	};

	public enum CubemapSide{
		POSITIVE_X,
		NEGATIVE_X,
		POSITIVE_Y,
		NEGATIVE_Y,
		POSITIVE_Z,
		NEGATIVE_Z,
		MAX
	};

	public static class Format{
		public final static int UNKNOWN=			0;

		public final static int BIT_L=				1<<0;
		public final static int BIT_A=				1<<1;
		public final static int BIT_LA=				1<<2;
		public final static int BIT_RGB=			1<<3;
		public final static int BIT_RGBA=			1<<4;
		public final static int BIT_DEPTH=			1<<5;

		// These formats are stored sequentially in memory
		public final static int BIT_UINT_8=			1<<10;
		public final static int BIT_FLOAT_32=		1<<11;
		// These formats are stored high to low byte in memory
		public final static int BIT_UINT_5_6_5=		1<<12;
		public final static int BIT_UINT_5_5_5_1=	1<<13;
		public final static int BIT_UINT_4_4_4_4=	1<<14;

		public final static int L_8=				BIT_L|BIT_UINT_8;
		public final static int A_8=				BIT_A|BIT_UINT_8;
		public final static int LA_8=				BIT_LA|BIT_UINT_8;
		public final static int RGB_8=				BIT_RGB|BIT_UINT_8;
		public final static int RGBA_8=				BIT_RGBA|BIT_UINT_8;
		public final static int DEPTH_8=			BIT_DEPTH|BIT_UINT_8;
		public final static int L_32=				BIT_L|BIT_FLOAT_32;
		public final static int A_32=				BIT_A|BIT_FLOAT_32;
		public final static int LA_32=				BIT_LA|BIT_FLOAT_32;
		public final static int RGB_32=				BIT_RGB|BIT_FLOAT_32;
		public final static int RGBA_32=			BIT_RGBA|BIT_FLOAT_32;
		public final static int DEPTH_32=			BIT_DEPTH|BIT_FLOAT_32;
		public final static int RGB_5_6_5=			BIT_RGB|BIT_UINT_5_6_5;
		public final static int RGBA_5_5_5_1=		BIT_RGBA|BIT_UINT_5_5_5_1;
		public final static int RGBA_4_4_4_4=		BIT_RGBA|BIT_UINT_4_4_4_4;
	}
}
