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
#include <toadlet/tadpole/RenderVariableSet.h>

using namespace toadlet::egg;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{

RenderVariableSet::RenderVariableSet(){
}

RenderVariableSet::~RenderVariableSet(){
}

bool RenderVariableSet::addBuffer(VariableBuffer::ptr buffer){
	BufferVariableSet set;
	set.buffer=buffer;
	mBufferVariables.add(set);
	return true;
}

void RenderVariableSet::removeBuffer(VariableBuffer::ptr buffer){
	int i;
	for(i=0;i<mBufferVariables.size();++i){
		if(mBufferVariables[i].buffer==buffer){
			mBufferVariables.removeAt(i);
			return;
		}
	}
}

// Search for the correct buffer and correct index
bool RenderVariableSet::addVariable(const String &name,RenderVariable::ptr variable){
	int i=name.find(".");
	BufferVariableSet *set=NULL;
	if(i>0){
		String possibleBufferName=name.substr(0,i);
		for(i=0;i<mBufferVariables.size();++i){
			if(possibleBufferName.equals(mBufferVariables[i].buffer->getVariableBufferFormat()->getName())){
				set=&mBufferVariables[i];
				break;
			}
		}
	}
	if(set==NULL){
		for(i=0;i<mBufferVariables.size();++i){
			if(mBufferVariables[i].buffer->getVariableBufferFormat()->getPrimary()){
				set=&mBufferVariables[i];
				break;
			}
		}
	}
	if(set==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"VariableBuffer not found for RenderVariable with name:"+name);
		return false;
	}

	VariableBufferFormat *format=set->buffer->getVariableBufferFormat();
	VariableBufferFormat::Variable *formatVariable=NULL;
	for(i=0;i<format->getSize();++i){
		if(name.equals(format->getVariable(i)->getFullName())){
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
	v.variable=variable;
	set->variables.add(v);
	
	return true;
}

void RenderVariableSet::removeVariable(RenderVariable::ptr variable){
	int i,j;
	for(i=0;i<mBufferVariables.size();++i){
		BufferVariableSet *set=&mBufferVariables[i];
		for(j=0;j<set->variables.size();++j){
			if(set->variables[j].variable==variable){
				set->variables.removeAt(j);
				return;
			}
		}
	}
}

}
}
