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

#ifndef TOADLET_TADPOLE_MATERIAL_RENDERVARIABLESET_H
#define TOADLET_TADPOLE_MATERIAL_RENDERVARIABLESET_H

#include <toadlet/peeper/VariableBuffer.h>
#include <toadlet/peeper/Shader.h>
#include <toadlet/peeper/ShaderState.h>
#include <toadlet/peeper/RenderState.h>
#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/material/RenderVariable.h>

namespace toadlet{
namespace tadpole{
namespace material{

class RenderPass;

/// @todo: This should be merged into RenderPass
class TOADLET_API RenderVariableSet:public Object{
public:
	TOADLET_OBJECT(RenderVariableSet);

	RenderVariableSet();
	
	void destroy();

	bool addBuffer(Shader::ShaderType shaderType,int index,VariableBuffer *buffer);
	void removeBuffer(VariableBuffer *buffer);
	int findBuffer(const String &name);
	inline int getNumBuffers() const{return mBuffers.size();}
	inline VariableBuffer *getBuffer(int i){return mBuffers[i].buffer;}
	inline int getBufferScope(int i){return mBuffers[i].scope;}
	inline Shader::ShaderType getBufferShaderType(int i){return mBuffers[i].shaderType;}
	inline int getBufferIndex(int i){return mBuffers[i].index;}

	/// @todo: The getNumTextures through getTextureTextureState only work for unassigned resources, currently used by Material duplication
	bool addTexture(const String &name,Texture *texture,const String &samplerName,const SamplerState &samplerState,const TextureState &textureState);
	bool findTexture(const String &name,Shader::ShaderType &type,int &index);
	int getNumTextures(){return getNumResourceInfos();}
	Texture::ptr getTexture(int i){return getResourceInfo(i)->texture;}
	const String &getTextureName(int i){return getResourceInfo(i)->name;}
	const String &getTextureSamplerName(int i){return getResourceInfo(i)->samplerName;}
	SamplerState getTextureSamplerState(int i){return getResourceInfo(i)->samplerState;}
	TextureState getTextureTextureState(int i){return getResourceInfo(i)->textureState;}

	bool addVariable(const String &name,RenderVariable::ptr variable,int scope);
	void removeVariable(RenderVariable::ptr variable);
	RenderVariable::ptr findVariable(const String &name);
	int getNumVariables(){return getNumVariableInfos();}
	RenderVariable::ptr getVariable(int i){return getVariableInfo(i)->variable;}
	const String &getVariableName(int i){return getVariableInfo(i)->name;}
	int getVariableScope(int i){return getVariableInfo(i)->scope;}

	void buildBuffers(BufferManager *manager,RenderPass *pass);
	void update(int scope,SceneParameters *parameters);

protected:
	class VariableInfo{
	public:
		String name;
		int location;
		int scope;
		RenderVariable::ptr variable;
	};

	class ResourceInfo{
	public:
		String name;
		Texture::ptr texture;
		String samplerName;
		SamplerState samplerState;
		TextureState textureState;
	};

	class BufferInfo{
	public:
		VariableBuffer::ptr buffer;
		int scope;
		Shader::ShaderType shaderType;
		int index;
		Collection<VariableInfo> variables;
	};

	inline int getNumVariableInfos(){
		int count=0,j;
		for(j=0;j<mBuffers.size();++j){
			count+=mBuffers[j].variables.size();
		}
		count+=mUnassignedVariables.size();
		return count;
	}

	inline VariableInfo *getVariableInfo(int i){
		int j;
		for(j=0;j<mBuffers.size() && i>=mBuffers[j].variables.size();++j){
			i-=mBuffers[j].variables.size();
		}
		if(j<mBuffers.size() && i<mBuffers[j].variables.size()){
			return &mBuffers[j].variables[i];
		}
		else{
			return &mUnassignedVariables[i];
		}
	}

	inline int getNumResourceInfos(){
		return mUnassignedResources.size();
	}

	inline ResourceInfo *getResourceInfo(int i){
		return &mUnassignedResources[i];
	}

	VariableBufferFormat::Variable *findFormatVariable(const String &name,BufferInfo *&buffer);
	VariableBufferFormat::Variable *findResourceVariable(const String &name,Shader::ShaderType &type);

	Collection<BufferInfo> mBuffers;
	Collection<VariableInfo> mUnassignedVariables;
	Collection<ResourceInfo> mUnassignedResources;
	RenderPass *mRenderPass;
	RenderState *mRenderState;
	ShaderState *mShaderState;
};

}
}
}

#endif
