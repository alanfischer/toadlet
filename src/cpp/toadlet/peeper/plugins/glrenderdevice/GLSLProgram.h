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

#ifndef TOADLET_PEEPER_GLSLPROGRAM_H
#define TOADLET_PEEPER_GLSLPROGRAM_H

#include "GLSLShader.h"
#include "GLSLVertexLayout.h"

namespace toadlet{
namespace peeper{

class GLVertexFormat;

class GLSLProgram{
public:
	TOADLET_SHARED_POINTERS(GLSLProgram);

	GLSLProgram(GLRenderDevice *renderDevice);
	virtual ~GLSLProgram();
	
	bool create();
	void destroy();

	bool attachShader(Shader *shader);
	bool removeShader(Shader *shader);

	bool activate();

	GLSLVertexLayout *findVertexLayout(GLVertexFormat *vertexFormat);

protected:
	bool createContext();
	bool destroyContext();

	GLRenderDevice *mDevice;

	GLuint mHandle;
	bool mNeedsLink;
	egg::Collection<GLSLShader*> mShaders;
	
	egg::Collection<GLSLVertexLayout::ptr> mLayouts;

	friend class GLRenderDevice;
	friend class GLSLVertexLayout;
};

}
}

#endif
