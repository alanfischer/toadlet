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

#include <toadlet/egg/net/platform/win32/Win32BluetoothClient.h>
#include <toadlet/egg/net/platform/win32/Win32BluetoothIncludes.h>

namespace toadlet{
namespace egg{
namespace net{

TOADLET_C_API BluetoothClient *new_BluetoothClient(){return new Win32BluetoothClient();}

Win32BluetoothClient::Win32BluetoothClient():
	Socket(PF_BTH,SOCK_STREAM,BTHPROTO_RFCOMM)
{}

Win32BluetoothClient::~Win32BluetoothClient(){
	close();
}

bool Win32BluetoothClient::connect(BluetoothAddress *btAddress){
	SOCKADDR_BTH address={0};
	address.addressFamily=AF_BTH;
	address.btAddr=btAddress->getAddress();
	address.port=btAddress->getPort();

	int result=::connect(mHandle,(struct sockaddr*)&address,sizeof(address));
	if(result==TOADLET_SOCKET_ERROR){
		Socket::error("connect");
		return false;
	}

	mBound=true;
	mConnected=true;

	return true;
}

bool Win32BluetoothClient::isConnected() const{
	return Socket::isConnected();
}

void Win32BluetoothClient::close(){
	Socket::close();
}

int Win32BluetoothClient::receive(byte *buffer,int length){
	return Socket::receive(buffer,length);
}

int Win32BluetoothClient::send(const byte *buffer,int length){
	return Socket::send(buffer,length);
}

}
}
}
