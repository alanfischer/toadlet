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

#include <toadlet/knot/SimplePredictedClient.h>

using namespace toadlet::egg;
using namespace toadlet::knot::event;

namespace toadlet{
namespace knot{

SimplePredictedClient::SimplePredictedClient(EventFactory *factory,Connector::ptr connector):SimpleClient(factory,connector),
	mLastReceivedClientFrameNumber(0),
	mLastAppliedClientFrameNumber(0)
{}

SimplePredictedClient::~SimplePredictedClient(){}

void SimplePredictedClient::handleServerUpdateEvent(BaseServerUpdateEvent::ptr event){
	mLastReceivedClientFrameNumber=event->getLastClientFrameNumber();
	while(mSentClientEvents.size()>1 && mSentClientEvents[0]->getClientFrameNumber()<=mLastReceivedClientFrameNumber){
		mSentClientEvents.removeAt(0);
	}
	mLastAppliedClientFrameNumber=mLastReceivedClientFrameNumber;
}

void SimplePredictedClient::sendClientUpdateEvent(BaseClientUpdateEvent::ptr event){
	mSentClientEvents.add(event);
	send(event);
}

Collection<BaseClientUpdateEvent::ptr> SimplePredictedClient::enumerateClientUpdateEvents(){
	Collection<BaseClientUpdateEvent::ptr> events;
	int i;
	for(i=0;i<mSentClientEvents.size();++i){
		if(mLastAppliedClientFrameNumber<mSentClientEvents[i]->getClientFrameNumber()){
			mLastAppliedClientFrameNumber=mSentClientEvents[i]->getClientFrameNumber();
			events.add(mSentClientEvents[i]);
		}
	}
	return events;
}

}
}
