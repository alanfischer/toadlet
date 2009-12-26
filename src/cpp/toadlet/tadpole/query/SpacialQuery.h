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

#ifndef TOADLET_TADPOLE_QUERY_SPACIALQUERY_H
#define TOADLET_TADPOLE_QUERY_SPACIALQUERY_H

#include <toadlet/peeper/Query.h>
#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{
namespace node{

class Scene;
class Node;

}
namespace query{

class SpacialQuery:public peeper::Query{
public:
	TOADLET_SHARED_POINTERS(SpacialQuery);

	virtual ~SpacialQuery(){}

	virtual void create(node::Scene *scene)=0;
	virtual void destroy()=0;

	virtual void beginQuery()=0;
	virtual void endQuery()=0;
	virtual bool performQuery()=0;

	virtual const egg::Collection<node::Node*> &getResult()=0;
};

}
}
}

#endif
