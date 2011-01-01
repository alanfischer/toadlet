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
#include "D3D10PixelBuffer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

D3D10Texture::D3D10Texture(D3D10Renderer *renderer):BaseResource(),
	mRenderer(NULL),

	mUsage(0),
	mDimension(Dimension_UNKNOWN),
	mFormat(0),
	mWidth(0),
	mHeight(0),
	mDepth(0),
	mMipLevels(0),

	mTexture(NULL),
	mShaderResourceView(NULL)
{
	mRenderer=renderer;
}

D3D10Texture::~D3D10Texture(){
	if(mTexture!=NULL){
		destroy();
	}
}

bool D3D10Texture::create(int usage,Dimension dimension,int format,int width,int height,int depth,int mipLevels,byte *mipDatas[]){
	destroy();

	mUsage=usage;
	mDimension=dimension;
	mFormat=format;
	mWidth=width;
	mHeight=height;
	mDepth=depth;
	mMipLevels=mipLevels;

	if((mUsage&Usage_BIT_STATIC)>0){
		if(mMipLevels!=0){
			createContext(mipLevels,mipDatas);
		}
		else{
			mUsage&=~Usage_BIT_STATIC;
			mUsage|=Usage_BIT_DYNAMIC;
		}
	}
	
	if((mUsage&Usage_BIT_STATIC)==0){
		createContext(0,NULL);

		int specifiedMipLevels=mMipLevels>0?mMipLevels:1;
		if(mipDatas!=NULL){
			int level;
			for(level=0;level<specifiedMipLevels;++level){
				load(width,height,depth,level,mipDatas[level]);
				width/=2;height/=2;depth/=2;
			}
		}
	}

	return true;
}

void D3D10Texture::destroy(){
	destroyContext();
}

bool D3D10Texture::createContext(int mipLevels,byte *mipDatas[]){
	ID3D10Device *device=mRenderer->getD3D10Device();

	D3D10_USAGE d3dUsage=D3D10_USAGE_DEFAULT;
	if((mUsage&Usage_BIT_STATIC)>0){
		d3dUsage=D3D10_USAGE_IMMUTABLE;
	}
	else if((mUsage&Usage_BIT_STREAM)>0){
		d3dUsage=D3D10_USAGE_DYNAMIC;
	}
	else if((mUsage&Usage_BIT_STAGING)>0){
		d3dUsage=D3D10_USAGE_STAGING;
	}

	int cpuFlags=0;
//  Perhaps we need to add a flag to specify we want to read from a texture
//	if((mUsage&Usage_BIT_STATIC)==0){
//		if((mUsage&(Usage_BIT_DYNAMIC|Usage_BIT_STREAM))>0){
//			cpuFlags|=D3D10_CPU_ACCESS_WRITE;
//		}
//	}

	int miscFlags=0;
	int bindFlags=D3D10_BIND_SHADER_RESOURCE;
	if((mUsage&(Usage_BIT_AUTOGEN_MIPMAPS|Usage_BIT_RENDERTARGET))==(Usage_BIT_AUTOGEN_MIPMAPS|Usage_BIT_RENDERTARGET)){
		miscFlags|=D3D10_RESOURCE_MISC_GENERATE_MIPS;
		bindFlags|=D3D10_BIND_RENDER_TARGET;
	}

	DXGI_FORMAT dxgiFormat=D3D10Renderer::getTextureDXGI_FORMAT(mFormat);

	D3D10_SUBRESOURCE_DATA *sData=NULL;
	if(mipDatas!=NULL){
		int numMipDatas=mipLevels>0?mipLevels:1;
		int hwidth=mWidth,hheight=mHeight,hdepth=mDepth;
		sData=new D3D10_SUBRESOURCE_DATA[numMipDatas];
		int i;
		for(i=0;i<numMipDatas;++i){
			sData[i].pSysMem=mipDatas[i];
			sData[i].SysMemPitch=ImageFormatConversion::getPixelSize(mFormat)*hwidth;
			sData[i].SysMemSlicePitch=sData[i].SysMemPitch*hheight;

			hwidth/=2;hheight/=2;hdepth/=2;
			hwidth=hwidth>0?hwidth:1;hheight=hheight>0?hheight:1;hdepth=hdepth>0?hdepth:1;
		}
	}

	HRESULT result=E_FAIL;
	switch(mDimension){
		case Texture::Dimension_D1:{
			D3D10_TEXTURE1D_DESC desc={0};
			desc.Width=mWidth;
			desc.MipLevels=mMipLevels;
			desc.Usage=d3dUsage;
			desc.CPUAccessFlags=cpuFlags;
			desc.BindFlags=bindFlags;
			desc.MiscFlags=miscFlags;
			desc.Format=dxgiFormat;
			desc.ArraySize=1;

			ID3D10Texture1D *texture=NULL;
			result=device->CreateTexture1D(&desc,sData,&texture);
			TOADLET_CHECK_D3D10ERROR(result,"CreateTexture1D");
			mTexture=texture;
		}break;
		case Texture::Dimension_D2:{
			D3D10_TEXTURE2D_DESC desc={0};
			desc.Width=mWidth;
			desc.Height=mHeight;
			desc.MipLevels=mMipLevels;
			desc.SampleDesc.Count=1;
			desc.SampleDesc.Quality=0;
			desc.Usage=d3dUsage;
			desc.CPUAccessFlags=cpuFlags;
			desc.BindFlags=bindFlags;
			desc.MiscFlags=miscFlags;
			desc.Format=dxgiFormat;
			desc.ArraySize=1;

			ID3D10Texture2D *texture=NULL;
			result=device->CreateTexture2D(&desc,sData,&texture);
			TOADLET_CHECK_D3D10ERROR(result,"CreateTexture2D");
			mTexture=texture;
		}break;
		case Texture::Dimension_D3:{
			D3D10_TEXTURE3D_DESC desc={0};
			desc.Width=mWidth;
			desc.Height=mHeight;
			desc.Depth=mDepth;
			desc.MipLevels=mMipLevels;
			desc.Usage=d3dUsage;
			desc.CPUAccessFlags=cpuFlags;
			desc.BindFlags=bindFlags;
			desc.MiscFlags=miscFlags;
			desc.Format=dxgiFormat;

			ID3D10Texture3D *texture=NULL;
			result=device->CreateTexture3D(&desc,sData,&texture);
			TOADLET_CHECK_D3D10ERROR(result,"CreateTexture3D");
			mTexture=texture;
		}break;
	}

	if(SUCCEEDED(result)){
		result=device->CreateShaderResourceView(mTexture,NULL,&mShaderResourceView);
	}

	if(sData!=NULL){
		delete sData;
	}

	return SUCCEEDED(result);
}

