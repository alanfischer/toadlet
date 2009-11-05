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

#ifndef TOADLET_PEEPER_VERSION_H
#define TOADLET_PEEPER_VERSION_H

#include <toadlet/Types.h>

#cmakedefine TOADLET_BUILD_VERSION_PEEPER_MAJOR ${TOADLET_BUILD_VERSION_PEEPER_MAJOR}
#cmakedefine TOADLET_BUILD_VERSION_PEEPER_MINOR ${TOADLET_BUILD_VERSION_PEEPER_MINOR}
#cmakedefine TOADLET_BUILD_VERSION_PEEPER_MICRO ${TOADLET_BUILD_VERSION_PEEPER_MICRO}

namespace toadlet{
namespace peeper{

class TOADLET_API Version{
public:
	static const int MAJOR;
	static const int MINOR;
	static const int MICRO;

	static const char *STRING;
};

}
}

#endif
