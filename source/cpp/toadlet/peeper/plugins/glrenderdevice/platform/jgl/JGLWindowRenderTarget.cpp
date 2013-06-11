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

namespace toadlet{
namespace peeper{

TOADLET_C_API RenderTarget *new_JGLWindowRenderTarget(JNIEnv *jenv,jobject jobj){
	return new JGLWindowRenderTarget(jenv,jobj);
}

JGLWindowRenderTarget::JGLWindowRenderTarget(JNIEnv *jenv,jobject jobj):
	vm(NULL),
	obj(NULL)
{
	jenv->GetJavaVM(&vm);
	JNIEnv *env=jenv;
	obj=env->NewGlobalRef(jobj);

	jclass renderTargetClass=env->GetObjectClass(obj);
	{
		swapID=env->GetMethodID(renderTargetClass,"swap","()Z");
		resetID=env->GetMethodID(renderTargetClass,"reset","()V");
		activateID=env->GetMethodID(renderTargetClass,"activate","()Z");
		deactivateID=env->GetMethodID(renderTargetClass,"deactivate","()Z");
		activateAdditionalContextID=env->GetMethodID(renderTargetClass,"activateAdditionalContext","()Z");
		deactivateAdditionalContextID=env->GetMethodID(renderTargetClass,"deactivateAdditionalContext","()V");
	
		destroyID=env->GetMethodID(renderTargetClass,"destroy","()V");
		isPrimaryID=env->GetMethodID(renderTargetClass,"isPrimary","()Z");
		isValidID=env->GetMethodID(renderTargetClass,"isValid","()Z");
		getWidthID=env->GetMethodID(renderTargetClass,"getWidth","()I");
		getHeightID=env->GetMethodID(renderTargetClass,"getHeight","()I");
	}
	env->DeleteLocalRef(renderTargetClass);
}

JGLWindowRenderTarget::~JGLWindowRenderTarget(){
	getEnv()->DeleteGlobalRef(obj);
	obj=NULL;
	vm=NULL;
}

void JGLWindowRenderTarget::destroy(){
	getEnv()->CallVoidMethod(obj,destroyID);
}

bool JGLWindowRenderTarget::swap(){
	return getEnv()->CallBooleanMethod(obj,swapID);
}

void JGLWindowRenderTarget::reset(){
	getEnv()->CallVoidMethod(obj,resetID);
}

bool JGLWindowRenderTarget::activate(){
	return getEnv()->CallBooleanMethod(obj,activateID);
}

bool JGLWindowRenderTarget::deactivate(){
	return getEnv()->CallBooleanMethod(obj,deactivateID);
}

bool JGLWindowRenderTarget::activateAdditionalContext(){
	return getEnv()->CallBooleanMethod(obj,activateAdditionalContextID);
}

void JGLWindowRenderTarget::deactivateAdditionalContext(){
	return getEnv()->CallVoidMethod(obj,deactivateAdditionalContextID);
}

bool JGLWindowRenderTarget::isPrimary() const{
	return getEnv()->CallBooleanMethod(obj,isPrimaryID);
}

bool JGLWindowRenderTarget::isValid() const{
	return getEnv()->CallBooleanMethod(obj,isValidID);
}

int JGLWindowRenderTarget::getWidth() const{
	return getEnv()->CallIntMethod(obj,getWidthID);
}

int JGLWindowRenderTarget::getHeight() const{
	return getEnv()->CallIntMethod(obj,getHeightID);
}

JNIEnv *JGLWindowRenderTarget::getEnv() const{
	JNIEnv *env=NULL;
	vm->AttachCurrentThread(&env,NULL);
	return env;
}

}
}
