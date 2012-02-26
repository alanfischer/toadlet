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

#ifndef TOADLET_PEEPER_WINDOWRENDERTARGETFORMAT_H
#define TOADLET_PEEPER_WINDOWRENDERTARGETFORMAT_H

#include <toadlet/peeper/Types.h>

namespace toadlet{
namespace peeper{

class TOADLET_API WindowRenderTargetFormat{
public:
	TOADLET_SPTR(WindowRenderTargetFormat);

	WindowRenderTargetFormat(int pixelFormat=0,int depthBits=0,int stencilBits=0,int multisamples=0,bool vsync=false,int threads=0,bool debug=false,int flags=0){
		this->pixelFormat=pixelFormat;
		this->depthBits=depthBits;
		this->stencilBits=stencilBits;
		this->multisamples=multisamples;
		this->vsync=vsync;
		this->threads=threads;
		this->debug=debug;
		this->flags=flags;
	}

	int pixelFormat;
	int depthBits;
	int stencilBits;
	int multisamples;
	bool vsync;
	int threads;
	bool debug;
	int flags;
};

}
}

#endif
