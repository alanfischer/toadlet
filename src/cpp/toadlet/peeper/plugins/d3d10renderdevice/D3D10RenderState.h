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

#ifndef TOADLET_PEEPER_D3D10RENDERSTATE_H
#define TOADLET_PEEPER_D3D10RENDERSTATE_H

#include "D3D10Includes.h"
#include <toadlet/peeper/RenderState.h>

namespace toadlet{
namespace peeper{

class D3D10RenderDevice;

class TOADLET_API D3D10RenderState:public RenderState{
public:
	D3D10RenderState(D3D10RenderDevice *renderDevice):
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
	virtual ~D3D10RenderState(){
		destroy();
	}

	RenderState *getRootRenderState(){return this;}

	void setRenderStateDestroyedListener(RenderStateDestroyedListener *listener){mListener=listener;}

	bool create(){return true;}

	void destroy(){
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

	void setBlendState(const BlendState &state){
		if(mBlendState==NULL){mBlendState=new BlendState(state);}else{mBlendState->set(state);}
		if(mD3DBlendState!=NULL){mD3DBlendState->Release();}
		D3D10_BLEND_DESC desc; mDevice->getD3D10_BLEND_DESC(desc,state);
		mD3DDevice->CreateBlendState(&desc,&mD3DBlendState);
	}
	bool getBlendState(BlendState &state) const{if(mBlendState==NULL){return false;}else{state.set(*mBlendState);return true;}}

	void setDepthState(const DepthState &state){
		if(mDepthState==NULL){mDepthState=new DepthState(state);}else{mDepthState->set(state);}
		if(mD3DDepthStencilState!=NULL){mD3DDepthStencilState->Release();}
		D3D10_DEPTH_STENCIL_DESC desc; mDevice->getD3D10_DEPTH_STENCIL_DESC(desc,state);
		mD3DDevice->CreateDepthStencilState(&desc,&mD3DDepthStencilState);
	}
	bool getDepthState(DepthState &state) const{if(mDepthState==NULL){return false;}else{state.set(*mDepthState);return true;}}

	void setRasterizerState(const RasterizerState &state){
		if(mRasterizerState==NULL){mRasterizerState=new RasterizerState(state);}else{mRasterizerState->set(state);}
		if(mD3DRasterizerState!=NULL){mD3DRasterizerState->Release();}
		D3D10_RASTERIZER_DESC desc; mDevice->getD3D10_RASTERIZER_DESC(desc,state);
		mD3DDevice->CreateRasterizerState(&desc,&mD3DRasterizerState);
	}
	bool getRasterizerState(RasterizerState &state) const{if(mRasterizerState==NULL){return false;}else{state.set(*mRasterizerState);return true;}}

	void setFogState(const FogState &state){}
	bool getFogState(FogState &state) const{return false;}

	void setPointState(const PointState &state){}
	bool getPointState(PointState &state) const{return false;}

	void setMaterialState(const MaterialState &state){if(mMaterialState==NULL){mMaterialState=new MaterialState(state);}else{mMaterialState->set(state);}}
	bool getMaterialState(MaterialState &state) const{if(mMaterialState==NULL){return false;}else{state.set(*mMaterialState);return true;}}

	int getNumSamplerStates() const{return mSamplerStates.size();}
	void setSamplerState(int i,const SamplerState &state){
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
	bool getSamplerState(int i,SamplerState &state) const{if(mSamplerStates.size()<=i || mSamplerStates[i]==NULL){return false;}else{state.set(*mSamplerStates[i]);return true;}}

	int getNumTextureStates() const{return mTextureStates.size();}
	void setTextureState(int i,const TextureState &state){
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
	bool getTextureState(int i,TextureState &state) const{if(mTextureStates.size()<=i || mTextureStates[i]==NULL){return false;}else{state.set(*mTextureStates[i]);return true;}}

protected:
	D3D10RenderDevice *mDevice;
	ID3D10Device *mD3DDevice;

	RenderStateDestroyedListener *mListener;
	BlendState *mBlendState;
	ID3D10BlendState *mD3DBlendState;
	DepthState *mDepthState;
	ID3D10DepthStencilState *mD3DDepthStencilState;
	RasterizerState *mRasterizerState;
	ID3D10RasterizerState *mD3DRasterizerState;
	egg::Collection<SamplerState*> mSamplerStates;
	egg::Collection<ID3D10SamplerState*> mD3DSamplerStates;

	egg::Collection<TextureState*> mTextureStates;
	MaterialState *mMaterialState;

	friend class D3D10RenderDevice;
};

}
}

#endif
