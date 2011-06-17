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

#ifndef TOADLET_PEEPER_SHADERDATA_H
#define TOADLET_PEEPER_SHADERDATA_H

#include <toadlet/egg/Collection.h>
#include <toadlet/peeper/ConstantBuffer.h>
#include <toadlet/peeper/UpdateParameter.h>
#include <toadlet/peeper/Shader.h>

namespace toadlet{
namespace peeper{

class TOADLET_API ShaderData{
public:
	TOADLET_SHARED_POINTERS(ShaderData);

	class ParameterData{
	public:
		ParameterData(const egg::String &inname,UpdateParameter::ptr inparameter,int inlocation):
			name(inname),
			parameter(inparameter),
			location(inlocation)
		{}

		egg::String name;
		UpdateParameter::ptr parameter;
		int location;
	};

	ShaderData(Shader::ptr shader,ConstantBuffer::ptr buffer);
	virtual ~ShaderData();

	void destroy();

	bool addParameter(const egg::String &name,UpdateParameter::ptr parameter);

	void update();

	Shader::ptr shader;
	ConstantBuffer::ptr buffer;
	egg::Collection<ParameterData*> parameters;
};

}
}

#endif
