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

#include <toadlet/peeper/Version.h>

namespace toadlet{
namespace peeper{

const int Version::MAJOR=TOADLET_BUILD_VERSION_PEEPER_MAJOR;
const int Version::MINOR=TOADLET_BUILD_VERSION_PEEPER_MINOR;
const int Version::MICRO=TOADLET_BUILD_VERSION_PEEPER_MICRO;

const char *Version::STRING=TOADLET_VERSION_MAKESTRING(TOADLET_BUILD_VERSION_PEEPER_MAJOR,TOADLET_BUILD_VERSION_PEEPER_MINOR,TOADLET_BUILD_VERSION_PEEPER_MICRO);

}
}
