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
#if defined(TOADLET_HAS_DIRECT3DMOBILE)
	#include <d3dm.h>
#else
	#include <d3d9.h>
#endif

#include <toadlet/egg/EndianConversion.h>

namespace toadlet{
namespace peeper{

#if defined(TOADLET_HAS_DIRECT3DMOBILE)
	#if defined(TOADLET_FIXED_POINT)
		#define	TOADLET_D3DMFMT ,D3DMFMT_D3DMVALUE_FIXED
	#else
		#define	TOADLET_D3DMFMT ,D3DMFMT_D3DMVALUE_FLOAT
	#endif
	#define TOADLET_SHAREDHANDLE
#else
	#define	TOADLET_D3DMFMT
	#define	TOADLET_SHAREDHANDLE ,NULL
#endif

#if defined(TOADLET_HAS_DIRECT3DMOBILE)
	inline D3DMCOLOR toD3DCOLOR(const Color &c){
		return D3DMCOLOR_RGBA(Math::toInt(c.r*255),Math::toInt(c.g*255),Math::toInt(c.b*255),Math::toInt(c.a*255));
	}
#else
	inline D3DCOLOR toD3DCOLOR(const Color &c){
		return D3DCOLOR_RGBA(Math::toInt(c.r*255),Math::toInt(c.g*255),Math::toInt(c.b*255),Math::toInt(c.a*255));
	}
#endif

#if defined(TOADLET_HAS_DIRECT3DMOBILE)
	inline void toD3DCOLORVALUE(D3DMCOLORVALUE &r,const Color &c){
		r.r=c.r; r.g=c.g; r.b=c.b; r.a=c.a;
	}
#else
	#if defined(TOADLET_FIXED_POINT)
		inline void toD3DCOLORVALUE(D3DCOLORVALUE &r,const Color &c){
			r.r=Math::toFloat(c.r); r.g=Math::toFloat(c.g); r.b=Math::toFloat(c.b); r.a=Math::toFloat(c.a);
		}
	#else
		inline void toD3DCOLORVALUE(D3DCOLORVALUE &r,const Color &c){
			r.r=c.r; r.g=c.g; r.b=c.b; r.a=c.a;
		}
	#endif
#endif

#if defined(TOADLET_HAS_DIRECT3DMOBILE)
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

#if defined(TOADLET_HAS_DIRECT3DMOBILE)
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

#if defined(TOADLET_HAS_DIRECT3DMOBILE)
	#define TOADLET_D3D_DLL_NAME TEXT("D3DM.DLL")
	#define TOADLET_D3D_CREATE_NAME TEXT("Direct3DMobileCreate")
#else
	#define TOADLET_D3D_DLL_NAME TEXT("D3D9.DLL")
	#define TOADLET_D3D_CREATE_NAME TEXT("Direct3DCreate9")
#endif

#if defined(TOADLET_HAS_DIRECT3DMOBILE)
	#define D3D_SDK_VERSION		D3DM_SDK_VERSION
	#define D3DADAPTER_DEFAULT	D3DMADAPTER_DEFAULT
	#define D3DFORMAT			D3DMFORMAT
	#define D3DCAPS9			D3DMCAPS
	#define D3DMATRIX			D3DMMATRIX
	#define D3DPRIMITIVETYPE	D3DMPRIMITIVETYPE
	#define D3DVIEWPORT9		D3DMVIEWPORT
	#define D3DMATERIAL9		D3DMMATERIAL
	#define D3DCOLOR			D3DMCOLOR
	#define D3DLIGHT9			D3DMLIGHT
	#define D3DSURFACE_DESC		D3DMSURFACE_DESC
	#define D3DLOCKED_RECT		D3DMLOCKED_RECT
	#define D3DTRANSFORMSTATETYPE D3DMTRANSFORMSTATETYPE
	#define D3DADAPTER_IDENTIFIER9 D3DMADAPTER_IDENTIFIER
	#define D3DPRESENT_PARAMETERS D3DMPRESENT_PARAMETERS

	#define D3DPOOL			D3DMPOOL
	#define D3DPOOL_MANAGED	D3DMPOOL_MANAGED

