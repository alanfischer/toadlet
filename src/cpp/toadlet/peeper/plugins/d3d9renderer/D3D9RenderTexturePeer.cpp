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
/*
#include "D3D9Renderer.h"
#include "D3D9RenderTexturePeer.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

D3D9RenderTexturePeer::D3D9RenderTexturePeer(D3D9Renderer *renderer,RenderTexture *renderTexture):D3D9RenderTargetPeer(),D3D9TexturePeer(renderer,renderTexture),
	colorSurface(NULL)
	,depthSurface(NULL)
{
	const CapabilitySet &capabilitySet=renderer->getCapabilitySet();
	IDirect3DDevice9 *device=renderer->getDirect3DDevice9();

	int width=renderTexture->getWidth();
	int height=renderTexture->getHeight();

	if((renderTexture->getFormat()&Texture::Format_BIT_DEPTH)>0){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3D9TexturePeer: Render to depth texture not currently supported");
		return;
	}

	HRESULT result=E_FAIL;

	UINT levels=renderTexture->getAutoGenerateMipMaps()?0:1;
	DWORD usage=D3DUSAGE_RENDERTARGET;
	D3DFORMAT format=D3DFMT_R5G6B5;
	D3DPOOL pool=D3DPOOL_DEFAULT;

	IDirect3DTexture9 *texture=NULL;
	result=device->CreateTexture(width,height,levels,usage,format,pool,&texture,NULL);
	baseTexture=texture;
	if(FAILED(result)){
		TOADLET_CHECK_D3D9ERROR(result,"CreateTexture");
		return;
	}
	
	result=texture->GetSurfaceLevel(0,&colorSurface);
	if(FAILED(result)){
		TOADLET_CHECK_D3D9ERROR(result,"GetSurfaceLevel");
		return;
	}

	// Build depth surface
	{
		IDirect3DSurface9 *deviceDepthSurface=NULL;
		result=device->GetDepthStencilSurface(&deviceDepthSurface);
		if(FAILED(result)){
			TOADLET_CHECK_D3D9ERROR(result,"GetDepthStencilSurface");
			return;
		}

		D3DSURFACE_DESC desc;
		result=deviceDepthSurface->GetDesc(&desc);
		deviceDepthSurface->Release();
		if(FAILED(result)){
			TOADLET_CHECK_D3D9ERROR(result,"GetDesc");
			return;
		}

		result=device->CreateDepthStencilSurface(width,height,desc.Format,desc.MultiSampleType,NULL,FALSE,&depthSurface,NULL);
		if(FAILED(result)){
			TOADLET_CHECK_D3D9ERROR(result,"CreateDepthStencilSurface");
			return;
		}
	}
}

D3D9RenderTexturePeer::~D3D9RenderTexturePeer(){
	if(colorSurface!=NULL){
		colorSurface->Release();
		colorSurface=NULL;
	}

	if(depthSurface!=NULL){
		depthSurface->Release();
		depthSurface=NULL;
	}
}

int D3D9RenderTexturePeer::getWidth() const{
	if(colorSurface!=NULL){
		D3DSURFACE_DESC desc;
		colorSurface->GetDesc(&desc);
		return desc.Width;
	}
	else{
		return 0;
	}
}

int D3D9RenderTexturePeer::getHeight() const{
	if(colorSurface!=NULL){
		D3DSURFACE_DESC desc;
		colorSurface->GetDesc(&desc);
		return desc.Height;
	}
	else{
		return 0;
	}
}

bool D3D9RenderTexturePeer::isValid() const{
	return colorSurface!=NULL || depthSurface!=NULL;
}

void D3D9RenderTexturePeer::makeCurrent(IDirect3DDevice9 *device){
	HRESULT result=device->SetRenderTarget(0,colorSurface);
	TOADLET_CHECK_D3D9ERROR(result,"Error in SetRenderTarget");

	result=device->SetDepthStencilSurface(depthSurface);
	TOADLET_CHECK_D3D9ERROR(result,"Error in SetDepthStencilSurface");
}

}
}
*/
