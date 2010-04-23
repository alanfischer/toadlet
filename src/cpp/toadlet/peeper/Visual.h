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

#ifndef TOADLET_PEEPER_VISUAL_H
#define TOADLET_PEEPER_VISUAL_H

#include <toadlet/egg/image/ImageDefinitions.h>

namespace toadlet{
namespace peeper{

class Visual{
public:
	Visual():
		pixelFormat(egg::image::ImageDefinitions::Format_RGB_5_6_5),
		depthBits(24),
		stencilBits(0),
		multisamples(2),
		vsync(true)
	{}

	Visual(int pixelFormat,int depthBits,int multisamples):
		stencilBits(0),
		vsync(true)
	{
		this->pixelFormat=pixelFormat;
		this->depthBits=depthBits;
		this->multisamples=multisamples;
	}

	int pixelFormat;
	int depthBits;
	int stencilBits;
	int multisamples;
	bool vsync;
};

}
}

#endif
