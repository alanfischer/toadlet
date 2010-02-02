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

#ifndef TOADLET_KNOT_CONNECTION_H
#define TOADLET_KNOT_CONNECTION_H

#include <toadlet/egg/SharedPointer.h>
#include <toadlet/egg/WeakPointer.h>

namespace toadlet{
namespace knot{

/// Thought: On Java this can just be the ByteChannel

/// A Connection sends and receives packets of data.
class Connection{
public:
	TOADLET_SHARED_POINTERS(Connection);

	virtual ~Connection(){}

	virtual bool opened()=0;
	virtual void close()=0;

	/// If the connection is blocking or not
	virtual bool blocking()=0;

	/// Send a packet of data
	virtual int send(const byte *data,int length)=0;
	/// Receive a packet of data
	virtual int receive(byte *data,int length)=0;
};

}
}

#endif
