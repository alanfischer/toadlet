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

/// @todo: Add a dynamic usage flag, and also use D3DLOCK_DISCARD when using that
D3D9Texture::D3D9Texture(D3D9Renderer *renderer):BaseResource(),
	mRenderer(NULL),

	mUsage(0),
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

bool D3D9Texture::create(int usage,Dimension dimension,int format,int width,int height,int depth,int mipLevels,byte *mipDatas[]){
	if((Math::isPowerOf2(width)==false || Math::isPowerOf2(height)==false || (dimension!=Dimension_CUBE && Math::isPowerOf2(depth)==false)) &&
		mRenderer->getCapabilitySet().textureNonPowerOf2==false &&
		(mRenderer->getCapabilitySet().textureNonPowerOf2==false || (usage&Usage_BIT_NPOT_RESTRICTED)==0))
	{
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: Cannot load a non power of 2 texture");
		return false;
	}

	if(format!=mRenderer->getClosestTextureFormat(format)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: Invalid texture format");
		return false;
	}

	mUsage=usage;
	mDimension=dimension;
	mFormat=format;
	mWidth=width;
	mHeight=height;
	mDepth=depth;
	mMipLevels=mipLevels;

	createContext(false);

	int specifiedMipLevels=mMipLevels>0?mMipLevels:1;
	if(mipDatas!=NULL){
		int level;
		for(level=0;level<specifiedMipLevels;++level){
			load(width,height,depth,level,mipDatas[level]);
			width/=2;height/=2;depth/=2;
		}
	}

	return mTexture!=NULL;
}

void D3D9Texture::destroy(){
	destroyContext(false);
}

void D3D9Texture::resetCreate(){
	if(needsReset()){
		createContext(true);
	}
}

void D3D9Texture::resetDestroy(){
	if(needsReset()){
		destroyContext(true);
	}
}

bool D3D9Texture::createContext(bool restore){
	IDirect3DDevice9 *device=mRenderer->getDirect3DDevice9();

	mD3DUsage=
		#if defined(TOADLET_SET_D3DM)
			D3DUSAGE_LOCKABLE;
		#else
			0;
		#endif
	if((mUsage&Usage_BIT_RENDERTARGET)>0){
		mD3DUsage|=D3DUSAGE_RENDERTARGET;
	}
	if((mUsage&Usage_BIT_STREAM)>0){
		mD3DUsage|=D3DUSAGE_DYNAMIC;
	}

	mD3DPool=
		#if defined(TOADLET_SET_D3DM)
			D3DPOOL_MANAGED;
		#else
			(mUsage&Usage_BIT_RENDERTARGET)>0 ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
		#endif

	mInternalFormat=mRenderer->getClosestTextureFormat(mFormat);
	mD3DFormat=D3D9Renderer::getD3DFORMAT(mInternalFormat);
	if(!mRenderer->isD3DFORMATValid(mD3DFormat,mD3DUsage)){
		mInternalFormat=Format_BGRA_8;
		mD3DFormat=D3DFMT_X8R8G8B8;
	}

	mManuallyGenerateMipLevels=(mUsage&Usage_BIT_AUTOGEN_MIPMAPS)>0;
	#if !defined(TOADLET_SET_D3DM)
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
		#if !defined(TOADLET_SET_D3DM)
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

	#if !defined(TOADLET_SET_D3DM)
		if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
			if(restore && mBackupSurface!=NULL){
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
	#endif

	TOADLET_CHECK_D3D9ERROR(result,"CreateTexture");

	return SUCCEEDED(result);
}

bool D3D9Texture::destroyContext(bool backup){
	if(backup){
		HRESULT result=S_OK;
		#if !defined(TOADLET_SET_D3DM)
			if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
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
		#endif
	}
	// Otherwise if the backup surface is still around for some reason, just release it
	else if(mBackupSurface!=NULL){
		mBackupSurface->Release();
		mBackupSurface=NULL;
	}

	HRESULT result=S_OK;
	if(mTexture!=NULL){
		result=mTexture->Release();
		mTexture=NULL;
	}

	return SUCCEEDED(result);
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
	#if !defined(TOADLET_SET_D3DM)
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

bool D3D9Texture::load(int width,int height,int depth,int mipLevel,byte *mipData){
	if(mTexture==NULL){
		return false;
	}

	width=width>0?width:1;height=height>0?height:1;depth=depth>0?depth:1;

	if(mipLevel==0 && (width!=mWidth || height!=mHeight || depth!=mDepth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: data of incorrect dimensions");
		return false;
	}

	int format=mFormat;
	int rowPitch=width*ImageFormatConversion::getPixelSize(format);
	int slicePitch=rowPitch*height;

	HRESULT result;
	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;

		#if !defined(TOADLET_SET_D3DM)
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
							ImageFormatConversion::convert(mipData,format,rowPitch,slicePitch,(uint8*)rect.pBits,mInternalFormat,rect.Pitch,rect.Pitch*height,width,height,depth);
							offscreenSurface->UnlockRect();
						}

						result=mRenderer->getDirect3DDevice9()->UpdateSurface(offscreenSurface,NULL,textureSurface,NULL);
						TOADLET_CHECK_D3D9ERROR(result,"UpdateSurface");

						textureSurface->Release();
					}
					offscreenSurface->Release();
				}
			}
			else
		#endif
		{
			D3DLOCKED_RECT rect={0};
			result=texture->LockRect(mipLevel,&rect,NULL,0);
			TOADLET_CHECK_D3D9ERROR(result,"LockRect");
			if(SUCCEEDED(result)){
				int pixelSize=ImageFormatConversion::getPixelSize(format);
				ImageFormatConversion::convert(mipData,format,rowPitch,slicePitch,(uint8*)rect.pBits,mInternalFormat,rect.Pitch,rect.Pitch*height,width,height,depth);
				texture->UnlockRect(mipLevel);
			}
		}
	}
	#if !defined(TOADLET_SET_D3DM)
		else if(mDimension==Texture::Dimension_D3){
			IDirect3DVolumeTexture9 *texture=(IDirect3DVolumeTexture9*)mTexture;

			D3DLOCKED_BOX box={0};
			result=texture->LockBox(mipLevel,&box,NULL,0);
			TOADLET_CHECK_D3D9ERROR(result,"LockBox");
			if(SUCCEEDED(result)){
				int pixelSize=ImageFormatConversion::getPixelSize(format);
				ImageFormatConversion::convert(mipData,format,rowPitch,slicePitch,(uint8*)box.pBits,mInternalFormat,box.RowPitch,box.SlicePitch,width,height,depth);
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
					ImageFormatConversion::convert((mipData+width*height*pixelSize*i),format,rowPitch,slicePitch,(uint8*)rect.pBits,mInternalFormat,rect.Pitch,rect.Pitch*height,width,height,1);
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
		#if !defined(TOADLET_SET_D3DM)
			mTexture->GenerateMipSubLevels();
		#endif
	}

	return true;
}

