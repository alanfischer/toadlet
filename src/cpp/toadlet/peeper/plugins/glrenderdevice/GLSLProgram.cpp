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

#include "GLSLProgram.h"

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

GLSLProgram::GLSLProgram(GLRenderDevice *renderDevice):
	mDevice(NULL),

	mHandle(0),
	mNeedsLink(false)
{
	mDevice=renderDevice;
}

GLSLProgram::~GLSLProgram(){
	destroy();
}

bool GLSLProgram::create(){
	destroy();

	return createContext();
}

void GLSLProgram::destroy(){
	destroyContext();
}

bool GLSLProgram::attachShader(Shader *shader){
	GLSLShader *glshader=(GLSLShader*)shader->getRootShader();

	glAttachShader(mHandle,glshader->mHandle);

	mNeedsLink=true;

	return true;
}

bool GLSLProgram::removeShader(Shader *shader){
	GLSLShader *glshader=(GLSLShader*)shader->getRootShader();

	glDetachShader(mHandle,glshader->mHandle);

	mNeedsLink=true;

	return true;
}

bool GLSLProgram::activate(){
	if(mNeedsLink){
		glLinkProgram(mHandle);

		GLint status=0;
		glGetProgramiv(mHandle,GL_LINK_STATUS,&status);
		if(status==GL_FALSE){
			Error::unknown("error linking program");
			return false;
		}
	}

	glUseProgram(mHandle);

	return true;
}

bool GLSLProgram::createContext(){
	mHandle=glCreateProgram();
	
	return true;
}

bool GLSLProgram::destroyContext(){
	if(mHandle!=0){
		glDeleteProgram(mHandle);
		mHandle=0;

		// Check this only if we had a handle, to eliminate errors at shutdown
		TOADLET_CHECK_GLERROR("GLSLProgram::destroy");
	}
	
	return true;
}

}
}
