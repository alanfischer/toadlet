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

#ifndef TOADLET_PEEPER_D3D910UERY_H
#define TOADLET_PEEPER_D3D910UERY_H

#include "D3D10Includes.h"
#include <toadlet/peeper/Query.h>

namespace toadlet{
namespace peeper{

class D3D10Renderer;

class TOADLET_API D3D10Query:public Query{
public:
	D3D10Query(D3D10Renderer *renderer);
	virtual ~D3D10Query();

	Query *getRootQuery(){return this;}

	bool create(QueryType queryType);
	void destroy();

	void beginQuery();
	void endQuery();
	bool performQuery(){return false;}

	uint64 getResult();

protected:
	static D3D10_QUERY getD3D10_QUERY(QueryType queryType);

	D3D10Renderer *mRenderer;

	QueryType mQueryType;

	ID3D10Query *mQuery;

	friend class D3D10Renderer;
};

}
}

#endif
