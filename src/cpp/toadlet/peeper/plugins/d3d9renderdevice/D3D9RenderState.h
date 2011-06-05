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

#ifndef TOADLET_PEEPER_D3D9RENDERSTATE_H
#define TOADLET_PEEPER_D3D9RENDERSTATE_H

#include "D3D9Includes.h"
#include <toadlet/peeper/RenderState.h>

namespace toadlet{
namespace peeper{

class D3D9RenderDevice;

class TOADLET_API D3D9RenderState:public RenderState{
public:
	D3D9RenderState(D3D9RenderDevice *renderDevice):
		mListener(NULL),
		mBlendState(NULL),
		mDepthState(NULL),
		mRasterizerState(NULL),
		mFogState(NULL),
		mPointState(NULL),
		mMaterialState(NULL)
	{}
	virtual ~D3D9RenderState(){
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

		int i;
		for(i=0;i<mSamplerStates.size();++i){
			delete mSamplerStates[i];
		}
		mSamplerStates.clear();
		for(i=0;i<mTextureStates.size();++i){
			delete mTextureStates[i];
		}
		mTextureStates.clear();

		if(mListener!=NULL){
			mListener->renderStateDestroyed(this);
			mListener=NULL;
		}
	}

	void setBlendState(const BlendState &state){if(mBlendState==NULL){mBlendState=new BlendState(state);}else{mBlendState->set(state);}}
	bool getBlendState(BlendState &state) const{if(mBlendState==NULL){return false;}else{state.set(*mBlendState);return true;}}

	void setDepthState(const DepthState &state){if(mDepthState==NULL){mDepthState=new DepthState(state);}else{mDepthState->set(state);}}
	bool getDepthState(DepthState &state) const{if(mDepthState==NULL){return false;}else{state.set(*mDepthState);return true;}}

	void setRasterizerState(const RasterizerState &state){if(mRasterizerState==NULL){mRasterizerState=new RasterizerState(state);}else{mRasterizerState->set(state);}}
	bool getRasterizerState(RasterizerState &state) const{if(mRasterizerState==NULL){return false;}else{state.set(*mRasterizerState);return true;}}

	void setFogState(const FogState &state){if(mFogState==NULL){mFogState=new FogState(state);}else{mFogState->set(state);}}
	bool getFogState(FogState &state) const{if(mFogState==NULL){return false;}else{state.set(*mFogState);return true;}}

	void setPointState(const PointState &state){if(mPointState==NULL){mPointState=new PointState(state);}else{mPointState->set(state);}}
	bool getPointState(PointState &state) const{if(mPointState==NULL){return false;}else{state.set(*mPointState);return true;}}

	void setMaterialState(const MaterialState &state){if(mMaterialState==NULL){mMaterialState=new MaterialState(state);}else{mMaterialState->set(state);}}
	bool getMaterialState(MaterialState &state) const{if(mMaterialState==NULL){return false;}else{state.set(*mMaterialState);return true;}}

	int getNumSamplerStates() const{return mSamplerStates.size();}
	void setSamplerState(int i,const SamplerState &state){
		if(mSamplerStates.size()<=i){
			mSamplerStates.resize(i+1,NULL);
		}
		if(mSamplerStates[i]==NULL){
			mSamplerStates[i]=new SamplerState(state);
		}
		else{
			mSamplerStates[i]->set(state);
		}
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
	RenderStateDestroyedListener *mListener;
	BlendState *mBlendState;
	DepthState *mDepthState;
	RasterizerState *mRasterizerState;
	FogState *mFogState;
	PointState *mPointState;
	MaterialState *mMaterialState;
	egg::Collection<SamplerState*> mSamplerStates;
	egg::Collection<TextureState*> mTextureStates;
	
	friend class D3D9RenderDevice;
};

}
}

#endif
