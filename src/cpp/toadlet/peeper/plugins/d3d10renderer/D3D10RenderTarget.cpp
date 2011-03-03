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

#include "D3D10RenderTarget.h"
#include "D3D10Renderer.h"

namespace toadlet{
namespace peeper{

D3D10RenderTarget::D3D10RenderTarget():
	mD3DDevice(NULL),
	//mRenderTargetViews
	mDepthStencilView(NULL)
{}

bool D3D10RenderTarget::create(){
	return true;
}

void D3D10RenderTarget::destroy(){
}

void D3D10RenderTarget::clear(int clearFlags,const Color &clearColor){
	#if defined(TOADLET_FIXED_POINT)
		float d3dcolor[4];
		toD3DColor(d3dcolor,clearColor);
	#else
		const float *d3dcolor=clearColor.getData();
	#endif
	UINT d3dclearFlags=0;
	if(clearFlags&Renderer::ClearFlag_DEPTH){
		d3dclearFlags|=D3D10_CLEAR_DEPTH;
	}
	if(clearFlags&Renderer::ClearFlag_STENCIL){
		d3dclearFlags|=D3D10_CLEAR_STENCIL;
	}

	int i;
	for(i=0;i<mRenderTargetViews.size();++i){
		mD3DDevice->ClearRenderTargetView(mRenderTargetViews[i],d3dcolor);
	}
	if(mDepthStencilView!=NULL){
		mD3DDevice->ClearDepthStencilView(mDepthStencilView,d3dclearFlags,1.0f,0);
	}
}

bool D3D10RenderTarget::activate(){
	mD3DDevice->OMSetRenderTargets(mRenderTargetViews.size(),mRenderTargetViews.begin(),mDepthStencilView);
	return true;
}

bool D3D10RenderTarget::deactivate(){
	ID3D10RenderTargetView *view=NULL;
	mD3DDevice->OMSetRenderTargets(1,&view,NULL);
	return true;
}

}
}
