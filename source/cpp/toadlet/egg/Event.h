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

#ifndef TOADLET_EGG_EVENT_H
#define TOADLET_EGG_EVENT_H

#include <toadlet/egg/io/DataStream.h>

namespace toadlet{
namespace egg{

class Event{
public:
	TOADLET_SPTR(Event);

	enum{
		// General events
		Type_UNKNOWN=0,

		// Knot events
		Type_ROUTED=100,
		Type_PING,
	};
	
	Event():mType(Type_UNKNOWN){}
	Event(int type):mType(type){}
	virtual ~Event(){}

	virtual Event *getRootEvent(){return this;}

	inline int getType() const{return mType;}

	virtual int read(DataStream *stream){return 0;}
	virtual int write(DataStream *stream){return 0;}

protected:
	int mType;
};

}
}

#endif
