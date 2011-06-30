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

#ifndef TOADLET_TADPOLE_RENDERPASS_H
#define TOADLET_TADPOLE_RENDERPASS_H

#include <toadlet/tadpole/Types.h>
#include <toadlet/peeper/RenderState.h>
#include <toadlet/peeper/ShaderState.h>
#include <toadlet/peeper/RenderDevice.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API RenderPass{
public:
	TOADLET_SHARED_POINTERS(RenderPass);

	RenderPass();
	virtual ~RenderPass();

	void destroy();

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

	int getNumSamplerStates() const{return mRenderState->getNumSamplerStates();}
	void setSamplerState(int i,const SamplerState &state){return mRenderState->setSamplerState(i,state);}
	bool getSamplerState(int i,SamplerState &state) const{return mRenderState->getSamplerState(i,state);}

	int getNumTextureStates() const{return mRenderState->getNumTextureStates();}
	void setTextureState(int i,const TextureState &state){return mRenderState->setTextureState(i,state);}
	bool getTextureState(int i,TextureState &state) const{return mRenderState->getTextureState(i,state);}

	void setShader(Shader::ShaderType type,Shader::ptr shader);
	Shader::ptr getShader(Shader::ShaderType type);

	int getNumTextures() const{return mTextures.size();}
	void setTexture(Texture::ptr texture){setTexture(0,texture);}
	void setTexture(int i,Texture::ptr texture);
	Texture::ptr getTexture(int i=0) const{return i<mTextures.size()?mTextures[i]:NULL;}
	void setTextureName(const String &name){setTextureName(0,name);}
	void setTextureName(int i,const String &name);
	String getTextureName(int i=0) const{return i<mTextureNames.size()?mTextureNames[i]:(char*)NULL;}

	RenderState::ptr getRenderState() const{return mRenderState;}
	ShaderState::ptr getShaderState() const{return mShaderState;}

	void setupRenderDevice(RenderDevice *renderDevice);
	
protected:
	RenderState::ptr mRenderState;
	ShaderState::ptr mShaderState;
	Collection<Texture::ptr> mTextures;
	Collection<String> mTextureNames;
};

}
}

#endif

/*
#include <toadlet/egg/BaseResource.h>
#include <toadlet/tadpole/RenderPass.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API Material:public BaseResource{
public:
	TOADLET_SHARED_POINTERS(Material);

	Material();
	virtual ~Material();

	void destroy();
	Material::ptr clone();

	inline void getNumRenderPasses(){return mRenderPasses.size();}
	inline RenderPass::ptr getRenderPass(int i){return mRenderPasses[i];}
	RenderPass::ptr addRenderPass();
	void removeRenderPass(RenderPass *pass);

protected:
	Collection<RenderPass::ptr> mRenderPasses;
};

}
}
*/