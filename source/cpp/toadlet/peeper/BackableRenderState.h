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

#ifndef TOADLET_PEEPER_BACKABLERENDERSTATE_H
#define TOADLET_PEEPER_BACKABLERENDERSTATE_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/RenderState.h>

#define FULLY_BACKED

namespace toadlet{
namespace peeper{

class TOADLET_API BackableRenderState:public BaseResource,public RenderState{
public:
	TOADLET_RESOURCE(BackableRenderState,RenderState);

	BackableRenderState();

	virtual RenderState *getRootRenderState(){return mBack;}

	virtual bool create(){return true;}
	virtual void destroy();

	int getSetStates() const{
		if(mBack!=NULL){return mBack->getSetStates();}
		else{return ((mBlendState!=NULL)<<StateType_BLEND) | ((mDepthState!=NULL)<<StateType_DEPTH) | ((mRasterizerState!=NULL)<<StateType_RASTERIZER) | ((mFogState!=NULL)<<StateType_FOG) | ((mPointState!=NULL)<<StateType_POINT) | ((mMaterialState!=NULL)<<StateType_MATERIAL);}
	}

	virtual void setBlendState(const BlendState &state){
		if(mBlendState==NULL){mBlendState=new BlendState(state);}else{mBlendState->set(state);}
		if(mBack!=NULL){mBack->setBlendState(state);}
	}
	virtual bool getBlendState(BlendState &state) const{
		if(mBack!=NULL){return mBack->getBlendState(state);}
		else if(mBlendState==NULL){return false;}else{state.set(*mBlendState);return true;}
	}

	virtual void setDepthState(const DepthState &state){
		if(mDepthState==NULL){mDepthState=new DepthState(state);}else{mDepthState->set(state);}
		if(mBack!=NULL){mBack->setDepthState(state);}
	}
	virtual bool getDepthState(DepthState &state) const{
		if(mBack!=NULL){return mBack->getDepthState(state);}
		else if(mDepthState==NULL){return false;}else{state.set(*mDepthState);return true;}
	}

	virtual void setRasterizerState(const RasterizerState &state){
		if(mRasterizerState==NULL){mRasterizerState=new RasterizerState(state);}else{mRasterizerState->set(state);}
		if(mBack!=NULL){mBack->setRasterizerState(state);}
	}
	virtual bool getRasterizerState(RasterizerState &state) const{
		if(mBack!=NULL){return mBack->getRasterizerState(state);}
		else if(mRasterizerState==NULL){return false;}else{state.set(*mRasterizerState);return true;}
	}

	virtual void setFogState(const FogState &state){
		if(mFogState==NULL){mFogState=new FogState(state);}else{mFogState->set(state);}
		if(mBack!=NULL){mBack->setFogState(state);}
	}
	virtual bool getFogState(FogState &state) const{
		if(mBack!=NULL){return mBack->getFogState(state);}
		else if(mFogState==NULL){return false;}else{state.set(*mFogState);return true;}
	}

	virtual void setPointState(const PointState &state){
		if(mPointState==NULL){mPointState=new PointState(state);}else{mPointState->set(state);}
		if(mBack!=NULL){mBack->setPointState(state);}
	}
	virtual bool getPointState(PointState &state) const{
		if(mBack!=NULL){return mBack->getPointState(state);}
		else if(mPointState==NULL){return false;}else{state.set(*mPointState);return true;}
	}

