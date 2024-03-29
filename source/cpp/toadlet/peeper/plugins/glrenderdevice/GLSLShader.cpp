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

#include "GLSLShader.h"
#include "GLRenderDevice.h"

#if defined(TOADLET_HAS_GLSL)

namespace toadlet{
namespace peeper{

GLSLShader::GLSLShader(GLRenderDevice *renderDevice):
	mDevice(NULL),

	mShaderType((ShaderType)0),
	//mProfile,
	//mCode,

	mHandle(0),
	mTarget(0)
{
	mDevice=renderDevice;
}

bool GLSLShader::create(ShaderType shaderType,const char *profile,const char *code){
	mShaderType=shaderType;
	mProfile=profile;
	mCode=code;
	bool result=createContext();

	return result;
}

void GLSLShader::destroy(){
	destroyContext();

	BaseResource::destroy();
}

bool GLSLShader::createContext(){
	// Create context data
	switch(mShaderType){
		case ShaderType_VERTEX:
			mTarget=GL_VERTEX_SHADER;
		break;
		case ShaderType_FRAGMENT:
			mTarget=GL_FRAGMENT_SHADER;
		break;
		#if defined(GL_GEOMETRY_SHADER_EXT)
			case ShaderType_GEOMETRY:
				mTarget=GL_GEOMETRY_SHADER_EXT;
			break;
		#endif
		default:
			return false;
		break;
	}

	const char *string=mCode.c_str();
	int length=mCode.length();

	if(length==0){
		return true;
	}

	mHandle=glCreateShader(mTarget);
	
	glShaderSource(mHandle,1,&string,&length);
	glCompileShader(mHandle);
	
	GLint status=0;
	glGetShaderiv(mHandle,GL_COMPILE_STATUS,&status);
	if(status==GL_FALSE){
		String logString;
		GLint length=0;
		glGetShaderiv(mHandle,GL_INFO_LOG_LENGTH,&length);
		if(length>0){
			char *log=new char[length];
			glGetShaderInfoLog(mHandle,length,NULL,log);
			logString=log;
			delete log;
		}

		TOADLET_CHECK_GLERROR("GLSLShader::createContext");
		
		Log::debug(Categories::TOADLET_PEEPER,"program being compiled:\n"+mCode);
		Error::unknown(Categories::TOADLET_PEEPER,"glsl compile error:\n"+logString);

		return false;
	}
	
	TOADLET_CHECK_GLERROR("GLSLShader::createContext");

	return true;
}

bool GLSLShader::destroyContext(){
	if(mHandle!=0){
		glDeleteShader(mHandle);
		mHandle=0;
		mTarget=0;

		// Check this only if we had a handle, to eliminate errors at shutdown
		TOADLET_CHECK_GLERROR("GLSLShader::destroy");
	}
	
	return true;
}

}
}

#endif
