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

#ifndef TOADLET_PEEPER_D3D10RENDERSTATESET_H
#define TOADLET_PEEPER_D3D10RENDERSTATESET_H

#include "D3D10Includes.h"
#include <toadlet/peeper/RenderStateSet.h>

namespace toadlet{
namespace peeper{

class D3D10Renderer;

class TOADLET_API D3D10RenderStateSet:public RenderStateSet{
public:
	D3D10RenderStateSet(D3D10Renderer *renderer):
		mListener(NULL),
		mBlendState(NULL),
		mD3DBlendState(NULL),
		mDepthState(NULL),
		mD3DDepthStencilState(NULL),
		mRasterizerState(NULL),
		mD3DRasterizerState(NULL),
		mMaterialState(NULL)
	{
		mRenderer=renderer;
		mDevice=mRenderer->getD3D10Device();
	}
	virtual ~D3D10RenderStateSet(){
		destroy();
	}

	RenderStateSet *getRootRenderStateSet(){return this;}

	void setRenderStateSetDestroyedListener(RenderStateSetDestroyedListener *listener){mListener=listener;}

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
			mListener->renderStateSetDestroyed(this);
			mListener=NULL;
		}
	}

	void setBlendState(const BlendState &state){
		if(mBlendState==NULL){mBlendState=new BlendState(state);}else{mBlendState->set(state);}
		if(mD3DBlendState!=NULL){mD3DBlendState->Release();}
		D3D10_BLEND_DESC desc; mRenderer->getD3D10_BLEND_DESC(desc,state);
		mDevice->CreateBlendState(&desc,&mD3DBlendState);
	}
	bool getBlendState(BlendState &state) const{if(mBlendState==NULL){return false;}else{state.set(*mBlendState);return true;}}

	void setDepthState(const DepthState &state){
		if(mDepthState==NULL){mDepthState=new DepthState(state);}else{mDepthState->set(state);}
		if(mD3DDepthStencilState!=NULL){mD3DDepthStencilState->Release();}
		D3D10_DEPTH_STENCIL_DESC desc; mRenderer->getD3D10_DEPTH_STENCIL_DESC(desc,state);
		mDevice->CreateDepthStencilState(&desc,&mD3DDepthStencilState);
	}
	bool getDepthState(DepthState &state) const{if(mDepthState==NULL){return false;}else{state.set(*mDepthState);return true;}}

	void setRasterizerState(const RasterizerState &state){
		if(mRasterizerState==NULL){mRasterizerState=new RasterizerState(state);}else{mRasterizerState->set(state);}
		if(mD3DRasterizerState!=NULL){mD3DRasterizerState->Release();}
		D3D10_RASTERIZER_DESC desc; mRenderer->getD3D10_RASTERIZER_DESC(desc,state);
		mDevice->CreateRasterizerState(&desc,&mD3DRasterizerState);
	}
	bool getRasterizerState(RasterizerState &state) const{if(mRasterizerState==NULL){return false;}else{state.set(*mRasterizerState);return true;}}

	void setFogState(const FogState &state){}
	bool getFogState(FogState &state) const{return false;}

	void setPointState(const PointState &state){}
	bool getPointState(PointState &state) const{return false;}

	void setMaterialState(const MaterialState &state){if(mMaterialState==NULL){mMaterialState=new MaterialState(state);}else{mMaterialState->set(state);}}
	bool getMaterialState(MaterialState &state) const{if(mMaterialState==NULL){return false;}else{state.set(*mMaterialState);return true;}}

protected:
	D3D10Renderer *mRenderer;
	ID3D10Device *mDevice;

	RenderStateSetDestroyedListener *mListener;
	BlendState *mBlendState;
	ID3D10BlendState *mD3DBlendState;
	DepthState *mDepthState;
	ID3D10DepthStencilState *mD3DDepthStencilState;
	RasterizerState *mRasterizerState;
	ID3D10RasterizerState *mD3DRasterizerState;
	
	MaterialState *mMaterialState;

	friend class D3D10Renderer;
};

}
}

#endif
