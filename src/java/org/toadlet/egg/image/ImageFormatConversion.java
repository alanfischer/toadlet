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

public class ImageFormatConversion implements ImageDefinitions{
	public static byte getRedBits(int format){
		switch(format){
			case Format.RGB_8:
			case Format.RGBA_8:
				return 8;
			case Format.RGB_32:
			case Format.RGBA_32:
				return 32;
			case Format.RGB_5_6_5:
			case Format.RGBA_5_5_5_1:
				return 5;
			case Format.RGBA_4_4_4_4:
				return 4;
			default:
				return 0;
		}
	}		

	public static byte getGreenBits(int format){
		switch(format){
			case Format.RGB_8:
			case Format.RGBA_8:
				return 8;
			case Format.RGB_32:
			case Format.RGBA_32:
				return 32;
			case Format.RGB_5_6_5:
				return 6;
			case Format.RGBA_5_5_5_1:
				return 5;
			case Format.RGBA_4_4_4_4:
				return 4;
			default:
				return 0;
		}
	}		

	public static byte getBlueBits(int format){
		switch(format){
			case Format.RGB_8:
			case Format.RGBA_8:
				return 8;
			case Format.RGB_32:
			case Format.RGBA_32:
				return 32;
			case Format.RGB_5_6_5:
			case Format.RGBA_5_5_5_1:
				return 5;
			case Format.RGBA_4_4_4_4:
				return 4;
			default:
				return 0;
		}
	}		

	public static byte getAlphaBits(int format){
		switch(format){
			case Format.A_8:
			case Format.LA_8:
			case Format.RGBA_8:
				return 8;
			case Format.A_32:
			case Format.LA_32:
			case Format.RGBA_32:
				return 32;
			case Format.RGBA_5_5_5_1:
				return 1;
			case Format.RGBA_4_4_4_4:
				return 4;
			default:
				return 0;
		}
	}

	public static int getRedFromRGB565(int pixel){return (pixel&0xF800)>>8;}
	public static int getGreenFromRGB565(int pixel){return (pixel&0x7E0)>>3;}
	public static int getBlueFromRGB565(int pixel){return (pixel&0x1F)<<3;}

	public static int getRedFromRGBA5551(int pixel){return (pixel&0xF800)>>8;}
	public static int getGreenFromRGBA5551(int pixel){return (pixel&0x7C0)>>3;}
	public static int getBlueFromRGBA5551(int pixel){return (pixel&0x3E)<<2;}
	public static int getAlphaFromRGBA5551(int pixel){return (pixel&0x1)<<7;}

	public static int getRedFromRGBA4444(int pixel){return (pixel&0xF000)>>8;}
	public static int getGreenFromRGBA4444(int pixel){return (pixel&0xF00)>>4;}
	public static int getBlueFromRGBA4444(int pixel){return (pixel&0xF0);}
	public static int getAlphaFromRGBA4444(int pixel){return (pixel&0xF)<<4;}

	public static int makeRGB565(byte red,byte green,byte blue){
		return ((red<<8)&0xF800) | ((green<<3)&0x7E0) | ((blue>>3)&0x1F);
	}

	public static int makeRGBA5551(byte red,byte green,byte blue,byte alpha){
		return ((red<<8)&0xF800) | ((green<<3)&0x7C0) | ((blue>>2)&0x3E) | ((alpha>>7)&0x1);
	}

	public static int makeRGBA4444(byte red,byte green,byte blue,byte alpha){
		return ((red<<8)&0xF000) | ((green<<4)&0xF00) | ((blue)&0xF0) | ((alpha<<4)&0xF);
	}
	
	public static int getPixelSize(int format){
		switch(format){
			case Format.L_8:
			case Format.A_8:
				return 1;
			case Format.LA_8:
				return 2;
			case Format.RGB_8:
				return 3;
			case Format.RGBA_8:
				return 4;
			case Format.RGB_5_6_5:
			case Format.RGBA_5_5_5_1:
			case Format.RGBA_4_4_4_4:
				return 2;
			case Format.L_32:
			case Format.A_32:
				return 4;
			case Format.LA_32:
				return 8;
			case Format.RGB_32:
				return 12;
			case Format.RGBA_32:
				return 16;
			default:
				return 0;
		}
	}
}
