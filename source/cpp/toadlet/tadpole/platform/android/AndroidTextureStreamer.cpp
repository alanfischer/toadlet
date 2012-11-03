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
#include <toadlet/egg/Log.h>
#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/platform/android/AndroidTextureStreamer.h>

namespace toadlet{
namespace tadpole{

AndroidTextureStreamer::AndroidTextureStreamer(TextureManager *textureManager,JNIEnv *jenv):
	mTextureManager(NULL),
	env(NULL),
	jvm(NULL)
{
	mTextureManager=textureManager;
	env=jenv;
	env->GetJavaVM(&jvm);

	jclass bitmapClass=env->FindClass("android/graphics/Bitmap");
	{
		recycleBitmapID=env->GetMethodID(bitmapClass,"recycle","()V");
		getWidthBitmapID=env->GetMethodID(bitmapClass,"getWidth","()I");
		getHeightBitmapID=env->GetMethodID(bitmapClass,"getHeight","()I");
		getPixelsBitmapID=env->GetMethodID(bitmapClass,"getPixels","([IIIIIII)V");
	}
	env->DeleteLocalRef(bitmapClass);

	factoryClass=(jclass)env->NewGlobalRef(env->FindClass("android/graphics/BitmapFactory"));
	{
		decodeFactoryID=env->GetStaticMethodID(factoryClass,"decodeByteArray","([BII)Landroid/graphics/Bitmap;");
	}
}

AndroidTextureStreamer::~AndroidTextureStreamer(){
	if(factoryClass!=NULL){
		env->DeleteGlobalRef(factoryClass);
		factoryClass=NULL;
	}
}

Resource::ptr AndroidTextureStreamer::load(Stream::ptr stream,ResourceData *data,ProgressListener *listener){
	int usage=(data!=NULL)?((TextureData*)data)->usage:0;

	Log::debug(Categories::TOADLET_TADPOLE,"AndroidTextureStreamer::load");

	jvm->AttachCurrentThread(&env,NULL);

	int length=stream->length();
	tbyte *streamData=new tbyte[length];
	length=stream->read(streamData,length);
	jbyteArray bytes=env->NewByteArray(length);
	env->SetByteArrayRegion(bytes,0,length,(jbyte*)streamData);
	delete[] streamData;

	jobject bitmap=env->CallStaticObjectMethod(factoryClass,decodeFactoryID,bytes,0,length);
	jthrowable exc=env->ExceptionOccurred();
	if(exc!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
	env->DeleteLocalRef(bytes);
	if(exc!=NULL || bitmap==NULL){
		return NULL;
	}
	
	int width=env->CallIntMethod(bitmap,getWidthBitmapID);
	int height=env->CallIntMethod(bitmap,getHeightBitmapID);

	jintArray pixels=env->NewIntArray(width*height);

	env->CallVoidMethod(bitmap,getPixelsBitmapID,pixels,0,width,0,0,width,height);

	exc=env->ExceptionOccurred();
	if(exc!=NULL){
		env->ExceptionDescribe();
		env->ExceptionClear();
	}
	env->CallVoidMethod(bitmap,recycleBitmapID);
	env->DeleteLocalRef(bitmap);
	if(exc!=NULL){
		return NULL;
	}

	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_BGRA_8,width,height,1,0);
	tbyte *textureData=new tbyte[textureFormat->getDataSize()];

	env->GetIntArrayRegion(pixels,0,width*height,(jint*)textureData);

	env->DeleteLocalRef(pixels);

	int linePitch=textureFormat->getXPitch();
	tbyte *line=new tbyte[linePitch];
	int i;
	for(i=0;i<height/2;++i){
		memcpy(line,textureData+linePitch*i,linePitch);
		memcpy(textureData+linePitch*i,textureData+linePitch*(height-i-1),linePitch);
		memcpy(textureData+linePitch*(height-i-1),line,linePitch);
	}
	delete[] line;

	Texture::ptr texture=mTextureManager->createTexture(usage,textureFormat,textureData);
	
	delete[] textureData;
	
	return texture;
}

}
}
