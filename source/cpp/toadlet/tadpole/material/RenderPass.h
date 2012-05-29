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

#ifndef TOADLET_TADPOLE_MATERIAL_RENDERPASS_H
#define TOADLET_TADPOLE_MATERIAL_RENDERPASS_H

#include <toadlet/peeper/RenderState.h>
#include <toadlet/peeper/ShaderState.h>
#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/tadpole/material/RenderVariableSet.h>
#include <toadlet/tadpole/material/SceneParameters.h>

namespace toadlet{
namespace tadpole{

class MaterialManager;

namespace material{

class TOADLET_API RenderPass{
public:
	TOADLET_SPTR(RenderPass);

	RenderPass(MaterialManager *manager,RenderState::ptr renderState=RenderState::ptr(),ShaderState::ptr shaderState=ShaderState::ptr());
	virtual ~RenderPass();

	void destroy();

	void setName(const String &name){mName=name;}
	const String &getName(){return mName;}

	void setBlendState(const BlendState &state){mRenderState->setBlendState(state);}
	bool getBlendState(BlendState &state) const{return mRenderState->getBlendState(state);}

	void setDepthState(const DepthState &state){mRenderState->setDepthState(state);}
	bool getDepthState(DepthState &state) const{return mRenderState->getDepthState(state);}

	void setRasterizerState(const RasterizerState &state){mRenderState->setRasterizerState(state);}
	bool getRasterizerState(RasterizerState &state) const{return mRenderState->getRasterizerState(state);}

	void setFogState(const FogState &state){mRenderState->setFogState(state);}
	bool getFogState(FogState &state) const{return mRenderState->getFogState(state);}

	void setPointState(const PointState &state){mRenderState->setPointState(state);}
	bool getPointState(PointState &state) const{return mRenderState->getPointState(state);}

	void setMaterialState(const MaterialState &state){mRenderState->setMaterialState(state);}
	bool getMaterialState(MaterialState &state) const{return mRenderState->getMaterialState(state);}

	void setSamplerState(Shader::ShaderType type,int i,const SamplerState &state){mRenderState->setSamplerState(type,i,state);}
	bool getSamplerState(Shader::ShaderType type,int i,SamplerState &state){return mRenderState->getSamplerState(type,i,state);}

	void setTextureState(Shader::ShaderType type,int i,const TextureState &state){mRenderState->setTextureState(type,i,state);}
	bool getTextureState(Shader::ShaderType type,int i,TextureState &state){return mRenderState->getTextureState(type,i,state);}

	void setShader(Shader::ShaderType type,Shader::ptr shader);
	Shader::ptr getShader(Shader::ShaderType type){return mShaderState->getShader(type);}

	int getNumTextures(Shader::ShaderType type=Shader::ShaderType_FRAGMENT) const{return mTextures[type].size();}
	inline void setTexture(Texture::ptr texture,const SamplerState &samplerState,const TextureState &textureState){setTexture(Shader::ShaderType_FRAGMENT,0,texture,samplerState,textureState);}
	void setTexture(Shader::ShaderType type,int i,Texture::ptr texture,const SamplerState &samplerState,const TextureState &textureState);
	Texture::ptr getTexture(Shader::ShaderType type=Shader::ShaderType_FRAGMENT,int i=0) const{return i<mTextures[type].size()?mTextures[type][i]:NULL;}

	inline RenderState::ptr getRenderState() const{return mRenderState;}
	inline ShaderState::ptr getShaderState() const{return mShaderState;}

	RenderVariableSet::ptr makeVariables();
	inline RenderVariableSet::ptr getVariables() const{return mVariables;}
	void updateVariables(int scope,SceneParameters *parameters);

	bool isDepthSorted() const;

	void compile(); 

protected:
	MaterialManager *mManager;
	String mName;
	RenderState::ptr mRenderState,mOwnRenderState;
	ShaderState::ptr mShaderState,mOwnShaderState;
	Collection<Texture::ptr> mTextures[Shader::ShaderType_MAX];
	RenderVariableSet::ptr mVariables;
};

}
}
}

#endif
