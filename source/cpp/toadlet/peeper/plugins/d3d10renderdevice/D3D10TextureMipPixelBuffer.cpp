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

#include "D3D10TextureMipPixelBuffer.h"
#include "D3D10RenderDevice.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

namespace toadlet{
namespace peeper{

D3D10TextureMipPixelBuffer::D3D10TextureMipPixelBuffer(D3D10Texture *texture,int level,int cubeSide):
	mDevice(NULL),
	mTexture(NULL),
	mD3DTexture(NULL),
	mD3DRenderTargetView(NULL),
	mD3DDepthStencilView(NULL),
	mLevel(0),
	mCubeSide(0),
	//mFormat,
	mDataSize(0)
{
	mDevice=texture->mDevice;
	mTexture=texture;
	mD3DTexture=mTexture->mTexture;
	mLevel=level;
	mCubeSide=cubeSide;

	/// @todo: Unify this with the GLTextureMipPixelBuffer creation
	int l=level;
	int w=texture->getFormat()->getWidth(),h=texture->getFormat()->getHeight(),d=texture->getFormat()->getDepth();
	while(l>0){
		w/=2; h/=2; d/=2;
		l--;
	}

	mFormat=TextureFormat::ptr(new TextureFormat(texture->getFormat()));
	mFormat->setSize(w,h,d);
	mDataSize=mFormat->getDataSize();

	if((mTexture->getUsage()&Texture::Usage_BIT_RENDERTARGET)>0){
		createViews(mTexture->getFormat()->getDimension(),mTexture->getFormat()->getPixelFormat(),level);
	}
}

D3D10TextureMipPixelBuffer::D3D10TextureMipPixelBuffer(D3D10RenderDevice *renderDevice):
	mDevice(NULL),
	mTexture(NULL),
	mD3DTexture(NULL),
	mD3DRenderTargetView(NULL),
	mD3DDepthStencilView(NULL),
	mLevel(0),
	mCubeSide(0),
	//mFormat
	mDataSize(0)
{
	mDevice=renderDevice;
}

D3D10TextureMipPixelBuffer::~D3D10TextureMipPixelBuffer(){
	destroy();
}

bool D3D10TextureMipPixelBuffer::create(int usage,int access,TextureFormat::ptr format){
	if(mTexture!=NULL){
		// We are a D3D10Texture backed PixelBuffer, so we can not be created
		return false;
	}

	mFormat=format;

	mBufferTexture=Texture::ptr(mDevice->createTexture());
	mTexture=shared_static_cast<D3D10Texture>(mBufferTexture);
	mTexture->create(usage|Texture::Usage_BIT_RENDERTARGET,mFormat,NULL);
	mD3DTexture=mTexture->mTexture;
	mLevel=0;
	mCubeSide=0;

	if((mTexture->getUsage()&Usage_BIT_STAGING)==0){
		createViews(TextureFormat::Dimension_D2,mFormat->getPixelFormat(),0);
	}

	return true;
}

void D3D10TextureMipPixelBuffer::destroy(){
	mTexture=NULL;

	BaseResource::destroy();
}

uint8 *D3D10TextureMipPixelBuffer::lock(int lockAccess){
	int subresource=D3D10CalcSubresource(mLevel,0,0);
	D3D10_MAP mapType=D3D10RenderDevice::getD3D10_MAP(lockAccess,mTexture->mUsage);
	UINT mapFlags=0;
	tbyte *data=NULL;

	HRESULT result=S_OK;
 	switch(mTexture->getFormat()->getDimension()){
		case TextureFormat::Dimension_D1:{
			tbyte *mappedTex=NULL;
			ID3D10Texture1D *texture=(ID3D10Texture1D*)mD3DTexture;
			result=texture->Map(subresource,mapType,mapFlags,(void**)&mappedTex);
			if(SUCCEEDED(result)){
				data=mappedTex;
			}
		}break;
		case TextureFormat::Dimension_D2:{
			D3D10_MAPPED_TEXTURE2D mappedTex;
			ID3D10Texture2D *texture=(ID3D10Texture2D*)mD3DTexture;
			result=texture->Map(subresource,mapType,mapFlags,&mappedTex);
			if(SUCCEEDED(result)){
				data=(tbyte*)mappedTex.pData;
			}
		}break;
		case TextureFormat::Dimension_D3:{
			D3D10_MAPPED_TEXTURE3D mappedTex;
			ID3D10Texture3D *texture=(ID3D10Texture3D*)mD3DTexture;
			result=texture->Map(subresource,mapType,mapFlags,&mappedTex);
			if(SUCCEEDED(result)){
				data=(tbyte*)mappedTex.pData;
			}
		}break;
	}
	
	return data;
}

bool D3D10TextureMipPixelBuffer::unlock(){
	int subresource=D3D10CalcSubresource(mLevel,0,0);

	switch(mTexture->getFormat()->getDimension()){
		case TextureFormat::Dimension_D1:{
			ID3D10Texture1D *texture=(ID3D10Texture1D*)mD3DTexture;
			texture->Unmap(subresource);
		}break;
		case TextureFormat::Dimension_D2:{
			ID3D10Texture2D *texture=(ID3D10Texture2D*)mD3DTexture;
			texture->Unmap(subresource);
		}break;
		case TextureFormat::Dimension_D3:{
			ID3D10Texture3D *texture=(ID3D10Texture3D*)mD3DTexture;
			texture->Unmap(subresource);
		}break;
	}
	
	return true;
}

bool D3D10TextureMipPixelBuffer::createViews(int dimension,int pixelFormat,int level){
	HRESULT result=S_OK;
	if((pixelFormat&TextureFormat::Format_MASK_SEMANTICS)!=TextureFormat::Format_SEMANTIC_DEPTH){
		D3D10_RENDER_TARGET_VIEW_DESC desc;
		switch(dimension){
			case TextureFormat::Dimension_D1:
				desc.ViewDimension=D3D10_RTV_DIMENSION_TEXTURE1D;
				desc.Texture1D.MipSlice=level;
			break;
			case TextureFormat::Dimension_D2:
				desc.ViewDimension=D3D10_RTV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice=level;
			break;
			case TextureFormat::Dimension_D3:
				desc.ViewDimension=D3D10_RTV_DIMENSION_TEXTURE3D;
				desc.Texture3D.MipSlice=level;
			break;
		}
		desc.Format=mDevice->getTextureDXGI_FORMAT(pixelFormat);
		result=mDevice->getD3D10Device()->CreateRenderTargetView(mD3DTexture,&desc,&mD3DRenderTargetView);
	}
	else{
		D3D10_DEPTH_STENCIL_VIEW_DESC desc;
		switch(dimension){
			case TextureFormat::Dimension_D1:
				desc.ViewDimension=D3D10_DSV_DIMENSION_TEXTURE1D;
				desc.Texture1D.MipSlice=level;
			break;
			case TextureFormat::Dimension_D2:
				desc.ViewDimension=D3D10_DSV_DIMENSION_TEXTURE2D;
				desc.Texture2D.MipSlice=level;
			break;
		}
		desc.Format=mDevice->getTextureDXGI_FORMAT(pixelFormat);
		result=mDevice->getD3D10Device()->CreateDepthStencilView(mD3DTexture,&desc,&mD3DDepthStencilView);
	}
	if(FAILED(result)){
		Error::unknown(Categories::TOADLET_PEEPER,"unable to create views");
		return false;
	}
	return true;
}

}
}
