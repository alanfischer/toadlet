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
#include "GLVertexFormat.h"
#include "GLSLVertexLayout.h"
#include "GLRenderDevice.h"

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

GLSLProgram::GLSLProgram(GLRenderDevice *renderDevice):
	mDevice(NULL),

	mHandle(0),
	mNeedsLink(false)
	//mShaders
{
	mDevice=renderDevice;
}

GLSLProgram::~GLSLProgram(){
	destroy();
}

bool GLSLProgram::create(){
	bool result=createContext();

	if(mDevice!=NULL){
		mDevice->programCreated(this);
	}

	return result;
}

void GLSLProgram::destroy(){
	destroyContext();

	mShaders.clear();

	if(mDevice!=NULL){
		mDevice->programDestroyed(this);
	}

	int i;
	for(i=0;i<mLayouts.size();++i){
		if(mLayouts[i]!=NULL){
			mLayouts[i]->destroy();
			mLayouts[i]=NULL;
		}
	}
	mLayouts.clear();
}

bool GLSLProgram::attachShader(Shader *shader){
	GLSLShader *glshader=(GLSLShader*)shader->getRootShader();

	glAttachShader(mHandle,glshader->mHandle);

	mShaders.add(glshader);

	mNeedsLink=true;

	return true;
}

bool GLSLProgram::removeShader(Shader *shader){
	GLSLShader *glshader=(GLSLShader*)shader->getRootShader();

	glDetachShader(mHandle,glshader->mHandle);

	mShaders.remove(glshader);

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

GLSLVertexLayout *GLSLProgram::findVertexLayout(GLVertexFormat *vertexFormat){
	int handle=vertexFormat->mUniqueHandle;
	GLSLVertexLayout::ptr layout;
	if(handle<mLayouts.size()){
		layout=mLayouts[handle];
	}
	else{
		layout=GLSLVertexLayout::ptr(new GLSLVertexLayout(mDevice));
		layout->create(vertexFormat,this);
		mLayouts.resize(handle+1,NULL);
		mLayouts[handle]=layout;
	}
	return layout;
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
