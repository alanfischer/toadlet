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

#ifndef TOADLET_TADPOLE_QUERY_AABOXQUERY_H
#define TOADLET_TADPOLE_QUERY_AABOXQUERY_H

#include <toadlet/tadpole/query/SpacialQuery.h>

namespace toadlet{
namespace tadpole{
namespace query{

class TOADLET_API AABoxQuery:public SpacialQuery{
public:
	TOADLET_SHARED_POINTERS(AABoxQuery);

	AABoxQuery();
	virtual ~AABoxQuery();

	virtual peeper::Query *getRootQuery(){return this;}

	virtual void create(node::Scene *scene);
	virtual void destroy();

	virtual void setQueryDestroyedListener(peeper::QueryDestroyedListener *listener){mListener=listener;}

	virtual void setAABox(const AABox &box){mBox.set(box);}

	virtual void beginQuery(){}
	virtual void endQuery(){}
	virtual bool performQuery();

	virtual const egg::Collection<node::Node*> &getResult(){return mResults;}

public:
	node::Scene *mScene;

	peeper::QueryDestroyedListener *mListener;
	AABox mBox;
	egg::Collection<node::Node*> mResults;
};

}
}
}

#endif
