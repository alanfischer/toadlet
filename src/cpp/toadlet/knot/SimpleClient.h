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

#ifndef TOADLET_KNOT_SIMPLECLIENT_H
#define TOADLET_KNOT_SIMPLECLIENT_H

#include <toadlet/knot/Connector.h>
#include <toadlet/knot/SimpleEventConnection.h>

namespace toadlet{
namespace knot{

class TOADLET_API SimpleClient:public SimpleEventConnection{
public:
	TOADLET_SHARED_POINTERS(SimpleClient);

	SimpleClient(egg::EventFactory *eventFactory,Connector::ptr connector=NULL);
	virtual ~SimpleClient();

	virtual void close();

	virtual void setConnector(Connector::ptr connector);
	virtual Connector::ptr getConnector(){return mConnector;}

	virtual bool sendToClient(int toClientID,egg::Event::ptr event);

	virtual int getClientID(){return mClientID;}

	virtual int update(){return 0;}

protected:
	int mClientID;
	Connector::ptr mConnector;
};

}
}

#endif
