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
	mDXGIFormat(DXGI_FORMAT_UNKNOWN),
	mD3DUsage((D3D10_USAGE)0),
	mTexture(NULL),
	mShaderResourceView(NULL),
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
	int miscFlags=((mUsageFlags&UsageFlags_AUTOGEN_MIPMAPS)>0)?D3D10_RESOURCE_MISC_GENERATE_MIPS:0;
	int bindFlags=D3D10_BIND_SHADER_RESOURCE;

	mInternalFormat=getClosestTextureFormat(mFormat);
	mDXGIFormat=getDXGI_FORMAT(mInternalFormat);

	D3D10_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format=mDXGIFormat;

	HRESULT result=E_FAIL;
	switch(mDimension){
		case Texture::Dimension_D1:{
			D3D10_TEXTURE1D_DESC desc={0};
			desc.Width=mWidth;
			desc.MipLevels=mMipLevels;
			desc.Usage=mD3DUsage;
			desc.CPUAccessFlags=0;
			desc.BindFlags=bindFlags;
			desc.MiscFlags=miscFlags;
			desc.Format=mDXGIFormat;
			desc.ArraySize=1;
			srvDesc.ViewDimension=D3D10_SRV_DIMENSION_TEXTURE1D;
			srvDesc.Texture1D.MipLevels=mMipLevels;
			srvDesc.Texture1D.MostDetailedMip=0;

			ID3D10Texture1D *texture=NULL;
			result=device->CreateTexture1D(&desc,NULL,&texture);
			TOADLET_CHECK_D3D10ERROR(result,"CreateTexture1D");
			mTexture=texture;
		}break;
		case Texture::Dimension_D2:
		case Texture::Dimension_CUBE:{
			D3D10_TEXTURE2D_DESC desc={0};
			desc.Width=mWidth;
			desc.Height=mHeight;
			desc.MipLevels=mMipLevels;
			desc.SampleDesc.Count=1;
			desc.SampleDesc.Quality=0;
			desc.Usage=D3D10_USAGE_DEFAULT;//mD3DUsage;
			desc.CPUAccessFlags=0;
			desc.BindFlags=D3D10_BIND_SHADER_RESOURCE;//bindFlags;
			desc.MiscFlags=0;//miscFlags;
			desc.Format=DXGI_FORMAT_R8G8B8A8_UINT;//mDXGIFormat;

			if(mDimension==Texture::Dimension_CUBE){
				desc.ArraySize=6;
				srvDesc.ViewDimension=D3D10_SRV_DIMENSION_TEXTURECUBE;
				srvDesc.TextureCube.MipLevels=mMipLevels;
				srvDesc.TextureCube.MostDetailedMip=0;
			}
			else{
				desc.ArraySize=1;
				srvDesc.ViewDimension=D3D10_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels=mMipLevels;
				srvDesc.Texture2D.MostDetailedMip=0;
			}

			ID3D10Texture2D *texture=NULL;
			result=device->CreateTexture2D(&desc,NULL,&texture);
			TOADLET_CHECK_D3D10ERROR(result,"CreateTexture2D");
			mTexture=texture;
		}break;
		case Texture::Dimension_D3:{
			D3D10_TEXTURE3D_DESC desc={0};
			desc.Width=mWidth;
			desc.Height=mHeight;
			desc.Depth=mDepth;
			desc.MipLevels=mMipLevels;
			desc.Usage=mD3DUsage;
			desc.BindFlags=bindFlags;
			desc.MiscFlags=miscFlags;
			desc.Format=mDXGIFormat;
			srvDesc.ViewDimension=D3D10_SRV_DIMENSION_TEXTURE3D;
			srvDesc.Texture3D.MipLevels=mMipLevels;
			srvDesc.Texture3D.MostDetailedMip=0;

			ID3D10Texture3D *texture=NULL;
			result=device->CreateTexture3D(&desc,NULL,&texture);
			TOADLET_CHECK_D3D10ERROR(result,"CreateTexture3D");
			mTexture=texture;
		}break;
	}

//	device->CreateShaderResourceView(mTexture,&srvDesc,&mShaderResourceView);

	// TODO: Add mBacking data

	return SUCCEEDED(result);
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

Surface::ptr D3D10Texture::getMipSurface(int level,int cubeSide){
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
//		result=texture->Map(subresource,mapType,0,&mappedTex);
//		if(FAILED(result)){
//			TOADLET_CHECK_D3D10ERROR(result,"Map");
//			return false;
//		}

//		int pixelSize=ImageFormatConversion::getPixelSize(format);
//		unsigned char *dst=(unsigned char*)mappedTex.pData;
//		unsigned char *src=(unsigned char*)data;

//		ImageFormatConversion::convert(src,format,width*pixelSize,width*height*pixelSize,dst,mInternalFormat,mappedTex.RowPitch,mappedTex.RowPitch*height,width,height,depth);

//		texture->Unmap(subresource);
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

	if(mManuallyGenerateMipLevels){
		mRenderer->getD3D10Device()->GenerateMips(mShaderResourceView);
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
return false;
}

int D3D10Texture::getClosestTextureFormat(int textureFormat){
	if(textureFormat==Format_A_8){
		textureFormat=Format_LA_8;
	}
	else if(textureFormat==Format_RGB_8){
		textureFormat=Format_RGBA_8;
	}

textureFormat=Format_RGBA_8;

	return textureFormat;
}

DXGI_FORMAT D3D10Texture::getDXGI_FORMAT(int textureFormat){
	switch(textureFormat){
		case Format_L_8:
			return DXGI_FORMAT_R8_UNORM;
		case Format_LA_8:
			return DXGI_FORMAT_R8G8_UNORM;
		case Format_BGRA_8:
			return DXGI_FORMAT_R8G8B8A8_UNORM;
		case Format_RGBA_8:
			return DXGI_FORMAT_B8G8R8A8_UNORM;
		case Format_RGB_5_6_5:
			return DXGI_FORMAT_B5G6R5_UNORM;
		case Format_RGBA_5_5_5_1:
			return DXGI_FORMAT_B5G5R5A1_UNORM;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"D3D10Texture::getDXGI_FORMAT: Invalid type");
			return DXGI_FORMAT_UNKNOWN;
	}
}

}
}
