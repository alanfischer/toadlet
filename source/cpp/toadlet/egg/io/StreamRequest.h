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

#ifndef TOADLET_EGG_IO_STREAMREQUEST_H
#define TOADLET_EGG_IO_STREAMREQUEST_H

#include <toadlet/egg/io/Stream.h>

namespace toadlet{
namespace egg{
namespace io{

class StreamRequest:public Interface{
public:
	TOADLET_INTERFACE(StreamRequest);

	virtual ~StreamRequest(){}

	virtual void streamReady(Stream *stream)=0;
	virtual void streamException(const Exception &ex)=0;
	virtual void streamProgress(float progress)=0;
};

}
}
}

#endif
