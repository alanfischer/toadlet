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

#include "D3D9Renderer.h"
#include "D3D9Texture.h"
#include "D3D9Surface.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

D3D9Texture::D3D9Texture(D3D9Renderer *renderer):BaseResource(),
	mRenderer(NULL),

	mUsageFlags(UsageFlags_NONE),
	mDimension(Dimension_UNKNOWN),
	mFormat(0),
	mWidth(0),
	mHeight(0),
	mDepth(0),
	mMipLevels(0),

	mD3DFormat(D3DFMT_X8R8G8B8),
	mD3DUsage(0),
	mD3DPool(D3DPOOL_MANAGED),
	mTexture(NULL),
	mManuallyGenerateMipLevels(false)
{
	mRenderer=renderer;
}

D3D9Texture::~D3D9Texture(){
	if(mTexture!=NULL){
		destroy();
	}
}

bool D3D9Texture::create(int usageFlags,Dimension dimension,int format,int width,int height,int depth,int mipLevels){
	destroy();

	if((Math::isPowerOf2(width)==false || Math::isPowerOf2(height)==false || Math::isPowerOf2(depth)==false) &&
		mRenderer->getCapabilitySet().textureNonPowerOf2==false &&
		(mRenderer->getCapabilitySet().textureNonPowerOf2==false || (usageFlags&UsageFlags_NPOT_RESTRICTED)==0))
	{
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: Cannot load a non power of 2 texture");
		return false;
	}

	mUsageFlags=usageFlags;
	mDimension=dimension;
	mFormat=format;
	mWidth=width;
	mHeight=height;
	mDepth=depth;
	mMipLevels=mipLevels;

	bool result=createContext();

	return result;
}

void D3D9Texture::destroy(){
	destroyContext(false);
}

bool D3D9Texture::createContext(){
	IDirect3DDevice9 *device=mRenderer->getDirect3DDevice9();
	IDirect3D9 *d3d=NULL; device->GetDirect3D(&d3d);

	mD3DFormat=getD3DFORMAT(mFormat);
	if(!isD3DFORMATValid(d3d,D3DFMT_X8R8G8B8,mD3DFormat,0)){
		mD3DFormat=D3DFMT_X8R8G8B8;
	}
	
	mD3DUsage=(mUsageFlags&UsageFlags_RENDERTARGET)>0 ? D3DUSAGE_RENDERTARGET : 0;
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		mD3DUsage|=D3DUSAGE_LOCKABLE;
	#endif
	mManuallyGenerateMipLevels=(mUsageFlags&UsageFlags_AUTOGEN_MIPMAPS)>0;
	#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
		if(mManuallyGenerateMipLevels && isD3DFORMATValid(d3d,D3DFMT_X8R8G8B8,mD3DFormat,D3DUSAGE_AUTOGENMIPMAP)){
			mD3DUsage|=D3DUSAGE_AUTOGENMIPMAP;
			mManuallyGenerateMipLevels=false;
		}
	#endif

	mD3DPool=
		#if defined(TOADLET_HAS_DIRECT3DMOBILE)
			D3DPOOL_MANAGED;
		#else
			(mUsageFlags&UsageFlags_RENDERTARGET)>0 ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
		#endif

	HRESULT result=E_FAIL;
	switch(mDimension){
		case Texture::Dimension_D1:
		case Texture::Dimension_D2:{
			IDirect3DTexture9 *texture=NULL;
			result=device->CreateTexture(mWidth,mHeight,mMipLevels,mD3DUsage,mD3DFormat,mD3DPool,&texture TOADLET_SHAREDHANDLE);
			mTexture=texture;
		}break;
		#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
			case Texture::Dimension_D3:{
				IDirect3DVolumeTexture9 *texture=NULL;
				result=device->CreateVolumeTexture(mWidth,mHeight,mDepth,mMipLevels,mD3DUsage,mD3DFormat,mD3DPool,&texture TOADLET_SHAREDHANDLE);
				mTexture=texture;
			}break;
			case Texture::Dimension_CUBEMAP:{
				IDirect3DCubeTexture9 *texture=NULL;
				result=device->CreateCubeTexture(mWidth,mMipLevels,mD3DUsage,mD3DFormat,mD3DPool,&texture TOADLET_SHAREDHANDLE);
				mTexture=texture;
			}break;
		#endif
	}

	// TODO: Add mBacking data

	if(FAILED(result)){
		TOADLET_CHECK_D3D9ERROR(result,"CreateTexture");
		return false;
	}

	return true;
}

void D3D9Texture::destroyContext(bool backData){
	if(backData){
		// TODO: backup data
	}

	if(mTexture!=NULL){
		HRESULT result=mTexture->Release();
		mTexture=NULL;

		if(FAILED(result)){
			TOADLET_CHECK_D3D9ERROR(result,"CreateTexture");
			return;
		}
	}
}

bool D3D9Texture::contextNeedsReset(){
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		return false;
	#else
		return mD3DPool==D3DPOOL_DEFAULT;
	#endif
}

Surface::ptr D3D9Texture::getMipSuface(int i){
	if(mTexture==NULL){
		return NULL;
	}

	IDirect3DSurface9 *surface=NULL;

	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;
		texture->GetSurfaceLevel(i,&surface);
	}

	if(surface!=NULL){
		return Surface::ptr(new D3D9Surface(surface));
	}
	else{
		return NULL;
	}
}

