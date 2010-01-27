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

#ifndef TOADLET_KNOT_EVENTCONNECTION_H
#define TOADLET_KNOT_EVENTCONNECTION_H

#include <toadlet/egg/SharedPointer.h>
#include <toadlet/egg/WeakPointer.h>
#include <toadlet/egg/Event.h>

namespace toadlet{
namespace knot{

/// An EventConnection sends & receives events
/// @todo: There currently isnt any clean way to include routing information in this class, so that ends up being done
///  by the sub-classes.  Perhaps I could come up with a standard "address" structure that could be added to these methods.
///  Or switch it to include the routing information in the Events, if they are off a certain type, say a Routed Event.
///  Would it be desireable to use the Decorator pattern here?  A RoutedEvent could take an Event, add the routing information, and pass it on.
///  Then the Client/Server specific EventConnections could look for Events that ARE RoutedEvents, and then extract the necessary information and pass it on.
///  Non routed events just get sent to the end point of the connection, and then are to be dealt with.
class EventConnection{
public:
	TOADLET_SHARED_POINTERS(EventConnection);

	virtual ~EventConnection(){}

	virtual bool opened()=0;
	virtual void close()=0;

	/// Send an Event
	virtual bool send(egg::Event::ptr event)=0;
	/// Receive an Event
	virtual egg::Event::ptr receive()=0;
	
	// Some EventConnections may requre an update by the main thread
	virtual int update()=0;
};

}
}

#endif
