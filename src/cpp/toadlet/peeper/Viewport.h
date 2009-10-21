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

#ifndef TOADLET_PEEPER_VIEWPORT_H
#define TOADLET_PEEPER_VIEWPORT_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TOADLET_API Viewport{
public:
	inline Viewport():x(0),y(0),width(0),height(0){}

	inline Viewport(int x1,int y1,int width1,int height1):x(x1),y(y1),width(width1),height(height1){}

	inline Viewport &set(const Viewport &v){
		x=v.x;
		y=v.y;
		width=v.width;
		height=v.height;
		return *this;
	}

	inline Viewport &set(int x1,int y1,int width1,int height1){
		x=x1;
		y=y1;
		width=width1;
		height=height1;
		return *this;
	}

	inline Viewport &reset(){
		x=0;
		y=0;
		width=0;
		height=0;
		return *this;
	}

	int x;
	int y;
	int width;
	int height;
};

}
}

#endif
