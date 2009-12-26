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

#include <toadlet/tadpole/QueryManager.h>

using namespace toadlet::peeper;
using namespace toadlet::tadpole::query;

namespace toadlet{
namespace tadpole{

QueryManager::QueryManager(Engine *engine):
	//mQueries,

	mEngine(NULL)
{
	mEngine=engine;
}

QueryManager::~QueryManager(){
	destroy();
}

void QueryManager::destroy(){
}

RenderQuery::ptr QueryManager::createOcclusionQuery(){
	RenderQuery::ptr query=RenderQuery::ptr(mEngine->getRenderer()->createQuery());
	query->setQueryDestroyedListener(this);
	query->create(RenderQuery::QueryType_OCCLUSION);
	return query;
}

RenderQuery::ptr QueryManager::createFinishedQuery(){
	RenderQuery::ptr query=RenderQuery::ptr(mEngine->getRenderer()->createQuery());
	query->setQueryDestroyedListener(this);
	query->create(RenderQuery::QueryType_FINISHED);
	return query;
}

SpacialQuery::ptr QueryManager::createSegmentQuery(){
	return NULL;
}

SpacialQuery::ptr QueryManager::createSphereQuery(){
	return NULL;
}

AABoxQuery::ptr QueryManager::createAABoxQuery(){
	AABoxQuery::ptr query=AABoxQuery::ptr(new AABoxQuery());
	query->setQueryDestroyedListener(this);
	query->create(mEngine->getScene());
	return query;
}

SpacialQuery::ptr QueryManager::createNodeQuery(){
	return NULL;
}

void QueryManager::queryDestroyed(peeper::Query *query){
	mQueries.remove(query);
}

}
}
