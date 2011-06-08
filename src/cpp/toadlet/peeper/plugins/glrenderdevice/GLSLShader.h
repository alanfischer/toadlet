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

#ifndef TOADLET_PEEPER_GLSLSHADER_H
#define TOADLET_PEEPER_GLSLSHADER_H

#include "GLIncludes.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/peeper/Shader.h>

namespace toadlet{
namespace peeper{

class GLRenderDevice;
class GLSLProgram;

class GLSLShader:protected egg::BaseResource,public Shader{
	TOADLET_BASERESOURCE_PASSTHROUGH(Shader);
public:
	TOADLET_SHARED_POINTERS(GLSLShader);

	GLSLShader(GLRenderDevice *renderDevice);
	virtual ~GLSLShader();
	
	Shader *getRootShader(){return this;}

	bool create(ShaderType shaderType,const egg::String &code);
	void destroy();

	ShaderType getShaderType() const{return mShaderType;}

protected:
	bool createContext();
	bool destroyContext();

	GLRenderDevice *mDevice;

	ShaderType mShaderType;
	egg::String mCode;

	GLuint mHandle;
	GLenum mTarget;
	
	friend class GLRenderDevice;
	friend class GLSLProgram;
};

}
}

#endif
