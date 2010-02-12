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

#ifndef TOADLET_KNOT_EVENT_BASECLIENTUPDATEEVENT_H
#define TOADLET_KNOT_EVENT_BASECLIENTUPDATEEVENT_H

#include <toadlet/egg/Event.h>

namespace toadlet{
namespace knot{
namespace event{

// This event is sent from the client each frame with the client dt and commands
//  The Counter could be replaced with a reliable packet sequence number, but that is not implemented yet
class BaseClientUpdateEvent:public egg::Event{
public:
	TOADLET_SHARED_POINTERS(BaseClientUpdateEvent);

	BaseClientUpdateEvent(int type):Event(type),
		mCounter(0),
		mDT(0)
	{}
	BaseClientUpdateEvent(int type,int counter,int dt):Event(type){
		mCounter=counter;
		mDT=dt;
	}

	int getCounter(){return mCounter;}
	int getDT(){return mDT;}

	virtual int read(egg::io::DataStream *stream){
		int amount=0;
		amount+=stream->readBigInt16(mCounter);
		amount+=stream->readBigInt32(mDT);
		return amount;
	}

	virtual int write(egg::io::DataStream *stream){
		int amount=0;
		amount+=stream->writeBigInt16(mCounter);
		amount+=stream->writeBigInt32(mDT);
		return amount;
	}

protected:
	short mCounter;
	int mDT;
};

}
}
}

#endif
