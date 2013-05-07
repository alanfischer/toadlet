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
#include <toadlet/peeper/VariableBuffer.h>
#include <toadlet/tadpole/material/RenderVariable.h>
#include <toadlet/tadpole/material/SceneParameters.h>

namespace toadlet{
namespace tadpole{

class MaterialManager;

namespace material{

class TOADLET_API RenderPass:public Object{
public:
	TOADLET_OBJECT(RenderPass);

	RenderPass(MaterialManager *manager);
	RenderPass(MaterialManager *manager,RenderState *renderState,ShaderState *shaderState);
	RenderPass(MaterialManager *manager,RenderPass *pass);

	void destroy();

	void setName(const String &name){mName=name;}
	const String &getName(){return mName;}

	void setBlendState(const BlendState &state){mRenderState->setBlendState(state);}
	bool getBlendState(BlendState &state) const{return mRenderState!=NULL?mRenderState->getBlendState(state):false;}

	void setDepthState(const DepthState &state){mRenderState->setDepthState(state);}
	bool getDepthState(DepthState &state) const{return mRenderState!=NULL?mRenderState->getDepthState(state):false;}

	void setRasterizerState(const RasterizerState &state){mRenderState->setRasterizerState(state);}
	bool getRasterizerState(RasterizerState &state) const{return mRenderState!=NULL?mRenderState->getRasterizerState(state):false;}

	void setFogState(const FogState &state){mRenderState->setFogState(state);}
	bool getFogState(FogState &state) const{return mRenderState!=NULL?mRenderState->getFogState(state):false;}

	void setPointState(const PointState &state){mRenderState->setPointState(state);}
	bool getPointState(PointState &state) const{return mRenderState!=NULL?mRenderState->getPointState(state):false;}

	void setMaterialState(const MaterialState &state){mRenderState->setMaterialState(state);}
	bool getMaterialState(MaterialState &state) const{return mRenderState!=NULL?mRenderState->getMaterialState(state):false;}

	void setSamplerState(Shader::ShaderType type,int i,const SamplerState &state){mRenderState->setSamplerState(type,i,state);}
	bool getSamplerState(Shader::ShaderType type,int i,SamplerState &state){return mRenderState!=NULL?mRenderState->getSamplerState(type,i,state):false;}

	void setTextureState(Shader::ShaderType type,int i,const TextureState &state){mRenderState->setTextureState(type,i,state);}
	bool getTextureState(Shader::ShaderType type,int i,TextureState &state){return mRenderState!=NULL?mRenderState->getTextureState(type,i,state):false;}

	void setRenderState(RenderState *renderState);
	inline RenderState *getRenderState() const{return mRenderState;}

	void setShaderState(ShaderState *shaderState);
	inline ShaderState *getShaderState() const{return mShaderState;}

	void setShader(Shader::ShaderType type,Shader *shader);
	Shader *getShader(Shader::ShaderType type){return mShaderState!=NULL?mShaderState->getShader(type):NULL;}

	void setModelMatrixFlags(int flags){mModelMatrixFlags=flags;}
	int getModelMatrixFlags() const{return mModelMatrixFlags;}

	int getNumTextures(Shader::ShaderType type) const{return mTextures[type].size();}
	void setTexture(const String &name,Texture *texture,const String &samplerName,const SamplerState &samplerState,const TextureState &textureState);
	void setTexture(Shader::ShaderType type,int i,Texture *texture,const SamplerState &samplerState,const TextureState &textureState);
	Texture *getTexture(Shader::ShaderType type,int i) const{return i<mTextures[type].size()?mTextures[type][i].texture:NULL;}
	inline bool findTexture(Shader::ShaderType &type,int &index,const String &name){return findTexture(type,index,name,"");}
	bool findTexture(Shader::ShaderType &type,int &index,const String &name,const String &samplerName);

	void setTextureLocationName(Shader::ShaderType type,int i,const String &name);
	int getNumTextureLocationNames(Shader::ShaderType type) const{return mTextures[type].size();}
	String getTextureLocationName(Shader::ShaderType type,int i) const{return mTextures[type][i].locationName;}

	int getNumBuffers(Shader::ShaderType type) const{return mBuffers[type].size();}
	void setBuffer(Shader::ShaderType type,int i,VariableBuffer *buffer);
	VariableBuffer *getBuffer(Shader::ShaderType type,int i) const{return i<mBuffers[type].size()?mBuffers[type][i].buffer:NULL;}
	int getBufferScope(Shader::ShaderType type,int i) const{return i<mBuffers[type].size()?mBuffers[type][i].scope:0;}
	bool findBuffer(Shader::ShaderType &type,int &index,const String &name);

	void setBufferLocationName(Shader::ShaderType type,int i,const String &name);
	int getNumBufferLocationNames(Shader::ShaderType type) const{return mBuffers[type].size();}
	String getBufferLocationName(Shader::ShaderType type,int i) const{return mBuffers[type][i].locationName;}

	bool addVariable(const String &name,RenderVariable::ptr variable,int scope);
	void updateVariables(int scope,SceneParameters *parameters);

	bool isDepthSorted() const;

	void compile(); 

protected:
	void populateLocationNames();
	void createBuffers();
	VariableBufferFormat::Variable *findFormatVariable(const String &name,Shader::ShaderType &bufferType,int &bufferIndex);

	class BufferData{
	public:
		BufferData():scope(0){}

		String locationName;
		VariableBuffer::ptr buffer;
		int scope;
		Collection<int> variables;
	};

	class TextureData{
	public:
		String locationName;
		Texture::ptr texture;
	};

	class VariableData{
	public:
		VariableData():location(0),scope(0){}

		String name;
		RenderVariable::ptr variable;
		int location;
		int scope;
	};

	MaterialManager *mManager;
	String mName;
	RenderState::ptr mRenderState,mOwnRenderState;
	ShaderState::ptr mShaderState,mOwnShaderState;
	Collection<BufferData> mBuffers[Shader::ShaderType_MAX];
	Collection<TextureData> mTextures[Shader::ShaderType_MAX];
	int mModelMatrixFlags;

	Collection<VariableData> mVariables;
};

}
}
}

#endif
