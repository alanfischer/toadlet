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

#include "D3D10RenderState.h"
#include "D3D10RenderDevice.h"

namespace toadlet{
namespace peeper{

D3D10RenderState::D3D10RenderState(D3D10RenderDevice *renderDevice):
	mListener(NULL),
	mBlendState(NULL),
	mD3DBlendState(NULL),
	mDepthState(NULL),
	mD3DDepthStencilState(NULL),
	mRasterizerState(NULL),
	mD3DRasterizerState(NULL),
	mMaterialState(NULL)
{
	mDevice=renderDevice;
	mD3DDevice=mDevice->getD3D10Device();
}

D3D10RenderState::~D3D10RenderState(){
	destroy();
}

void D3D10RenderState::destroy(){
	if(mBlendState!=NULL){
		delete mBlendState;
		mBlendState=NULL;
	}
	if(mD3DBlendState!=NULL){
		mD3DBlendState->Release();
		mD3DBlendState=NULL;
	}
	if(mDepthState!=NULL){
		delete mDepthState;
		mDepthState=NULL;
	}
	if(mD3DDepthStencilState!=NULL){
		mD3DDepthStencilState->Release();
		mD3DDepthStencilState=NULL;
	}
	if(mRasterizerState!=NULL){
		delete mRasterizerState;
		mRasterizerState=NULL;
	}
	if(mD3DRasterizerState!=NULL){
		mD3DRasterizerState->Release();
		mD3DRasterizerState=NULL;
	}

	if(mMaterialState!=NULL){
		delete mMaterialState;
		mMaterialState=NULL;
	}

	if(mListener!=NULL){
		mListener->renderStateDestroyed(this);
		mListener=NULL;
	}
}

void D3D10RenderState::setBlendState(const BlendState &state){
	if(mBlendState==NULL){mBlendState=new BlendState(state);}else{mBlendState->set(state);}
	if(mD3DBlendState!=NULL){mD3DBlendState->Release();}
	D3D10_BLEND_DESC desc; mDevice->getD3D10_BLEND_DESC(desc,state);
	mD3DDevice->CreateBlendState(&desc,&mD3DBlendState);
}

bool D3D10RenderState::getBlendState(BlendState &state) const{
	if(mBlendState==NULL){return false;}else{state.set(*mBlendState);return true;}
}

void D3D10RenderState::setDepthState(const DepthState &state){
	if(mDepthState==NULL){mDepthState=new DepthState(state);}else{mDepthState->set(state);}
	if(mD3DDepthStencilState!=NULL){mD3DDepthStencilState->Release();}
	D3D10_DEPTH_STENCIL_DESC desc; mDevice->getD3D10_DEPTH_STENCIL_DESC(desc,state);
	mD3DDevice->CreateDepthStencilState(&desc,&mD3DDepthStencilState);
}

bool D3D10RenderState::getDepthState(DepthState &state) const{
	if(mDepthState==NULL){return false;}else{state.set(*mDepthState);return true;}
}

void D3D10RenderState::setRasterizerState(const RasterizerState &state){
	if(mRasterizerState==NULL){mRasterizerState=new RasterizerState(state);}else{mRasterizerState->set(state);}
	if(mD3DRasterizerState!=NULL){mD3DRasterizerState->Release();}
	D3D10_RASTERIZER_DESC desc; mDevice->getD3D10_RASTERIZER_DESC(desc,state);
	mD3DDevice->CreateRasterizerState(&desc,&mD3DRasterizerState);
}

bool D3D10RenderState::getRasterizerState(RasterizerState &state) const{
	if(mRasterizerState==NULL){return false;}else{state.set(*mRasterizerState);return true;}
}

void D3D10RenderState::setSamplerState(int i,const SamplerState &state){
	if(mSamplerStates.size()<=i){
		mSamplerStates.resize(i+1,NULL);
		mD3DSamplerStates.resize(i+1,NULL);
	}
	if(mSamplerStates[i]==NULL){
		mSamplerStates[i]=new SamplerState(state);
	}
	else{
		mSamplerStates[i]->set(state);
	}
	if(mD3DSamplerStates[i]!=NULL){mD3DSamplerStates[i]->Release();}
	D3D10_SAMPLER_DESC desc; mDevice->getD3D10_SAMPLER_DESC(desc,state);
	mD3DDevice->CreateSamplerState(&desc,&mD3DSamplerStates[i]);
}

bool D3D10RenderState::getSamplerState(int i,SamplerState &state) const{
	if(mSamplerStates.size()<=i || mSamplerStates[i]==NULL){return false;}else{state.set(*mSamplerStates[i]);return true;}
}

void D3D10RenderState::setTextureState(int i,const TextureState &state){
	if(mTextureStates.size()<=i){
		mTextureStates.resize(i+1,NULL);
	}
	if(mTextureStates[i]==NULL){
		mTextureStates[i]=new TextureState(state);
	}
	else{
		mTextureStates[i]->set(state);
	}
}

bool D3D10RenderState::getTextureState(int i,TextureState &state) const{
	if(mTextureStates.size()<=i || mTextureStates[i]==NULL){return false;}else{state.set(*mTextureStates[i]);return true;}
}

}
}
