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

#ifndef TOADLET_KNOT_SIMPLEPREDICTEDCLIENT_H
#define TOADLET_KNOT_SIMPLEPREDICTEDCLIENT_H

#include <toadlet/egg/Collection.h>
#include <toadlet/knot/SimpleClient.h>
#include <toadlet/knot/event/BaseClientUpdateEvent.h>
#include <toadlet/knot/event/BaseServerUpdateEvent.h>

namespace toadlet{
namespace knot{

class TOADLET_API SimplePredictedClient:public SimpleClient{
public:
	TOADLET_SPTR(SimplePredictedClient);

	SimplePredictedClient(EventFactory *factory=NULL,Connector::ptr connector=Connector::ptr());
	virtual ~SimplePredictedClient();

	virtual void handleServerUpdateEvent(event::BaseServerUpdateEvent::ptr event);

	virtual void sendClientUpdateEvent(event::BaseClientUpdateEvent::ptr event);

	virtual Collection<event::BaseClientUpdateEvent::ptr> enumerateClientUpdateEvents();
	
protected:
	Collection<event::BaseClientUpdateEvent::ptr> mSentClientEvents;
	int mLastReceivedClientFrameNumber;
	int mLastAppliedClientFrameNumber;
};

}
}

#endif
