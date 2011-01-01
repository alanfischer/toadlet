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

#include "WGLRenderTarget.h"
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace peeper{

WGLRenderTarget::WGLRenderTarget():GLRenderTarget(),
	mGLRC(0),
	mDC(0)
{}

bool WGLRenderTarget::activate(){
	return wglMakeCurrent(mDC,mGLRC)==TRUE;
}

bool WGLRenderTarget::deactivate(){
	return wglMakeCurrent(NULL,NULL)==TRUE;
}

bool WGLRenderTarget::share(GLRenderTarget *target){
	return wglShareLists(((WGLRenderTarget*)target)->getGLRC(),mGLRC)==TRUE;
}

bool WGLRenderTarget::wglIsExtensionSupported(const char *extension){
	const size_t extlen=strlen(extension);
	const char *supported=NULL;

	PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");

	if(wglGetExtString){
		supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wglGetCurrentDC());
	}

	if(supported==NULL){
		supported=(char*)glGetString(GL_EXTENSIONS);
	}

	if(supported==NULL){
		return false;
	}

	// Start search at start, increment by 1 on no match
	const char *p;
	for(p=supported;;++p){
		// Set p to next possible match
		p=strstr(p,extension);

		if(p==NULL){
			return false;
		}

		// Make sure that the match is at the start of the string,
		// or that the previous char is a space, otherwise we could
		// match wglSomethingwglExtension to wglExtension
		// Also make sure that the following character is a space or
		// NULL, otherwise wglExtensionSomething might match wglExtension
		if((p==supported || p[-1]==' ') && (p[extlen]=='\0' || p[extlen]==' ')){
			return true;
		}
	}
}

}
}
