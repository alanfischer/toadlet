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

#ifndef TOADLET_TADPOLE_RESOURCESTREAMER_H
#define TOADLET_TADPOLE_RESOURCESTREAMER_H

#include <toadlet/egg/Interface.h>
#include <toadlet/egg/Resource.h>
#include <toadlet/egg/io/Stream.h>
#include <toadlet/tadpole/ResourceData.h>
#include <toadlet/tadpole/ProgressListener.h>

namespace toadlet{
namespace tadpole{

class ResourceStreamer:public Interface{
public:
	TOADLET_IPTR(ResourceStreamer);

	virtual ~ResourceStreamer(){}

	virtual void destroy(){}

	virtual Resource::ptr load(Stream::ptr stream,ResourceData *data,ProgressListener *listener)=0;
	virtual bool save(Stream::ptr stream,Resource::ptr resource,ResourceData *data,ProgressListener *listener){return false;}
};

}
}

#endif