bool D3D9Texture::read(int width,int height,int depth,int mipLevel,byte *mipData){
	if(mTexture==NULL){
		return false;
	}

	width=width>0?width:1;height=height>0?height:1;depth=depth>0?depth:1;

	if(mipLevel==0 && (width!=mWidth || height!=mHeight || depth!=mDepth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: data of incorrect dimensions");
		return false;
	}

	int format=mFormat;
	int rowPitch=width*ImageFormatConversion::getPixelSize(format);
	int slicePitch=rowPitch*height;

	HRESULT result=S_OK;

	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;

		#if !defined(TOADLET_SET_D3DM)
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
								ImageFormatConversion::convert((uint8*)rect.pBits,mInternalFormat,rect.Pitch,rect.Pitch*height,mipData,format,rowPitch,slicePitch,width,height,depth);
								offscreenSurface->UnlockRect();
							}
						}
						textureSurface->Release();
					}
					offscreenSurface->Release();
				}
			}
			else
		#endif
		{
			D3DLOCKED_RECT rect={0};
			result=texture->LockRect(mipLevel,&rect,NULL,D3DLOCK_READONLY);
			TOADLET_CHECK_D3D9ERROR(result,"LockRect");
			if(SUCCEEDED(result)){
				int pixelSize=ImageFormatConversion::getPixelSize(format);
				ImageFormatConversion::convert((uint8*)rect.pBits,mInternalFormat,rect.Pitch,rect.Pitch*height,mipData,format,rowPitch,slicePitch,width,height,depth);
				texture->UnlockRect(mipLevel);
			}
		}
	}
	#if !defined(TOADLET_SET_D3DM)
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
				ImageFormatConversion::convert((uint8*)box.pBits,mInternalFormat,box.RowPitch,box.SlicePitch,mipData,format,rowPitch,slicePitch,width,height,depth);
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

bool D3D9Texture::needsReset(){
	#if defined(TOADLET_SET_D3DM)
		return false;
	#else
		return mD3DPool==D3DPOOL_DEFAULT;
	#endif
}

}
}
