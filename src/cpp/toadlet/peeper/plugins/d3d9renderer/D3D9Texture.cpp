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

// TODO: Add a dynamic usage flag, and also use D3DLOCK_DISCARD when using that
D3D9Texture::D3D9Texture(D3D9Renderer *renderer):BaseResource(),
	mRenderer(NULL),

	mUsageFlags(UsageFlags_NONE),
	mDimension(Dimension_UNKNOWN),
	mFormat(0),
	mWidth(0),
	mHeight(0),
	mDepth(0),
	mMipLevels(0),

	mInternalFormat(0),
	mD3DFormat(D3DFMT_X8R8G8B8),
	mD3DUsage(0),
	mD3DPool(D3DPOOL_MANAGED),
	mTexture(NULL),
	mManuallyGenerateMipLevels(false),
	mBackupSurface(NULL)
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
	if(mBackupSurface!=NULL){
		mBackupSurface->Release();
		mBackupSurface=NULL;
	}

	destroyContext(false);
}

bool D3D9Texture::createContext(){
	IDirect3DDevice9 *device=mRenderer->getDirect3DDevice9();
	IDirect3D9 *d3d=NULL; device->GetDirect3D(&d3d);

	mD3DUsage=(mUsageFlags&UsageFlags_RENDERTARGET)>0 ? D3DUSAGE_RENDERTARGET : 0;
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		mD3DUsage|=D3DUSAGE_LOCKABLE;
	#endif

	mD3DPool=
		#if defined(TOADLET_HAS_DIRECT3DMOBILE)
			D3DPOOL_MANAGED;
		#else
			(mUsageFlags&UsageFlags_RENDERTARGET)>0 ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
		#endif

	mInternalFormat=getClosestTextureFormat(mFormat);
	mD3DFormat=getD3DFORMAT(mInternalFormat);
	if(!mRenderer->isD3DFORMATValid(mD3DFormat,mD3DUsage)){
		mInternalFormat=Format_RGBA_8;
		mD3DFormat=D3DFMT_X8R8G8B8;
	}

	mManuallyGenerateMipLevels=(mUsageFlags&UsageFlags_AUTOGEN_MIPMAPS)>0;
	#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
		if(mManuallyGenerateMipLevels && mRenderer->isD3DFORMATValid(mD3DFormat,D3DUSAGE_AUTOGENMIPMAP)){
			mD3DUsage|=D3DUSAGE_AUTOGENMIPMAP;
			mManuallyGenerateMipLevels=false;
		}
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
			case Texture::Dimension_CUBE:{
				IDirect3DCubeTexture9 *texture=NULL;
				result=device->CreateCubeTexture(mWidth,mMipLevels,mD3DUsage,mD3DFormat,mD3DPool,&texture TOADLET_SHAREDHANDLE);
				mTexture=texture;
			}break;
		#endif
	}

	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		if(mBackupSurface!=NULL){
			IDirect3DSurface9 *surface=NULL;
			result=((IDirect3DTexture9*)mTexture)->GetSurfaceLevel(0,&surface);
			TOADLET_CHECK_D3D9ERROR(result,"CreateOffscreenPlainSurface");
			if(SUCCEEDED(result)){
				result=mRenderer->getDirect3DDevice9()->UpdateSurface(mBackupSurface,NULL,surface,NULL);
				TOADLET_CHECK_D3D9ERROR(result,"UpdateSurface");
				surface->Release();
			}
			mBackupSurface->Release();
			mBackupSurface=NULL;
		}
	}

	if(FAILED(result)){
		TOADLET_CHECK_D3D9ERROR(result,"CreateTexture");
		return false;
	}

	return true;
}

void D3D9Texture::destroyContext(bool backData){
	HRESULT result=S_OK;

	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		if(backData){
			result=mRenderer->getDirect3DDevice9()->CreateOffscreenPlainSurface(mWidth,mHeight,mD3DFormat,D3DPOOL_SYSTEMMEM,&mBackupSurface,NULL);
			TOADLET_CHECK_D3D9ERROR(result,"CreateOffscreenPlainSurface");
			if(SUCCEEDED(result)){
				IDirect3DSurface9 *surface=NULL;
				result=((IDirect3DTexture9*)mTexture)->GetSurfaceLevel(0,&surface);
				TOADLET_CHECK_D3D9ERROR(result,"CreateOffscreenPlainSurface");
				if(SUCCEEDED(result)){
					result=mRenderer->getDirect3DDevice9()->GetRenderTargetData(surface,mBackupSurface);
					TOADLET_CHECK_D3D9ERROR(result,"GetRenderTargetData");
					surface->Release();
				}
			}
		}
	}

	if(mTexture!=NULL){
		HRESULT result=mTexture->Release();
		TOADLET_CHECK_D3D9ERROR(result,"Release");
		mTexture=NULL;
	}
}

bool D3D9Texture::contextNeedsReset(){
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		return false;
	#else
		return mD3DPool==D3DPOOL_DEFAULT;
	#endif
}

