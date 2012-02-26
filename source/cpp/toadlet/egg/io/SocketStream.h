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

#ifndef TOADLET_EGG_IO_SOCKETSTREAM_H
#define TOADLET_EGG_IO_SOCKETSTREAM_H

#include <toadlet/egg/io/Stream.h>
#include <toadlet/egg/net/Socket.h>

namespace toadlet{
namespace egg{
namespace io{

class TOADLET_API SocketStream:public Stream{
public:
	TOADLET_SPTR(SocketStream);

	SocketStream(Socket::ptr socket);
	virtual ~SocketStream();

	virtual void close(){mSocket->close();}
	virtual bool closed(){return mSocket->closed();}

	virtual bool readable(){return true;}
	virtual int read(tbyte *buffer,int length);

	virtual bool writeable(){return true;}
	virtual int write(const tbyte *buffer,int length);

	virtual bool reset(){return false;}
	virtual int length(){return -1;}
	virtual int position(){return -1;}
	virtual bool seek(int offs){return false;}

protected:
	Socket::ptr mSocket;
};

}
}
}

#endif

