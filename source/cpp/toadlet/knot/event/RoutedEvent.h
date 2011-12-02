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

#ifndef TOADLET_KNOT_EVENT_ROUTEDEVENT_H
#define TOADLET_KNOT_EVENT_ROUTEDEVENT_H

#include <toadlet/egg/Event.h>
#include <toadlet/knot/Types.h>

namespace toadlet{
namespace knot{
namespace event{

/// @todo: Replace IDs with an Address structure
class RoutedEvent:public Event{
public:
	TOADLET_SHARED_POINTERS(RoutedEvent);

	RoutedEvent(Event::ptr event,int sourceID,int destinationID):Event(Type_ROUTED),mEvent(event),mSourceID(sourceID),mDestinationID(destinationID){}

	Event *getRootEvent(){return mEvent;}

	// RoutedEvents do not send themselves over the connection, instead they are left to the Connections to deal with
	int read(DataStream *stream){return mEvent->read(stream);}
	int write(DataStream *stream){return mEvent->write(stream);}

	void setEvent(Event::ptr event){mEvent=event;}
	Event::ptr getEvent(){return mEvent;}
	
	void setSourceID(int sourceID){mSourceID=sourceID;}
	int getSourceID(){return mSourceID;}

	void setDestinationID(int destinationID){mDestinationID=destinationID;}
	int getDestinationID(){return mDestinationID;}
	
protected:
	Event::ptr mEvent;
	int mSourceID;
	int mDestinationID;
};

}
}
}

#endif