	#define D3DERR_DEVICELOST			D3DMERR_DEVICELOST
	#define D3DERR_DEVICENOTRESET		D3DMERR_DEVICENOTRESET
	#define D3DERR_DRIVERINTERNALERROR	D3DMERR_DRIVERINTERNALERROR

	#define D3DTEXOPCAPS_DOTPRODUCT3	D3DMTEXOPCAPS_DOTPRODUCT3

	#define D3DPTEXTURECAPS_SQUAREONLY	D3DMPTEXTURECAPS_SQUAREONLY
	#define D3DPTEXTURECAPS_POW2		D3DMPTEXTURECAPS_POW2

	#define D3DRS_ALPHABLENDENABLE	D3DMRS_ALPHABLENDENABLE
	#define D3DRS_SPECULARENABLE	D3DMRS_SPECULARENABLE
	#define D3DRS_ALPHATESTENABLE	D3DMRS_ALPHATESTENABLE
	#define D3DRS_ALPHAREF			D3DMRS_ALPHAREF
	#define D3DRS_ALPHAFUNC			D3DMRS_ALPHAFUNC
	#define D3DRS_SRCBLEND			D3DMRS_SRCBLEND
	#define D3DRS_DESTBLEND			D3DMRS_DESTBLEND
	#define D3DRS_CULLMODE			D3DMRS_CULLMODE
	#define D3DRS_ZFUNC				D3DMRS_ZFUNC
	#define D3DRS_ZENABLE			D3DMRS_ZENABLE
	#define D3DRS_ZWRITEENABLE		D3DMRS_ZWRITEENABLE
	#define D3DRS_DITHERENABLE		D3DMRS_DITHERENABLE
	#define D3DRS_FOGENABLE			D3DMRS_FOGENABLE
	#define D3DRS_FOGCOLOR			D3DMRS_FOGCOLOR
	#define D3DRS_FOGVERTEXMODE		D3DMRS_FOGVERTEXMODE
	#define D3DRS_FOGSTART			D3DMRS_FOGSTART
	#define D3DRS_FOGEND			D3DMRS_FOGEND
	#define D3DRS_COLORVERTEX		D3DMRS_COLORVERTEX
	#define D3DRS_FILLMODE			D3DMRS_FILLMODE
	#define D3DRS_LIGHTING			D3DMRS_LIGHTING
	#define D3DRS_DEPTHBIAS			D3DMRS_DEPTHBIAS
	#define D3DRS_BLENDOP			D3DMRS_BLENDOP
	#define D3DRS_SHADEMODE			D3DMRS_SHADEMODE
	#define D3DRS_AMBIENT			D3DMRS_AMBIENT
	#define D3DRS_COLORWRITEENABLE	D3DMRS_COLORWRITEENABLE
	#define D3DRS_NORMALIZENORMALS	D3DMRS_NORMALIZENORMALS
	#define D3DRS_SLOPESCALEDEPTHBIAS D3DMRS_SLOPESCALEDEPTHBIAS
	#define D3DRS_AMBIENTMATERIALSOURCE D3DMRS_AMBIENTMATERIALSOURCE
	#define D3DRS_DIFFUSEMATERIALSOURCE D3DMRS_DIFFUSEMATERIALSOURCE

	#define D3DCLEAR_TARGET		D3DMCLEAR_TARGET
	#define D3DCLEAR_ZBUFFER	D3DMCLEAR_ZBUFFER
	#define D3DCLEAR_STENCIL	D3DMCLEAR_STENCIL

	#define D3DTS_WORLD			D3DMTS_WORLD
	#define D3DTS_VIEW			D3DMTS_VIEW
	#define D3DTS_PROJECTION	D3DMTS_PROJECTION

	#define D3DUSAGE_DYNAMIC		D3DMUSAGE_DYNAMIC
	#define D3DUSAGE_WRITEONLY		D3DMUSAGE_WRITEONLY
	#define D3DUSAGE_RENDERTARGET	D3DMUSAGE_RENDERTARGET
	#define D3DUSAGE_LOCKABLE		D3DMUSAGE_LOCKABLE

	#define D3DLOCK_DISCARD		D3DMLOCK_DISCARD
	#define D3DLOCK_READONLY	D3DMLOCK_READONLY

