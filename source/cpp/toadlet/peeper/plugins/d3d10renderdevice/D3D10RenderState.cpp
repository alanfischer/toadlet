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
	mDepthState(NULL),
	mRasterizerState(NULL),
	mFogState(NULL),
	mPointState(NULL),
	mMaterialState(NULL),

	mD3DBlendState(NULL),
	mD3DDepthStencilState(NULL),
	mD3DRasterizerState(NULL)
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
	if(mDepthState!=NULL){
		delete mDepthState;
		mDepthState=NULL;
	}
	if(mRasterizerState!=NULL){
		delete mRasterizerState;
		mRasterizerState=NULL;
	}
	if(mFogState!=NULL){
		delete mFogState;
		mFogState=NULL;
	}
	if(mPointState!=NULL){
		delete mPointState;
		mPointState=NULL;
	}
	if(mMaterialState!=NULL){
		delete mMaterialState;
		mMaterialState=NULL;
	}
	int i,j;
	for(j=0;j<Shader::ShaderType_MAX;++j){
		for(i=0;i<mSamplerStates[j].size();++i){
			if(mSamplerStates[j][i]!=NULL){
				delete mSamplerStates[j][i];
				mSamplerStates[j][i]=NULL;
			}
		}
		mSamplerStates[j].clear();
		for(i=0;i<mTextureStates[j].size();++i){
			if(mTextureStates[i]!=NULL){
				delete mTextureStates[j][i];
				mTextureStates[j][i]=NULL;
			}
		}
		mTextureStates[j].clear();
	}

	if(mD3DBlendState!=NULL){
		mD3DBlendState->Release();
		mD3DBlendState=NULL;
	}
	if(mD3DDepthStencilState!=NULL){
		mD3DDepthStencilState->Release();
		mD3DDepthStencilState=NULL;
	}
	if(mD3DRasterizerState!=NULL){
		mD3DRasterizerState->Release();
		mD3DRasterizerState=NULL;
	}
	for(j=0;j<Shader::ShaderType_MAX;++j){
		for(i=0;i<mD3DSamplerStates[j].size();++i){
			if(mD3DSamplerStates[j][i]!=NULL){
				mD3DSamplerStates[j][i]->Release();
				mD3DSamplerStates[j][i]=NULL;
			}
		}
		mD3DSamplerStates[j].clear();
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

void D3D10RenderState::setFogState(const FogState &state){
	if(mFogState==NULL){mFogState=new FogState(state);}else{mFogState->set(state);}
}

bool D3D10RenderState::getFogState(FogState &state) const{
	if(mFogState==NULL){return false;}else{state.set(*mFogState);return true;}
}

void D3D10RenderState::setPointState(const PointState &state){
	if(mPointState==NULL){mPointState=new PointState(state);}else{mPointState->set(state);}
}

bool D3D10RenderState::getPointState(PointState &state) const{
	if(mPointState==NULL){return false;}else{state.set(*mPointState);return true;}
}

void D3D10RenderState::setMaterialState(const MaterialState &state){
	if(mMaterialState==NULL){mMaterialState=new MaterialState(state);}else{mMaterialState->set(state);}
}

bool D3D10RenderState::getMaterialState(MaterialState &state) const{
	if(mMaterialState==NULL){return false;}else{state.set(*mMaterialState);return true;}
}

void D3D10RenderState::setSamplerState(Shader::ShaderType type,int i,const SamplerState &state){
	if(mSamplerStates[type].size()<=i){
		mSamplerStates[type].resize(i+1,NULL);
		mD3DSamplerStates[type].resize(i+1,NULL);
	}
	if(mSamplerStates[type][i]==NULL){
		mSamplerStates[type][i]=new SamplerState(state);
	}
	else{
		mSamplerStates[type][i]->set(state);
	}
	if(mD3DSamplerStates[type][i]!=NULL){mD3DSamplerStates[type][i]->Release();}
	D3D10_SAMPLER_DESC desc; mDevice->getD3D10_SAMPLER_DESC(desc,state);
	mD3DDevice->CreateSamplerState(&desc,&mD3DSamplerStates[type][i]);
}

bool D3D10RenderState::getSamplerState(Shader::ShaderType type,int i,SamplerState &state) const{
	if(mSamplerStates[type].size()<=i || mSamplerStates[type][i]==NULL){return false;}else{state.set(*mSamplerStates[type][i]);return true;}
}

void D3D10RenderState::setTextureState(Shader::ShaderType type,int i,const TextureState &state){
	if(mTextureStates[type].size()<=i){
		mTextureStates[type].resize(i+1,NULL);
	}
	if(mTextureStates[type][i]==NULL){
		mTextureStates[type][i]=new TextureState(state);
	}
	else{
		mTextureStates[type][i]->set(state);
	}
}

bool D3D10RenderState::getTextureState(Shader::ShaderType type,int i,TextureState &state) const{
	if(mTextureStates[type].size()<=i || mTextureStates[type][i]==NULL){return false;}else{state.set(*mTextureStates[type][i]);return true;}
}

}
}
