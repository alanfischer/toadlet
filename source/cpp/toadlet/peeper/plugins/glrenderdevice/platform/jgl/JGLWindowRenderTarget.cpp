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

#include "JGLWindowRenderTarget.h"
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace peeper{

TOADLET_C_API RenderTarget *new_JGLWindowRenderTarget(JNIEnv *jenv,jobject jobj){
	return new JGLWindowRenderTarget(jenv,jobj);
}

JGLWindowRenderTarget::JGLWindowRenderTarget(JNIEnv *jenv,jobject jobj):
	env(NULL),
	obj(NULL)
{
	env=jenv;
	obj=env->NewGlobalRef(jobj);

	jclass clazz=env->GetObjectClass(obj);

	swapID=env->GetMethodID(clazz,"swap","()Z");
	resetID=env->GetMethodID(clazz,"reset","()V");
	activateID=env->GetMethodID(clazz,"activate","()Z");
	deactivateID=env->GetMethodID(clazz,"deactivate","()Z");
	activateAdditionalContextID=env->GetMethodID(clazz,"activateAdditionalContext","()Z");
	deactivateAdditionalContextID=env->GetMethodID(clazz,"deactivateAdditionalContext","()V");
	
	destroyID=env->GetMethodID(clazz,"destroy","()V");
	isPrimaryID=env->GetMethodID(clazz,"isPrimary","()Z");
	isValidID=env->GetMethodID(clazz,"isValid","()Z");
	getWidthID=env->GetMethodID(clazz,"getWidth","()I");
	getHeightID=env->GetMethodID(clazz,"getHeight","()I");

	env->DeleteLocalRef(clazz);
}

JGLWindowRenderTarget::~JGLWindowRenderTarget(){
	env->DeleteGlobalRef(obj);
	obj=NULL;
	env=NULL;
}

void JGLWindowRenderTarget::destroy(){
	env->CallVoidMethod(obj,destroyID);
}

bool JGLWindowRenderTarget::swap(){
	return env->CallBooleanMethod(obj,swapID);
}

void JGLWindowRenderTarget::reset(){
	env->CallVoidMethod(obj,resetID);
}

bool JGLWindowRenderTarget::activate(){
	return env->CallBooleanMethod(obj,activateID);
}

bool JGLWindowRenderTarget::deactivate(){
	return env->CallBooleanMethod(obj,deactivateID);
}

bool JGLWindowRenderTarget::activateAdditionalContext(){
	return env->CallBooleanMethod(obj,activateAdditionalContextID);
}

void JGLWindowRenderTarget::deactivateAdditionalContext(){
	return env->CallVoidMethod(obj,deactivateAdditionalContextID);
}

bool JGLWindowRenderTarget::isPrimary() const{
	return env->CallBooleanMethod(obj,isPrimaryID);
}

bool JGLWindowRenderTarget::isValid() const{
	return env->CallBooleanMethod(obj,isValidID);
}

int JGLWindowRenderTarget::getWidth() const{
	return env->CallIntMethod(obj,getWidthID);
}

int JGLWindowRenderTarget::getHeight() const{
	return env->CallIntMethod(obj,getHeightID);
}

}
}
