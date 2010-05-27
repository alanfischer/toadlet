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

#ifndef TOADLET_KNOT_EVENT_BASESERVERUPDATEEVENT_H
#define TOADLET_KNOT_EVENT_BASESERVERUPDATEEVENT_H

#include <toadlet/egg/Event.h>

namespace toadlet{
namespace knot{
namespace event{

// This event is sent from the server occasionally to update information about objects or possibly confirm the receipt of a ClientUpdateEvent
//  The LastCounter could be replaced with a reliable packet sequence number, but that is not implemented yet
class BaseServerUpdateEvent:public egg::Event{
public:
	TOADLET_SHARED_POINTERS(BaseServerUpdateEvent);

	BaseServerUpdateEvent(int type):egg::Event(type),
		mServerFrameNumber(0),
		mLastClientFrameNumber(0),
		mTime(0)
	{}
	BaseServerUpdateEvent(int type,int serverFrameNumber,int lastClientFrameNumber,int time):Event(type){
		mServerFrameNumber=serverFrameNumber;
		mLastClientFrameNumber=lastClientFrameNumber;
		mTime=time;
	}

	int getServerFrameNumber(){return mServerFrameNumber;}
	int getLastClientFrameNumber(){return mLastClientFrameNumber;}
	int getTime(){return mTime;}

	virtual int read(egg::io::DataStream *stream){
		int amount=0;
		amount+=stream->readBigInt32(mServerFrameNumber);
		amount+=stream->readBigInt32(mLastClientFrameNumber);
		amount+=stream->readBigInt32(mTime);
		return amount;
	}

	virtual int write(egg::io::DataStream *stream){
		int amount=0;
		amount+=stream->writeBigInt32(mServerFrameNumber);
		amount+=stream->writeBigInt32(mLastClientFrameNumber);
		amount+=stream->writeBigInt32(mTime);
		return amount;
	}

protected:
	int mServerFrameNumber; // Use an int, a short overflows too quickly.
	int mLastClientFrameNumber; // Use an int, a short overflows too quickly.
	int mTime;
};

}
}
}

#endif
