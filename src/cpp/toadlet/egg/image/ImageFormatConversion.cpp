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

#include <toadlet/egg/image/ImageFormatConversion.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace egg{
namespace image{

bool ImageFormatConversion::convert(tbyte *src,int srcFormat,int srcRowPitch,int srcSlicePitch,tbyte *dst,int dstFormat,int dstRowPitch,int dstSlicePitch,int width,int height,int depth){
	int i,j,k;

	if(width==0) width=1;
	if(height==0) height=1;
	if(depth==0) depth=1;

	if(srcFormat==Format_L_8 && dstFormat==Format_LA_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcSlicePitch+j*srcRowPitch+i);
					uint16 *d=(uint16*)(dst+k*dstSlicePitch+j*dstRowPitch+i*2);
					*d=((*s)<<8)|0xFF;
				}
			}
		}
	}
	else if(srcFormat==Format_LA_8 && dstFormat==Format_L_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint16 *s=(uint16*)(src+k*srcSlicePitch+j*srcRowPitch+i*2);
					uint8 *d=(uint8*)(dst+k*dstSlicePitch+j*dstRowPitch+i);
					*d=((*s)>>8);
				}
			}
		}
	}
	if(srcFormat==Format_A_8 && dstFormat==Format_LA_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcSlicePitch+j*srcRowPitch+i);
					uint16 *d=(uint16*)(dst+k*dstSlicePitch+j*dstRowPitch+i*2);
					*d=((*s)|0xFF00);
				}
			}
		}
	}
	else if(srcFormat==Format_LA_8 && dstFormat==Format_A_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint16 *s=(uint16*)(src+k*srcSlicePitch+j*srcRowPitch+i*2);
					uint8 *d=(uint8*)(dst+k*dstSlicePitch+j*dstRowPitch+i);
					*d=((*s)&0xFF);
				}
			}
		}
	}
	else if((srcFormat==Format_L_8 || srcFormat==Format_R_8) && (dstFormat==Format_RGBA_8 || dstFormat==Format_BGRA_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcSlicePitch+j*srcRowPitch+i);
					uint32 *d=(uint32*)(dst+k*dstSlicePitch+j*dstRowPitch+i*4);
					*d=(*s)|((*s)<<8)|((*s)<<16)|(0xFF<<24);
				}
			}
		}
	}
	else if((srcFormat==Format_RGBA_8 || srcFormat==Format_BGRA_8) && dstFormat==Format_L_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint32 *s=(uint32*)(src+k*srcSlicePitch+j*srcRowPitch+i*4);
					uint8 *d=(uint8*)(dst+k*dstSlicePitch+j*dstRowPitch+i);
					*d=(*s)&0xFF;
				}
			}
		}
	}
	else if((srcFormat==Format_L_8 || srcFormat==Format_R_8) && (dstFormat==Format_RGB_8 || dstFormat==Format_BGR_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcSlicePitch+j*srcRowPitch+i);
					uint8 *d=(uint8*)(dst+k*dstSlicePitch+j*dstRowPitch+i*3);
					*d=*s;
					*(d+1)=*s;
					*(d+2)=*s;
				}
			}
		}
	}
	else if((srcFormat==Format_RGB_8 || srcFormat==Format_BGR_8) && (dstFormat==Format_L_8 || dstFormat==Format_R_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcSlicePitch+j*srcRowPitch+i*3);
					uint8 *d=(uint8*)(dst+k*dstSlicePitch+j*dstRowPitch+i);
					*d=*s;
				}
			}
		}
	}
	else if(srcFormat==Format_A_8 && (dstFormat==Format_RGBA_8 || dstFormat==Format_BGRA_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcSlicePitch+j*srcRowPitch+i);
					uint32 *d=(uint32*)(dst+k*dstSlicePitch+j*dstRowPitch+i*4);
					*d=0xFFFFFF|((*s)<<24);
				}
			}
		}
	}
	else if((srcFormat==Format_RGBA_8 || srcFormat==Format_BGRA_8) && dstFormat==Format_A_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint32 *s=(uint32*)(src+k*srcSlicePitch+j*srcRowPitch+i*4);
					uint8 *d=(uint8*)(dst+k*dstSlicePitch+j*dstRowPitch+i);
					*d=((*s)&0xFF000000)>>24;
				}
			}
		}
	}
	else if(srcFormat==Format_LA_8 && (dstFormat==Format_RGBA_8 || dstFormat==Format_BGRA_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint16 *s=(uint16*)(src+k*srcSlicePitch+j*srcRowPitch+i*2);
					uint32 *d=(uint32*)(dst+k*dstSlicePitch+j*dstRowPitch+i*4);
					*d=((*s)&0xFF)|(((*s)&0xFF)<<8)|(((*s)&0xFF)<<16)|(((*s)&0xFF00)<<16);
				}
			}
		}
	}
	else if((srcFormat==Format_RGBA_8 || srcFormat==Format_BGRA_8) && dstFormat==Format_LA_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint32 *s=(uint32*)(src+k*srcSlicePitch+j*srcRowPitch+i*4);
					uint16 *d=(uint16*)(dst+k*dstSlicePitch+j*dstRowPitch+i*2);
					*d=((*s)&0xFF)|(((*s)&0xFF000000)>>16);
				}
			}
		}
	}
	else if((srcFormat==Format_RGB_8 && dstFormat==Format_RGBA_8) || (srcFormat==Format_BGR_8 && dstFormat==Format_BGRA_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcSlicePitch+j*srcRowPitch+i*3);
					uint32 *d=(uint32*)(dst+k*dstSlicePitch+j*dstRowPitch+i*4);
					*d=(0xFF<<24) | (*(s+2)<<16) | (*(s+1)<<8) | (*(s+0)<<0);
				}
			}
		}
	}
	else if((srcFormat==Format_RGBA_8 && dstFormat==Format_RGB_8) || (srcFormat==Format_BGRA_8 && dstFormat==Format_BGR_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint32 *s=(uint32*)(src+k*srcSlicePitch+j*srcRowPitch+i*4);
					uint8 *d=(uint8*)(dst+k*dstSlicePitch+j*dstRowPitch+i*3);
					*(d+0)=(*s)>>0;
					*(d+1)=(*s)>>8;
					*(d+2)=(*s)>>16;
				}
			}
		}
	}
	else if((srcFormat==Format_RGB_8 && dstFormat==Format_BGRA_8) || (srcFormat==Format_BGR_8 && dstFormat==Format_RGBA_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcSlicePitch+j*srcRowPitch+i*3);
					uint32 *d=(uint32*)(dst+k*dstSlicePitch+j*dstRowPitch+i*4);
					*d=(0xFF<<24) | (*(s+0)<<16) | (*(s+1)<<8) | (*(s+2)<<0);
				}
			}
		}
	}
	else if((srcFormat==Format_BGRA_8 && dstFormat==Format_RGB_8) || (srcFormat==Format_RGBA_8 && dstFormat==Format_BGR_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint32 *s=(uint32*)(src+k*srcSlicePitch+j*srcRowPitch+i*4);
					uint8 *d=(uint8*)(dst+k*dstSlicePitch+j*dstRowPitch+i*3);
					*(d+0)=(*s)>>16;
					*(d+1)=(*s)>>8;
					*(d+2)=(*s)>>0;
				}
			}
		}
	}
	else if((srcFormat==Format_RGB_8 && dstFormat==Format_BGR_8) || (srcFormat==Format_BGR_8 && dstFormat==Format_RGB_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcSlicePitch+j*srcRowPitch+i*3);
					uint8 *d=(uint8*)(dst+k*dstSlicePitch+j*dstRowPitch+i*3);
					*(d+0)=*(s+2);
					*(d+1)=*(s+1);
					*(d+2)=*(s+0);
				}
			}
		}
	}
	else if((srcFormat==Format_RGBA_8 && dstFormat==Format_BGRA_8) || (srcFormat==Format_BGRA_8 && dstFormat==Format_RGBA_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcSlicePitch+j*srcRowPitch+i*4);
					uint8 *d=(uint8*)(dst+k*dstSlicePitch+j*dstRowPitch+i*4);
					*(d+0)=*(s+2);
					*(d+1)=*(s+1);
					*(d+2)=*(s+0);
					*(d+3)=*(s+3);
				}
			}
		}
	}
	else if(srcFormat==dstFormat){
		// See if dst pitches are multiples of the src, in which case just use the src pitch.
		// (Can happen in compressed texture formats with D3D, since it will specify pitch in row-multiples of 4)
		if(dstRowPitch/srcRowPitch>1 && dstRowPitch%srcRowPitch==0){
			dstRowPitch=srcRowPitch;
		}
		if(dstSlicePitch/srcSlicePitch>1 && dstSlicePitch%srcSlicePitch==0){
			dstSlicePitch=srcSlicePitch;
		}

		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				memcpy(dst+k*dstSlicePitch+j*dstRowPitch,src+k*srcSlicePitch+j*srcRowPitch,dstRowPitch);
			}
		}
	}
	else{
		Error::unknown(Categories::TOADLET_EGG,
			"no format conversion available"
		);
		return false;
	}

	return true;
}

}
}
}

