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

	mInternalFormat=getClosestTextureFormat(mFormat);
	mD3DFormat=getD3DFORMAT(mInternalFormat);
	if(!isD3DFORMATValid(d3d,D3DFMT_X8R8G8B8,mD3DFormat,mD3DUsage)){
		mInternalFormat=Format_RGBA_8;
		mD3DFormat=D3DFMT_X8R8G8B8;
	}

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
			TOADLET_CHECK_D3D9ERROR(result,"Release");
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
	if(mTexture==NULL || mD3DPool==D3DPOOL_DEFAULT){
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

		D3DLOCKED_RECT rect={0};
		result=texture->LockRect(mipLevel,&rect,NULL,0);
		if(FAILED(result)){
			TOADLET_CHECK_D3D9ERROR(result,"LockRect");
			return false;
		}

		int pixelSize=ImageFormatConversion::getPixelSize(format);
		unsigned char *dst=(unsigned char*)rect.pBits;
		unsigned char *src=(unsigned char*)data;

		ImageFormatConversion::convert(src,format,width*pixelSize,width*height*pixelSize,dst,mInternalFormat,rect.Pitch,rect.Pitch*height,width,height,depth);

		texture->UnlockRect(mipLevel);
	}
	#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
		else if(mDimension==Texture::Dimension_D3){
			IDirect3DVolumeTexture9 *texture=(IDirect3DVolumeTexture9*)mTexture;

			D3DLOCKED_BOX box={0};
			result=texture->LockBox(mipLevel,&box,NULL,0);
			if(FAILED(result)){
				TOADLET_CHECK_D3D9ERROR(result,"LockBox");
				return false;
			}

			int pixelSize=ImageFormatConversion::getPixelSize(format);
			unsigned char *dst=(unsigned char*)box.pBits;
			unsigned char *src=(unsigned char*)data;

			ImageFormatConversion::convert(src,format,width*pixelSize,width*height*pixelSize,dst,mInternalFormat,box.RowPitch,box.SlicePitch,width,height,depth);

			texture->UnlockBox(mipLevel);
		}
		else if(mDimension==Texture::Dimension_CUBE){
			IDirect3DCubeTexture9 *texture=(IDirect3DCubeTexture9*)mTexture;

			int i;
			for(i=0;i<6;++i){
				D3DLOCKED_RECT rect={0};
				result=texture->LockRect((D3DCUBEMAP_FACES)i,mipLevel,&rect,NULL,0);
				if(FAILED(result)){
					TOADLET_CHECK_D3D9ERROR(result,"LockBox");
					return false;
				}

				int pixelSize=ImageFormatConversion::getPixelSize(format);
				unsigned char *dst=(unsigned char*)rect.pBits;
				unsigned char *src=(unsigned char*)(data+width*height*pixelSize*i);

				ImageFormatConversion::convert(src,format,width*pixelSize,width*height*pixelSize,dst,mInternalFormat,rect.Pitch,rect.Pitch*height,width,height,1);

				texture->UnlockRect((D3DCUBEMAP_FACES)i,mipLevel);
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
	if(mTexture==NULL || mD3DPool==D3DPOOL_DEFAULT){
		return false;
	}

	if(mipLevel==0 && (width!=mWidth || height!=mHeight || depth!=mDepth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: data of incorrect dimensions");
		return false;
	}

	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;

		D3DLOCKED_RECT rect={0};
		HRESULT result=texture->LockRect(mipLevel,&rect,NULL,D3DLOCK_READONLY);
		if(FAILED(result)){
			TOADLET_CHECK_D3D9ERROR(result,"LockRect");
			return false;
		}

		int pixelSize=ImageFormatConversion::getPixelSize(format);
		unsigned char *dst=(unsigned char*)data;
		unsigned char *src=(unsigned char*)rect.pBits;

		ImageFormatConversion::convert(src,mInternalFormat,rect.Pitch,rect.Pitch*height,dst,format,width*pixelSize,width*height*pixelSize,width,height,depth);

		texture->UnlockRect(mipLevel);

		return true;
	}
	#if !defined(TOADLET_HAS_DIRECT3DMOBILE)
		else if(mDimension==Texture::Dimension_D3){
			IDirect3DVolumeTexture9 *texture=(IDirect3DVolumeTexture9*)mTexture;

			D3DLOCKED_BOX box={0};
			HRESULT result=texture->LockBox(mipLevel,&box,NULL,D3DLOCK_READONLY);
			if(FAILED(result)){
				TOADLET_CHECK_D3D9ERROR(result,"LockBox");
				return false;
			}

			int pixelSize=ImageFormatConversion::getPixelSize(format);
			unsigned char *dst=(unsigned char*)data;
			unsigned char *src=(unsigned char*)box.pBits;

			ImageFormatConversion::convert(src,mInternalFormat,box.RowPitch,box.SlicePitch,dst,format,width*pixelSize,width*height*pixelSize,width,height,depth);

			texture->UnlockBox(mipLevel);

			return true;
		}
	#endif
	else{
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"D3D9Texture: unimplemented");
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
	D3DFORMAT format=D3DFMT_UNKNOWN;

	if(textureFormat==Format_L_8){
		format=D3DFMT_L8;
	}
	else if(textureFormat==Format_LA_8){
		format=D3DFMT_A8L8;
	}
	else if(textureFormat==Format_RGB_8){
		format=D3DFMT_R8G8B8;
	}
	else if(textureFormat==Format_RGBA_8){
		format=D3DFMT_A8R8G8B8;
	}
	else if(textureFormat==Format_RGB_5_6_5){
		format=D3DFMT_R5G6B5;
	}
	else if(textureFormat==Format_RGBA_5_5_5_1){
		format=D3DFMT_A1R5G5B5;
	}
	else if(textureFormat==Format_RGBA_4_4_4_4){
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
