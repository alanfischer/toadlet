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

#ifndef TOADLET_KNOT_EVENT_BASECONNECTIONEVENT_H
#define TOADLET_KNOT_EVENT_BASECONNECTIONEVENT_H

#include <toadlet/egg/Event.h>
#include <toadlet/knot/Types.h>

namespace toadlet{
namespace knot{
namespace event{

class BaseConnectionEvent:public Event{
public:
	TOADLET_SPTR(BaseConnectionEvent);

	BaseConnectionEvent(int type):Event(type),
		mID(0)
	{}
	BaseConnectionEvent(int type,int id):Event(type){
		mID=id;
	}

	int getID(){return mID;}

	virtual int read(DataStream *stream){
		int amount=0;
		mID=stream->readBInt32();amount+=4;
		return amount;
	}

	virtual int write(DataStream *stream){
		int amount=0;
		amount+=stream->writeBInt32(mID);
		return amount;
	}

protected:
	int mID;
};

}
}
}

#endif
