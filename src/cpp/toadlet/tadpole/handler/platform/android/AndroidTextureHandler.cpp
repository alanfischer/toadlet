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
#include <toadlet/egg/Logger.h>
#include <toadlet/tadpole/handler/platform/android/AndroidTextureHandler.h>

namespace toadlet{
namespace tadpole{
namespace handler{

AndroidTextureHandler::AndroidTextureHandler(TextureManager *textureManager,JNIEnv *jenv):
	mTextureManager(NULL)
{
	mTextureManager=textureManager;
	env=jenv;

	jclass bitmapClass=env->FindClass("android/graphics/Bitmap");
	{
		getWidthBitmapID=env->GetMethodID(bitmapClass,"getWidth","()I");
		getHeightBitmapID=env->GetMethodID(bitmapClass,"getHeight","()I");
		getPixelsBitmapID=env->GetMethodID(bitmapClass,"getPixels","([IIIIIII)V");
	}
	env->DeleteLocalRef(bitmapClass);

	factoryClass=(jclass)env->NewGlobalRef(env->FindClass("android/graphics/BitmapFactory"));
	{
		decodeFactoryID=env->GetStaticMethodID(factoryClass,"decodeByteArray","([BII)Landroid/graphics/Bitmap;");
	}
	env->DeleteLocalRef(factoryClass);
}

AndroidTextureHandler::~AndroidTextureHandler(){
	if(factoryClass!=NULL){
		env->DeleteGlobalRef(factoryClass);
		factoryClass=NULL;
	}
}

Resource::ptr AndroidTextureHandler::load(Stream::ptr in,ResourceData *data,ProgressListener *listener){
	Logger::debug(Categories::TOADLET_TADPOLE,"AndroidTextureHandler::load");

	int length=in->length();
	tbyte *streamData=new tbyte[length];
	in->read(streamData,length);
	jbyteArray bytes=env->NewByteArray(length);
	env->SetByteArrayRegion(bytes,0,length,(jbyte*)streamData);
	delete[] streamData;

	jobject bitmap=env->CallStaticObjectMethod(factoryClass,decodeFactoryID,bytes,0,length);

	jthrowable exc=env->ExceptionOccurred();
	if(exc!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
		env->DeleteLocalRef(bytes);
		return NULL;
	}

	env->DeleteLocalRef(bytes);
	
	int width=env->CallIntMethod(bitmap,getWidthBitmapID);
	int height=env->CallIntMethod(bitmap,getHeightBitmapID);
	jintArray pixels=env->NewIntArray(width*height);
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_RGBA_8,width,height));
	
	env->CallVoidMethod(bitmap,getPixelsBitmapID,pixels,0,width,0,0,width,height);
	env->GetIntArrayRegion(pixels,0,width*height,(jint*)image->getData());

	env->DeleteLocalRef(pixels);
	env->DeleteLocalRef(bitmap);

	Texture::ptr texture=mTextureManager->createTexture(image);

	return texture;
}

}
}
}
