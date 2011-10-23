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

#if defined(TOADLET_HAS_GLSL)

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
	mNeedsLink=true;
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
	if(mShaders.size()<=type){
		mShaders.resize(type+1);
	}

	GLSLShader *glshader=mShaders[type]!=NULL?(GLSLShader*)mShaders[type]->getRootShader():NULL;
	if(glshader!=NULL){
		glDetachShader(mHandle,glshader->mHandle);
	}

	mShaders[type]=shader;

	glshader=mShaders[type]!=NULL?(GLSLShader*)mShaders[type]->getRootShader():NULL;
	if(glshader!=NULL){
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
		TOADLET_TRY
			link();
		TOADLET_CATCH(const Exception &){}
	}

	if(type==Shader::ShaderType_VERTEX){
		return mVariableBufferFormats.size();
	}
	else{
		return 0;
	}
}

VariableBufferFormat::ptr GLSLShaderState::getVariableBufferFormat(Shader::ShaderType type,int i){
	if(mNeedsLink){
		TOADLET_TRY
			link();
		TOADLET_CATCH(const Exception &){}
	}

	if(type==Shader::ShaderType_VERTEX){
		return mVariableBufferFormats[i];
	}
	else{
		return 0;
	}
}

bool GLSLShaderState::activate(){
	if(mNeedsLink){
		TOADLET_TRY
			link();
		TOADLET_CATCH(const Exception &){}
	}

	// Only use the program if we succesfully linked
	if(!mNeedsLink){
		glUseProgram(mHandle);
	}
	else{
		glUseProgram(0);
	}

	TOADLET_CHECK_GLERROR("GLSLShaderState::activate");

	return !mNeedsLink;
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
	int i;
	for(i=0;i<mShaders.size();++i){
		if(mShaders[i]!=NULL) break;
	}
	if(i==mShaders.size()){
		return false;
	}

	glLinkProgram(mHandle);

	GLint status=0;
	glGetProgramiv(mHandle,GL_LINK_STATUS,&status);
	if(status==GL_FALSE){
		String logString;
		GLint length=0;
		glGetProgramiv(mHandle,GL_INFO_LOG_LENGTH,&length);
		if(length>0){
			char *log=new char[length];
			glGetProgramInfoLog(mHandle,length,NULL,log);
			logString=log;
			delete log;
		}
		Error::unknown(Categories::TOADLET_PEEPER,"glsl link error:\n"+logString);
		return false;
	}

	TOADLET_CHECK_GLERROR("GLSLShaderState::link");

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

	Collection<VariableBufferFormat::Variable::ptr> primaryVariables(numUniforms);

	int i;
	for(i=0;i<numUniforms;++i){
		glGetActiveUniform(mHandle,i,sizeof(name),&nameLength,&size,&type,name);
		int location=glGetUniformLocation(mHandle,name);

		VariableBufferFormat::Variable::ptr variable(new VariableBufferFormat::Variable());
		variable->setName(name);
		variable->setFormat(GLRenderDevice::getVariableFormat(type));
		variable->setSize(size);
		variable->setIndex(location);
		primaryVariables[i]=variable;
	}

	#if defined(TOADLET_HAS_GLUBOS)
		if(mDevice->gl_version>=31){
			int numUniformBlocks=0;
			glGetProgramiv(mHandle,GL_ACTIVE_UNIFORM_BLOCKS,&numUniformBlocks);

			for(i=0;i<numUniformBlocks;++i){
				glGetActiveUniformBlockName(mHandle,i,sizeof(name),&nameLength,name);
				glGetActiveUniformBlockiv(mHandle,i,GL_UNIFORM_BLOCK_DATA_SIZE,&dataSize);
				glGetActiveUniformBlockiv(mHandle,i,GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS,&numUniforms);

				VariableBufferFormat::ptr variableFormat(new VariableBufferFormat(false,name,dataSize,numUniforms));

				GLint *indexes=new GLint[numUniforms];
				glGetActiveUniformBlockiv(mHandle,i,GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES,indexes);

				int j;
				for(j=0;j<numUniforms;++j){
					Logger::alert(String("Uniform block uniform index:")+indexes[j]);

					int index=indexes[j];
					variableFormat->setStructVariable(j,primaryVariables[index]);
					variableFormat->getStructVariable(j)->setIndex(index);

					primaryVariables[index]=NULL;
				}

				delete[] indexes;

				variableFormat->compile();

				mVariableBufferFormats.add(variableFormat);
			}
		}
	#endif

	dataSize=0;
	for(i=0;i<primaryVariables.size();++i){
		if(primaryVariables[i]!=NULL){
			primaryVariables[i]->setOffset(dataSize);
			dataSize+=VariableBufferFormat::getFormatSize(primaryVariables[i]->getFormat());
		}
		else{
			primaryVariables.removeAt(i);
			i--;
		}
	}

	VariableBufferFormat::ptr primaryFormat(new VariableBufferFormat(true,(char*)NULL,dataSize,primaryVariables.size()));
	for(i=0;i<primaryFormat->getStructSize();++i){
		primaryFormat->setStructVariable(i,primaryVariables[i]);
	}

	if(primaryFormat->getStructSize()>0){
		primaryFormat->compile();

		mVariableBufferFormats.insert(mVariableBufferFormats.begin(),primaryFormat);
	}

	TOADLET_CHECK_GLERROR("GLSLShaderState::reflect");

	return true;
}

}
}

#endif
