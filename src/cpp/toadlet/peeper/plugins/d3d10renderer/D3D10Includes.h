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

#ifndef TOADLET_PEEPER_D3D10INCLUDES_H
#define TOADLET_PEEPER_D3D10INCLUDES_H

#include <toadlet/peeper/Types.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Error.h>
#include <windows.h>
#include <commctrl.h>
#if defined(TOADLET_SET_D3D10)
	#include <d3d10.h>
#else
	#include <d3d11.h>
#endif

#include <toadlet/egg/EndianConversion.h>

namespace toadlet{
namespace peeper{

#if !defined(TOADLET_SET_D3D10)
	#define D3D10Buffer					D3D11Buffer
	#define D3D10Query					D3D11Query
	#define D3D10Renderer				D3D11Renderer
	#define D3D10RenderTarget			D3D11RenderTarget
	#define D3D10TextureMipPixelBuffer	D3D11TextureMipPixelBuffer
	#define D3D10PixelBufferRenderTarget D3D11PixelBufferRenderTarget
	#define D3D10Texture				D3D11Texture
	#define D3D10VertexFormat			D3D11VertexFormat
	#define D3D10WindowRenderTarget		D3D11WindowRenderTarget
#endif

#if defined(TOADLET_FIXED_POINT)
	inline void toD3DColor(float *r,const Color &c){
		r[0]=Math::toFloat(c.r); r[1]=Math::toFloat(c.g); r[2]=Math::toFloat(c.b); r[3]=Math::toFloat(c.a);
	}

	inline void toD3DMatrix(float *r,const Matrix4x4 &s){
		r[0]=Math::toFloat(s.at(0,0));
		r[1]=Math::toFloat(s.at(0,1));
		r[2]=Math::toFloat(s.at(0,2));
		r[3]=Math::toFloat(s.at(0,3));
		r[4]=Math::toFloat(s.at(1,0));
		r[5]=Math::toFloat(s.at(1,1));
		r[6]=Math::toFloat(s.at(1,2));
		r[7]=Math::toFloat(s.at(1,3));
		r[8]=Math::toFloat(s.at(2,0));
		r[9]=Math::toFloat(s.at(2,1));
		r[10]=Math::toFloat(s.at(2,2));
		r[11]=Math::toFloat(s.at(2,3));
		r[12]=Math::toFloat(s.at(3,0));
		r[13]=Math::toFloat(s.at(3,1));
		r[14]=Math::toFloat(s.at(3,2));
		r[15]=Math::toFloat(s.at(3,3));
	}
#endif

#if !defined(TOADLET_SET_D3D10)
	#define TOADLET_D3D10_DLL_NAME TEXT("D3D11.DLL")
	#define TOADLET_D3D10_CREATE_DEVICE_NAME TEXT("D3D11CreateDevice")
	#define TOADLET_D3D10_CREATE_DEVICE_AND_SWAP_CHAIN_NAME TEXT("D3D11CreateDeviceAndSwapChain")
#else
	#define TOADLET_D3D10_DLL_NAME TEXT("D3D10.DLL")
	#define TOADLET_D3D10_CREATE_DEVICE_NAME TEXT("D3D10CreateDevice")
	#define TOADLET_D3D10_CREATE_DEVICE_AND_SWAP_CHAIN_NAME TEXT("D3D10CreateDeviceAndSwapChain")
#endif

#if defined(TOADLET_DEBUG)
	#define TOADLET_CHECK_D3D10ERROR(result,function) \
		if(FAILED(result)) \
			toadlet::egg::Error::unknown(toadlet::egg::Categories::TOADLET_PEEPER, \
			toadlet::egg::String("D3D10 Error in ") + function + ": error=" + (int)result); \

#else
	#define TOADLET_CHECK_D3D10ERROR(result,function)
#endif

}
}

#endif
