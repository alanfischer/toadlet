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

#if defined(TOADLET_HAS_GLSL)

namespace toadlet{
namespace peeper{

class GLRenderDevice;
class GLSLShaderState;

class GLSLShader:public BaseResource,public Shader{
public:
	TOADLET_RESOURCE(GLSLShader,Shader);

	GLSLShader(GLRenderDevice *renderDevice);
	
	Shader *getRootShader(){return this;}

	bool create(ShaderType shaderType,const char *profile,const char *code);
	void destroy();

	ShaderType getShaderType() const{return mShaderType;}
	String getProfile() const{return mProfile;}

protected:
	bool createContext();
	bool destroyContext();

	GLRenderDevice *mDevice;

	ShaderType mShaderType;
	String mProfile;
	String mCode;

	GLuint mHandle;
	GLenum mTarget;
	
	friend class GLRenderDevice;
	friend class GLSLShaderState;
};

}
}

#endif

#endif
