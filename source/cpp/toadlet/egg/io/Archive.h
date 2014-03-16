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

#ifndef TOADLET_EGG_IO_ARCHIVE_H
#define TOADLET_EGG_IO_ARCHIVE_H

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/ResourceRequest.h>
#include <toadlet/egg/io/StreamRequest.h>

namespace toadlet{
namespace egg{
namespace io{

class Archive:public Resource{
public:
	TOADLET_INTERFACE(Archive);

	virtual bool openStream(const String &name,StreamRequest *request)=0;
	virtual bool openResource(const String &name,ResourceRequest *request)=0;

	virtual const Collection<String> &getEntries()=0;
};

}
}
}

#endif

