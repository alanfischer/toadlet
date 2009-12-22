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

#ifndef TOADLET_PEEPER_GLQUERY_H
#define TOADLET_PEEPER_GLQUERY_H

#include "GLIncludes.h"
#include <toadlet/peeper/Query.h>

namespace toadlet{
namespace peeper{

class GLRenderer;

class GLQuery:public Query{
public:
	GLQuery(GLRenderer *renderer);
	virtual ~GLQuery();

	Query *getRootQuery(){return this;}

	virtual bool create(QueryType type);
	virtual void destroy();

	virtual void setQueryDestroyedListener(QueryDestroyedListener *listener){mListener=listener;}

	virtual void beginQuery();
	virtual void endQuery();

	virtual uint64 getResult();

protected:
	GLRenderer *mRenderer;

	QueryDestroyedListener *mListener;
	QueryType mQueryType;

	GLuint mHandle;

	friend class GLRenderer;
};

}
}

#endif
