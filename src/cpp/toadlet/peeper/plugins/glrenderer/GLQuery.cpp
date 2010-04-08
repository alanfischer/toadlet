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

#include "GLQuery.h"
#include "GLRenderer.h"
#include <toadlet/egg/Error.h>

using namespace toadlet::egg;

namespace toadlet{
namespace peeper{

GLQuery::GLQuery(GLRenderer *renderer):
	mRenderer(NULL),

	mQueryType(QueryType_UNKNOWN),

	mHandle(0)
{
	mRenderer=renderer;
}

GLQuery::~GLQuery(){
	destroy();
}

bool GLQuery::create(QueryType queryType){
	destroy();

	if(queryType!=QueryType_OCCLUSION){
		Error::unknown("GLQuery only supports QueryType_OCCLUSION");
		return false;
	}
	
	mQueryType=queryType;

	glGenQueries(1,&mHandle);
	
	TOADLET_CHECK_GLERROR("GLQuery::create");

	return true;
}

void GLQuery::destroy(){
	if(mHandle!=0){
		glDeleteQueries(1,&mHandle);
		mHandle=0;
	}

	TOADLET_CHECK_GLERROR("GLQuery::destroy");
}

void GLQuery::beginQuery(){
	glBeginQuery(GL_SAMPLES_PASSED_ARB,mHandle);
}

void GLQuery::endQuery(){
	glEndQuery(GL_SAMPLES_PASSED_ARB);
}

uint64 GLQuery::getResult(){
	GLuint result=0;

    glGetQueryObjectuiv(mHandle,GL_QUERY_RESULT_ARB,&result);
	TOADLET_CHECK_GLERROR("GLQuery::getResult");

	return result;
}

}
}
