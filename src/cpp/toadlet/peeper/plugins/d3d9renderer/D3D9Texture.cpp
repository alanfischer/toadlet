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

D3D9Texture::D3D9Texture(D3D9Renderer *renderer):
	mRenderer(NULL),

	mUsageFlags(UsageFlags_NONE),
	mDimension(Dimension_UNKNOWN),
	mFormat(0),
	mWidth(0),
	mHeight(0),
	mDepth(0),

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
	
	IDirect3DDevice9 *device=mRenderer->getDirect3DDevice9();
	IDirect3D9 *d3d=NULL; device->GetDirect3D(&d3d);

	D3DFORMAT d3dformat=getD3DFORMAT(mFormat);
	if(!isD3DFORMATValid(d3d,D3DFMT_X8R8G8B8,d3dformat,0)){
		d3dformat=D3DFMT_X8R8G8B8;
	}
	
	DWORD usage=(mUsageFlags&UsageFlags_RENDERTARGET)>0 ? D3DUSAGE_RENDERTARGET : 0;
	mManuallyGenerateMipLevels=(usageFlags&UsageFlags_AUTOGEN_MIPMAPS)>0;
	if(mManuallyGenerateMipLevels && isD3DFORMATValid(d3d,D3DFMT_X8R8G8B8,d3dformat,D3DUSAGE_AUTOGENMIPMAP)){
		usage|=D3DUSAGE_AUTOGENMIPMAP;
		mManuallyGenerateMipLevels=false;
	}

	D3DPOOL pool=D3DPOOL_MANAGED;

	HRESULT result=E_FAIL;
	switch(dimension){
		case Texture::Dimension_D1:
		case Texture::Dimension_D2:{
			IDirect3DTexture9 *texture=NULL;
			result=device->CreateTexture(mWidth,mHeight,mipLevels,usage,d3dformat,pool,&texture,NULL);
			mTexture=texture;
		}break;
		case Texture::Dimension_D3:{
			IDirect3DVolumeTexture9 *texture=NULL;
			result=device->CreateVolumeTexture(mWidth,mHeight,depth,mipLevels,usage,d3dformat,pool,&texture,NULL);
			mTexture=texture;
		}break;
		case Texture::Dimension_CUBEMAP:{
			IDirect3DCubeTexture9 *texture=NULL;
			result=device->CreateCubeTexture(mWidth,mipLevels,usage,d3dformat,pool,&texture,NULL);
			mTexture=texture;
		}break;
	}

	if(FAILED(result)){
		TOADLET_CHECK_D3D9ERROR(result,"CreateTexture");
		return false;
	}

	return true;
}

void D3D9Texture::destroy(){
	if(mTexture!=NULL){
		HRESULT result=mTexture->Release();
		mTexture=NULL;

		if(FAILED(result)){
			TOADLET_CHECK_D3D9ERROR(result,"CreateTexture");
			return;
		}
	}
}

Surface::ptr D3D9Texture::getMipSuface(int i) const{
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

void D3D9Texture::load(int format,int width,int height,int depth,uint8 *data){
	if(width!=mWidth || height!=mHeight || depth!=mDepth){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9Texture: Texture data of incorrect dimensions");
		return;
	}

	D3DFORMAT d3dformat=getD3DFORMAT(mFormat);
	HRESULT result;
	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;

		D3DLOCKED_RECT rect={0};
		result=texture->LockRect(0,&rect,NULL,D3DLOCK_DISCARD);
		if(FAILED(result)){
			TOADLET_CHECK_D3D9ERROR(result,"LockRect");
			return;
		}

		int pixelSize=ImageFormatConversion::getPixelSize(format);
		unsigned char *dst=(unsigned char*)rect.pBits;
		unsigned char *src=(unsigned char*)data;
Logger::log("LOADING DATAS");

		int i,j;
		if(mFormat==Texture::Format_A_8 && d3dformat==D3DFMT_A8L8){
			for(i=0;i<height;++i){
				for(j=0;j<width;++j){
					*(uint16*)(dst+rect.Pitch*i+j*2)=A8toA8L8(*(uint8*)(src+width*pixelSize*i+j*1));
				}
			}
		}
		else if(mFormat==Texture::Format_RGBA_8 && d3dformat==D3DFMT_A8R8G8B8){
			for(i=0;i<height;++i){
				for(j=0;j<width;++j){
					*(uint32*)(dst+rect.Pitch*i+j*4)=RGBA8toA8R8G8B8(*(uint32*)(src+width*pixelSize*i+j*pixelSize));
				}
			}
		}
		else if(mFormat==Texture::Format_RGB_8 && d3dformat==D3DFMT_X8R8G8B8){
			for(i=0;i<height;++i){
				for(j=0;j<width;++j){
					*(uint32*)(dst+rect.Pitch*i+j*4)=RGB8toX8R8G8B8(src+width*pixelSize*i+j*pixelSize);
				}
			}
		}
		else{
			for(i=0;i<height;++i){
				memcpy(dst+rect.Pitch*i,src+width*pixelSize*i,width*pixelSize);
			}
		}

		texture->UnlockRect(0);
	}
	else{
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"D3D9Texture: Volume & Cube loading not yet implemented");
		return;
	}

	// TODO: I dont think this will work if the driver doesnt support autogen mipmaps, so for this & GL, we should probably
	//  replace it with a custom routine of ours
	if(mManuallyGenerateMipLevels){
		mTexture->GenerateMipSubLevels();
	}
}

bool D3D9Texture::read(int format,int width,int height,int depth,uint8 *data){
	if(mDimension==Texture::Dimension_D1 || mDimension==Texture::Dimension_D2){
		IDirect3DTexture9 *texture=(IDirect3DTexture9*)mTexture;

		D3DLOCKED_RECT rect={0};
		HRESULT result=texture->LockRect(0,&rect,NULL,D3DLOCK_READONLY);
		
		// TODO: convert & copy back, inverse of above
		
		texture->UnlockRect(0);

		return true;
	}
	else{
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"D3D9Texture: Volume & Cube reading not yet implemented");
		return false;
	}
}

bool D3D9Texture::isD3DFORMATValid(IDirect3D9 *d3d,D3DFORMAT adapterFormat,D3DFORMAT textureFormat,DWORD usage){
	return SUCCEEDED(d3d->CheckDeviceFormat(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,adapterFormat,usage,D3DRTYPE_TEXTURE,textureFormat));
}

D3DFORMAT D3D9Texture::getD3DFORMAT(int textureFormat){
	D3DFORMAT format=D3DFMT_UNKNOWN;

	if(textureFormat==Texture::Format_L_8){
		format=D3DFMT_L8;
	}
	else if(textureFormat==Texture::Format_A_8){
		format=D3DFMT_A8L8; // Use A8L8 and fill L8 with white
	}
	else if(textureFormat==Texture::Format_LA_8){
		format=D3DFMT_A8L8;
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
