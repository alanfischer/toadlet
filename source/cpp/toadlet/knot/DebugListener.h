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

#ifndef TOADLET_KNOT_DEBUGLISTENER_H
#define TOADLET_KNOT_DEBUGLISTENER_H

#include <toadlet/egg/String.h>
#include <toadlet/knot/ConnectionListener.h>

namespace toadlet{
namespace knot{

class TOADLET_API DebugListener:public ConnectionListener{
public:
	TOADLET_SHARED_POINTERS(DebugListener);

	DebugListener();
	virtual ~DebugListener();

	void connected(Connection::ptr connection);
	void disconnected(Connection::ptr connection);
	void dataReady(Connection *connection){}
};

}
}

#endif