Surface::ptr D3D9Texture::getMipSurface(int level,int cubeSide){
	if(mTexture==NULL){
		return NULL;
	}

	IDirect3DSurface9 *surface=NULL;

	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;
		texture->GetSurfaceLevel(level,&surface);
	}
	#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
		else if(mDimension==Texture::Dimension_CUBE){
			IDirect3DCubeTexture9 *texture=(IDirect3DCubeTexture9*)mTexture;
			texture->GetCubeMapSurface((D3DCUBEMAP_FACES)cubeSide,level,&surface);
		}
	#endif
	else{
		Error::unimplemented("D3D9Texture::getMipSurface unimplemented for this texture");
		return NULL;
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
			"D3D9Texture: data of incorrect dimensions");
		return false;
	}

	HRESULT result;
	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;

		if(mD3DPool==D3DPOOL_DEFAULT){
			IDirect3DSurface9 *offscreenSurface=NULL;
			result=mRenderer->getDirect3DDevice9()->CreateOffscreenPlainSurface(mWidth,mHeight,mD3DFormat,D3DPOOL_SYSTEMMEM,&offscreenSurface,NULL);
			TOADLET_CHECK_D3D9ERROR(result,"CreateOffscreenPlainSurface");
			if(SUCCEEDED(result)){
				IDirect3DSurface9 *textureSurface=NULL;
				result=texture->GetSurfaceLevel(mipLevel,&textureSurface);
				TOADLET_CHECK_D3D9ERROR(result,"GetSurfaceLevel");
				if(SUCCEEDED(result)){
					D3DLOCKED_RECT rect={0};
					HRESULT result=offscreenSurface->LockRect(&rect,NULL,D3DLOCK_READONLY);
					TOADLET_CHECK_D3D9ERROR(result,"LockRect");
					if(SUCCEEDED(result)){
						int pixelSize=ImageFormatConversion::getPixelSize(format);
						ImageFormatConversion::convert(data,format,width*pixelSize,width*height*pixelSize,(uint8*)rect.pBits,mInternalFormat,rect.Pitch,rect.Pitch*height,width,height,depth);
						offscreenSurface->UnlockRect();
					}

					result=mRenderer->getDirect3DDevice9()->UpdateSurface(offscreenSurface,NULL,textureSurface,NULL);
					TOADLET_CHECK_D3D9ERROR(result,"UpdateSurface");

					textureSurface->Release();
				}
				offscreenSurface->Release();
			}
		}
		else{
			D3DLOCKED_RECT rect={0};
			result=texture->LockRect(mipLevel,&rect,NULL,0);
			TOADLET_CHECK_D3D9ERROR(result,"LockRect");
			if(SUCCEEDED(result)){
				int pixelSize=ImageFormatConversion::getPixelSize(format);
				ImageFormatConversion::convert(data,format,width*pixelSize,width*height*pixelSize,(uint8*)rect.pBits,mInternalFormat,rect.Pitch,rect.Pitch*height,width,height,depth);
				texture->UnlockRect(mipLevel);
			}
		}
	}
	#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
		else if(mDimension==Texture::Dimension_D3){
			IDirect3DVolumeTexture9 *texture=(IDirect3DVolumeTexture9*)mTexture;

			D3DLOCKED_BOX box={0};
			result=texture->LockBox(mipLevel,&box,NULL,0);
			TOADLET_CHECK_D3D9ERROR(result,"LockBox");
			if(SUCCEEDED(result)){
				int pixelSize=ImageFormatConversion::getPixelSize(format);
				ImageFormatConversion::convert(data,format,width*pixelSize,width*height*pixelSize,(uint8*)box.pBits,mInternalFormat,box.RowPitch,box.SlicePitch,width,height,depth);
				texture->UnlockBox(mipLevel);
			}
		}
		else if(mDimension==Texture::Dimension_CUBE){
			IDirect3DCubeTexture9 *texture=(IDirect3DCubeTexture9*)mTexture;

			int i;
			for(i=0;i<6;++i){
				D3DLOCKED_RECT rect={0};
				result=texture->LockRect((D3DCUBEMAP_FACES)i,mipLevel,&rect,NULL,0);
				TOADLET_CHECK_D3D9ERROR(result,"LockRect");
				if(SUCCEEDED(result)){
					int pixelSize=ImageFormatConversion::getPixelSize(format);
					ImageFormatConversion::convert((data+width*height*pixelSize*i),format,width*pixelSize,width*height*pixelSize,(uint8*)rect.pBits,mInternalFormat,rect.Pitch,rect.Pitch*height,width,height,1);
					texture->UnlockRect((D3DCUBEMAP_FACES)i,mipLevel);
				}
			}
		}
	#endif
	else{
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"D3D9Texture: unimplemented");
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

	if(mipLevel==0 && (width!=mWidth || height!=mHeight || depth!=mDepth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: data of incorrect dimensions");
		return false;
	}

	HRESULT result=S_OK;

	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;

		if(mD3DPool==D3DPOOL_DEFAULT){
			IDirect3DSurface9 *offscreenSurface=NULL;
			result=mRenderer->getDirect3DDevice9()->CreateOffscreenPlainSurface(mWidth,mHeight,mD3DFormat,D3DPOOL_SYSTEMMEM,&offscreenSurface,NULL);
			TOADLET_CHECK_D3D9ERROR(result,"CreateOffscreenPlainSurface");
			if(SUCCEEDED(result)){
				IDirect3DSurface9 *textureSurface=NULL;
				result=texture->GetSurfaceLevel(mipLevel,&textureSurface);
				TOADLET_CHECK_D3D9ERROR(result,"GetSurfaceLevel");
				if(SUCCEEDED(result)){
					result=mRenderer->getDirect3DDevice9()->GetRenderTargetData(textureSurface,offscreenSurface);
					TOADLET_CHECK_D3D9ERROR(result,"GetRenderTargetData");
					if(SUCCEEDED(result)){
						D3DLOCKED_RECT rect={0};
						HRESULT result=offscreenSurface->LockRect(&rect,NULL,D3DLOCK_READONLY);
						TOADLET_CHECK_D3D9ERROR(result,"LockRect");
						if(SUCCEEDED(result)){
							int pixelSize=ImageFormatConversion::getPixelSize(format);
							ImageFormatConversion::convert((uint8*)rect.pBits,mInternalFormat,rect.Pitch,rect.Pitch*height,data,format,width*pixelSize,width*height*pixelSize,width,height,depth);
							offscreenSurface->UnlockRect();
						}
					}
					textureSurface->Release();
				}
				offscreenSurface->Release();
			}
		}
		else{
			D3DLOCKED_RECT rect={0};
			result=texture->LockRect(mipLevel,&rect,NULL,D3DLOCK_READONLY);
			TOADLET_CHECK_D3D9ERROR(result,"LockRect");
			if(SUCCEEDED(result)){
				int pixelSize=ImageFormatConversion::getPixelSize(format);
				ImageFormatConversion::convert((uint8*)rect.pBits,mInternalFormat,rect.Pitch,rect.Pitch*height,data,format,width*pixelSize,width*height*pixelSize,width,height,depth);
				texture->UnlockRect(mipLevel);
			}
		}
	}
	#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
		else if(mDimension==Texture::Dimension_D3){
			if(mD3DPool==D3DPOOL_DEFAULT){
				return false;
			}

			IDirect3DVolumeTexture9 *texture=(IDirect3DVolumeTexture9*)mTexture;

			D3DLOCKED_BOX box={0};
			result=texture->LockBox(mipLevel,&box,NULL,D3DLOCK_READONLY);
			TOADLET_CHECK_D3D9ERROR(result,"LockBox");
			if(SUCCEEDED(result)){
				int pixelSize=ImageFormatConversion::getPixelSize(format);
				ImageFormatConversion::convert((uint8*)box.pBits,mInternalFormat,box.RowPitch,box.SlicePitch,data,format,width*pixelSize,width*height*pixelSize,width,height,depth);
				texture->UnlockBox(mipLevel);
			}
		}
	#endif
	else{
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"D3D9Texture: unimplemented");
		return false;
	}

	return SUCCEEDED(result);
}

