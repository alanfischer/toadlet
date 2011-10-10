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

jbyteArray bytes;//=env->NewByteArray(length);
jintArray pixels;//=env->NewIntArray(width*height);

AndroidTextureHandler::AndroidTextureHandler(TextureManager *textureManager,JNIEnv *jenv):
	mTextureManager(NULL)
{
	mTextureManager=textureManager;
	env=jenv;

	jclass bitmapClass=env->FindClass("android/graphics/Bitmap");
	{
		recycleBitmapID=env->GetMethodID(bitmapClass,"recycle","()V");
		getWidthBitmapID=env->GetMethodID(bitmapClass,"getWidth","()I");
		getHeightBitmapID=env->GetMethodID(bitmapClass,"getHeight","()I");
		getPixelsBitmapID=env->GetMethodID(bitmapClass,"getPixels","([IIIIIII)V");
	}
	env->DeleteLocalRef(bitmapClass);

	systemClass=(jclass)env->NewGlobalRef(env->FindClass("java/lang/System"));
	{
		gcID=env->GetStaticMethodID(systemClass,"gc","()V");
	}

	factoryClass=(jclass)env->NewGlobalRef(env->FindClass("android/graphics/BitmapFactory"));
	{
		decodeFactoryID=env->GetStaticMethodID(factoryClass,"decodeByteArray","([BII)Landroid/graphics/Bitmap;");
	}

//bytes=(jbyteArray)env->NewGlobalRef(env->NewByteArray(65536));
//pixels=(jintArray)env->NewGlobalRef(env->NewIntArray(256*256));//1024*1024));
}

AndroidTextureHandler::~AndroidTextureHandler(){
	if(factoryClass!=NULL){
		env->DeleteGlobalRef(factoryClass);
		factoryClass=NULL;
	}
//env->DeleteGlobalRef(bytes);
//env->DeleteGlobalRef(pixels);
}

Resource::ptr AndroidTextureHandler::load(Stream::ptr in,ResourceData *data,ProgressListener *listener){
	Logger::debug(Categories::TOADLET_TADPOLE,"AndroidTextureHandler::load");

	int length=in->length();
	tbyte *streamData=new tbyte[length];
	in->read(streamData,length);
Logger::alert(String("1:")+length);
	jbyteArray bytes=env->NewByteArray(length);
Logger::alert("2");
	env->SetByteArrayRegion(bytes,0,length,(jbyte*)streamData);
Logger::alert(String("3:")+(int)streamData);
	delete[] streamData;
Logger::alert(String("factoryClassstuff:")+(int)factoryClass+":"+(int)decodeFactoryID);
	jobject bitmap=env->CallStaticObjectMethod(factoryClass,decodeFactoryID,bytes,0,length);

Logger::alert("4");
	jthrowable exc=env->ExceptionOccurred();
Logger::alert("5");
	if(exc!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
	env->DeleteLocalRef(bytes);
	if(exc!=NULL){
		return NULL;
	}
	
	int width=env->CallIntMethod(bitmap,getWidthBitmapID);
	int height=env->CallIntMethod(bitmap,getHeightBitmapID);
	Logger::alert(String("5.1:")+width*height);

	jintArray pixels=env->NewIntArray(width*height);
Logger::alert("8");
	env->CallVoidMethod(bitmap,getPixelsBitmapID,pixels,0,width,0,0,width,height);
Logger::alert("7");
	exc=env->ExceptionOccurred();
	if(exc!=NULL){
Logger::alert("5");
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
	env->CallVoidMethod(bitmap,recycleBitmapID);
	env->DeleteLocalRef(bitmap);
	if(exc!=NULL){
		return NULL;
	}

Logger::alert("9");
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_RGBA_8,width,height));
	
Logger::alert("7.5");
	env->GetIntArrayRegion(pixels,0,width*height,(jint*)image->getData());

Logger::alert("6");
	env->DeleteLocalRef(pixels);
Logger::alert("7");
Logger::alert("7,5");
Logger::alert("8");

Texture::ptr texture=mTextureManager->createTexture(image);
	Logger::alert("DONE");
	return texture;
}

}
}
}