bool D3D9Texture::load(int format,int width,int height,int depth,int mipLevel,uint8 *data){
	if(mTexture==NULL){
		return false;
	}

	if(mipLevel==0 && (width!=mWidth || height!=mHeight || depth!=mDepth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: Texture data of incorrect dimensions");
		return false;
	}

	HRESULT result;
	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;

		D3DLOCKED_RECT rect={0};
		result=texture->LockRect(mipLevel,&rect,NULL,D3DLOCK_DISCARD);
		if(FAILED(result)){
			TOADLET_CHECK_D3D9ERROR(result,"LockRect");
			return false;
		}

		int pixelSize=ImageFormatConversion::getPixelSize(format);
		unsigned char *dst=(unsigned char*)rect.pBits;
		unsigned char *src=(unsigned char*)data;

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

		texture->UnlockRect(mipLevel);
	}
	else{
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"D3D9Texture: Volume & Cube loading not yet implemented");
		return false;
	}

	if(mManuallyGenerateMipLevels){
		#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
			mTexture->GenerateMipSubLevels();
		#endif
	}

	return true;
}

bool D3D9Texture::read(int format,int width,int height,int depth,int mipLevel,uint8 *data){
	if(mTexture==NULL){
		return false;
	}

	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;

		D3DLOCKED_RECT rect={0};
		HRESULT result=texture->LockRect(mipLevel,&rect,NULL,D3DLOCK_READONLY);
		
		// TODO: convert & copy back, inverse of above
		
		texture->UnlockRect(mipLevel);

		return true;
	}
	else{
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"D3D9Texture: Volume & Cube reading not yet implemented");
		return false;
	}
}

bool D3D9Texture::isD3DFORMATValid(IDirect3D9 *d3d,D3DFORMAT adapterFormat,D3DFORMAT textureFormat,DWORD usage){
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		return SUCCEEDED(d3d->CheckDeviceFormat(D3DMADAPTER_DEFAULT,D3DMDEVTYPE_DEFAULT,adapterFormat,usage,D3DMRTYPE_TEXTURE,textureFormat));
	#else
		return SUCCEEDED(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,adapterFormat,usage,D3DRTYPE_TEXTURE,textureFormat));
	#endif
}

D3DFORMAT D3D9Texture::getD3DFORMAT(int textureFormat){
	D3DFORMAT format=D3DFMT_UNKNOWN;

	if(textureFormat==Texture::Format_L_8){
		#if defined(TOADLET_HAS_DIRECT3DMOBILE)
			format=D3DMFMT_R5G6B5;
		#else
			format=D3DFMT_L8;
		#endif
	}
	else if(textureFormat==Texture::Format_A_8){
		#if defined(TOADLET_HAS_DIRECT3DMOBILE)
			format=D3DMFMT_A8R8G8B8;
		#else
			format=D3DFMT_A8L8; // Use A8L8 and fill L8 with white
		#endif
	}
	else if(textureFormat==Texture::Format_LA_8){
		#if defined(TOADLET_HAS_DIRECT3DMOBILE)
			format=D3DMFMT_A8R8G8B8;
		#else
			format=D3DFMT_A8L8;
		#endif
	}
	else if(textureFormat==Texture::Format_RGB_8){
		format=D3DFMT_R8G8B8;
	}
	else if(textureFormat==Texture::Format_RGBA_8){
		format=D3DFMT_A8R8G8B8;
	}
	else if(textureFormat==Texture::Format_RGB_5_6_5){
		format=D3DFMT_R5G6B5;
	}
	else if(textureFormat==Texture::Format_RGBA_5_5_5_1){
		format=D3DFMT_A1R5G5B5;
	}
	else if(textureFormat==Texture::Format_RGBA_4_4_4_4){
		format=D3DFMT_A4R4G4B4;
	}

	if(format==D3DFMT_UNKNOWN){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture::getD3DFORMAT: Invalid type");
	}

	return format;
}

DWORD D3D9Texture::getD3DTADDRESS(TextureStage::AddressMode addressMode){
	DWORD taddress=0;

	switch(addressMode){
		case TextureStage::AddressMode_REPEAT:
			taddress=D3DTADDRESS_WRAP;
		break;
		case TextureStage::AddressMode_CLAMP_TO_EDGE:
			taddress=D3DTADDRESS_CLAMP;
		break;
		case TextureStage::AddressMode_CLAMP_TO_BORDER:
			taddress=D3DTADDRESS_BORDER;
		break;
		case TextureStage::AddressMode_MIRRORED_REPEAT:
			taddress=D3DTADDRESS_MIRROR;
		break;
	}

	if(taddress==0){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture::getD3DTADDRESS: Invalid address mode");
	}

	return taddress;
}

DWORD D3D9Texture::getD3DTEXF(TextureStage::Filter filter){
	DWORD texf=D3DTEXF_NONE;

	switch(filter){
		case TextureStage::Filter_NONE:
			texf=D3DTEXF_NONE;
		break;
		case TextureStage::Filter_NEAREST:
			texf=D3DTEXF_POINT;
		break;
		case TextureStage::Filter_LINEAR:
			texf=D3DTEXF_LINEAR;
		break;
	}

	return texf;
}

}
}
