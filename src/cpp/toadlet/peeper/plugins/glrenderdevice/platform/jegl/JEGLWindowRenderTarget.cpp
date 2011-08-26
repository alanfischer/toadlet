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

#include "JEGLWIndowRenderTarget.h"
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace peeper{

JEGLWindowRenderTarget::JEGLWindowRenderTarget(JNIEnv *jenv,jobject jobj):GLRenderTarget()
	env(jenv),
	obj(jobj)
{
	jclass clazz=GetObjectClass(env,obj);

	swapID=env->GetMethodID(env,clazz,"swap","()Z");
	resetID=env->GetMethodID(env,clazz,"reset","()V");
	activateID=env->GetMethodID(env,clazz,"activate","()Z");
	deactivateID=env->GetMethodID(env,clazz,"deactivate","()Z");
	activateAdditionalContextID=env->GetMethodID(env,clazz,"activateAdditionalContext","()Z");
	deactivateAdditionalContextID=env->GetMethodID(env,clazz,"deactivateAdditionalContext","()V");
	
	isPrimaryID=env->GetMethodID(env,clazz,"isPrimary","()Z");
	isValidID=env->GetMethodID(env,clazz,"isValid","()Z");
	getWidthID=env->GetMethodID(env,clazz,"getWidth","()I");
	getHeightID=env->GetMethodID(env,clazz,"getHeight","()I");
}

bool EGLWindowRenderTarget::swap(){
	return env->CallBooleanMethod(env,obj,swapID);
}

void EGLWindowRenderTarget::reset(){
	env->CallVoidMethod(env,obj,resetID);
}

bool EGLWindowRenderTarget::activate(){
	return env->CallBooleanMethod(env,obj,activateID);
}

bool EGLWindowRenderTarget::deactivate(){
	return env->CallBooleanMethod(env,obj,deactivateID);
}

bool EGLWindowRenderTarget::activateAdditionalContext(){
	return env->CallBooleanMethod(env,obj,activateAdditionalContextID);
}

void EGLWindowRenderTarget::deactivateAdditionalContext(){
	return env->CallVoidMethod(env,obj,deactivateAdditionalContextID);
}

bool EGLWindowRenderTarget::isPrimary() const{
	return env->CallBooleanMethod(env,obj,isPrimaryID);
}

bool EGLWindowRenderTarget::isValid() const{
	return env->CallBooleanMethod(env,obj,isValidID);
}

int EGLWindowRenderTarget::getWidth() const{
	return env->CallIntMethod(env,obj,getWidthID);
}

int EGLWindowRenderTarget::getHeight() const{
	return env->CallIntMethod(env,obj,getHeightID);
}

}
}
