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

#include "GLSLVertexLayout.h"
#include "GLRenderDevice.h"
#include "GLSLShaderState.h"

#if defined(TOADLET_HAS_GLSL)

namespace toadlet{
namespace peeper{

GLSLVertexLayout::GLSLVertexLayout(GLRenderDevice *renderDevice){
	mDevice=renderDevice;
}

bool GLSLVertexLayout::create(GLVertexFormat *vertexFormat,GLSLShaderState *shaderState){
	mSemanticIndexes.resize(vertexFormat->getNumElements());

	int i;
	for(i=0;i<vertexFormat->getNumElements();++i){
		int location=glGetAttribLocation(shaderState->mHandle,vertexFormat->getElementName(i));
		if(location==-1){
			location=glGetAttribLocation(shaderState->mHandle,vertexFormat->getElementName(i)+vertexFormat->getElementIndex(i));
		}
		Logger::excess(Categories::TOADLET_PEEPER,
			String("Semantic name:")+vertexFormat->getElementName(i)+" found location:"+location);
		if(location==-1){
			location=GLRenderDevice::getFixedAttribFromSemantic(vertexFormat->getElementSemantic(i),vertexFormat->getElementIndex(i));
		}
		mSemanticIndexes[i]=location;
	}

	return true;
}

void GLSLVertexLayout::destroy(){
	mSemanticIndexes.clear();
}

}
}

#endif
