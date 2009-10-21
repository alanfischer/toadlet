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

#ifndef TOADLET_PEEPER_D3DMINCLUDES_H
#define TOADLET_PEEPER_D3DMINCLUDES_H

#include <toadlet/peeper/Types.h>
#include <toadlet/peeper/Color.h>
#include <toadlet/egg/Logger.h>
#include <windows.h>
#include <commctrl.h>
#include <d3dm.h>

namespace toadlet{
namespace peeper{

inline D3DMCOLOR toD3DMCOLOR(const Color &color){
	return D3DMCOLOR_RGBA(Math::toInt(color.r*255),Math::toInt(color.g*255),Math::toInt(color.b*255),Math::toInt(color.a*255));
}

inline const D3DMCOLORVALUE &toD3DMCOLORVALUE(const Color &color){
	return *(D3DMCOLORVALUE*)color.getData();
}

inline const D3DMVECTOR &toD3DMVECTOR(const Vector3 &vector){
	return *(D3DMVECTOR*)vector.getData();
}

inline D3DMVALUE toD3DMVALUE(scalar s){return *(D3DMVALUE*)&s;}

inline void toD3DMMATRIX(D3DMMATRIX &result,const Matrix4x4 &src){
	result.m[0][0]=toD3DMVALUE(src.at(0,0));
	result.m[1][0]=toD3DMVALUE(src.at(0,1));
	result.m[2][0]=toD3DMVALUE(src.at(0,2));
	result.m[3][0]=toD3DMVALUE(src.at(0,3));
	result.m[0][1]=toD3DMVALUE(src.at(1,0));
	result.m[1][1]=toD3DMVALUE(src.at(1,1));
	result.m[2][1]=toD3DMVALUE(src.at(1,2));
	result.m[3][1]=toD3DMVALUE(src.at(1,3));
	result.m[0][2]=toD3DMVALUE(src.at(2,0));
	result.m[1][2]=toD3DMVALUE(src.at(2,1));
	result.m[2][2]=toD3DMVALUE(src.at(2,2));
	result.m[3][2]=toD3DMVALUE(src.at(2,3));
	result.m[0][3]=toD3DMVALUE(src.at(3,0));
	result.m[1][3]=toD3DMVALUE(src.at(3,1));
	result.m[2][3]=toD3DMVALUE(src.at(3,2));
	result.m[3][3]=toD3DMVALUE(src.at(3,3));
}

// This function needs to be endian aware, since it takes an RGBA_8 in as a uint32 instead of individual components
inline uint32 RGBA8toA8R8G8B8(uint32 rgba){
#	if defined(TOADLET_LITTLE_ENDIAN)
		return ((rgba&0xFF)<<16) | (rgba&0xFF00FF00) | ((rgba&0xFF0000)>>16);
#	else
		return ((rgba&0xFFFFFF00)>>8) | ((rgba&0xFF)<<24) ;
#	endif
}

inline uint16 A8toA8L8(const uint8 a){
	return (a<<8)|0xFF;
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

#if defined(TOADLET_DEBUG)
	inline bool checkD3DMError(HRESULT result,const egg::String &function){
		if(FAILED(result)){
			toadlet::egg::String error;
			switch(result){
				case E_OUTOFMEMORY:
					error="E_OUTOFMEMORY";
				break;
				case D3DMERR_MEMORYPOOLEMPTY:
					error="D3DMERR_MEMORYPOOLEMPTY";
				break;
				case D3DMERR_DRIVERINVALIDCALL:
					error="D3DMERR_DRIVERINVALIDCALL";
				break;
				case D3DMERR_INVALIDCALL:
					error="D3DMERR_INVALIDCALL";
				break;
				default:
					error="UNKNOWN";
				break;
			}
			toadlet::egg::Logger::log(toadlet::egg::Categories::TOADLET_PEEPER,toadlet::egg::Logger::Level_ALERT,
				toadlet::egg::String("D3DM Error in ") + function + ": error=" + (int) result + " " + error);
			return false;
		}
		else{
			return true;
		}
	}
#	define TOADLET_CHECK_D3DMERROR(result,function) toadlet::peeper::checkD3DMError(result,function)
#else
#	define TOADLET_CHECK_D3DMERROR(result,function)
#endif

}
}

#endif
