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

#include "GLSLShaderState.h"
#include "GLVertexFormat.h"
#include "GLSLVertexLayout.h"
#include "GLRenderDevice.h"

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

GLSLShaderState::GLSLShaderState(GLRenderDevice *renderDevice):
	mDevice(NULL),

	mListener(NULL),
	mHandle(0),
	mNeedsLink(false)
	//mShaders
{
	mDevice=renderDevice;
}

GLSLShaderState::~GLSLShaderState(){
	destroy();
}

bool GLSLShaderState::create(){
	bool result=createContext();

	if(mDevice!=NULL){
		mDevice->shaderStateCreated(this);
	}

	return result;
}

void GLSLShaderState::destroy(){
	destroyContext();

	mShaders.clear();

	if(mDevice!=NULL){
		mDevice->shaderStateDestroyed(this);
	}

	int i;
	for(i=0;i<mLayouts.size();++i){
		if(mLayouts[i]!=NULL){
			mLayouts[i]->destroy();
			mLayouts[i]=NULL;
		}
	}
	mLayouts.clear();

	if(mListener!=NULL){
		mListener->shaderStateDestroyed(this);
		mListener=NULL;
	}
}

void GLSLShaderState::setShader(Shader::ShaderType type,Shader::ptr shader){
	GLSLShader *glshader=(GLSLShader*)shader->getRootShader();

	if(mShaders.size()<=type){
		mShaders.resize(type+1);
	}

	if(mShaders[type]!=NULL){
		glDetachShader(mHandle,glshader->mHandle);
	}

	mShaders[type]=shader;

	if(mShaders[type]!=NULL){
		glAttachShader(mHandle,glshader->mHandle);
	}

	mNeedsLink=true;
}

Shader::ptr GLSLShaderState::getShader(Shader::ShaderType type){
	if(mShaders.size()<=type){
		return NULL;
	}

	return mShaders[type];
}

int GLSLShaderState::getNumVariableBuffers(Shader::ShaderType type){
	if(mNeedsLink){
		link();
	}

	return mVariableBufferFormats.size();
}

VariableBufferFormat::ptr GLSLShaderState::getVariableBufferFormat(Shader::ShaderType type,int i){
	if(mNeedsLink){
		link();
	}

	return mVariableBufferFormats[i];
}

bool GLSLShaderState::activate(){
	if(mNeedsLink){
		link();
	}

	glUseProgram(mHandle);

	return true;
}

GLSLVertexLayout *GLSLShaderState::findVertexLayout(GLVertexFormat *vertexFormat){
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

bool GLSLShaderState::createContext(){
	mHandle=glCreateProgram();
	
	return true;
}

bool GLSLShaderState::destroyContext(){
	if(mHandle!=0){
		glDeleteProgram(mHandle);
		mHandle=0;

		// Check this only if we had a handle, to eliminate errors at shutdown
		TOADLET_CHECK_GLERROR("GLSLProgram::destroy");
	}
	
	return true;
}

bool GLSLShaderState::link(){
	glLinkProgram(mHandle);

	GLint status=0;
	glGetProgramiv(mHandle,GL_LINK_STATUS,&status);
	if(status==GL_FALSE){
		Error::unknown("error linking program");
		return false;
	}

	reflect();

	mNeedsLink=false;

	return true;
}

bool GLSLShaderState::reflect(){
	int numUniforms=0;
	int dataSize=0;
	int size=0;
	GLenum type=0;
	GLchar name[128];
	GLsizei nameLength=0;

	glGetProgramiv(mHandle,GL_ACTIVE_UNIFORMS,&numUniforms);

	VariableBufferFormat::ptr defaultFormat(new VariableBufferFormat((char*)NULL,0,numUniforms));
	defaultFormat->default=true;

	int i;
	for(i=0;i<numUniforms;++i){
		glGetActiveUniform(mHandle,i,sizeof(name),&nameLength,&size,&type,name);

		defaultFormat->variableNames[i]=name;
		defaultFormat->variableFormats[i]=GLRenderDevice::getVariableFormat(type);
		defaultFormat->variableOffsets[i]=dataSize;
		defaultFormat->variableIndexes[i]=i;

		dataSize+=VariableBufferFormat::getFormatSize(defaultFormat->variableFormats[i]);
	}

	defaultFormat->dataSize=dataSize;

	#if defined(TOADLET_HAS_GLUBOS)
		int numUniformBlocks=0;
		glGetProgramiv(mHandle,GL_ACTIVE_UNIFORM_BLOCKS,&numUniformBlocks);

		for(i=0;i<numUniformBlocks;++i){
			glGetActiveUniformBlockName(mHandle,i,sizeof(name),&nameLength,name);
			glGetActiveUniformBlockiv(mHandle,i,GL_UNIFORM_BLOCK_DATA_SIZE,&dataSize);
			glGetActiveUniformBlockiv(mHandle,i,GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,&numUniforms);

			VariableBufferFormat::ptr variableFormat(new VariableBufferFormat(name,dataSize,numUniforms));

			GLint *indexes=new GLint[numUniforms];
			glGetActiveUniformBlockiv(mHandle,i,GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,indexes);

			int j;
			for(j=0;j<numUniforms;++j){
				Logger::alert(String("Uniform block uniform index:")+indexes[j]);

				int index=indexes[j];
				variableFormat->variableNames[j]=defaultFormat->variableNames[index];
				variableFormat->variableFormats[j]=defaultFormat->variableFormats[index];
				variableFormat->variableOffsets[j]=defaultFormat->variableOffsets[index];
				variableFormat->variableIndexes[j]=index;

				defaultFormat->variableIndexes[index]=-1;
			}

			delete[] indexes;

			mVariableBufferFormats.add(variableFormat);
		}

		for(i=0;i<defaultFormat->variableOffsets.size();++i){
			if(defaultFormat->variableIndexes[i]==-1){
				defaultFormat->variableNames.removeAt(i);
				defaultFormat->variableFormats.removeAt(i);
				defaultFormat->variableOffsets.removeAt(i);
				defaultFormat->variableIndexes.removeAt(i);
				i--;
			}
		}
	#endif

	if(defaultFormat->variableNames.size()>0){
		mVariableBufferFormats.insert(mVariableBufferFormats.begin(),defaultFormat);
	}

	return true;
}

}
}
