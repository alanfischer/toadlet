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

#include "D3DMRenderer.h"
#include "D3DMRenderTexturePeer.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

D3DMRenderTexturePeer::D3DMRenderTexturePeer(D3DMRenderer *renderer,RenderTexture *renderTexture):D3DMRenderTargetPeer(),D3DMTexturePeer(renderer,renderTexture),
	colorSurface(NULL)
	,depthSurface(NULL)
{
	const CapabilitySet &capabilitySet=renderer->getCapabilitySet();
	IDirect3DMobileDevice *device=renderer->getDirect3DMobileDevice();

	int width=renderTexture->getWidth();
	int height=renderTexture->getHeight();

	if((renderTexture->getFormat()&Image::Format_BIT_DEPTH)>0){
		Error::unknown(Categories::TOADLET_PEEPER,
			"D3DMTexturePeer: Render to depth texture not currently supported");
		return;
	}

	HRESULT result=E_FAIL;

	UINT levels=renderTexture->getAutoGenerateMipMaps()?0:1;
	DWORD usage=D3DMUSAGE_RENDERTARGET;
	D3DMFORMAT format=D3DMFMT_R5G6B5;
	D3DMPOOL pool=D3DMPOOL_MANAGED;

	IDirect3DMobileTexture *texture=NULL;
	result=device->CreateTexture(width,height,levels,usage,format,pool,&texture);
	baseTexture=texture;
	if(FAILED(result)){
		TOADLET_CHECK_D3DMERROR(result,"CreateTexture");
		return;
	}
	
	result=texture->GetSurfaceLevel(0,&colorSurface);
	if(FAILED(result)){
		TOADLET_CHECK_D3DMERROR(result,"GetSurfaceLevel");
		return;
	}

	// Build depth surface
	{
		IDirect3DMobileSurface *deviceDepthSurface=NULL;
		result=device->GetDepthStencilSurface(&deviceDepthSurface);
		if(FAILED(result)){
			TOADLET_CHECK_D3DMERROR(result,"GetDepthStencilSurface");
			return;
		}

		D3DMSURFACE_DESC desc;
		result=deviceDepthSurface->GetDesc(&desc);
		deviceDepthSurface->Release();
		if(FAILED(result)){
			TOADLET_CHECK_D3DMERROR(result,"GetDesc");
			return;
		}

		result=device->CreateDepthStencilSurface(width,height,desc.Format,desc.MultiSampleType,&depthSurface);
		if(FAILED(result)){
			TOADLET_CHECK_D3DMERROR(result,"CreateDepthStencilSurface");
			return;
		}
	}
}

D3DMRenderTexturePeer::~D3DMRenderTexturePeer(){
	if(colorSurface!=NULL){
		colorSurface->Release();
		colorSurface=NULL;
	}

	if(depthSurface!=NULL){
		depthSurface->Release();
		depthSurface=NULL;
	}
}

int D3DMRenderTexturePeer::getWidth() const{
	if(colorSurface!=NULL){
		D3DMSURFACE_DESC desc;
		colorSurface->GetDesc(&desc);
		return desc.Width;
	}
	else{
		return 0;
	}
}

int D3DMRenderTexturePeer::getHeight() const{
	if(colorSurface!=NULL){
		D3DMSURFACE_DESC desc;
		colorSurface->GetDesc(&desc);
		return desc.Height;
	}
	else{
		return 0;
	}
}

bool D3DMRenderTexturePeer::isValid() const{
	return colorSurface!=NULL || depthSurface!=NULL;
}

void D3DMRenderTexturePeer::makeCurrent(IDirect3DMobileDevice *device){
	HRESULT result=device->SetRenderTarget(colorSurface,depthSurface);
	TOADLET_CHECK_D3DMERROR(result,"SetRenderTarget");
}

}
}

