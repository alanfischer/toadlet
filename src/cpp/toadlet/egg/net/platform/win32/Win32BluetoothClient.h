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

#ifndef TOADLET_EGG_NET_WIN32BLUETOOTHCLIENT_H
#define TOADLET_EGG_NET_WIN32BLUETOOTHCLIENT_H

#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/net/BluetoothClient.h>
#include <toadlet/egg/net/Socket.h>

namespace toadlet{
namespace egg{
namespace net{

class Win32BluetoothClient:public BluetoothClient,public Socket{
public:
	Win32BluetoothClient();
	virtual ~Win32BluetoothClient();

	virtual bool connect(BluetoothAddress *address);
	virtual bool isConnected() const;

	virtual void close();

	virtual int receive(char *buffer,int length);

	virtual int send(const char *buffer,int length);

protected:
};

}
}
}

#endif
