/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 * 
 * All source code for the Toadlet Engine, including 
 * this file, is the sole property of Lightning Toads 
 * Productions, LLC. It has been developed on our own 
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed 
 * without our explicit permission. 
 *ahahs, LLC
 *
 ********** Copyright header - do not remove **********/

#ifndef TOADLET_PEEPER_NDSINCLUDES_H
#define TOADLET_PEEPER_NDSINCLUDES_H

#include "../../Types.h"

#include <nds.h>

namespace toadlet{
namespace peeper{

inline void toNDSm4x4(const Matrix4x4 &src,m4x4 &result,int renderShiftBits){
	renderShiftBits+=4;
	result.m[0]=src.data[0]>>4;
	result.m[1]=src.data[1]>>4;
	result.m[2]=src.data[2]>>4;
	result.m[3]=src.data[3]>>4;
	result.m[4]=src.data[4]>>4;
	result.m[5]=src.data[5]>>4;
	result.m[6]=src.data[6]>>4;
	result.m[7]=src.data[7]>>4;
	result.m[8]=src.data[8]>>4;
	result.m[9]=src.data[9]>>4;
	result.m[10]=src.data[10]>>4;
	result.m[11]=src.data[11]>>4;
	result.m[12]=src.data[12]>>renderShiftBits;
	result.m[13]=src.data[13]>>renderShiftBits;
	result.m[14]=src.data[14]>>renderShiftBits;
	result.m[15]=src.data[15]>>4;
}

inline void toProjectionNDSm4x4(const Matrix4x4 &src,m4x4 &result,int renderShiftBits){
	renderShiftBits+=4;
	result.m[0]=src.data[0]>>4;
	result.m[1]=src.data[1]>>4;
	result.m[2]=src.data[2]>>4;
	result.m[3]=src.data[3]>>4;
	result.m[4]=src.data[4]>>4;
	result.m[5]=src.data[5]>>4;
	result.m[6]=src.data[6]>>4;
	result.m[7]=src.data[7]>>4;
	result.m[8]=src.data[8]>>4;
	result.m[9]=src.data[9]>>4;
	result.m[10]=src.data[10]>>renderShiftBits;
	result.m[11]=src.data[11]>>4;
	result.m[12]=src.data[12]>>4;
	result.m[13]=src.data[13]>>4;
	result.m[14]=src.data[14]>>renderShiftBits;
	result.m[15]=src.data[15]>>4;
}

inline int keepPos(int v){
	return v>0?v:0;
}

inline rgb toNDSrgb(const Color &color){
	return (keepPos(color.a-1)&0x8000)|((keepPos(color.b-1)&0xF800)>>1)|((keepPos(color.g-1)&0xF800)>>6)|((keepPos(color.r-1)&0xF800)>>11);
}

inline rgb toNDSv10(scalar s){
	return s>0xFFFF?0x1FF:s>>7;
}

}
}

#endif