	virtual void setMaterialState(const MaterialState &state){
		if(mMaterialState==NULL){mMaterialState=new MaterialState(state);}else{mMaterialState->set(state);}
		if(mBack!=NULL){mBack->setMaterialState(state);}
	}
	virtual bool getMaterialState(MaterialState &state) const{
		if(mBack!=NULL){return mBack->getMaterialState(state);}
		else if(mMaterialState==NULL){return false;}else{state.set(*mMaterialState);return true;}
	}

#if defined(FULLY_BACKED)
	virtual int getNumSamplerStates(Shader::ShaderType type) const{
		if(mBack!=NULL){return mBack->getNumSamplerStates(type);}
		else return mSamplerStates[type].size();
	}
	virtual void setSamplerState(Shader::ShaderType type,int i,const SamplerState &state){
		if(mSamplerStates[type].size()<=i){
			mSamplerStates[type].resize(i+1,NULL);
		}
		if(mSamplerStates[type][i]==NULL){
			mSamplerStates[type][i]=new SamplerState(state);
		}
		else{
			mSamplerStates[type][i]->set(state);
		}
		if(mBack!=NULL){mBack->setSamplerState(type,i,state);}
	}
	virtual bool getSamplerState(Shader::ShaderType type,int i,SamplerState &state) const{
		if(mBack!=NULL){return mBack->getSamplerState(type,i,state);}
		else if(mSamplerStates[type].size()<=i || mSamplerStates[type][i]==NULL){return false;}else{state.set(*mSamplerStates[type][i]);return true;}
	}

	virtual int getNumTextureStates(Shader::ShaderType type) const{
		if(mBack!=NULL){return mBack->getNumTextureStates(type);}
		else return mTextureStates[type].size();
	}
	virtual void setTextureState(Shader::ShaderType type,int i,const TextureState &state){
		if(mTextureStates[type].size()<=i){
			mTextureStates[type].resize(i+1,NULL);
		}
		if(mTextureStates[type][i]==NULL){
			mTextureStates[type][i]=new TextureState(state);
		}
		else{
			mTextureStates[type][i]->set(state);
		}
		if(mBack!=NULL){mBack->setTextureState(type,i,state);}
	}
	virtual bool getTextureState(Shader::ShaderType type,int i,TextureState &state) const{
		if(mBack!=NULL){return mBack->getTextureState(type,i,state);}
		else if(mTextureStates[type].size()<=i || mTextureStates[type][i]==NULL){return false;}else{state.set(*mTextureStates[type][i]);return true;}
	}
#else
	virtual int getNumSamplerStates(Shader::ShaderType type) const{
		if(mBack!=NULL){return mBack->getNumSamplerStates(type);}
		else return mSamplerState!=NULL?1:0;
	}
	virtual void setSamplerState(Shader::ShaderType type,int i,const SamplerState &state){
		if(mSamplerState==NULL){mSamplerState=new SamplerState(state);}else{mSamplerState->set(state);}
		if(mBack!=NULL){mBack->setSamplerState(type,i,state);}
	}
	virtual bool getSamplerState(Shader::ShaderType type,int i,SamplerState &state) const{
		if(mBack!=NULL){return mBack->getSamplerState(type,i,state);}
		else if(mSamplerState==NULL){return false;}else{state.set(*mSamplerState);return true;}
	}

	virtual int getNumTextureStates(Shader::ShaderType type) const{
		if(mBack!=NULL){return mBack->getNumTextureStates(type);}
		else return mTextureState!=NULL?1:0;
	}
	virtual void setTextureState(Shader::ShaderType type,int i,const TextureState &state){
		if(mTextureState==NULL){mTextureState=new TextureState(state);}else{mTextureState->set(state);}
		if(mBack!=NULL){mBack->setTextureState(type,i,state);}
	}
	virtual bool getTextureState(Shader::ShaderType type,int i,TextureState &state) const{
		if(mBack!=NULL){return mBack->getTextureState(type,i,state);}
		else if(mTextureState==NULL){return false;}else{state.set(*mTextureState);return true;}
	}
#endif

	virtual void setBack(RenderState::ptr back);
	virtual RenderState::ptr getBack(){return mBack;}

protected:
	BlendState *mBlendState;
	DepthState *mDepthState;
	RasterizerState *mRasterizerState;
	FogState *mFogState;
	PointState *mPointState;
	MaterialState *mMaterialState;
#if defined(FULLY_BACKED)
	Collection<SamplerState*> mSamplerStates[Shader::ShaderType_MAX];
	Collection<TextureState*> mTextureStates[Shader::ShaderType_MAX];
#else
	SamplerState* mSamplerState;
	TextureState* mTextureState;
#endif

	RenderState::ptr mBack;
};

}
}

#endif
