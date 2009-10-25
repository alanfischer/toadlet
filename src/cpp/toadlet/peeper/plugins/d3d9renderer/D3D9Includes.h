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
#if defined(TOADLET_HAS_DIRECT3D_MOBILE)
	#include <d3dm.h>
#else
	#include <d3d9.h>
#endif

#include <toadlet/egg/EndianConversion.h>

namespace toadlet{
namespace peeper{

#if defined(TOADLET_HAS_DIRECT3D_MOBILE)
	inline D3DMCOLOR toD3DCOLOR(const Color &c){
		return D3DMCOLOR_RGBA(Math::toInt(c.r*255),Math::toInt(c.g*255),Math::toInt(c.b*255),Math::toInt(c.a*255));
	}
#else
	inline D3DCOLOR toD3DCOLOR(const Color &c){
		return D3DCOLOR_RGBA(Math::toInt(c.r*255),Math::toInt(c.g*255),Math::toInt(c.b*255),Math::toInt(c.a*255));
	}
#endif

#if defined(TOADLET_HAS_DIRECT3D_MOBILE)
	inline void toD3DCOLORVALUE(D3DMCOLORVALUE &r,const Color &c){
		r.r=c.r; r.g=c.g; r.b=c.b; r.a=c.a;
	}
#else
	#if defined(TOADLET_FIXED_POINT)
		inline void toD3DCOLORVALUE(D3DCOLORVALUE &r,const Color &c){
			r.r=Math::toFloat(c.r); r.g=Math::toFloat(c.g); r.b=Math::toFloat(c.b); c.a=Math::toFloat(c.a);
		}
	#else
		inline void toD3DCOLORVALUE(D3DCOLORVALUE &r,const Color &c){
			r.r=c.r; r.g=c.g; r.b=c.b; r.a=c.a;
		}
	#endif
#endif

#if defined(TOADLET_HAS_DIRECT3D_MOBILE)
	inline void toD3DVECTOR(D3DMVECTOR &r,const Vector3 &s){
		r.x=s.x; r.y=s.y; r.z=s.z;
	}
#else
	#if defined(TOADLET_FIXED_POINT)
		inline void toD3DVECTOR(D3DVECTOR &r,const Vector3 &s){
			r.x=Math::toFloat(s.x); r.y=Math::toFloat(s.y); r.z=Math::toFloat(s.z);
		}
	#else
		inline void toD3DVECTOR(D3DVECTOR &r,const Vector3 &s){
			r.x=s.x; r.y=s.y; r.z=s.z;
		}
	#endif
#endif

#if defined(TOADLET_HAS_DIRECT3D_MOBILE)
	inline void toD3DMATRIX(D3DMMATRIX &r,const Matrix4x4 &s){
		r.m[0][0]=s.at(0,0);
		r.m[1][0]=s.at(0,1);
		r.m[2][0]=s.at(0,2);
		r.m[3][0]=s.at(0,3);
		r.m[0][1]=s.at(1,0);
		r.m[1][1]=s.at(1,1);
		r.m[2][1]=s.at(1,2);
		r.m[3][1]=s.at(1,3);
		r.m[0][2]=s.at(2,0);
		r.m[1][2]=s.at(2,1);
		r.m[2][2]=s.at(2,2);
		r.m[3][2]=s.at(2,3);
		r.m[0][3]=s.at(3,0);
		r.m[1][3]=s.at(3,1);
		r.m[2][3]=s.at(3,2);
		r.m[3][3]=s.at(3,3);
	}
#else
	#if defined(TOADLET_FIXED_POINT)
		inline void toD3DMATRIX(D3DMATRIX &r,const Matrix4x4 &s){
			r.m[0][0]=Math::toFloat(s.at(0,0));
			r.m[1][0]=Math::toFloat(s.at(0,1));
			r.m[2][0]=Math::toFloat(s.at(0,2));
			r.m[3][0]=Math::toFloat(s.at(0,3));
			r.m[0][1]=Math::toFloat(s.at(1,0));
			r.m[1][1]=Math::toFloat(s.at(1,1));
			r.m[2][1]=Math::toFloat(s.at(1,2));
			r.m[3][1]=Math::toFloat(s.at(1,3));
			r.m[0][2]=Math::toFloat(s.at(2,0));
			r.m[1][2]=Math::toFloat(s.at(2,1));
			r.m[2][2]=Math::toFloat(s.at(2,2));
			r.m[3][2]=Math::toFloat(s.at(2,3));
			r.m[0][3]=Math::toFloat(s.at(3,0));
			r.m[1][3]=Math::toFloat(s.at(3,1));
			r.m[2][3]=Math::toFloat(s.at(3,2));
			r.m[3][3]=Math::toFloat(s.at(3,3));
		}
	#else
		inline void toD3DMATRIX(D3DMATRIX &r,const Matrix4x4 &s){
			r.m[0][0]=s.at(0,0);
			r.m[1][0]=s.at(0,1);
			r.m[2][0]=s.at(0,2);
			r.m[3][0]=s.at(0,3);
			r.m[0][1]=s.at(1,0);
			r.m[1][1]=s.at(1,1);
			r.m[2][1]=s.at(1,2);
			r.m[3][1]=s.at(1,3);
			r.m[0][2]=s.at(2,0);
			r.m[1][2]=s.at(2,1);
			r.m[2][2]=s.at(2,2);
			r.m[3][2]=s.at(2,3);
			r.m[0][3]=s.at(3,0);
			r.m[1][3]=s.at(3,1);
			r.m[2][3]=s.at(3,2);
			r.m[3][3]=s.at(3,3);
		}
	#endif
#endif

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
