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

#ifndef TOADLET_CONFIG_H
#define TOADLET_CONFIG_H

// CMake fills in this file with the proper values for our build

// Setup the platform
#cmakedefine TOADLET_PLATFORM_WIN32
#cmakedefine TOADLET_PLATFORM_WINCE
#cmakedefine TOADLET_PLATFORM_POSIX
#cmakedefine TOADLET_PLATFORM_OSX
#cmakedefine TOADLET_PLATFORM_IOS
#cmakedefine TOADLET_PLATFORM_ANDROID
#cmakedefine ANDROID_NDK_API_LEVEL ${ANDROID_NDK_API_LEVEL}

// Setup specific build options
#cmakedefine TOADLET_FIXED_POINT
#cmakedefine TOADLET_RTTI
#cmakedefine TOADLET_EXCEPTIONS
#cmakedefine TOADLET_HAS_SSE

// Optional packages
#cmakedefine TOADLET_HAS_GIF
#cmakedefine TOADLET_HAS_JPEG
#cmakedefine TOADLET_HAS_ZLIB
#cmakedefine TOADLET_HAS_PNG
#cmakedefine TOADLET_HAS_FREETYPE
#cmakedefine TOADLET_HAS_MXML
#cmakedefine TOADLET_HAS_ZZIP
#cmakedefine TOADLET_HAS_D3DM
#cmakedefine TOADLET_HAS_D3D9
#cmakedefine TOADLET_HAS_D3D10
#cmakedefine TOADLET_HAS_D3D11
#cmakedefine TOADLET_HAS_OPENGL
#cmakedefine TOADLET_HAS_OPENAL
#cmakedefine TOADLET_HAS_OGGVORBIS
#cmakedefine TOADLET_HAS_SIDPLAY
#cmakedefine TOADLET_HAS_GDIPLUS

#endif
