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

namespace toadlet{
namespace knot{
namespace event{

class BaseConnectionEvent:public egg::Event{
public:
	TOADLET_SHARED_POINTERS(BaseConnectionEvent);

	BaseConnectionEvent(int type):Event(type),
		mID(0)
	{}
	BaseConnectionEvent(int type,int id):Event(type){
		mID=id;
	}

	int getID(){return mID;}

	virtual int read(egg::io::DataStream *stream){
		int amount=0;
		amount+=stream->readBigInt32(mID);
		return amount;
	}

	virtual int write(egg::io::DataStream *stream){
		int amount=0;
		amount+=stream->writeBigInt32(mID);
		return amount;
	}

protected:
	int mID;
};

}
}
}

#endif
