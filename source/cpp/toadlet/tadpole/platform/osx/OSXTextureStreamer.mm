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
#include <toadlet/tadpole/platform/osx/OSXTextureStreamer.h>

#if defined(TOADLET_PLATFORM_IOS)
#	import <UIKit/UIKit.h>
#else
#	import <AppKit/AppKit.h>
#endif

namespace toadlet{
namespace tadpole{

size_t TextureStreamGetBytes(void *info,void *buffer,size_t count){
	return ((Stream*)info)->read((tbyte*)buffer,count);
}

off_t TextureStreamSkipForward(void *info,off_t count){
	tbyte *buffer=new tbyte[count];
	int amount=((Stream*)info)->read(buffer,count);
	delete[] buffer;
	return amount;
}

void TextureStreamRewind(void *info){
	((Stream*)info)->reset();
}
	
OSXTextureStreamer::OSXTextureStreamer(TextureManager *textureManager){
	mTextureManager=textureManager;
}

Resource::ptr OSXTextureStreamer::load(Stream::ptr in,ResourceData *data,ProgressListener *listener){
	CGDataProviderSequentialCallbacks callbacks={0};
	callbacks.version=0;
	callbacks.getBytes=TextureStreamGetBytes;
	callbacks.skipForward=TextureStreamSkipForward;
	callbacks.rewind=TextureStreamRewind;
	callbacks.releaseInfo=NULL;

	CGDataProviderRef dataProvider=CGDataProviderCreateSequential(in.get(),&callbacks);

	CGImageRef cgimage=CGImageCreateWithPNGDataProvider(dataProvider,NULL,false,kCGRenderingIntentDefault);
	if(cgimage==NULL){
		cgimage=CGImageCreateWithJPEGDataProvider(dataProvider,NULL,false,kCGRenderingIntentDefault);
	}

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
	tbyte *bitmapData=(tbyte*)calloc(textureHeight,textureWidth*4);
	int bitsPerComponent=8;
	int bytesPerRow=textureWidth*4;
	CGContextRef context=CGBitmapContextCreate(bitmapData,
		textureWidth,textureHeight,
		bitsPerComponent,bytesPerRow,colorSpace,
		kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big
	);
	CGColorSpaceRelease(colorSpace);

	CGContextDrawImage(context,CGRectMake(0,0,textureWidth,textureHeight),cgimage);

	// Build texture
	TextureFormat::ptr textureFormat(new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_RGBA_8,textureWidth,textureHeight,1,0));
	tbyte *textureData=new tbyte[textureFormat->getDataSize()];
	int textureStride=textureFormat->getXPitch();

	// Flip the bitmap and copy it into the image
	int i;
	for(i=0;i<textureHeight;++i){
		memcpy(textureData+textureStride*(textureHeight-i-1),bitmapData+textureStride*i,textureStride);
	}

	Texture::ptr texture=mTextureManager->createTexture(textureFormat,textureData);

	delete[] textureData;

	CGContextRelease(context);
	free(bitmapData);

	CGImageRelease(cgimage);

	return texture;
}

}
}