int D3D9Texture::getClosestTextureFormat(int textureFormat){
	#if defined(TOADLET_HAS_DIRECT3DMOBILE)
		if(textureFormat==Format_L_8){
			textureFormat=Format_RGB_5_6_5;
		}
		else if(textureFormat==Format_A_8){
			textureFormat=Format_RGBA_8;
		}
		else if(textureFormat==Format_LA_8){
			textureFormat=Format_RGBA_8;
		}
	#else
		if(textureFormat==Format_A_8){
			textureFormat=Format_LA_8;
		}
	#endif
	return textureFormat;
}

D3DFORMAT D3D9Texture::getD3DFORMAT(int textureFormat){
	switch(textureFormat){
		case Format_L_8:
			return D3DFMT_L8;
		case Format_LA_8:
			return D3DFMT_A8L8;
		case Format_RGB_8:
			return D3DFMT_R8G8B8;
		case Format_RGBA_8:
			return D3DFMT_A8R8G8B8;
		case Format_RGB_5_6_5:
			return D3DFMT_R5G6B5;
		case Format_RGBA_5_5_5_1:
			return D3DFMT_A1R5G5B5;
		case Format_RGBA_4_4_4_4:
			return D3DFMT_A4R4G4B4;
		case Format_DEPTH_16:
			return D3DFMT_D16;
		case Format_DEPTH_24:
			return D3DFMT_D24X8;
		case Format_DEPTH_32:
			return D3DFMT_D32;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D9Texture::getD3DFORMAT: Invalid type");
			return D3DFMT_UNKNOWN;
	}
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
