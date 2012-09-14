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

#ifndef TOADLET_PEEPER_D3D9SHADERSTATE_H
#define TOADLET_PEEPER_D3D9SHADERSTATE_H

#include "D3D9Includes.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/ShaderState.h>

namespace toadlet{
namespace peeper{

class D3D9RenderDevice;

class TOADLET_API D3D9ShaderState:public BaseResource,public ShaderState{
public:
	TOADLET_RESOURCE(D3D9ShaderState,ShaderState);

	D3D9ShaderState(D3D9RenderDevice *renderDevice);

	ShaderState *getRootShaderState(){return this;}

	bool create(){return true;}
	void destroy();

	void setShader(Shader::ShaderType type,Shader::ptr shader);
	Shader::ptr getShader(Shader::ShaderType type);

	int getNumVariableBuffers(Shader::ShaderType type);
	VariableBufferFormat::ptr getVariableBufferFormat(Shader::ShaderType type,int i);

	bool activate();

protected:
	D3D9RenderDevice *mDevice;
	Collection<Shader::ptr> mShaders;
	
	friend class D3D9RenderDevice;
};

}
}

#endif
