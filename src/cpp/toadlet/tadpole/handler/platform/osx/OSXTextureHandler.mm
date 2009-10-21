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

#include <toadlet/egg/Collection.h>
#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/handler/platform/osx/OSXTextureHandler.h>

#if defined(TOADLET_PLATFORM_IPHONE)
#	import <UIKit/UIKit.h>
#else
#	import <AppKit/AppKit.h>
#endif

using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace handler{

size_t TextureStreamGetBytes(void *info,void *buffer,size_t count){
	return ((InputStream*)info)->read((char*)buffer,count);
}

off_t TextureStreamSkipForward(void *info,off_t count){
	char *buffer=new char[count];
	int amount=((InputStream*)info)->read(buffer,count);
	delete[] buffer;
	return amount;
}

void TextureStreamRewind(void *info){
	((InputStream*)info)->reset();
}
	
OSXTextureHandler::OSXTextureHandler(){
}

Resource *OSXTextureHandler::load(InputStream::ptr in,const ResourceHandlerData *handlerData){
	CGDataProviderSequentialCallbacks callbacks={0};
	callbacks.version=0;
	callbacks.getBytes=TextureStreamGetBytes;
	callbacks.skipForward=TextureStreamSkipForward;
	callbacks.rewind=TextureStreamRewind;
	callbacks.releaseInfo=NULL;

	CGDataProviderRef dataProvider=CGDataProviderCreateSequential(in.get(),&callbacks);

	CGImageRef cgimage=CGImageCreateWithPNGDataProvider(dataProvider,NULL,false,kCGRenderingIntentDefault);

	CGDataProviderRelease(dataProvider);

	if(cgimage==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"error loading image");
		return NULL;
	}

	// Find texture sizes
	int textureWidth=CGImageGetWidth(cgimage);
	int textureHeight=CGImageGetHeight(cgimage);

	// Create context
	CGColorSpaceRef colorSpace=CGColorSpaceCreateDeviceRGB();
	void *data=calloc(textureHeight,textureWidth*4);
	int bitsPerComponent=8;
	int bytesPerRow=textureWidth*4;
	CGContextRef context=CGBitmapContextCreate(data,
		textureWidth,textureHeight,
		bitsPerComponent,bytesPerRow,colorSpace,
		kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big
	);
	CGColorSpaceRelease(colorSpace);

	CGContextDrawImage(context,CGRectMake(0,0,textureWidth,textureHeight),cgimage);

	// Build texture
	Image::ptr image(new Image(Image::Dimension_D2,Image::Format_RGBA_8,textureWidth,textureHeight));
	uint8 *imageData=image->getData();

	int imageStride=textureWidth*image->getPixelSize();
	memcpy(imageData,data,imageStride*textureHeight);

	Texture *texture(new Texture(image));

	CGContextRelease(context);
	free(data);

	CGImageRelease(cgimage);

	return texture;
}

}
}
}
