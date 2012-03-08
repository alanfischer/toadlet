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

#ifndef TOADLET_PEEPER_GLSLSHADERSTATE_H
#define TOADLET_PEEPER_GLSLSHADERSTATE_H

#include "GLSLShader.h"
#include "GLSLVertexLayout.h"
#include <toadlet/peeper/ShaderState.h>

#if defined(TOADLET_HAS_GLSL)

namespace toadlet{
namespace peeper{

class GLVertexFormat;

class GLSLShaderState:protected BaseResource,public ShaderState{
public:
	TOADLET_RESOURCE(GLSLShaderState,ShaderState);

	GLSLShaderState(GLRenderDevice *renderDevice);
	virtual ~GLSLShaderState();

	ShaderState *getRootShaderState(){return this;}

	bool create();
	void destroy();

	void setShader(Shader::ShaderType type,Shader::ptr shader);
	Shader::ptr getShader(Shader::ShaderType type);

	/// @todo: Integrate the ShaderType into these
	int getNumVariableBuffers(Shader::ShaderType type);
	VariableBufferFormat::ptr getVariableBufferFormat(Shader::ShaderType type,int i);

	bool activate();

	GLSLVertexLayout *findVertexLayout(GLVertexFormat *vertexFormat);

protected:
	bool createContext();
	bool destroyContext();
	bool link();
	bool reflect();

	GLRenderDevice *mDevice;

	GLuint mHandle;
	bool mNeedsLink;
	Collection<Shader::ptr> mShaders;
	
	Collection<VariableBufferFormat::ptr> mVariableBufferFormats;

	Collection<GLSLVertexLayout::ptr> mLayouts;

	friend class GLRenderDevice;
	friend class GLSLVertexLayout;
};

}
}

#endif

#endif
