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

package com.lightningtoads.toadlet.peeper;

#include <com/lightningtoads/toadlet/peeper/Types.h>

public class Blend{
	public enum Operation{
		ONE,
		ZERO,
		DEST_COLOR,
		SOURCE_COLOR,
		ONE_MINUS_DEST_COLOR,
		ONE_MINUS_SOURCE_COLOR,
		DEST_ALPHA,
		SOURCE_ALPHA,
		ONE_MINUS_DEST_ALPHA,
		ONE_MINUS_SOURCE_ALPHA
	}

	// Ideally C++ would become like this if we could
	public static class Combination{
		public static final Blend DISABLED=			new Blend(Operation.ONE,Operation.ZERO);
		public static final Blend COLOR=			new Blend(Operation.ONE,Operation.ONE_MINUS_SOURCE_COLOR);
		public static final Blend COLOR_ADDITIVE=	new Blend(Operation.ONE,Operation.ONE);
		public static final Blend ALPHA=			new Blend(Operation.SOURCE_ALPHA,Operation.ONE_MINUS_SOURCE_ALPHA);
		public static final Blend ALPHA_ADDITIVE=	new Blend(Operation.SOURCE_ALPHA,Operation.ONE);
	}

	public Blend(){}

	public Blend(Blend blend){
		set(blend);
	}

	public Blend(Operation src,Operation dst){
		set(src,dst);
	}

	public Blend set(Blend blend){
		source=blend.source;
		dest=blend.dest;

		return this;
	}

	public Blend set(Operation src,Operation dst){
		source=src;
		dest=dst;

		return this;
	}

	public boolean equals(Object object){
		if(this==object){
			return true;
		}
		if((object==null) || (object.getClass()!=getClass())){
			return false;
		}
		Blend blend=(Blend)object;
		return source==blend.source && dest==blend.dest;
	}

	public Operation source=Operation.ONE;
	public Operation dest=Operation.ZERO;
}
