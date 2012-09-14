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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/tadpole/material/RenderVariableSet.h>
#include <toadlet/tadpole/material/RenderPass.h>

namespace toadlet{
namespace tadpole{
namespace material{

RenderVariableSet::RenderVariableSet():
	mRenderPass(NULL),
	mRenderState(NULL),
	mShaderState(NULL)
{
}

void RenderVariableSet::destroy(){
	int i;
	for(i=0;i<mBuffers.size();++i){
		mBuffers[i].buffer->destroy();
	}
	mBuffers.clear();
}

bool RenderVariableSet::addBuffer(Shader::ShaderType shaderType,int index,VariableBuffer *buffer){
	BufferInfo set;
	set.buffer=buffer;
	set.scope=0;
	set.shaderType=shaderType;
	set.index=index;

	mBuffers.add(set);
	return true;
}

void RenderVariableSet::removeBuffer(VariableBuffer *buffer){
	int i;
	for(i=0;i<mBuffers.size();++i){
		if(mBuffers[i].buffer==buffer){
			mBuffers.removeAt(i);
			return;
		}
	}
}

bool RenderVariableSet::addTexture(const String &name,Texture *texture,const String &samplerName,const SamplerState &samplerState,const TextureState &textureState){
	Shader::ShaderType type;
	VariableBufferFormat::Variable *formatVariable=findResourceVariable(name,type);
	if(formatVariable==NULL){
		// If not found, then search for the d3d9 style appended name
		formatVariable=findResourceVariable(samplerName+"+"+name,type);
	}
	if(formatVariable!=NULL){
		if((formatVariable->getFormat()&VariableBufferFormat::Format_MASK_TYPES)!=VariableBufferFormat::Format_TYPE_RESOURCE){
			Logger::warning(Categories::TOADLET_TADPOLE,
				String("RenderVariable:")+name+" format does not match format:"+formatVariable->getFormat()+"!="+(int)VariableBufferFormat::Format_TYPE_RESOURCE);
			return false;
		}

		mRenderPass->setTextureLocationName(type,formatVariable->getResourceIndex(),name);

		/// @todo: Set the sampler by name also
		mRenderPass->setTexture(type,formatVariable->getResourceIndex(),texture,samplerState,textureState);
	}
	else{
		if(mBuffers.size()>0){
			Logger::warning(Categories::TOADLET_TADPOLE,
				"RenderVariable not found with name:"+name);
			return false;
		}

		int i;
		for(i=0;i<mUnassignedResources.size();++i){
			if(mUnassignedResources[i].name==name){
				mUnassignedResources.removeAt(i);
				break;
			}
		}

		ResourceInfo r;
		r.name=name;
		r.texture=texture;
		r.samplerName=samplerName;
		r.samperState=samplerState;
		r.textureState=textureState;
		mUnassignedResources.add(r);

		Logger::alert("Adding unassigned texture");
	}

	return true;
}

bool RenderVariableSet::findTexture(const String &name,Shader::ShaderType &type,int &index){
	VariableBufferFormat::Variable *formatVariable=findResourceVariable(name,type);
	if(formatVariable!=NULL){
		index=formatVariable->getResourceIndex();
		return true;
	}
	else{
		type=(Shader::ShaderType)0;
		index=0;
		return false;
	}
}

// Search for the correct buffer and correct index
bool RenderVariableSet::addVariable(const String &name,RenderVariable::ptr variable,int scope){
	BufferInfo *bufferInfo=NULL;
	VariableBufferFormat::Variable *formatVariable=findFormatVariable(name,bufferInfo);
	if(formatVariable!=NULL){
		if((formatVariable->getFormat()&~VariableBufferFormat::Format_MASK_OPTIONS)!=(variable->getFormat()&~VariableBufferFormat::Format_MASK_OPTIONS)){
			Logger::warning(Categories::TOADLET_TADPOLE,
				String("RenderVariable:")+name+" format does not match format:"+formatVariable->getFormat()+"!="+variable->getFormat());
			return false;
		}
	}

	VariableInfo v;
	v.name=name;
	v.location=0;
	v.scope=scope;
	v.variable=variable;

	if(formatVariable==NULL){
		if(mBuffers.size()>0){
			Logger::warning(Categories::TOADLET_TADPOLE,
				"RenderVariable not found with name:"+name);
			return false;
		}

		mUnassignedVariables.add(v);
	}
	else{
		v.location=formatVariable->getOffset();
		variable->linked(this);
		bufferInfo->variables.add(v);
		bufferInfo->scope|=scope;

		// Combine the two format, not sure this is ideal, but it will let us transfer information about the RenderVariable to the Buffer for items like Format_SAMPLER_MATRIX
		formatVariable->setFormat(formatVariable->getFormat()|variable->getFormat());
	}
	
	return true;
}

void RenderVariableSet::removeVariable(RenderVariable::ptr variable){
	int i,j;
	for(i=0;i<mBuffers.size();++i){
		BufferInfo *buffer=&mBuffers[i];
		buffer->scope=0;
		for(j=0;j<buffer->variables.size();++j){
			if(buffer->variables[j].variable==variable){
				buffer->variables.removeAt(j);
				j--;
			}
			else{
				buffer->scope|=buffer->variables[j].scope;
			}
		}
	}
	for(i=0;i<mUnassignedVariables.size();++i){
		if(mUnassignedVariables[i].variable==variable){
			mUnassignedVariables.removeAt(i);
			i--;
		}
	}
}

RenderVariable::ptr RenderVariableSet::findVariable(const String &name){
	int i,j;
	for(i=0;i<mBuffers.size();++i){
		BufferInfo *buffer=&mBuffers[i];
		for(j=0;j<buffer->variables.size();++j){
			if(buffer->variables[j].name==name){
				return buffer->variables[j].variable;
			}
		}
	}
	for(i=0;i<mUnassignedVariables.size();++i){
		if(mUnassignedVariables[i].name==name){
			return mUnassignedVariables[i].variable;
		}
	}
	return NULL;
}

VariableBufferFormat::Variable *RenderVariableSet::findFormatVariable(const String &name,BufferInfo *&buffer){
	if(mBuffers.size()==0){
		return NULL;
	}

	int i=name.find("."),j=0;
	String fullName=name;
	VariableBufferFormat::Variable *formatVariable=NULL;

	if(i>0){
		String possibleBufferName=name.substr(0,i);
		for(i=0;i<mBuffers.size();++i){
			if(possibleBufferName.equals(mBuffers[i].buffer->getVariableBufferFormat()->getName())){
				buffer=&mBuffers[i];
				fullName=name.substr(i+1,name.length());
				break;
			}
		}
	}

	if(buffer!=NULL){
		VariableBufferFormat *format=buffer->buffer->getVariableBufferFormat();
		for(j=0;j<format->getSize();++j){
			if(fullName.equals(format->getVariable(j)->getFullName())){
				formatVariable=format->getVariable(j);
			}
		}
	}
	else{
		for(i=0;i<mBuffers.size();++i){
			if(mBuffers[i].buffer->getVariableBufferFormat()->getPrimary()){
				buffer=&mBuffers[i];

				VariableBufferFormat *format=buffer->buffer->getVariableBufferFormat();
				for(j=0;j<format->getSize();++j){
					if(fullName.equals(format->getVariable(j)->getFullName())){
						formatVariable=format->getVariable(j);
						i=mBuffers.size();
						break;
					}
				}
			}
		}
	}

	return formatVariable;	
}

VariableBufferFormat::Variable *RenderVariableSet::findResourceVariable(const String &name,Shader::ShaderType &type){
	VariableBufferFormat::Variable *formatVariable=NULL;
	int i,j,k;
	for(j=0;j<Shader::ShaderType_MAX;++j){
		for(i=0;i<mShaderState->getNumVariableBuffers((Shader::ShaderType)j);++i){
			VariableBufferFormat::ptr format=mShaderState->getVariableBufferFormat((Shader::ShaderType)j,i);
			for(k=0;k<format->getSize();++k){
				if(name.equals(format->getVariable(k)->getFullName())){
					formatVariable=format->getVariable(k);
					type=(Shader::ShaderType)j;
					return formatVariable;
				}
			}
		}
	}
	return NULL;
}

void RenderVariableSet::buildBuffers(BufferManager *manager,RenderPass *pass){
	int i,j;
	Collection<VariableInfo> variables=mUnassignedVariables;
	mUnassignedVariables.clear();

	Collection<ResourceInfo> resources=mUnassignedResources;
	mUnassignedResources.clear();

	mRenderPass=pass;
	mRenderState=pass->getRenderState();
	mShaderState=pass->getShaderState();

	while(mBuffers.size()>0){
		BufferInfo *buffer=&mBuffers[0];
		for(j=0;j<buffer->variables.size();++j){
			variables.add(buffer->variables[j]);
		}
		mBuffers.removeAt(0);
	}

	for(i=0;i<Shader::ShaderType_MAX;++i){
		for(j=0;j<mShaderState->getNumVariableBuffers((Shader::ShaderType)i);++j){
			VariableBufferFormat::ptr format=mShaderState->getVariableBufferFormat((Shader::ShaderType)i,j);
			VariableBuffer::ptr buffer;
			if(manager!=NULL){
				buffer=manager->createVariableBuffer(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,format);
			}
			else{
				buffer=new BackableBuffer();
				buffer->create(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,format);
			}
			addBuffer((Shader::ShaderType)i,j,buffer);
		}
	}

	for(i=0;i<variables.size();++i){
		addVariable(variables[i].name,variables[i].variable,variables[i].scope);
	}

	for(i=0;i<resources.size();++i){
		addTexture(resources[i].name,resources[i].texture,resources[i].samplerName,resources[i].samperState,resources[i].textureState);
	}
}

void RenderVariableSet::update(int scope,SceneParameters *parameters){
	int i,j;
	for(i=0;i<mBuffers.size();++i){
		BufferInfo *buffer=&mBuffers[i];
		// We update the whole buffer on it's largest scope, since updating the buffer in sections doesn't work in D3D10 with dynamic usage.
		if((buffer->scope&scope)==0 || (buffer->scope&~scope)>scope){
			continue;
		}

		tbyte *data=buffer->buffer->lock(Buffer::Access_BIT_WRITE);
		for(j=0;j<buffer->variables.size();++j){
			VariableInfo *variable=&buffer->variables[j];
			variable->variable->update(data+variable->location,parameters);
		}
		buffer->buffer->unlock();
	}
}

}
}
}
