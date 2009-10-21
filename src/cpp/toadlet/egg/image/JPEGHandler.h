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

#ifndef TOADLET_EGG_IMAGE_JPEGHANDLER_H
#define TOADLET_EGG_IMAGE_JPEGHANDLER_H

#include <toadlet/egg/image/Image.h>
#include <toadlet/egg/io/InputStream.h>
#include <toadlet/egg/io/OutputStream.h>
#include <toadlet/egg/io/PushbackInputStream.h>

namespace toadlet{
namespace egg{
namespace image{

class TOADLET_API JPEGHandler{
public:
	JPEGHandler();
	virtual ~JPEGHandler();
	virtual Image *loadImage(io::InputStream *in);
	virtual bool saveImage(Image *image,io::OutputStream *out);

	// A somewhat temporary method to handle the image loading through use of a pushback stream, so we wont sacrafice
	// extra data beyond the jpeg file
	virtual Image *loadImage(io::PushbackInputStream *in);

protected:
	virtual Image *loadImage(io::InputStream *in,io::PushbackInputStream *pin);
};

}
}
}

#endif
