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

#ifndef TOADLET_EGG_RESOURCE_H
#define TOADLET_EGG_RESOURCE_H

#include <toadlet/egg/Interface.h>
#include <toadlet/egg/String.h>

namespace toadlet{
namespace egg{

class ResourceDestroyedListener;

class Resource:public Interface{
public:
	TOADLET_INTERFACE(Resource);
	virtual ~Resource(){}

	virtual void destroy()=0;

	virtual void setDestroyedListener(ResourceDestroyedListener *listener)=0;

	virtual void setName(const String &name)=0;
	virtual const String &getName() const=0;
	
	virtual void internal_setUniqueHandle(int handle)=0;
	virtual int getUniqueHandle() const=0;
};

}
}

#endif