bool D3D10Texture::destroyContext(){
	HRESULT result=S_OK;
	if(mShaderResourceView!=NULL){
		result=mShaderResourceView->Release();
		mShaderResourceView=NULL;
	}

	if(mTexture!=NULL){
		result=mTexture->Release();
		mTexture=NULL;
	}

	return SUCCEEDED(result);
}

PixelBuffer::ptr D3D10Texture::getMipPixelBuffer(int level,int cubeSide){
	/// @todo: Check our cached list of SubTextures, and if the requested one isn't available, create it, and then return it.
	return NULL;
}

bool D3D10Texture::load(int width,int height,int depth,int mipLevel,byte *mipData){
	if(mTexture==NULL){
		return false;
	}

	width=width>0?width:1;height=height>0?height:1;depth=depth>0?depth:1;

	if(mipLevel==0 && (width!=mWidth || height!=mHeight || depth!=mDepth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D10Texture: data of incorrect dimensions");
		return false;
	}

	ID3D10Device *device=mRenderer->getD3D10Device();

	int format=mFormat;
	int rowPitch=ImageFormatConversion::getPixelSize(format)*width;
	int slicePitch=rowPitch*height;
	int subresource=D3D10CalcSubresource(mipLevel,0,0);
	device->UpdateSubresource(mTexture,subresource,NULL,mipData,rowPitch,slicePitch);

	if((mUsage&(Usage_BIT_RENDERTARGET|Usage_BIT_AUTOGEN_MIPMAPS))==(Usage_BIT_RENDERTARGET|Usage_BIT_AUTOGEN_MIPMAPS)){
		device->GenerateMips(mShaderResourceView);
	}

	return true;
}

bool D3D10Texture::read(int width,int height,int depth,int mipLevel,byte *mipData){
	/// @todo: Lock the SubTexture and copy the data from there
	return false;
}

}
}
