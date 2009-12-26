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

#ifndef TOADLET_TADPOLE_QUERYMANAGER_H
#define TOADLET_TADPOLE_QUERYMANAGER_H

#include <toadlet/peeper/RenderQuery.h>
#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/tadpole/query/AABoxQuery.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API QueryManager:public peeper::QueryDestroyedListener{
public:
	QueryManager(Engine *engine);
	virtual ~QueryManager();

	void destroy();

	peeper::RenderQuery::ptr createOcclusionQuery();
	peeper::RenderQuery::ptr createFinishedQuery();
	query::SpacialQuery::ptr createSegmentQuery();
	query::SpacialQuery::ptr createSphereQuery();
	query::AABoxQuery::ptr createAABoxQuery();
	query::SpacialQuery::ptr createNodeQuery();

	virtual void queryDestroyed(peeper::Query *query);

protected:
	egg::Collection<peeper::Query::ptr> mQueries;

	Engine *mEngine;
};

}
}

#endif
