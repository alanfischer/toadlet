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
#include <toadlet/knot/SimpleEventClient.h>

using namespace toadlet::egg;

namespace toadlet{
namespace knot{

SimpleEventClient::SimpleEventClient(){
}

SimpleEventClient::~SimpleEventClient(){
}

void SimpleEventClient::connected(Connection::ptr connection){
	mConnection=connection;
}

void SimpleEventClient::disconnected(Connection::ptr connection){
	mConnection=NULL;
}

bool SimpleEventClient::sendEvent(Event::ptr event){
}

bool SimpleEventClient::sendEventToClient(Event::ptr event,int clientID){
}

bool SimpleEventClient::receiveEvent(Event::ptr &event,int &clientID){
}

}
}
