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

#include <toadlet/egg/io/SocketStream.h>

using namespace toadlet::egg::net;

namespace toadlet{
namespace egg{
namespace io{

SocketStream::SocketStream(Socket::ptr socket)
	//mSocket
{
	mSocket=socket;
}

SocketStream::~SocketStream(){
	// Don't close the socket on destruction, we'll leave that up to the socket itself
}

int SocketStream::read(tbyte *buffer,int length){
	return mSocket->receive(buffer,length);
}

int SocketStream::write(const tbyte *buffer,int length){
	return mSocket->send(buffer,length);
}

}	
}
}

