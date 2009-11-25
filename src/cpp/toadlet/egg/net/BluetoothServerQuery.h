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

#ifndef TOADLET_EGG_NET_BLUETOOTHSERVERQUERY_H
#define TOADLET_EGG_NET_BLUETOOTHSERVERQUERY_H

#include <toadlet/egg/net/BluetoothAddress.h>

namespace toadlet{
namespace egg{
namespace net{

class BluetoothServerQuery{
public:
	TOADLET_SHARED_POINTERS(BluetoothServerQuery);

	virtual ~BluetoothServerQuery(){}

	virtual int startQuery()=0;
	virtual void cancelQuery()=0;

	virtual int getNumServers()=0;
	virtual const String &getServerName(int i)=0;
	virtual BluetoothAddress *getServerAddress(int i)=0;
};

}
}
}

#endif

