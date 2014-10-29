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

#ifndef TOADLET_PEEPER_GLRENDERSTATE_H
#define TOADLET_PEEPER_GLRENDERSTATE_H

#include "GLIncludes.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/RenderState.h>

namespace toadlet{
namespace peeper{

class GLRenderDevice;

class TOADLET_API GLRenderState:public BaseResource,public RenderState{
public:
	TOADLET_RESOURCE(GLRenderState,RenderState);

	GLRenderState(GLRenderDevice *renderDevice);

	RenderState *getRootRenderState(){return this;}

	bool create(){return true;}
	void destroy();

	int getSetStates() const{return ((mBlendState!=NULL)<<StateType_BLEND) | ((mDepthState!=NULL)<<StateType_DEPTH) | ((mRasterizerState!=NULL)<<StateType_RASTERIZER) | ((mFogState!=NULL)<<StateType_FOG) | ((mGeometryState!=NULL)<<StateType_GEOMETRY) | ((mMaterialState!=NULL)<<StateType_MATERIAL);}

	void setBlendState(const BlendState &state){if(mBlendState==NULL){mBlendState=new BlendState(state);}else{mBlendState->set(state);}}
	bool getBlendState(BlendState &state) const{if(mBlendState==NULL){return false;}else{state.set(*mBlendState);return true;}}

	void setDepthState(const DepthState &state){if(mDepthState==NULL){mDepthState=new DepthState(state);}else{mDepthState->set(state);}}
	bool getDepthState(DepthState &state) const{if(mDepthState==NULL){return false;}else{state.set(*mDepthState);return true;}}

	void setRasterizerState(const RasterizerState &state){if(mRasterizerState==NULL){mRasterizerState=new RasterizerState(state);}else{mRasterizerState->set(state);}}
	bool getRasterizerState(RasterizerState &state) const{if(mRasterizerState==NULL){return false;}else{state.set(*mRasterizerState);return true;}}

	void setFogState(const FogState &state){if(mFogState==NULL){mFogState=new FogState(state);}else{mFogState->set(state);}}
	bool getFogState(FogState &state) const{if(mFogState==NULL){return false;}else{state.set(*mFogState);return true;}}

	void setGeometryState(const GeometryState &state){if(mGeometryState==NULL){mGeometryState=new GeometryState(state);}else{mGeometryState->set(state);}}
	bool getGeometryState(GeometryState &state) const{if(mGeometryState==NULL){return false;}else{state.set(*mGeometryState);return true;}}

	void setMaterialState(const MaterialState &state){if(mMaterialState==NULL){mMaterialState=new MaterialState(state);}else{mMaterialState->set(state);}}
	bool getMaterialState(MaterialState &state) const{if(mMaterialState==NULL){return false;}else{state.set(*mMaterialState);return true;}}

	int getNumSamplerStates(Shader::ShaderType type) const{return mSamplerStates.size();}
	void setSamplerState(Shader::ShaderType type,int i,const SamplerState &state){
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
	bool getSamplerState(Shader::ShaderType type,int i,SamplerState &state) const{if(mSamplerStates.size()<=i || mSamplerStates[i]==NULL){return false;}else{state.set(*mSamplerStates[i]);return true;}}

	int getNumTextureStates(Shader::ShaderType type) const{return mTextureStates.size();}
	void setTextureState(Shader::ShaderType type,int i,const TextureState &state){
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
	bool getTextureState(Shader::ShaderType type,int i,TextureState &state) const{if(mTextureStates.size()<=i || mTextureStates[i]==NULL){return false;}else{state.set(*mTextureStates[i]);return true;}}

protected:
	BlendState *mBlendState;
	DepthState *mDepthState;
	RasterizerState *mRasterizerState;
	FogState *mFogState;
	GeometryState *mGeometryState;
	MaterialState *mMaterialState;
	Collection<SamplerState*> mSamplerStates;
	Collection<TextureState*> mTextureStates;
	
	friend class GLRenderDevice;
};

}
}

#endif
