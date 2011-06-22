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

#include <toadlet/peeper/ShaderState.h>
#include "GLSLShader.h"
#include "GLSLVertexLayout.h"

namespace toadlet{
namespace peeper{

class GLVertexFormat;

class GLSLShaderState:public ShaderState{
public:
	TOADLET_SHARED_POINTERS(GLSLShaderState);

	GLSLShaderState(GLRenderDevice *renderDevice);
	virtual ~GLSLShaderState();

	ShaderState *getRootShaderState(){return this;}

	void setShaderStateDestroyedListener(ShaderStateDestroyedListener *listener){mListener=listener;}

	bool create();
	void destroy();

	void setShader(Shader::ShaderType type,Shader::ptr shader);
	Shader::ptr getShader(Shader::ShaderType type);

	bool activate();

	GLSLVertexLayout *findVertexLayout(GLVertexFormat *vertexFormat);

protected:
	class GLSLVariableBufferFormat:public VariableBufferFormat{
	public:
		TOADLET_SHARED_POINTERS(GLSLVariableBufferFormat);

		GLSLVariableBufferFormat(){}
		virtual ~GLSLVariableBufferFormat(){}

		egg::String getName(){return name;}
		int getSize(){return size;}
		int getNumVariables(){return variableNames.size();}
		egg::String getVariableName(int i){return variableNames[i];}
		int getVariableFormat(int i){return variableFormats[i];}
		int getVariableOffset(int i){return variableOffsets[i];}
		int getVariableIndex(int i){return variableIndexes[i];}

		egg::String name;
		int size;
		egg::Collection<egg::String> variableNames;
		egg::Collection<int> variableFormats;
		egg::Collection<int> variableOffsets;
		egg::Collection<int> variableIndexes;

		friend class GLSLShaderState;
	};

	bool createContext();
	bool destroyContext();
	bool reflect();

	GLRenderDevice *mDevice;

	ShaderStateDestroyedListener *mListener;
	GLuint mHandle;
	bool mNeedsLink;
	egg::Collection<Shader::ptr> mShaders;
	
	egg::Collection<VariableBufferFormat::ptr> mVariableBufferFormats;

	egg::Collection<GLSLVertexLayout::ptr> mLayouts;

	friend class GLRenderDevice;
	friend class GLSLVertexLayout;
};

}
}

#endif
