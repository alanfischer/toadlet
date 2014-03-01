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

#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/material/RenderPass.h>

namespace toadlet{
namespace tadpole{
namespace material{

RenderPass::RenderPass(MaterialManager *manager):
	mManager(manager)
{
	mRenderState=mManager->createRenderState();
	mOwnRenderState=mRenderState;
	
	mShaderState=mManager->createShaderState();
	mOwnShaderState=mShaderState;
}

RenderPass::RenderPass(MaterialManager *manager,RenderState *renderState,ShaderState *shaderState):
	mManager(manager)
{
	mRenderState=renderState;
	mShaderState=shaderState;
}

RenderPass::RenderPass(MaterialManager *manager,RenderPass *pass):
	mManager(manager)
{
	mName=pass->mName;
	mRenderState=pass->mRenderState;
	mShaderState=pass->mShaderState;
	for(int i=0;i<Shader::ShaderType_MAX;++i){
		mBuffers[i]=pass->mBuffers[i];
		mTextures[i]=pass->mTextures[i];
	}
	mUnlinkedVariables=pass->mUnlinkedVariables;
	mUnlinkedTextures=pass->mUnlinkedTextures;
}


void RenderPass::destroy(){
	if(mOwnRenderState!=NULL){
		mOwnRenderState->destroy();
		mOwnRenderState=NULL;
	}
	if(mOwnShaderState!=NULL){
		mOwnShaderState->destroy();
		mOwnShaderState=NULL;
	}
	mRenderState=NULL;
	mShaderState=NULL;

	int i;
	for(i=0;i<Shader::ShaderType_MAX;++i){
		mTextures[i].clear();
		mBuffers[i].clear();
	}
}

bool RenderPass::setTexture(const String &name,Texture *texture,const String &samplerName,const SamplerState &samplerState,const TextureState &textureState){
	TextureData t;
	t.locationName=name;
	t.texture=texture;
	t.samplerLocationName=samplerName;
	t.samplerState=samplerState;
	t.textureState=textureState;
	mUnlinkedTextures.add(t);

	return true;
}

bool RenderPass::setTexture(Shader::ShaderType type,int i,Texture *texture,const SamplerState &samplerState,const TextureState &textureState){
	if(mRenderState==NULL){
		return false;
	}

	if(i>=mTextures[type].size()){
		mTextures[type].resize(i+1);
	}
	mTextures[type][i].texture=texture;

	mRenderState->setSamplerState(type,i,samplerState);
	mRenderState->setTextureState(type,i,textureState);

	return true;
}

bool RenderPass::findTexture(Shader::ShaderType &type,int &index,const String &name,const String &samplerName){
	String combinedName=samplerName+"+"+name;

	int i,j;
	for(j=0;j<Shader::ShaderType_MAX;++j){
		for(i=0;i<mTextures[j].size();++i){
			const String &locationName=mTextures[j][i].locationName;
			if(locationName==name || locationName==combinedName){
				type=(Shader::ShaderType)j;
				index=i;
				return true;
			}
		}
	}
	return false;
}

void RenderPass::setBuffer(Shader::ShaderType type,int i,VariableBuffer *buffer){
	if(i>=mBuffers[type].size()){
		mBuffers[type].resize(i+1);
	}
	mBuffers[type][i].buffer=buffer;
}

void RenderPass::setRenderState(RenderState *renderState){
	if(mOwnRenderState!=NULL){
		mOwnRenderState->destroy();
		mOwnRenderState=NULL;
	}

	mRenderState=renderState;
}

void RenderPass::setShaderState(ShaderState *shaderState){
	if(mOwnShaderState!=NULL){
		mOwnShaderState->destroy();
		mOwnShaderState=NULL;
	}

	mShaderState=shaderState;
}

void RenderPass::setShader(Shader::ShaderType type,Shader *shader){
	if(mShaderState==NULL){
		mShaderState=mManager->createShaderState();
		mOwnShaderState=mShaderState;
	}
	
	mShaderState->setShader(type,shader);
}

void RenderPass::updateVariables(int scope,SceneParameters *parameters){
	int i,j,v;
	for(j=0;j<Shader::ShaderType_MAX;++j){
		for(i=0;i<mBuffers[j].size();++i){
			BufferData &bufferData=mBuffers[j][i];
			// We update the whole buffer on it's largest scope, since updating the buffer in sections doesn't work in D3D10 with dynamic usage.
			if((bufferData.scope&scope)==0 || (bufferData.scope&~scope)>scope){
				continue;
			}

			tbyte *data=bufferData.buffer->lock(Buffer::Access_BIT_WRITE);
			for(v=0;v<bufferData.variables.size();++v){
				VariableData &variableData=mUnlinkedVariables[bufferData.variables[v]];
				variableData.variable->update(data+variableData.location,parameters);
			}
			bufferData.buffer->unlock();
		}
	}
}

void RenderPass::setTextureLocationName(Shader::ShaderType type,int i,const String &name){
	if(i>=mTextures[type].size()){
		mTextures[type].resize(i+1);
	}
	mTextures[type][i].locationName=name;
}

void RenderPass::setBufferLocationName(Shader::ShaderType type,int i,const String &name){
	if(i>=mBuffers[type].size()){
		mBuffers[type].resize(i+1);
	}
	mBuffers[type][i].locationName=name;
}

bool RenderPass::addVariable(const String &name,RenderVariable::ptr variable,int scope){
	VariableData v;
	v.name=name;
	v.variable=variable;
	v.location=-1;
	v.scope=scope;
	mUnlinkedVariables.add(v);
	
	return true;
}

bool RenderPass::isDepthSorted() const{
	DepthState state;
	return mRenderState!=NULL && mRenderState->getDepthState(state) && state.write==false;
}

void RenderPass::compile(){
	populateLocationNames();
	createBuffers();
	linkVariables();
	linkTextures();
}

bool RenderPass::populateLocationNames(){
	if(mShaderState==NULL){
		return false;
	}

	int i,j,k;
	for(j=0;j<Shader::ShaderType_MAX;++j){
		for(i=0;i<mShaderState->getNumVariableBuffers((Shader::ShaderType)j);++i){
			VariableBufferFormat::ptr format=mShaderState->getVariableBufferFormat((Shader::ShaderType)j,i);

			setBufferLocationName((Shader::ShaderType)j,i,format->getName());

			for(k=0;k<format->getSize();++k){
				VariableBufferFormat::Variable *variable=format->getVariable(k);
				if((variable->getFormat()&VariableBufferFormat::Format_TYPE_RESOURCE)!=0){
					setTextureLocationName((Shader::ShaderType)j,variable->getResourceIndex(),variable->getName());
				}
			}
		}
	}

	return true;
}

bool RenderPass::createBuffers(){
	if(mShaderState==NULL){
		return false;
	}

	int i=0,j=0;
	for(j=0;j<Shader::ShaderType_MAX;++j){
		for(i=0;i<mShaderState->getNumVariableBuffers((Shader::ShaderType)j);++i){
			VariableBufferFormat::ptr format=mShaderState->getVariableBufferFormat((Shader::ShaderType)j,i);
			VariableBuffer::ptr buffer=mManager->getBufferManager()->createVariableBuffer(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,format);
			setBuffer((Shader::ShaderType)j,i,buffer);
		}
	}

	return true;
}

bool RenderPass::linkVariables(){
	int i=0;
	for(i=0;i<mUnlinkedVariables.size();++i){
		VariableData &vardata=mUnlinkedVariables[i];
		String name=vardata.name;
		int scope=vardata.scope;
		RenderVariable *variable=vardata.variable;

		Shader::ShaderType bufferType=(Shader::ShaderType)0;
		int bufferIndex=-1;
		VariableBufferFormat::Variable *formatVariable=findFormatVariable(name,bufferType,bufferIndex);
		if(formatVariable!=NULL){
			if((formatVariable->getFormat()&~VariableBufferFormat::Format_MASK_OPTIONS)!=(variable->getFormat()&~VariableBufferFormat::Format_MASK_OPTIONS)){
				Log::warning(Categories::TOADLET_TADPOLE,
					String("RenderVariable:")+name+" format does not match format:"+formatVariable->getFormat()+"!="+variable->getFormat());
				continue;
			}
		}

		if(formatVariable==NULL){
			if(mShaderState!=NULL){
				Log::warning(Categories::TOADLET_TADPOLE,
					"RenderVariable not found with name:"+name);
			}
			continue;
		}

		vardata.location=formatVariable->getOffset();

		variable->linked(this);
		mBuffers[bufferType][bufferIndex].variables.add(i);
		mBuffers[bufferType][bufferIndex].scope|=scope;

		// Combine the two format, not sure this is ideal, but it will let us transfer information about the RenderVariable to the Buffer for items like Format_SAMPLER_MATRIX
		formatVariable->setFormat(formatVariable->getFormat()|variable->getFormat());
	}
	
	return true;
}

bool RenderPass::linkTextures(){
	int i=0;
	for(i=0;i<mUnlinkedTextures.size();++i){
		const TextureData &texdata=mUnlinkedTextures[i];
		const String &name=texdata.locationName;
		const String &samplerName=texdata.samplerLocationName;
		Texture *texture=texdata.texture;
		const SamplerState &samplerState=texdata.samplerState;
		const TextureState &textureState=texdata.textureState;

		Shader::ShaderType type=(Shader::ShaderType)0;
		int index=-1;
		if(findTexture(type,index,name,samplerName)==false){
			Log::warning(Categories::TOADLET_TADPOLE,
				"Texture not found with name:"+name);
			continue;
		}
		else{
			setTexture(type,index,texture,samplerState,textureState);
		}
	}

	return true;
}

VariableBufferFormat::Variable *RenderPass::findFormatVariable(const String &name,Shader::ShaderType &bufferType,int &bufferIndex){
	int i=0,j=0,k=0,p=name.find(".");
	String fullName=name;
	VariableBufferFormat::Variable *formatVariable=NULL;
	bufferIndex=-1;

	if(p>0){
		String possibleBufferName=name.substr(0,p);
		for(j=0;j<Shader::ShaderType_MAX;++j){
			for(i=0;i<mBuffers[j].size();++i){
				if(possibleBufferName.equals(mBuffers[j][i].buffer->getVariableBufferFormat()->getName())){
					bufferType=(Shader::ShaderType)j;
					bufferIndex=i;
					fullName=name.substr(p+1,name.length());

					j=Shader::ShaderType_MAX;
					break;
				}
			}
		}
	}

	if(bufferIndex>=0){
		VariableBufferFormat *format=mBuffers[bufferType][bufferIndex].buffer->getVariableBufferFormat();
		for(j=0;j<format->getSize();++j){
			if(fullName.equals(format->getVariable(j)->getFullName())){
				formatVariable=format->getVariable(j);
				break;
			}
		}
	}
	else{
		for(j=0;j<Shader::ShaderType_MAX;++j){
			for(i=0;i<mBuffers[j].size();++i){
				VariableBuffer *buffer=mBuffers[j][i].buffer;
				if(buffer->getVariableBufferFormat()->getPrimary()){
					bufferType=(Shader::ShaderType)j;
					bufferIndex=i;

					VariableBufferFormat *format=buffer->getVariableBufferFormat();
					for(k=0;k<format->getSize();++k){
						if(fullName.equals(format->getVariable(k)->getFullName())){
							formatVariable=format->getVariable(k);
							break;
						}
					}

					if(formatVariable!=NULL){
						j=Shader::ShaderType_MAX;
						break;
					}
				}
			}
		}
	}

	return formatVariable;	
}

}
}
}
