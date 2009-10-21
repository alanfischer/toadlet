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

namespace toadlet{
namespace knot{

/// A connection delivers packets of data.
class Connection{
public:
	TOADLET_SHARED_POINTERS(Connection,Connection);

	virtual ~Connection(){}

	virtual bool disconnect()=0;

	/// Send a packet of data
	virtual int send(const char *data,int length)=0;
	/// Receive a packet of data, currently non-blocking
	virtual int receive(char *data,int length)=0;

	virtual int getPing() const=0;
};

}
}

#endif
