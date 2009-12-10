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
#include <string.h> //memcpy

namespace toadlet{
namespace egg{
namespace image{

bool ImageFormatConversion::convert(uint8 *src,int srcFormat,int srcRowPitch,int srcSlicePitch,uint8 *dst,int dstFormat,int dstRowPitch,int dstSlicePitch,int width,int height,int depth){
	int i,j,k;

	if(srcFormat==Format_L_8 && dstFormat==Format_LA_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(src+k*srcSlicePitch+j*srcRowPitch+i);
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
					uint8 *d=(dst+k*dstSlicePitch+j*dstRowPitch+i);
					*d=((*s)>>8);
				}
			}
		}
	}
	else if(srcFormat==Format_L_8 && dstFormat==Format_RGBA_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(src+k*srcSlicePitch+j*srcRowPitch+i);
					uint32 *d=(uint32*)(dst+k*dstSlicePitch+j*dstRowPitch+i*4);
					*d=(*s)|((*s)<<8)|((*s)<<16)|(0xFF<<24);
				}
			}
		}
	}
	else if(srcFormat==Format_RGBA_8 && dstFormat==Format_L_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint32 *s=(uint32*)(src+k*srcSlicePitch+j*srcRowPitch+i*4);
					uint8 *d=(dst+k*dstSlicePitch+j*dstRowPitch+i);
					*d=(*s)&0xFF;
				}
			}
		}
	}
	else if(srcFormat==Format_RGB_8 && dstFormat==Format_RGBA_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(src+k*srcSlicePitch+j*srcRowPitch+i*3);
					uint32 *d=(uint32*)(dst+k*dstSlicePitch+j*dstRowPitch+i*4);
					*d=(0xFF<<24) | (*(s+0)<<16) | (*(s+1)<<8) | (*(s+2)<<0);
				}
			}
		}
	}
	else if(srcFormat==Format_RGBA_8 && dstFormat==Format_RGB_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint32 *s=(uint32*)(src+k*srcSlicePitch+j*srcRowPitch+i*4);
					uint8 *d=(dst+k*dstSlicePitch+j*dstRowPitch+i*3);
					*(d+0)=(*s)>>16;
					*(d+1)=(*s)>>8;
					*(d+2)=(*s)>>0;
				}
			}
		}
	}
	// Just copy it over & hope for the best
	else{
		int pixelSize=getPixelSize(srcFormat);
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				memcpy(dst+k*dstSlicePitch+j*dstRowPitch,src+k*srcSlicePitch+j*srcRowPitch,width*pixelSize);
			}
		}
	}

	return true;
}

	/*


inline uint32 RGBA8toA8R8G8B8(uint32 rgba){
	return ((rgba&0xFF)<<16) | (rgba&0xFF00FF00) | ((rgba&0xFF0000)>>16);
}

inline uint32 RGB8toX8R8G8B8(uint8 *rgb){
	return (*(rgb+0)<<16) | (*(rgb+1)<<8) | (*(rgb+2)<<0);
}

inline uint16 A8toA8R8G8B8(const uint8 a){
	return (a<<24)|0xFFFFFF;
}

inline uint16 A8toA4R4G4B4(const uint8 a){
	return ((a&0xF0)<<8)|0xFFF;
}

inline uint16 L8toR5G6B5(const uint8 l){
	return ((l&0xF8)<<11)|((l&0xFC)<<5)|(l&0xF8);
}

inline uint16 L8toR8G8B8(const uint8 l){
	return (l<<16)|(l<<8)|l;
}

// This function needs to be endian aware, since it takes an LA_8 in as a uint16 instead of individual components
inline uint16 LA8toA4R4G4B4(const uint16 la){
#	if defined(TOADLET_LITTLE_ENDIAN)
		return ((la&0xF0)<8)|((la&0xF000)>>4)|((la&0xF000)>>8)|((la&0xF000)>>12);
#	else
		return (la&0xF000)|((la&0xF0)<<4)|(la&0xF0)|((la&0xF0)>>4)
#	endif
}

// This function needs to be endian aware, since it takes an LA_8 in as a uint16 instead of individual components
inline uint16 LA8toA8R8G8B8(const uint8 la){
#	if defined(TOADLET_LITTLE_ENDIAN)
		return ((la&0xFF00)<<16)|((la&0xFF)<<16)|((la&0xFF)<<8)|(la&0xFF);
#	else
		return ((la&0xFF)<<24)|((la&0xFF00)<<8)|(la&0xFF00)|((la&0xFF00)>>8);
#	endif
}












		int i,j;
		#if defined(TOADLET_HAS_DIRECT3DMOBILE)
			if(mFormat==Texture::Format_RGBA_8 && mD3DFormat==D3DMFMT_A8R8G8B8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint32*)(dst+rect.Pitch*i+j*4)=RGBA8toA8R8G8B8(*(uint32*)(src+width*pixelSize*i+j*pixelSize));
					}
				}
			}
			else if(mFormat==Texture::Format_L_8 && mD3DFormat==D3DMFMT_R8G8B8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint16*)(dst+rect.Pitch*i+j*2)=L8toR8G8B8(*(uint8*)(src+width*pixelSize*i+j*pixelSize));
					}
				}
			}
			else if(mFormat==Texture::Format_A_8 && mD3DFormat==D3DMFMT_A8R8G8B8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint16*)(dst+rect.Pitch*i+j*2)=A8toA8R8G8B8(*(uint8*)(src+width*pixelSize*i+j*pixelSize));
					}
				}
			}
			else if(mFormat==Texture::Format_LA_8 && mD3DFormat==D3DMFMT_A8R8G8B8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint16*)(dst+rect.Pitch*i+j*2)=LA8toA8R8G8B8(*(uint16*)(src+width*pixelSize*i+j*pixelSize));
					}
				}
			}
		#else
			if(mFormat==Texture::Format_A_8 && mD3DFormat==D3DFMT_A8L8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint16*)(dst+rect.Pitch*i+j*2)=A8toA8L8(*(uint8*)(src+width*pixelSize*i+j*1));
					}
				}
			}
			else if(mFormat==Texture::Format_RGBA_8 && mD3DFormat==D3DFMT_A8R8G8B8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint32*)(dst+rect.Pitch*i+j*4)=RGBA8toA8R8G8B8(*(uint32*)(src+width*pixelSize*i+j*pixelSize));
					}
				}
			}
			else if(mFormat==Texture::Format_RGB_8 && mD3DFormat==D3DFMT_X8R8G8B8){
				for(i=0;i<height;++i){
					for(j=0;j<width;++j){
						*(uint32*)(dst+rect.Pitch*i+j*4)=RGB8toX8R8G8B8(src+width*pixelSize*i+j*pixelSize);
					}
				}
			}
		#endif
		else{
			for(i=0;i<height;++i){
				memcpy(dst+rect.Pitch*i,src+width*pixelSize*i,width*pixelSize);
			}
		}


			}

			texture->UnlockBox(mipLevel);
		}
	#endif


*/

}
}
}

