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

bool D3D10Texture::create(int usage,TextureFormat::ptr format,byte *mipDatas[]){
	mUsage=usage;
	mFormat=format;

	// Perhaps this should be moved to TextureManager, but it allows us to make use of the D3D10 AutoGen Mipmaps functionality
	if((mUsage&(Usage_BIT_STATIC|Usage_BIT_AUTOGEN_MIPMAPS))==(Usage_BIT_STATIC|Usage_BIT_AUTOGEN_MIPMAPS)){
		mUsage=Usage_BIT_STREAM|Usage_BIT_AUTOGEN_MIPMAPS|Usage_BIT_RENDERTARGET;
	}

	bool result=false;
	if((mUsage&Usage_BIT_STATIC)>0){
		result=createContext(mFormat->getMipMax(),mipDatas);
	}
	else{
		result=createContext(0,NULL);

		if(result && mipDatas!=NULL){
			int level;
			for(level=0;level<1 || level<format->getMipMax();++level){
				TextureFormat::ptr format(new TextureFormat(mFormat,level));
				load(format,mipDatas[level]);
			}
		}
	}

	return result;
}

void D3D10Texture::destroy(){
	destroyContext();

	mBuffers.clear();

	BaseResource::destroy();
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
		bindFlags|=(D3D10_BIND_RENDER_TARGET|D3D10_BIND_SHADER_RESOURCE);
	}

	if((mUsage&Usage_BIT_RENDERTARGET)>0){
		if((mFormat->getPixelFormat()&TextureFormat::Format_MASK_SEMANTICS)==TextureFormat::Format_SEMANTIC_DEPTH){
			bindFlags|=D3D10_BIND_DEPTH_STENCIL;
		}
		else{
			bindFlags|=D3D10_BIND_RENDER_TARGET;
		}
	}

	/// @todo: Why can't I have a depth & shader resource?
	if((mFormat->getPixelFormat()&TextureFormat::Format_MASK_SEMANTICS)!=TextureFormat::Format_SEMANTIC_DEPTH){
		bindFlags|=D3D10_BIND_SHADER_RESOURCE;
	}

	if((mUsage&Usage_BIT_STAGING)>0){
		bindFlags=0;
	}

	DXGI_FORMAT dxgiFormat=D3D10RenderDevice::getTextureDXGI_FORMAT(mFormat->getPixelFormat());
	int width=mFormat->getWidth(),height=mFormat->getHeight(),depth=mFormat->getDepth();
	int mipMax=mFormat->getMipMax();

	D3D10_SUBRESOURCE_DATA *sData=NULL;
	if(mipDatas!=NULL){
		int numSubResources=numMipDatas>0 ? numMipDatas:1;
		sData=new D3D10_SUBRESOURCE_DATA[numSubResources];
		int i;
		for(i=0;i<numSubResources;++i){
			TextureFormat::ptr format(new TextureFormat(mFormat,i));
			sData[i].pSysMem=(i==0 || i<numMipDatas) ? mipDatas[i]:NULL;
			sData[i].SysMemPitch=format->getXPitch();
			sData[i].SysMemSlicePitch=format->getYPitch();
		}
	}

	HRESULT result=E_FAIL;
	switch(mFormat->getDimension()){
		case TextureFormat::Dimension_D1:{
			D3D10_TEXTURE1D_DESC desc={0};
			desc.Width=width;
			desc.MipLevels=mipMax;
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
			desc.MipLevels=mipMax;
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
			desc.MipLevels=mipMax;
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
	if(mFormat->getDimension()==TextureFormat::Dimension_CUBE){
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

bool D3D10Texture::load(TextureFormat *format,tbyte *data){
	if(format->getPixelFormat()!=mFormat->getPixelFormat()){
		Error::unknown(Categories::TOADLET_PEEPER,
			"data of incorrect format");
		return false;
	}

	int subresource=D3D10CalcSubresource(format->getMipMin(),0,0);

	if((mUsage&Usage_BIT_STAGING)==0){
		D3D10_BOX box;
		setD3D10_BOX(box,format);
		mD3DDevice->UpdateSubresource(mTexture,subresource,&box,data,format->getXPitch(),format->getYPitch());
	}
	else{
		D3D10_MAP mapType=D3D10_MAP_WRITE;
		UINT mapFlags=0;

		int xPitch=format->getXPitch(),yPitch=format->getYPitch();
		tbyte *mapData=NULL;
		int mapXPitch=xPitch,mapYPitch=yPitch;
		HRESULT result=S_OK;
		switch(mFormat->getDimension()){
			case TextureFormat::Dimension_D1:{
				ID3D10Texture1D *texture=(ID3D10Texture1D*)mTexture;
				result=texture->Map(subresource,mapType,mapFlags,(void**)&mapData);
			}break;
			case TextureFormat::Dimension_D2:{
				D3D10_MAPPED_TEXTURE2D mappedTex;
				ID3D10Texture2D *texture=(ID3D10Texture2D*)mTexture;
				result=texture->Map(subresource,mapType,mapFlags,&mappedTex);
				mapData=(tbyte*)mappedTex.pData;
				mapXPitch=mappedTex.RowPitch;
			}break;
			case TextureFormat::Dimension_D3:{
				D3D10_MAPPED_TEXTURE3D mappedTex;
				ID3D10Texture3D *texture=(ID3D10Texture3D*)mTexture;
				result=texture->Map(subresource,mapType,mapFlags,&mappedTex);
				mapData=(tbyte*)mappedTex.pData;
				mapXPitch=mappedTex.RowPitch;
				mapYPitch=mappedTex.DepthPitch;
			}break;
		}

		if(SUCCEEDED(result)){
			int i,j;
			for(j=0;j<format->getDepth();++j){
				for(i=0;i<format->getHeight();++i){
					memcpy(mapData+mapYPitch*j+mapXPitch*i,data+yPitch*j+xPitch*i,xPitch);
				}
			}

			switch(mFormat->getDimension()){
				case TextureFormat::Dimension_D1:{
					ID3D10Texture1D *texture=(ID3D10Texture1D*)mTexture;
					texture->Unmap(subresource);
				}break;
				case TextureFormat::Dimension_D2:{
					ID3D10Texture2D *texture=(ID3D10Texture2D*)mTexture;
					texture->Unmap(subresource);
				}break;
				case TextureFormat::Dimension_D3:{
					ID3D10Texture3D *texture=(ID3D10Texture3D*)mTexture;
					texture->Unmap(subresource);
				}break;
			}
		}

		if(FAILED(result)){
			Error::unknown(Categories::TOADLET_PEEPER,
				"failed to load texture");
			return false;
		}
	}

	generateMipLevels();

	return true;
}

bool D3D10Texture::read(TextureFormat *format,tbyte *data){
	if(format->getPixelFormat()!=mFormat->getPixelFormat()){
		Error::unknown(Categories::TOADLET_PEEPER,
			"data of incorrect format");
		return false;
	}

	TOADLET_ASSERT(format->getMipMin()==format->getMipMax());

	int subresource=D3D10CalcSubresource(format->getMipMin(),0,0);

	if((mUsage&Usage_BIT_STAGING)>0){
		D3D10_MAP mapType=D3D10_MAP_READ;
		UINT mapFlags=0;

		int xPitch=format->getXPitch(),yPitch=format->getYPitch();
		tbyte *mapData=NULL;
		int mapXPitch=xPitch,mapYPitch=yPitch;
		HRESULT result=S_OK;
		switch(mFormat->getDimension()){
			case TextureFormat::Dimension_D1:{
				ID3D10Texture1D *texture=(ID3D10Texture1D*)mTexture;
				result=texture->Map(subresource,mapType,mapFlags,(void**)&mapData);
			}break;
			case TextureFormat::Dimension_D2:{
				D3D10_MAPPED_TEXTURE2D mappedTex;
				ID3D10Texture2D *texture=(ID3D10Texture2D*)mTexture;
				result=texture->Map(subresource,mapType,mapFlags,&mappedTex);
				mapData=(tbyte*)mappedTex.pData;
				mapXPitch=mappedTex.RowPitch;
			}break;
			case TextureFormat::Dimension_D3:{
				D3D10_MAPPED_TEXTURE3D mappedTex;
				ID3D10Texture3D *texture=(ID3D10Texture3D*)mTexture;
				result=texture->Map(subresource,mapType,mapFlags,&mappedTex);
				mapData=(tbyte*)mappedTex.pData;
				mapXPitch=mappedTex.RowPitch;
				mapYPitch=mappedTex.DepthPitch;
			}break;
		}

		if(SUCCEEDED(result)){
			int i,j;
			for(j=0;j<format->getDepth();++j){
				for(i=0;i<format->getHeight();++i){
					memcpy(data+yPitch*j+xPitch*i,mapData+mapYPitch*j+mapXPitch*i,xPitch);
				}
			}

			switch(mFormat->getDimension()){
				case TextureFormat::Dimension_D1:{
					ID3D10Texture1D *texture=(ID3D10Texture1D*)mTexture;
					texture->Unmap(subresource);
				}break;
				case TextureFormat::Dimension_D2:{
					ID3D10Texture2D *texture=(ID3D10Texture2D*)mTexture;
					texture->Unmap(subresource);
				}break;
				case TextureFormat::Dimension_D3:{
					ID3D10Texture3D *texture=(ID3D10Texture3D*)mTexture;
					texture->Unmap(subresource);
				}break;
			}
		}

		if(FAILED(result)){
			Error::unknown(Categories::TOADLET_PEEPER,
				"failed to read texture");
			return false;
		}
	}

	return true;
}

bool D3D10Texture::generateMipLevels(){
	if((mUsage&Usage_BIT_AUTOGEN_MIPMAPS)==Usage_BIT_AUTOGEN_MIPMAPS){
		mD3DDevice->GenerateMips(mShaderResourceView);
	}
	return true;
}

void D3D10Texture::setD3D10_BOX(D3D10_BOX &box,TextureFormat *format){
	box.left=format->getXMin();box.right=format->getXMax();
	box.top=format->getYMin();box.bottom=format->getYMax();
	box.front=format->getZMin();box.back=format->getZMax();
}

}
}
