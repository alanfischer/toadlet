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

#include "D3D10RenderDevice.h"
#include "D3D10Texture.h"
#include "D3D10TextureMipPixelBuffer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/image/ImageFormatConversion.h>

namespace toadlet{
namespace peeper{

D3D10Texture::D3D10Texture(ID3D10Device *device):BaseResource(),
	mDevice(NULL),
	mD3DDevice(NULL),

	mUsage(0),
	//mFormat,

	mTexture(NULL),
	mShaderResourceView(NULL)
	//mBuffers
{
	mD3DDevice=device;
}

D3D10Texture::D3D10Texture(D3D10RenderDevice *renderDevice):BaseResource(),
	mDevice(NULL),
	mD3DDevice(NULL),

	mUsage(0),
	//mFormat,

	mTexture(NULL),
	mShaderResourceView(NULL)
	//mBuffers
{
	mDevice=renderDevice;
	mD3DDevice=mDevice->getD3D10Device();
}

D3D10Texture::~D3D10Texture(){
	if(mTexture!=NULL){
		destroy();
	}
}

bool D3D10Texture::create(int usage,TextureFormat::ptr format,byte *mipDatas[]){
	format->width=format->width>0?format->width:1;format->height=format->height>0?format->height:1;format->depth=format->depth>0?format->depth:1;

	mUsage=usage;
	mFormat=format;

	// Perhaps this should be moved to TextureManager, but it allows us to make use of the D3D10 AutoGen Mipmaps functionality
	if((mUsage&(Usage_BIT_STATIC|Usage_BIT_AUTOGEN_MIPMAPS))==(Usage_BIT_STATIC|Usage_BIT_AUTOGEN_MIPMAPS)){
		mUsage=Usage_BIT_STREAM|Usage_BIT_AUTOGEN_MIPMAPS|Usage_BIT_RENDERTARGET;
	}

	bool result=false;
	if((mUsage&Usage_BIT_STATIC)>0){
		result=createContext(mFormat->mipLevels,mipDatas);
	}
	else{
		result=createContext(0,NULL);

		if(result && mipDatas!=NULL){
			int width=mFormat->width,height=mFormat->height,depth=mFormat->depth;
			int specifiedMipLevels=mFormat->mipLevels>0?mFormat->mipLevels:1;
			int level;
			for(level=0;level<specifiedMipLevels;++level){
				if(mipDatas[level]!=NULL){
					load(width,height,depth,level,mipDatas[level]);
				}
				width/=2;height/=2;depth/=2;
			}
		}
	}

	return result;
}

void D3D10Texture::destroy(){
	destroyContext();

	mBuffers.clear();
}

bool D3D10Texture::createContext(int numMipDatas,byte *mipDatas[]){
	ID3D10Device *device=mD3DDevice;

	D3D10_USAGE d3dUsage=D3D10RenderDevice::getD3D10_USAGE(mUsage);

	int cpuFlags=0;
	if((mUsage&Usage_BIT_STAGING)>0){
		cpuFlags|=D3D10_CPU_ACCESS_READ|D3D10_CPU_ACCESS_WRITE;
	}

	int miscFlags=0;
	int bindFlags=0;

	if((mUsage&Usage_BIT_AUTOGEN_MIPMAPS|Usage_BIT_RENDERTARGET)==(Usage_BIT_AUTOGEN_MIPMAPS|Usage_BIT_RENDERTARGET)){
		miscFlags|=D3D10_RESOURCE_MISC_GENERATE_MIPS;
	}

	if((mUsage&Usage_BIT_RENDERTARGET)>0){
		if((mFormat->pixelFormat&TextureFormat::Format_MASK_SEMANTICS)==TextureFormat::Format_SEMANTIC_DEPTH){
			bindFlags|=D3D10_BIND_DEPTH_STENCIL;
		}
		else{
			bindFlags|=D3D10_BIND_RENDER_TARGET;
		}
	}

	/// @todo: Why can't I have a depth & shader resource?
	if((mFormat->pixelFormat&TextureFormat::Format_MASK_SEMANTICS)!=TextureFormat::Format_SEMANTIC_DEPTH){
		bindFlags|=D3D10_BIND_SHADER_RESOURCE;
	}

	if((mUsage&Usage_BIT_STAGING)>0){
		bindFlags=0;
	}

	DXGI_FORMAT dxgiFormat=D3D10RenderDevice::getTextureDXGI_FORMAT(mFormat->pixelFormat);
	int width=mFormat->width,height=mFormat->height,depth=mFormat->depth;
	int mipLevels=mFormat->mipLevels;

	D3D10_SUBRESOURCE_DATA *sData=NULL;
	if(mipDatas!=NULL){
		int hwidth=width,hheight=height,hdepth=depth;
		int numSubResources=numMipDatas>0 ? numMipDatas:1;
		sData=new D3D10_SUBRESOURCE_DATA[numSubResources];
		int i;
		for(i=0;i<numSubResources;++i){
			sData[i].pSysMem=(i==0 || i<numMipDatas) ? mipDatas[i]:NULL;
			sData[i].SysMemPitch=ImageFormatConversion::getRowPitch(mFormat->pixelFormat,hwidth);
			sData[i].SysMemSlicePitch=sData[i].SysMemPitch*hheight;

			hwidth/=2;hheight/=2;hdepth/=2;
			hwidth=hwidth>0?hwidth:1;hheight=hheight>0?hheight:1;hdepth=hdepth>0?hdepth:1;
		}
	}

	HRESULT result=E_FAIL;
	switch(mFormat->dimension){
		case TextureFormat::Dimension_D1:{
			D3D10_TEXTURE1D_DESC desc={0};
			desc.Width=width;
			desc.MipLevels=mipLevels;
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
		case TextureFormat::Dimension_D2:{
			D3D10_TEXTURE2D_DESC desc={0};
			desc.Width=width;
			desc.Height=height;
			desc.MipLevels=mipLevels;
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
		case TextureFormat::Dimension_D3:{
			D3D10_TEXTURE3D_DESC desc={0};
			desc.Width=width;
			desc.Height=height;
			desc.Depth=depth;
			desc.MipLevels=mipLevels;
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

	if(SUCCEEDED(result) && (bindFlags&D3D10_BIND_SHADER_RESOURCE)!=0){
		result=device->CreateShaderResourceView(mTexture,NULL,&mShaderResourceView);
	}

	if(sData!=NULL){
		delete sData;
	}

	if(FAILED(result)){
		Error::unknown("error in createContext");
		return false;
	}

	return true;
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

	int i;
	for(i=0;i<mBuffers.size();++i){
		mBuffers[i]->destroy();
	}

	return SUCCEEDED(result);
}

PixelBuffer::ptr D3D10Texture::getMipPixelBuffer(int level,int cubeSide){
	if(mTexture==NULL){
		return NULL;
	}

	int index=level;
	if(mFormat->dimension==TextureFormat::Dimension_CUBE){
		index=level*6+cubeSide;
	}

	if(mBuffers.size()<=index){
		mBuffers.resize(index+1);
	}

	if(mBuffers[index]==NULL){
		PixelBuffer::ptr buffer(new D3D10TextureMipPixelBuffer(this,level,cubeSide));
		mBuffers[index]=buffer;
	}

	return mBuffers[index];
}

bool D3D10Texture::load(int width,int height,int depth,int mipLevel,byte *mipData){
	if(mTexture==NULL){
		return false;
	}

	width=width>0?width:1;height=height>0?height:1;depth=depth>0?depth:1;

	if(mipLevel==0 && (width!=mFormat->width || height!=mFormat->height || depth!=mFormat->depth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D10Texture: data of incorrect dimensions");
		return false;
	}

	ID3D10Device *device=mD3DDevice;

	int pixelFormat=mFormat->pixelFormat;
	int rowPitch=ImageFormatConversion::getRowPitch(pixelFormat,width);
	int slicePitch=rowPitch*height;
	int subresource=D3D10CalcSubresource(mipLevel,0,0);
	device->UpdateSubresource(mTexture,subresource,NULL,mipData,rowPitch,slicePitch);

	generateMipLevels();

	return true;
}

bool D3D10Texture::read(int width,int height,int depth,int mipLevel,byte *mipData){
	int pixelFormat=mFormat->pixelFormat;
	int rowPitch=ImageFormatConversion::getRowPitch(pixelFormat,width);
	int slicePitch=rowPitch*height;

	int subresource=D3D10CalcSubresource(mipLevel,0,0);
	D3D10_MAP mapType=D3D10_MAP_READ;
	UINT mapFlags=0;

	HRESULT result=S_OK;
	switch(mFormat->dimension){
		case TextureFormat::Dimension_D1:{
			tbyte *mappedTex=NULL;
			ID3D10Texture1D *texture=(ID3D10Texture1D*)mTexture;
			result=texture->Map(subresource,mapType,mapFlags,(void**)&mappedTex);
			if(SUCCEEDED(result)){
				ImageFormatConversion::convert(mappedTex,mFormat->pixelFormat,rowPitch,slicePitch,mipData,pixelFormat,rowPitch,slicePitch,width,height,depth);
				texture->Unmap(subresource);
			}
		}break;
		case TextureFormat::Dimension_D2:{
			D3D10_MAPPED_TEXTURE2D mappedTex;
			ID3D10Texture2D *texture=(ID3D10Texture2D*)mTexture;
			result=texture->Map(subresource,mapType,mapFlags,&mappedTex);
			if(SUCCEEDED(result)){
				ImageFormatConversion::convert((tbyte*)mappedTex.pData,mFormat->pixelFormat,mappedTex.RowPitch,slicePitch,mipData,pixelFormat,rowPitch,slicePitch,width,height,depth);
				texture->Unmap(subresource);
			}
		}break;
		case TextureFormat::Dimension_D3:{
			D3D10_MAPPED_TEXTURE3D mappedTex;
			ID3D10Texture3D *texture=(ID3D10Texture3D*)mTexture;
			result=texture->Map(subresource,mapType,mapFlags,&mappedTex);
			if(SUCCEEDED(result)){
				ImageFormatConversion::convert((tbyte*)mappedTex.pData,mFormat->pixelFormat,mappedTex.RowPitch,mappedTex.DepthPitch,mipData,pixelFormat,rowPitch,slicePitch,width,height,depth);
				texture->Unmap(subresource);
			}
		}break;
	}

	if(FAILED(result)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"failed to read texture");
		return false;
	}

	return true;
}

bool D3D10Texture::generateMipLevels(){
	if((mUsage&Usage_BIT_AUTOGEN_MIPMAPS)==Usage_BIT_AUTOGEN_MIPMAPS){
		mD3DDevice->GenerateMips(mShaderResourceView);
	}
	return true;
}

}
}
