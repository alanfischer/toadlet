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

#ifndef TOADLET_PEEPER_SHADERSTATE_H
#define TOADLET_PEEPER_SHADERSTATE_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/Resource.h>
#include <toadlet/peeper/Shader.h>
#include <toadlet/peeper/Texture.h>

namespace toadlet{
namespace peeper{

class TOADLET_API ShaderState:public Resource{
public:
	TOADLET_IPTR(ShaderState);

	virtual ~ShaderState(){}

	virtual ShaderState *getRootShaderState()=0;

	virtual bool create()=0;
	virtual void destroy()=0;

	virtual void setShader(Shader::ShaderType type,Shader::ptr shader)=0;
	virtual Shader::ptr getShader(Shader::ShaderType type)=0;

	virtual int getNumVariableBuffers(Shader::ShaderType type)=0;
	virtual VariableBufferFormat::ptr getVariableBufferFormat(Shader::ShaderType type,int i)=0;
};

}
}

#endif
