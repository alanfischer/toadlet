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

bool RenderVariableSet::addBuffer(VariableBuffer::ptr buffer){
	BufferInfo set;
	set.buffer=buffer;
	set.scope=0;
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
	int i=name.find(".");
	String fullName=name;
	BufferInfo *buffer=NULL;
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
	if(buffer==NULL){
		for(i=0;i<mBuffers.size();++i){
			if(mBuffers[i].buffer->getVariableBufferFormat()->getPrimary()){
				buffer=&mBuffers[i];
				String bufferName=buffer->buffer->getVariableBufferFormat()->getName();
				break;
			}
		}
	}
	if(buffer==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"VariableBuffer not found for RenderVariable with name:"+name);
		return false;
	}

	VariableBufferFormat *format=buffer->buffer->getVariableBufferFormat();
	VariableBufferFormat::Variable *formatVariable=NULL;
	for(i=0;i<format->getSize();++i){
		if(fullName.equals(format->getVariable(i)->getFullName())){
			formatVariable=format->getVariable(i);
		}
	}
	
	if(formatVariable==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"RenderVariable not found with name:"+name);
		return false;
	}
	
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

void RenderVariableSet::update(int scope,SceneParameters *parameters){
	int i,j;
	for(i=0;i<mBuffers.size();++i){
		BufferInfo *buffer=&mBuffers[i];
		if((buffer->scope&scope)==0){
			continue;
		}

		tbyte *data=buffer->buffer->lock(Buffer::Access_BIT_WRITE);
		for(j=0;j<buffer->variables.size();++j){
			VariableInfo *variable=&buffer->variables[j];
			if((variable->scope&scope)!=0){
				variable->variable->update(data+variable->location,parameters);
			}
		}
		buffer->buffer->unlock();
	}
}

}
}
}
