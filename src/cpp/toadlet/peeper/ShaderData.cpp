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

#include <toadlet/peeper/ShaderData.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

ShaderData::ShaderData(Shader::ptr shader,ConstantBuffer::ptr buffer){
	this->shader=shader;
	this->buffer=buffer;
}

ShaderData::~ShaderData(){
	destroy();
}

void ShaderData::destroy(){
	int i;
	for(i=0;i<parameters.size();++i){
		delete parameters[i];
	}
	parameters.clear();
}

bool ShaderData::addParameter(const String &name,UpdateParameter::ptr parameter){
	int location=shader->findConstant(name);
	parameters.add(new ParameterData(name,parameter,location));
	return true;
}

void ShaderData::update(){
	int i;
	for(i=0;i<parameters.size();++i){
		ParameterData *data=parameters[i];
		data->parameter->update();
		buffer->setConstant(data->location,data->parameter->getData(),data->parameter->getSize());
	}
}

}
}
