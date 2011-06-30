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

#ifndef TOADLET_TADPOLE_RESOURCEHANDLER_H
#define TOADLET_TADPOLE_RESOURCEHANDLER_H

#include <toadlet/egg/Resource.h>
#include <toadlet/egg/io/Stream.h>
#include <toadlet/tadpole/ResourceHandlerData.h>
#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{

/// @todo: Just stuck this here, should be in its own file
class ProgressListener{
public:
	virtual void progressUpdated(float amount)=0;
};

class ResourceHandler{
public:
	TOADLET_SHARED_POINTERS(ResourceHandler);

	virtual ~ResourceHandler(){}

	virtual Resource::ptr load(Stream::ptr stream,const ResourceHandlerData *handlerData)=0;
};

}
}

#endif
