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

#include <toadlet/peeper/Renderer.h>
#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/query/SegmentQuery.h>
#include <toadlet/tadpole/query/SphereQuery.h>
#include <toadlet/tadpole/query/AABoxQuery.h>
#include <toadlet/tadpole/query/NodeQuery.h>

namespace toadlet{
namespace tadpole{

class TOADLET_API QueryManager:public peeper::QueryDestroyedListener{
public:
	QueryManager(Engine *engine);
	virtual ~QueryManager();

	void destroy();

	peeper::Query::ptr createOcclusionQuery();
	peeper::Query::ptr createFinishedQuery();
	query::SegmentQuery::ptr createSegmentQuery();
	query::SphereQuery::ptr createSphereQuery();
	query::AABoxQuery::ptr createAABoxQuery();
	query::NodeQuery::ptr createNodeQuery();

	virtual void queryDestroyed(peeper::Query *query);
	virtual void queryDestroyed(query::SpacialQuery *query);

protected:
	egg::Collection<peeper::Query::ptr> mVisualQueries;
	egg::Collection<query::SpacialQuery::ptr> mSpacialQueries;

	Engine *mEngine;
};

}
}

#endif
