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

#ifndef TOADLET_TADPOLE_SPACIALQUERY_H
#define TOADLET_TADPOLE_SPACIALQUERY_H

#include <toadlet/peeper/Query.h>
#include <toadlet/tadpole/Types.h>
#include <toadlet/tadpole/SpacialQueryResultsListener.h>

namespace toadlet{
namespace tadpole{

// TODO: The SpacialQuery structure really needs to be refactored
class TOADLET_API SpacialQuery:public peeper::Query{
public:
	TOADLET_SHARED_POINTERS(SpacialQuery);

	SpacialQuery();
	virtual ~SpacialQuery();

	virtual SpacialQuery *getRootQuery(){return this;}

	virtual void create();
	virtual void destroy();

	virtual void setQueryDestroyedListener(peeper::QueryDestroyedListener *listener){mDestroyedListener=listener;}
	virtual peeper::QueryDestroyedListener *getQueryDestroyedListener() const{return mDestroyedListener;}
	virtual void setResultsListener(SpacialQueryResultsListener *listener){mResultsListener=listener;}
	virtual SpacialQueryResultsListener *getSpacialQueryResultsListener() const{return mResultsListener;}

	virtual void beginQuery(){}
	virtual void endQuery(){}

protected:
	peeper::QueryDestroyedListener *mDestroyedListener;
	SpacialQueryResultsListener *mResultsListener;
};

}
}

#endif
