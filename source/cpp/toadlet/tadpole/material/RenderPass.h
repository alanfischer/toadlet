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

class TOADLET_API RenderPass:public Object{
public:
	TOADLET_OBJECT(RenderPass);

	RenderPass(MaterialManager *manager,RenderState *renderState=NULL,ShaderState *shaderState=NULL);

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

	void setShader(Shader::ShaderType type,Shader *shader);
	Shader *getShader(Shader::ShaderType type){return mShaderState!=NULL?mShaderState->getShader(type):NULL;}

	int getNumTextures(Shader::ShaderType type=Shader::ShaderType_FRAGMENT) const{return mTextures[type].size();}
	void setTexture(const String &name,Texture *texture,const String &samplerName,const SamplerState &samplerState,const TextureState &textureState);
	void setTexture(Shader::ShaderType type,int i,Texture *texture,const SamplerState &samplerState,const TextureState &textureState);
	Texture *getTexture(Shader::ShaderType type=Shader::ShaderType_FRAGMENT,int i=0) const{return i<mTextures[type].size()?mTextures[type][i]:NULL;}
	inline bool findTexture(Shader::ShaderType &type,int &index,const String &name){return findTexture(type,index,name,"");}
	bool findTexture(Shader::ShaderType &type,int &index,const String &name,const String &samplerName);

	void setTextureLocationName(Shader::ShaderType type,int i,const String &name);
	int getNumTextureLocationNames(Shader::ShaderType type) const{return mTextureLocationNames[type].size();}
	String getTextureLocationName(Shader::ShaderType type,int i) const{return mTextureLocationNames[type][i];}

	bool addVariable(const String &name,RenderVariable::ptr variable,int scope){return makeVariables()->addVariable(name,variable,scope);}

	inline RenderState *getRenderState() const{return mRenderState;}
	inline ShaderState *getShaderState() const{return mShaderState;}

	void setModelMatrixFlags(int flags){mModelMatrixFlags=flags;}
	int getModelMatrixFlags() const{return mModelMatrixFlags;}

	inline int getNumBuffers() const{return mVariables==NULL?0:mVariables->getNumBuffers();}
	inline VariableBuffer *getBuffer(int i){return mVariables->getBuffer(i);}
	inline int getBufferScope(int i){return mVariables->getBufferScope(i);}
	inline Shader::ShaderType getBufferShaderType(int i){return mVariables->getBufferShaderType(i);}
	inline int getBufferIndex(int i){return mVariables->getBufferIndex(i);}

	void setBufferLocationName(Shader::ShaderType type,int i,const String &name);
	int getNumBufferLocationNames(Shader::ShaderType type) const{return mBufferLocationNames[type].size();}
	String getBufferLocationName(Shader::ShaderType type,int i) const{return mBufferLocationNames[type][i];}

	bool isDepthSorted() const;

	void compile(); 

	// TODO: REMOVE
	RenderVariableSet::ptr makeVariables();
	inline RenderVariableSet::ptr getVariables() const{return mVariables;}
	void updateVariables(int scope,SceneParameters *parameters);

protected:
	void populateLocationNames();

	MaterialManager *mManager;
	String mName;
	RenderState::ptr mRenderState,mOwnRenderState;
	ShaderState::ptr mShaderState,mOwnShaderState;
	Collection<Texture::ptr> mTextures[Shader::ShaderType_MAX];
	int mModelMatrixFlags;

	RenderVariableSet::ptr mVariables;

	Collection<String> mTextureLocationNames[Shader::ShaderType_MAX];
	Collection<String> mBufferLocationNames[Shader::ShaderType_MAX];
};

}
}
}

#endif
