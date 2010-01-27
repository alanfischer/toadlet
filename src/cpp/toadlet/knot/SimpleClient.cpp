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

#include <toadlet/egg/Logger.h>
#include <toadlet/egg/System.h>
#include <toadlet/knot/SimpleClient.h>
#include <toadlet/knot/event/RoutedEvent.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;

namespace toadlet{
namespace knot{

SimpleClient::SimpleClient(EventFactory *eventFactory,Connector::ptr connector):SimpleEventConnection(eventFactory),
	mClientID(0)
	//mConnector
{
	if(connector!=NULL){
		setConnector(connector);
	}
}

SimpleClient::~SimpleClient(){
	close();
}

void SimpleClient::close(){
	if(mConnector!=NULL){
		mConnector->close();
	}
	setConnector(NULL);

	SimpleEventConnection::close();
}

void SimpleClient::setConnector(Connector::ptr connector){
	if(mConnector!=NULL){
		mConnector->removeConnectorListener(this,true);
	}

	mConnector=connector;

	if(mConnector!=NULL){
		mConnector->addConnectorListener(this,true);
	}
}

bool SimpleClient::sendToClient(int toClientID,Event::ptr event){
	return send(Event::ptr(new RoutedEvent(event,mClientID,toClientID)));
}

void SimpleClient::receiveError(){
	if(mConnector!=NULL){
		mConnector->close();
	}
}

}
}
