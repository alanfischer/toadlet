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

#include <toadlet/egg/Log.h>
#include <toadlet/peeper/BackableRenderState.h>
#include <toadlet/peeper/BackableShaderState.h>
#include <toadlet/tadpole/BufferManager.h>
#include <toadlet/tadpole/MaterialManager.h>
#include <toadlet/tadpole/material/RenderPass.h>

namespace toadlet{
namespace tadpole{
namespace material{

RenderPass::RenderPass(MaterialManager *manager):
	mManager(manager),
	mModelMatrixFlags(0)
{
	if(mManager!=NULL){
		mRenderState=mManager->createRenderState();
	}
	else{
		mRenderState=new BackableRenderState();
	}
	mOwnRenderState=mRenderState;
	
	if(mManager!=NULL){
		mShaderState=mManager->createShaderState();
	}
	else{
		mShaderState=new BackableShaderState();
	}
	mOwnShaderState=mShaderState;
}

RenderPass::RenderPass(MaterialManager *manager,RenderState *renderState,ShaderState *shaderState):
	mManager(manager),
	mModelMatrixFlags(0)
{
	mRenderState=renderState;
	mShaderState=shaderState;

	populateLocationNames();
	createBuffers();
}

RenderPass::RenderPass(MaterialManager *manager,RenderPass *pass):
	mManager(manager),
	mModelMatrixFlags(0)
{
	mName=pass->mName;
	mRenderState=pass->mRenderState;
	mShaderState=pass->mShaderState;
	for(int i=0;i<Shader::ShaderType_MAX;++i){
		mBuffers[i]=pass->mBuffers[i];
		mTextures[i]=pass->mTextures[i];
	}
	mModelMatrixFlags=pass->mModelMatrixFlags;
	mVariables=pass->mVariables;
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

void RenderPass::setTexture(const String &name,Texture *texture,const String &samplerName,const SamplerState &samplerState,const TextureState &textureState){
	Shader::ShaderType type;
	int index;
	if(findTexture(type,index,name,samplerName)){
		setTexture(type,index,texture,samplerState,textureState);
	}
}

void RenderPass::setTexture(Shader::ShaderType type,int i,Texture *texture,const SamplerState &samplerState,const TextureState &textureState){
	if(mRenderState==NULL){
		return;
	}

	if(i>=mTextures[type].size()){
		mTextures[type].resize(i+1);
	}
	mTextures[type][i].texture=texture;

	mRenderState->setSamplerState(type,i,samplerState);
	mRenderState->setTextureState(type,i,textureState);
}

bool RenderPass::findTexture(Shader::ShaderType &type,int &index,const String &name,const String &samplerName){
	String combinedName=samplerName+"+"+name;

	int i,j;
	for(j=0;j<Shader::ShaderType_MAX;++j){
		for(i=0;i<mTextures[j].size();++i){
			const String locationName=mTextures[j][i].locationName;
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
		if(mManager!=NULL){
			mShaderState=mManager->createShaderState();
		}
		else{
			mShaderState=new BackableShaderState();
		}
		mOwnShaderState=mShaderState;
	}
	
	mShaderState->setShader(type,shader);

	populateLocationNames();
	createBuffers();
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
				VariableData &variableData=mVariables[bufferData.variables[v]];
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
	Shader::ShaderType bufferType;
	int bufferIndex=-1;
	VariableBufferFormat::Variable *formatVariable=findFormatVariable(name,bufferType,bufferIndex);
	if(formatVariable!=NULL){
		if((formatVariable->getFormat()&~VariableBufferFormat::Format_MASK_OPTIONS)!=(variable->getFormat()&~VariableBufferFormat::Format_MASK_OPTIONS)){
			Log::warning(Categories::TOADLET_TADPOLE,
				String("RenderVariable:")+name+" format does not match format:"+formatVariable->getFormat()+"!="+variable->getFormat());
			return false;
		}
	}

	if(formatVariable==NULL){
		if(mShaderState!=NULL){
			Log::warning(Categories::TOADLET_TADPOLE,
				"RenderVariable not found with name:"+name);
			return false;
		}
	}
	else{
		VariableData v;
		v.name=name;
		v.variable=variable;
		v.location=formatVariable->getOffset();
		mVariables.add(v);
		
		variable->linked(this);
		mBuffers[bufferType][bufferIndex].variables.add(mVariables.size()-1);
		mBuffers[bufferType][bufferIndex].scope|=scope;

		// Combine the two format, not sure this is ideal, but it will let us transfer information about the RenderVariable to the Buffer for items like Format_SAMPLER_MATRIX
		formatVariable->setFormat(formatVariable->getFormat()|variable->getFormat());
	}
	
	return true;
}

bool RenderPass::isDepthSorted() const{
	DepthState state;
	return mRenderState!=NULL && mRenderState->getDepthState(state) && state.write==false;
}

void RenderPass::compile(){
}

void RenderPass::populateLocationNames(){
	if(mShaderState==NULL){
		return;
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
}

void RenderPass::createBuffers(){
	int i=0,j=0;
	if(mShaderState==NULL){
		return;
	}

	for(j=0;j<Shader::ShaderType_MAX;++j){
		for(i=0;i<mShaderState->getNumVariableBuffers((Shader::ShaderType)j);++i){
			VariableBufferFormat::ptr format=mShaderState->getVariableBufferFormat((Shader::ShaderType)j,i);
			VariableBuffer::ptr buffer;
			if(mManager!=NULL){
				buffer=mManager->getBufferManager()->createVariableBuffer(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,format);
			}
			else{
				buffer=new BackableBuffer();
				buffer->create(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,format);
			}
			setBuffer((Shader::ShaderType)j,i,buffer);
		}
	}
}

VariableBufferFormat::Variable *RenderPass::findFormatVariable(const String &name,Shader::ShaderType &bufferType,int &bufferIndex){
	int i=0,j=0,p=name.find(".");
	String fullName=name;
	VariableBufferFormat::Variable *formatVariable=NULL;
	bufferIndex=-1;

	if(p>0){
		String possibleBufferName=name.substr(0,i);
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
				if(mBuffers[j][i].buffer->getVariableBufferFormat()->getPrimary()){
					bufferType=(Shader::ShaderType)j;
					bufferIndex=i;

					VariableBufferFormat *format=mBuffers[j][i].buffer->getVariableBufferFormat();
					for(j=0;j<format->getSize();++j){
						if(fullName.equals(format->getVariable(j)->getFullName())){
							formatVariable=format->getVariable(j);
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
