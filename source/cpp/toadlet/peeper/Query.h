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

#ifndef TOADLET_PEEPER_QUERY_H
#define TOADLET_PEEPER_QUERY_H

#include <toadlet/egg/Resource.h>
#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TOADLET_API Query:public Resource{
public:
	TOADLET_INTERFACE(Query);

	enum QueryType{
		QueryType_UNKNOWN,
		QueryType_OCCLUSION,
		QueryType_FINISHED,
	};

	virtual ~Query(){}

	virtual Query *getRootQuery()=0;

	virtual bool create(QueryType type)=0;
	virtual void destroy()=0;

	virtual void beginQuery()=0;
	virtual void endQuery()=0;

	virtual uint64 getResult()=0;
};

}
}

#endif
