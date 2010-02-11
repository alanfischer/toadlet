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

package org.toadlet.egg;

#include <org/toadlet/Types.h>

#if defined(TOADLET_EXCEPTIONS)
	#define TOADLET_MAKE_ERROR_FUNCTION(name,type) \
		public static void name(String description){name(null,description);} \
		public static void name(String categoryName,String description){ \
			errorLog(categoryName,description); \
			throw new type(description); \
		}
#else
	#define TOADLET_MAKE_ERROR_FUNCTION(name,type) \
		public static void name(String description){name(null,description);} \
		public static void name(String categoryName,String description){ \
			errorLog(categoryName,description); \
		}
#endif
		
public final class Error{
	// General
	TOADLET_MAKE_ERROR_FUNCTION(unknown,RuntimeException)
	TOADLET_MAKE_ERROR_FUNCTION(invalidParameters,IllegalArgumentException)
	TOADLET_MAKE_ERROR_FUNCTION(nullPointer,NullPointerException)
	TOADLET_MAKE_ERROR_FUNCTION(unimplemented,UnsupportedOperationException)
	TOADLET_MAKE_ERROR_FUNCTION(overflow,ArithmeticException)

	// Flick
	TOADLET_MAKE_ERROR_FUNCTION(sequence,IllegalStateException)

	// Knot
	TOADLET_MAKE_ERROR_FUNCTION(alreadyConnected,RuntimeException)
	TOADLET_MAKE_ERROR_FUNCTION(incorrectProtocolName,RuntimeException)

	protected static void errorLog(String categoryName,String description){
		Logger.getInstance().addLogString(categoryName,Logger.Level.ERROR,description);
	}
}