	#define D3DFMT_UNKNOWN	D3DMFMT_UNKNOWN
	#define D3DFMT_INDEX16	D3DMFMT_INDEX16
	#define D3DFMT_INDEX32	D3DMFMT_INDEX32
	#define D3DFMT_R5G6B5	D3DMFMT_R5G6B5
	#define D3DFMT_R8G8B8	D3DMFMT_R8G8B8
	#define D3DFMT_X8R8G8B8	D3DMFMT_X8R8G8B8
	#define D3DFMT_A8R8G8B8	D3DMFMT_A8R8G8B8
	#define D3DFMT_A1R5G5B5	D3DMFMT_A1R5G5B5
	#define D3DFMT_A4R4G4B4	D3DMFMT_A4R4G4B4

	#define D3DCMP_LESS			D3DMCMP_LESS
	#define D3DCMP_EQUAL		D3DMCMP_EQUAL
	#define D3DCMP_LESSEQUAL	D3DMCMP_LESSEQUAL
	#define D3DCMP_GREATER		D3DMCMP_GREATER
	#define D3DCMP_NOTEQUAL		D3DMCMP_NOTEQUAL
	#define D3DCMP_GREATEREQUAL	D3DMCMP_GREATEREQUAL
	#define D3DCMP_ALWAYS		D3DMCMP_ALWAYS
	#define D3DCMP_NEVER		D3DMCMP_NEVER

	#define D3DTADDRESS_WRAP	D3DMTADDRESS_WRAP
	#define D3DTADDRESS_CLAMP	D3DMTADDRESS_CLAMP
	#define D3DTADDRESS_BORDER	D3DMTADDRESS_BORDER
	#define D3DTADDRESS_MIRROR	D3DMTADDRESS_MIRROR

	#define D3DTEXF_NONE	D3DMTEXF_NONE
	#define D3DTEXF_POINT	D3DMTEXF_POINT
	#define D3DTEXF_LINEAR	D3DMTEXF_LINEAR

	#define D3DBLEND		D3DMBLEND
	#define D3DBLEND_ONE	D3DMBLEND_ONE
	#define D3DBLEND_ZERO	D3DMBLEND_ZERO

	#define D3DCULL_NONE	D3DMCULL_NONE
	#define D3DCULL_CW		D3DMCULL_CW
	#define D3DCULL_CCW		D3DMCULL_CCW

	#define D3DZB_FALSE	D3DMZB_FALSE
	#define D3DZB_TRUE	D3DMZB_TRUE

	#define D3DFOG_LINEAR	D3DMFOG_LINEAR

	#define D3DMCS_COLOR1	D3DMMCS_COLOR1
	#define D3DMCS_MATERIAL	D3DMMCS_MATERIAL

	#define D3DFILL_POINT		D3DMFILL_POINT
	#define D3DFILL_WIREFRAME	D3DMFILL_WIREFRAME
	#define D3DFILL_SOLID		D3DMFILL_SOLID

	#define D3DSHADE_FLAT		D3DMSHADE_FLAT
	#define D3DSHADE_GOURAUD	D3DMSHADE_GOURAUD

	#define D3DCOLORWRITEENABLE_RED		D3DMCOLORWRITEENABLE_RED
	#define D3DCOLORWRITEENABLE_GREEN	D3DMCOLORWRITEENABLE_GREEN
	#define D3DCOLORWRITEENABLE_BLUE	D3DMCOLORWRITEENABLE_BLUE
	#define D3DCOLORWRITEENABLE_ALPHA	D3DMCOLORWRITEENABLE_ALPHA

	#define D3DTS_TEXTURE0	D3DMTS_TEXTURE0

	#define D3DTSS_COLOROP					D3DMTSS_COLOROP
	#define D3DTSS_ALPHAOP					D3DMTSS_ALPHAOP
	#define D3DTSS_COLORARG1				D3DMTSS_COLORARG1
	#define D3DTSS_COLORARG2				D3DMTSS_COLORARG2
	#define D3DTSS_TEXCOORDINDEX			D3DMTSS_TEXCOORDINDEX
	#define D3DTSS_TEXTURETRANSFORMFLAGS	D3DMTSS_TEXTURETRANSFORMFLAGS
	#define D3DTSS_TCI_PASSTHRU				D3DMTSS_TCI_PASSTHRU
	#define D3DTSS_TCI_CAMERASPACEPOSITION	D3DMTSS_TCI_CAMERASPACEPOSITION

