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

#include "D3D10Renderer.h"
#include "D3D10Texture.h"
#include "D3D10Surface.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

D3D10Texture::D3D10Texture(D3D10Renderer *renderer):BaseResource(),
	mRenderer(NULL),

	mUsageFlags(UsageFlags_NONE),
	mDimension(Dimension_UNKNOWN),
	mFormat(0),
	mWidth(0),
	mHeight(0),
	mDepth(0),
	mMipLevels(0),

	mInternalFormat(0),
//	mD3DFormat(D3DFMT_X8R8G8B8),
//	mD3DUsage(0),
//	mD3DPool(D3DPOOL_MANAGED),
	mTexture(NULL),
	mManuallyGenerateMipLevels(false)
{
	mRenderer=renderer;
}

D3D10Texture::~D3D10Texture(){
	if(mTexture!=NULL){
		destroy();
	}
}

bool D3D10Texture::create(int usageFlags,Dimension dimension,int format,int width,int height,int depth,int mipLevels){
	destroy();

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

void D3D10Texture::destroy(){
	destroyContext(false);
}

bool D3D10Texture::createContext(){
	ID3D10Device *device=mRenderer->getD3D10Device();

	// TODO: Get Usage flags working
	mD3DUsage=D3D10_USAGE_DEFAULT;

	mDXGIFormat=getDXGI_FORMAT(mFormat);

	HRESULT result=E_FAIL;
	switch(mDimension){
		case Texture::Dimension_D1:{
			D3D10_TEXTURE1D_DESC desc;
			desc.Width=mWidth;
			desc.MipLevels=mMipLevels;
			desc.Usage=mD3DUsage;
			desc.Format=mDXGIFormat;
			desc.ArraySize=1;

			ID3D10Texture1D *texture=NULL;
			result=device->CreateTexture1D(&desc,NULL,&texture);
			mTexture=texture;
		}break;
		case Texture::Dimension_D2:
		case Texture::Dimension_CUBE:{
			D3D10_TEXTURE2D_DESC desc;
			desc.Width=mWidth;
			desc.Height=mHeight;
			desc.MipLevels=mMipLevels;
			desc.Usage=mD3DUsage;
			desc.Format=mDXGIFormat;
			desc.ArraySize=mDimension==Texture::Dimension_CUBE?6:1;

			ID3D10Texture2D *texture=NULL;
			result=device->CreateTexture2D(&desc,NULL,&texture);
			mTexture=texture;
		}break;
		case Texture::Dimension_D3:{
			D3D10_TEXTURE3D_DESC desc;
			desc.Width=mWidth;
			desc.Height=mHeight;
			desc.Depth=mDepth;
			desc.MipLevels=mMipLevels;
			desc.Usage=mD3DUsage;
			desc.Format=mDXGIFormat;

			ID3D10Texture3D *texture=NULL;
			result=device->CreateTexture3D(&desc,NULL,&texture);
			mTexture=texture;
		}break;
	}

	// TODO: Add mBacking data

	if(FAILED(result)){
		TOADLET_CHECK_D3D10ERROR(result,"CreateTexture");
		return false;
	}

	return true;
}

void D3D10Texture::destroyContext(bool backData){
	if(backData){
		// TODO: backup data
	}

	if(mTexture!=NULL){
		HRESULT result=mTexture->Release();
		mTexture=NULL;

		if(FAILED(result)){
			TOADLET_CHECK_D3D10ERROR(result,"Release");
			return;
		}
	}
}

Surface::ptr D3D10Texture::getMipSuface(int level,int cubeSide){
return NULL;
/*	if(mTexture==NULL){
		return NULL;
	}

	IDirect3DSurface9 *surface=NULL;

	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;
		texture->GetSurfaceLevel(level,&surface);
	}
	else if(mDimension==Texture::Dimension_D3){
		Error::unimplemented("D3D9Texture::getMipSurface not implement for volume textures");
		return NULL;
	}
	else if(mDimension==Texture::Dimension_CUBE){
		IDirect3DCubeTexture9 *texture=(IDirect3DCubeTexture9*)mTexture;
		texture->GetCubeMapSurface((D3DCUBEMAP_FACES)cubeSide,level,&surface);
	}

	if(surface!=NULL){
		return Surface::ptr(new D3D9Surface(surface));
	}
	else{
		return NULL;
	}*/
}

bool D3D10Texture::load(int format,int width,int height,int depth,int mipLevel,uint8 *data){
	if(mTexture==NULL){
		return false;
	}

	if(mipLevel==0 && (width!=mWidth || height!=mHeight || depth!=mDepth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D10Texture: data of incorrect dimensions");
		return false;
	}

	// TODO:
	D3D10_MAP mapType=D3D10_MAP_READ_WRITE;

	HRESULT result;
	if(mDimension==Texture::Dimension_D1){
		int subresource=D3D10CalcSubresource(mipLevel,1,1);

		void *pData=NULL;
		ID3D10Texture1D *texture=(ID3D10Texture1D*)mTexture;
		result=texture->Map(subresource,mapType,0,&pData);
		if(FAILED(result)){
			TOADLET_CHECK_D3D10ERROR(result,"Map");
			return false;
		}

		int pixelSize=ImageFormatConversion::getPixelSize(format);
		unsigned char *dst=(unsigned char*)pData;
		unsigned char *src=(unsigned char*)data;

		ImageFormatConversion::convert(src,format,width*pixelSize,width*height*pixelSize,dst,mInternalFormat,width,width*height,width,height,depth);

		texture->Unmap(subresource);
	}
	else if(mDimension==Texture::Dimension_D2 || mDimension==Texture::Dimension_CUBE){ // TODO: Do cube
		int subresource=D3D10CalcSubresource(mipLevel,1,1);

		D3D10_MAPPED_TEXTURE2D mappedTex;
		ID3D10Texture2D *texture=(ID3D10Texture2D*)mTexture;
		result=texture->Map(subresource,mapType,0,&mappedTex);
		if(FAILED(result)){
			TOADLET_CHECK_D3D10ERROR(result,"Map");
			return false;
		}

		int pixelSize=ImageFormatConversion::getPixelSize(format);
		unsigned char *dst=(unsigned char*)mappedTex.pData;
		unsigned char *src=(unsigned char*)data;

		ImageFormatConversion::convert(src,format,width*pixelSize,width*height*pixelSize,dst,mInternalFormat,mappedTex.RowPitch,mappedTex.RowPitch*height,width,height,depth);

		texture->Unmap(subresource);
	}
	else if(mDimension==Texture::Dimension_D3){
		int subresource=D3D10CalcSubresource(mipLevel,1,1);

		D3D10_MAPPED_TEXTURE3D mappedTex;
		ID3D10Texture3D *texture=(ID3D10Texture3D*)mTexture;
		result=texture->Map(subresource,mapType,0,&mappedTex);
		if(FAILED(result)){
			TOADLET_CHECK_D3D10ERROR(result,"Map");
			return false;
		}

		int pixelSize=ImageFormatConversion::getPixelSize(format);
		unsigned char *dst=(unsigned char*)mappedTex.pData;
		unsigned char *src=(unsigned char*)data;

		ImageFormatConversion::convert(src,format,width*pixelSize,width*height*pixelSize,dst,mInternalFormat,mappedTex.RowPitch,mappedTex.DepthPitch,width,height,depth);

		texture->Unmap(subresource);
	}
	else{
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"D3D10Texture: unimplemented");
		return false;
	}

// TODO:
	if(mManuallyGenerateMipLevels){
//		mTexture->GenerateMipSubLevels();
	}

	return true;
}

bool D3D10Texture::read(int format,int width,int height,int depth,int mipLevel,uint8 *data){
/*	if(mTexture==NULL){
		return false;
	}

	if(mipLevel==0 && (width!=mWidth || height!=mHeight || depth!=mDepth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: data of incorrect dimensions");
		return false;
	}

	if(mDimension==Texture::Dimension_D1){
	}
	else if(mDimension==Texture::Dimension_D2 || mDimension==Texture::Dimension_CUBE){
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
	else{
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"D3D9Texture: unimplemented");
		return false;
	}
*/
}

int D3D10Texture::getClosestTextureFormat(int textureFormat){
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

DXGI_FORMAT D3D10Texture::getDXGI_FORMAT(int textureFormat){
	DXGI_FORMAT format=DXGI_FORMAT_UNKNOWN;

	if(textureFormat==Format_L_8){
		format=DXGI_FORMAT_R8_UNORM;
	}
	else if(textureFormat==Format_LA_8){
		format=DXGI_FORMAT_R8G8_UNORM;
	}
	else if(textureFormat==Format_RGBA_8){
		format=DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	else if(textureFormat==Format_RGB_5_6_5){
		format=DXGI_FORMAT_B5G6R5_UNORM;
	}
	else if(textureFormat==Format_RGBA_5_5_5_1){
		format=DXGI_FORMAT_B5G5R5A1_UNORM;
	}

	if(format==DXGI_FORMAT_UNKNOWN){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D10Texture::getDXGI_FORMAT: Invalid type");
	}

	return format;
}

}
}
