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

public final class Viewport{
	public Viewport(){}

	public Viewport(int x1,int y1,int width1,int height1){
		x=x1;
		y=y1;
		width=width1;
		height=height1;
	}

	public Viewport set(Viewport v){
		x=v.x;
		y=v.y;
		width=v.width;
		height=v.height;
		return this;
	}

	public Viewport set(int x1,int y1,int width1,int height1){
		x=x1;
		y=y1;
		width=width1;
		height=height1;
		return this;
	}

	public Viewport reset(){
		x=0;
		y=0;
		width=0;
		height=0;
		return this;
	}

	public int x;
	public int y;
	public int width;
	public int height;
}
