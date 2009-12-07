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

#ifndef TOADLET_PEEPER_GLINCLUDES_H
#define TOADLET_PEEPER_GLINCLUDES_H

#define TOADLET_HAS_EGL
#define TOADLET_HAS_EGL_11
#define TOADLET_HAS_GLES

import com.lightningtoads.toadlet.egg.Categories;
import com.lightningtoads.toadlet.egg.Error;
import com.lightningtoads.toadlet.egg.Logger;

#if defined(TOADLET_DEBUG)
	#define TOADLET_CHECK_EGLERROR(function) \
		{ int error=egl.eglGetError(); \
		if(error!=javax.microedition.khronos.egl.EGL10.EGL_SUCCESS) \
			com.lightningtoads.toadlet.egg.Logger.warning(com.lightningtoads.toadlet.egg.Categories.TOADLET_PEEPER, \
				"EGL Error in " + function + ": error=" + error); }

	#define TOADLET_CHECK_GLERROR(function) \
		{ int error=gl.glGetError(); \
		if(error!=javax.microedition.khronos.opengles.GL10.GL_NO_ERROR) \
			com.lightningtoads.toadlet.egg.Logger.warning(com.lightningtoads.toadlet.egg.Categories.TOADLET_PEEPER, \
				"GL Error in " + function + ": error=" + error); }
#else
	#define TOADLET_CHECK_EGLERROR(function)

	#define TOADLET_CHECK_GLERROR(function)
#endif

#if defined(TOADLET_HAS_GLES)
	#define glTexEnvi glTexEnvx
	#define glTexParameteri glTexParameterx
	#define GL_POINT_SPRITE GL_POINT_SPRITE_OES
	#define GL_COORD_REPLACE GL_COORD_REPLACE_OES
	#define GL_POINT_SIZE_ARRAY GL_POINT_SIZE_ARRAY_OES
	#define glPointSizePointer glPointSizePointerOES
#endif
