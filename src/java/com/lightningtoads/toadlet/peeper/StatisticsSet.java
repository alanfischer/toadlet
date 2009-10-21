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

public class StatisticsSet{
	public StatisticsSet(){}

	public void reset(){
		pointCount=0;
		lineCount=0;
		triangleCount=0;
		quadCount=0;
		bufferLockCount=0;
		textureChangeCount=0;
	}
	
	public int pointCount=0;
	public int lineCount=0;
	public int triangleCount=0;
	public int quadCount=0;
	public int bufferLockCount=0;
	public int textureChangeCount=0;
}
