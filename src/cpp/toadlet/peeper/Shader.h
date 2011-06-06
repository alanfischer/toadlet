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

#ifndef TOADLET_PEEPER_SHADER_H
#define TOADLET_PEEPER_SHADER_H

#include <toadlet/egg/Resource.h>
#include <toadlet/egg/String.h>
#include <toadlet/peeper/RenderDevice.h>

namespace toadlet{
namespace peeper{

class Shader:public egg::Resource{
public:
	TOADLET_SHARED_POINTERS(Shader);

	enum ShaderType{
		ShaderType_GEOMETRY,
		ShaderType_VERTEX,
		ShaderType_FRAGMENT,
	};

	enum Language{
		Language_UNKNOWN=0,
		Language_GLSL,
	};

	Shader(RenderDevice *renderDevice,ShaderType shaderType,Language language);
	virtual ~Shader(){}


	virtual Shader *getRootShader()=0;

	virtual void create(ShaderType shaderType)=0;
	virtual void destroy()=0;

	virtual ShaderType getShaderType() const=0;
};

}
}

#endif
