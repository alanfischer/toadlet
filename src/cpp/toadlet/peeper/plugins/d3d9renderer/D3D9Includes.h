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

#ifndef TOADLET_PEEPER_D3D9INCLUDES_H
#define TOADLET_PEEPER_D3D9INCLUDES_H

#include <toadlet/peeper/Types.h>
#include <toadlet/peeper/Color.h>
#include <toadlet/egg/Logger.h>
#include <windows.h>
#include <commctrl.h>
#include <d3d9.h>
#include <dxerr9.h>

#include <toadlet/egg/EndianConversion.h>

namespace toadlet{
namespace peeper{

#if defined(TOADLET_FIXED_POINT)
inline D3DCOLORVALUE toD3DCOLORVALUE(const Color &color){
	D3DCOLORVALUE value;
	value.r=egg::mathfixed::Math::toFloat(color.r);
	value.g=egg::mathfixed::Math::toFloat(color.g);
	value.b=egg::mathfixed::Math::toFloat(color.b);
	value.a=egg::mathfixed::Math::toFloat(color.a);
	return value;
}
#else
inline const D3DCOLORVALUE &toD3DCOLORVALUE(const Color &color){ return *(D3DCOLORVALUE*)&color; }
#endif

inline void toD3DVECTOR(D3DVECTOR &result,const egg::math::Vector3 &src){
	result.x=src.x;
	result.y=src.y;
	result.z=src.z;
}

inline void toD3DVECTOR(D3DVECTOR &result,const egg::mathfixed::Vector3 &src){
	result.x=egg::mathfixed::Math::toFloat(src.x);
	result.y=egg::mathfixed::Math::toFloat(src.y);
	result.z=egg::mathfixed::Math::toFloat(src.z);
}

inline void toD3DMATRIX(D3DMATRIX &result,const egg::math::Matrix4x4 &src){
	result.m[0][0]=src.at(0,0);
	result.m[1][0]=src.at(0,1);
	result.m[2][0]=src.at(0,2);
	result.m[3][0]=src.at(0,3);
	result.m[0][1]=src.at(1,0);
	result.m[1][1]=src.at(1,1);
	result.m[2][1]=src.at(1,2);
	result.m[3][1]=src.at(1,3);
	result.m[0][2]=src.at(2,0);
	result.m[1][2]=src.at(2,1);
	result.m[2][2]=src.at(2,2);
	result.m[3][2]=src.at(2,3);
	result.m[0][3]=src.at(3,0);
	result.m[1][3]=src.at(3,1);
	result.m[2][3]=src.at(3,2);
	result.m[3][3]=src.at(3,3);
}

inline void toD3DMATRIX(D3DMATRIX &result,const egg::mathfixed::Matrix4x4 &src){
	result.m[0][0]=egg::mathfixed::Math::toFloat(src.at(0,0));
	result.m[1][0]=egg::mathfixed::Math::toFloat(src.at(0,1));
	result.m[2][0]=egg::mathfixed::Math::toFloat(src.at(0,2));
	result.m[3][0]=egg::mathfixed::Math::toFloat(src.at(0,3));
	result.m[0][1]=egg::mathfixed::Math::toFloat(src.at(1,0));
	result.m[1][1]=egg::mathfixed::Math::toFloat(src.at(1,1));
	result.m[2][1]=egg::mathfixed::Math::toFloat(src.at(1,2));
	result.m[3][1]=egg::mathfixed::Math::toFloat(src.at(1,3));
	result.m[0][2]=egg::mathfixed::Math::toFloat(src.at(2,0));
	result.m[1][2]=egg::mathfixed::Math::toFloat(src.at(2,1));
	result.m[2][2]=egg::mathfixed::Math::toFloat(src.at(2,2));
	result.m[3][2]=egg::mathfixed::Math::toFloat(src.at(2,3));
	result.m[0][3]=egg::mathfixed::Math::toFloat(src.at(3,0));
	result.m[1][3]=egg::mathfixed::Math::toFloat(src.at(3,1));
	result.m[2][3]=egg::mathfixed::Math::toFloat(src.at(3,2));
	result.m[3][3]=egg::mathfixed::Math::toFloat(src.at(3,3));
}

inline uint16 A8toA8L8(const uint8 a){
	return (a<<8)|0xFF;
}

inline uint32 RGBA8toA8R8G8B8(uint32 rgba){
	return ((rgba&0xFF)<<16) | (rgba&0xFF00FF00) | ((rgba&0xFF0000)>>16);
}

inline uint32 RGB8toX8R8G8B8(uint8 *rgb){
	return (*(rgb+0)<<16) | (*(rgb+1)<<8) | (*(rgb+3)<<0);
}

#if defined(TOADLET_DEBUG)
	#define TOADLET_CHECK_D3D9ERROR(result,function) \
		if(FAILED(result)) \
			toadlet::egg::Logger::log(toadlet::egg::Categories::TOADLET_PEEPER,toadlet::egg::Logger::Level_ALERT, \
			toadlet::egg::String("D3D9 Error in ") + function + ": error=" + (int)result); \

#else
	#define TOADLET_CHECK_D3D9ERROR(result,function)
#endif

}
}

#endif