	#define D3DTTFF_DISABLE		D3DMTTFF_DISABLE
	#define D3DTTFF_COUNT2		D3DMTTFF_COUNT2
	#define D3DTTFF_COUNT4		0
	#define D3DTTFF_PROJECTED	D3DMTTFF_PROJECTED

	#define D3DTOP_DISABLE		D3DMTOP_DISABLE
	#define D3DTOP_MODULATE		D3DMTOP_MODULATE
	#define D3DTOP_MODULATE2X	D3DMTOP_MODULATE2X
	#define D3DTOP_MODULATE4X	D3DMTOP_MODULATE4X
	#define D3DTOP_ADD			D3DMTOP_ADD
	#define D3DTOP_DOTPRODUCT3	D3DMTOP_DOTPRODUCT3

	#define D3DBLENDOP_MAX	D3DMBLENDOP_MAX
	#define D3DBLENDOP_ADD	D3DMBLENDOP_ADD

	#define D3DLIGHT_DIRECTIONAL	D3DMLIGHT_DIRECTIONAL
	#define D3DLIGHT_POINT			D3DMLIGHT_POINT

	#define D3DTA_CURRENT	D3DMTA_CURRENT
	#define D3DTA_TEXTURE	D3DMTA_TEXTURE
	#define D3DTA_DIFFUSE	D3DMTA_DIFFUSE

	#define D3DBLEND_DESTCOLOR		D3DMBLEND_DESTCOLOR
	#define D3DBLEND_SRCCOLOR		D3DMBLEND_SRCCOLOR
	#define D3DBLEND_INVDESTCOLOR	D3DMBLEND_INVDESTCOLOR
	#define D3DBLEND_INVSRCCOLOR	D3DMBLEND_INVSRCCOLOR
	#define D3DBLEND_DESTALPHA		D3DMBLEND_DESTALPHA
	#define D3DBLEND_SRCALPHA		D3DMBLEND_SRCALPHA
	#define D3DBLEND_INVDESTALPHA	D3DMBLEND_INVDESTALPHA
	#define D3DBLEND_INVSRCALPHA	D3DMBLEND_INVSRCALPHA

	#define D3DPT_POINTLIST		D3DMPT_POINTLIST
	#define D3DPT_LINELIST		D3DMPT_LINELIST
	#define D3DPT_LINESTRIP		D3DMPT_LINESTRIP
	#define D3DPT_TRIANGLELIST	D3DMPT_TRIANGLELIST
	#define D3DPT_TRIANGLESTRIP	D3DMPT_TRIANGLESTRIP
	#define D3DPT_TRIANGLEFAN	D3DMPT_TRIANGLEFAN

	#define D3DFVF_TEX0				D3DMFVF_TEX0
	#define D3DFVF_TEX1				D3DMFVF_TEX1
	#define D3DFVF_TEX2				D3DMFVF_TEX2
	#define D3DFVF_TEX3				D3DMFVF_TEX3
	#define D3DFVF_TEX4				D3DMFVF_TEX4
	#define D3DFVF_DIFFUSE			D3DMFVF_DIFFUSE
	#define D3DFVF_SPECULAR			D3DMFVF_SPECULAR
	#define D3DFVF_TEXCOORDSIZE1	D3DMFVF_TEXCOORDSIZE1
	#define D3DFVF_TEXCOORDSIZE2	D3DMFVF_TEXCOORDSIZE2
	#define D3DFVF_TEXCOORDSIZE3	D3DMFVF_TEXCOORDSIZE3

	#define IDirect3D9 IDirect3DMobile
	#define IDirect3DDevice9 IDirect3DMobileDevice
	#define IDirect3DIndexBuffer9 IDirect3DMobileIndexBuffer
	#define IDirect3DVertexBuffer9 IDirect3DMobileVertexBuffer
	#define IDirect3DSurface9 IDirect3DMobileSurface
	#define IDirect3DBaseTexture9 IDirect3DMobileBaseTexture
	#define IDirect3DTexture9 IDirect3DMobileTexture
#endif

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
