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

package com.lightningtoads.toadlet.tadpole;

#define TOADLET_BUILD_VERSION_TADPOLE_MAJOR 0
#define TOADLET_BUILD_VERSION_TADPOLE_MINOR 1
#define TOADLET_BUILD_VERSION_TADPOLE_MICRO 0

public class Version{
	public static final int MAJOR=TOADLET_BUILD_VERSION_TADPOLE_MAJOR;
	public static final int MINOR=TOADLET_BUILD_VERSION_TADPOLE_MINOR;
	public static final int MICRO=TOADLET_BUILD_VERSION_TADPOLE_MICRO;

	public static final String STRING=TOADLET_BUILD_VERSION_TADPOLE_MAJOR+"."+TOADLET_BUILD_VERSION_TADPOLE_MINOR+"."+TOADLET_BUILD_VERSION_TADPOLE_MICRO;
};
