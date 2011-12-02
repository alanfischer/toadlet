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

#ifndef TOADLET_KNOT_PINGEVENT_H
#define TOADLET_KNOT_PINGEVENT_H

#include <toadlet/egg/Event.h>

namespace toadlet{
namespace knot{

class PingEvent:public Event{
public:
	TOADLET_SHARED_POINTERS(PingEvent);

	PingEvent():Event(Type_PING),mSendTime(0){}

	int read(DataStream *stream){
		mSendTime=stream->readBInt64();
		return 8;
	}
	
	int write(DataStream *stream){
		return stream->writeBInt64(mSendTime);
	}

	void setSendTime(int64 sendTime){mSendTime=sendTime;}
	int64 getSendTime() const{return mSendTime;}
	
protected:
	int64 mSendTime;
};

}
}

#endif
