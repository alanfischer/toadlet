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

namespace toadlet{
namespace tadpole{
namespace material{

RenderVariableSet::RenderVariableSet(){
}

RenderVariableSet::~RenderVariableSet(){
	destroy();
}

void RenderVariableSet::destroy(){
	int i;
	for(i=0;i<mBuffers.size();++i){
		mBuffers[i].buffer->destroy();
	}
	mBuffers.clear();
}

bool RenderVariableSet::addBuffer(Shader::ShaderType shaderType,int index,VariableBuffer::ptr buffer){
	BufferInfo set;
	set.buffer=buffer;
	set.scope=0;
	set.shaderType=shaderType;
	set.index=index;

	mBuffers.add(set);
	return true;
}

void RenderVariableSet::removeBuffer(VariableBuffer::ptr buffer){
	int i;
	for(i=0;i<mBuffers.size();++i){
		if(mBuffers[i].buffer==buffer){
			mBuffers.removeAt(i);
			return;
		}
	}
}

// Search for the correct buffer and correct index
bool RenderVariableSet::addVariable(const String &name,RenderVariable::ptr variable,int scope){
	if(mBuffers.size()==0){
		return false;
	}

	int i=name.find("."),j=0;
	String fullName=name;
	BufferInfo *buffer=NULL;
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
	
	if(formatVariable==NULL){
		Logger::warning(Categories::TOADLET_TADPOLE,
			"RenderVariable not found with name:"+name);
		return false;
	}
	if((variable->getFormat()&~VariableBufferFormat::Format_MASK_OPTIONS)!=(formatVariable->getFormat()&~VariableBufferFormat::Format_MASK_OPTIONS)){
		Logger::warning(Categories::TOADLET_TADPOLE,
			String("RenderVariable:")+name+" format does not match format:"+variable->getFormat()+"!="+formatVariable->getFormat());
		return false;
	}
	// Combine the two format, not sure this is ideal, but it will let us transfer information about the RenderVariable to the Buffer for items like Format_SAMPLER_MATRIX
	formatVariable->setFormat(formatVariable->getFormat()|variable->getFormat());

	VariableInfo v;
	v.name=name;
	v.location=formatVariable->getOffset();
	v.scope=scope;
	v.variable=variable;
	buffer->variables.add(v);

	buffer->scope|=scope;
	
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
			}
			else{
				buffer->scope|=buffer->variables[j].scope;
			}
		}
	}
}

void RenderVariableSet::buildBuffers(BufferManager *manager,ShaderState *state){
	if(state==NULL){
		return;
	}

	Collection<VariableInfo> variables;
	int i,j;

	while(mBuffers.size()>0){
		BufferInfo *buffer=&mBuffers[0];
		for(j=0;j<buffer->variables.size();++j){
			variables.add(buffer->variables[j]);
		}
		buffer->buffer->destroy();
		mBuffers.removeAt(0);
	}

	for(i=0;i<Shader::ShaderType_MAX;++i){
		for(j=0;j<state->getNumVariableBuffers((Shader::ShaderType)i);++j){
			VariableBufferFormat::ptr format=state->getVariableBufferFormat((Shader::ShaderType)i,j);
			VariableBuffer::ptr buffer=manager->createVariableBuffer(Buffer::Usage_BIT_DYNAMIC,Buffer::Access_BIT_WRITE,format);
			addBuffer((Shader::ShaderType)i,j,buffer);

			Logger::debug(Categories::TOADLET_TADPOLE,
				"\n"+format->toString());
		}
	}
	for(i=0;i<variables.size();++i){
		addVariable(variables[i].name,variables[i].variable,variables[i].scope);
	}
}

void RenderVariableSet::update(int scope,SceneParameters *parameters){
	int i,j;
	for(i=0;i<mBuffers.size();++i){
		BufferInfo *buffer=&mBuffers[i];
		// We update the whole buffer on it's largest scope, since updating the buffer in seconds doesn't work in D3D10 with dynamic usage.
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
