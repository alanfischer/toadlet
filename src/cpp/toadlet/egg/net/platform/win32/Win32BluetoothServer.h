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

#ifndef TOADLET_EGG_NET_WIN32BLUETOOTHSERVER_H
#define TOADLET_EGG_NET_WIN32BLUETOOTHSERVER_H

#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/net/BluetoothServer.h>
#include <toadlet/egg/net/Socket.h>
#include <toadlet/egg/net/platform/win32/Win32BluetoothIncludes.h>

namespace toadlet{
namespace egg{
namespace net{

class Win32BluetoothServer:public BluetoothServer,protected Socket{
public:
	Win32BluetoothServer();
	virtual ~Win32BluetoothServer();

	virtual bool accept(const String &guid);
	virtual bool isConnected() const;

	virtual void close();

	virtual int receive(byte *buffer,int length);

	virtual int send(const byte *buffer,int length);

protected:
	int registerService(const GUID &guid,unsigned char port);

	String mGUID;
	Mutex mLock;
	Socket *mClient;
};

}
}
}

#endif
