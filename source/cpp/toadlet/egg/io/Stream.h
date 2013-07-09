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

#ifndef TOADLET_EGG_IO_STREAM_H
#define TOADLET_EGG_IO_STREAM_H

#include <toadlet/egg/Types.h>
#include <toadlet/egg/Interface.h>

namespace toadlet{
namespace egg{
namespace io{

class Stream:public Interface{
public:
	TOADLET_INTERFACE(Stream);

	virtual ~Stream(){}

	virtual void close()=0;
	virtual bool closed()=0;

	virtual bool readable()=0;
	virtual int read(tbyte *buffer,int length)=0;

	virtual bool writeable()=0;
	virtual int write(const tbyte *buffer,int length)=0;

	virtual bool reset()=0;
	virtual int length()=0;
	virtual int position()=0;
	virtual bool seek(int offs)=0;
	virtual bool flush()=0;
};

}
}
}

#endif

