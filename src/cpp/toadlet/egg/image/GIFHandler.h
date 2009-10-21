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

#ifndef TOADLET_EGG_IMAGE_GIFHANDLER_H
#define TOADLET_EGG_IMAGE_GIFHANDLER_H

#include <toadlet/egg/image/Image.h>
#include <toadlet/egg/io/InputStream.h>
#include <toadlet/egg/io/OutputStream.h>
#include <toadlet/egg/Collection.h>

struct GifFileType;

namespace toadlet{
namespace egg{
namespace image{

class TOADLET_API GIFHandler{
public:
	GIFHandler();
	virtual ~GIFHandler();

	virtual Image *loadImage(io::InputStream *in);
	virtual bool saveImage(Image *image,io::OutputStream *out);

	virtual bool loadAnimatedImage(io::InputStream *in,Collection<Image*> &images,Collection<int> &delayMilliseconds);

protected:
	GifFileType *openFile(io::InputStream *in);
	int closeFile(GifFileType *file);
	void resetReader();
	int getNextImage(GifFileType *gifFile,Image *&image,int &delayMilliseconds);

	Image *flipImage(Image *image);

	Image *mWorking;
	Image *mBase;
	int mDelayMilliseconds;
};

}
}
}

#endif
