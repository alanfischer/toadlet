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

#include "WGLWindowRenderTarget.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/System.h>
#include <toadlet/peeper/TextureFormat.h>

namespace toadlet{
namespace peeper{

TOADLET_C_API RenderTarget *new_WGLWindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format){
	return new WGLWindowRenderTarget(wnd,format);
}

WGLWindowRenderTarget::WGLWindowRenderTarget():WGLRenderTarget(),
	mWnd(NULL)
	//mPFD,
	//mThreadContexts,
	//mThreadIDs
{}

WGLWindowRenderTarget::WGLWindowRenderTarget(HWND wnd,WindowRenderTargetFormat *format):WGLRenderTarget(),
	mWnd(NULL)
	//mPFD,
	//mThreadContexts,
	//mThreadIDs
{
	int winPixelFormat=0;

	if(format->multisamples>1){
		HWND tmpWnd=CreateWindow(TEXT("Static"),NULL,0,0,0,0,0,0,0,0,0);
		bool result=createContext(tmpWnd,format,winPixelFormat);

		PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormatARB=NULL;
		if(result && wglIsExtensionSupported("WGL_ARB_multisample") && (wglChoosePixelFormatARB=(PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB"))!=NULL){
			int pixelFormat=format->pixelFormat;
			int redBits=TextureFormat::getRedBits(pixelFormat);
			int greenBits=TextureFormat::getGreenBits(pixelFormat);
			int blueBits=TextureFormat::getBlueBits(pixelFormat);
			int alphaBits=TextureFormat::getAlphaBits(pixelFormat);
			int colorBits=checkDefaultColorBits(redBits+blueBits+greenBits);

			unsigned int numFormats;
			float fAttributes[]={0,0};
			int iAttributes[]={
				WGL_DRAW_TO_WINDOW_ARB,	GL_TRUE,
				WGL_SUPPORT_OPENGL_ARB,	GL_TRUE,
				WGL_ACCELERATION_ARB,	WGL_FULL_ACCELERATION_ARB,
				WGL_COLOR_BITS_ARB,		colorBits,
				WGL_ALPHA_BITS_ARB,		alphaBits,
				WGL_DEPTH_BITS_ARB,		format->depthBits,
				WGL_STENCIL_BITS_ARB,	format->stencilBits,
				WGL_DOUBLE_BUFFER_ARB,	GL_TRUE,
				WGL_SAMPLE_BUFFERS_ARB,	GL_TRUE,
				WGL_SAMPLES_ARB,		format->multisamples,
				0,0};
			const int sampleIndex=19;

			BOOL valid=false;
			do{
				valid=wglChoosePixelFormatARB(mDC,iAttributes,fAttributes,1,&winPixelFormat,&numFormats);
				iAttributes[sampleIndex]/=2;
			}while(valid==false && iAttributes[sampleIndex]>1);
		}

		destroyContext();
		DestroyWindow(tmpWnd);
	}

	createContext(wnd,format,winPixelFormat);
}

WGLWindowRenderTarget::~WGLWindowRenderTarget(){
	destroy();
}

bool WGLWindowRenderTarget::createContext(HWND wnd,WindowRenderTargetFormat *format,int winPixelFormat){
	BOOL result=0;

	mWnd=wnd;
	mDC=GetDC(mWnd);
	if(mDC==0){
		Error::unknown(Categories::TOADLET_PEEPER,
			"Failed to get the window DC");
		return false;
	}

	if(winPixelFormat==0){
		int pixelFormat=format->pixelFormat;
		int redBits=TextureFormat::getRedBits(pixelFormat);
		int greenBits=TextureFormat::getGreenBits(pixelFormat);
		int blueBits=TextureFormat::getBlueBits(pixelFormat);
		int alphaBits=TextureFormat::getAlphaBits(pixelFormat);
		int colorBits=checkDefaultColorBits(redBits+blueBits+greenBits);

		PIXELFORMATDESCRIPTOR pfd={0};
		pfd.nSize=sizeof(pfd);
		pfd.nVersion=1;
		pfd.dwFlags=PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
		pfd.iPixelType=PFD_TYPE_RGBA;
		pfd.cColorBits=colorBits;
		pfd.cAlphaBits=alphaBits;
		pfd.cDepthBits=format->depthBits;
		pfd.cStencilBits=format->stencilBits;
		pfd.iLayerType=PFD_MAIN_PLANE;
		mPFD=pfd;

		winPixelFormat=ChoosePixelFormat(mDC,&mPFD);
	}

	result=SetPixelFormat(mDC,winPixelFormat,&mPFD);
	if(result==0){
		destroyContext();
		Error::unknown(Categories::TOADLET_PEEPER,
			"Failed to set the PixelFormat");
		return false;
	}

	mGLRC=wglCreateContext(mDC);
	if(mGLRC==0){
		destroyContext();
		Error::unknown(Categories::TOADLET_PEEPER,
			"Failed to create a GL rendering context");
		return false;
	}

	result=wglMakeCurrent(mDC,mGLRC);
	if(result==0){
		destroyContext();
		Error::unknown(Categories::TOADLET_PEEPER,
			"Failed to activate the GL rendering context");
		return false;
	}

	if(wglIsExtensionSupported("WGL_EXT_swap_control")){
		PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT=
			(PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

		if(wglSwapIntervalEXT!=NULL){
			wglSwapIntervalEXT((int)format->vsync);
		}
	}

	// Use a HACK here to check to see if its a crashy pixel format on FireGL cards
	String rendererName=glGetString(GL_RENDERER);
	if(rendererName.equals("ATI FireGL V3100") && mPFD.cColorBits==16){
		destroyContext();
		Error::unknown(Categories::TOADLET_PEEPER,
			"The requested pixel format is known to crash on this card.  Try 24 color bits");
		return false;
	}

	int numThreads=format->threads<=1?0:format->threads-1;
	mThreadContexts.resize(numThreads,0);
	mThreadIDs.resize(numThreads,0);
	int i;
	for(i=0;i<numThreads;++i){
		HGLRC glrc=wglCreateContext(mDC);
		if(glrc==0){
			destroyContext();
			Error::unknown(Categories::TOADLET_PEEPER,
				"Failed to create threaded GL rendering context");
			return false;
		}
		mThreadContexts[i]=glrc;
		result=wglShareLists(mGLRC,glrc);
		if(result==FALSE){
			destroyContext();
			Error::unknown(Categories::TOADLET_PEEPER,
				"Failed to share threaded GL rendering context");
			return false;
		}
	}

	return true;
}

bool WGLWindowRenderTarget::destroyContext(){
	int i;
	for(i=0;i<mThreadContexts.size();++i){
		if(mThreadContexts[i]!=0){
			wglDeleteContext(mThreadContexts[i]);
		}
	}
	mThreadContexts.clear();
	mThreadIDs.clear();

	if(mGLRC!=0){
		wglMakeCurrent(NULL,NULL);
		wglDeleteContext(mGLRC);
		mGLRC=0;
	}

	if(mDC!=0){
		BOOL result=ReleaseDC(mWnd,mDC);
		mDC=0;
		if(result==0){
			Error::unknown(Categories::TOADLET_PEEPER,
				"Error releasing DC");
			return false;
		}
	}

	return true;
}

bool WGLWindowRenderTarget::activateAdditionalContext(){
	int threadID=System::threadID();
	int i;
	for(i=0;i<mThreadIDs.size();++i){
		if(mThreadIDs[i]==threadID){
			return false;
		}
	}

	for(i=0;i<mThreadIDs.size();++i){
		if(mThreadIDs[i]==0){
			break;
		}
	}
	if(i==mThreadIDs.size()){
		return false;
	}

	BOOL result=wglMakeCurrent(mDC,mThreadContexts[i]);
	if(result==TRUE){
		mThreadIDs[i]=threadID;
	}
	return result==TRUE;
}

void WGLWindowRenderTarget::deactivateAdditionalContext(){
	int threadID=System::threadID();
	int i;
	for(i=0;i<mThreadIDs.size();++i){
		if(mThreadIDs[i]==threadID){
			break;
		}
	}
	if(i==mThreadIDs.size()){
		return;
	}

	wglMakeCurrent(mDC,NULL);
	mThreadIDs[i]=0;
}

bool WGLWindowRenderTarget::swap(){
	return SwapBuffers(mDC)==TRUE;
}

int WGLWindowRenderTarget::checkDefaultColorBits(int colorBits) const{
	if(colorBits==0){
		colorBits=24;
	}
	return colorBits;
}

int WGLWindowRenderTarget::getWidth() const{
	RECT rect;
	GetClientRect(mWnd,&rect);
	return rect.right-rect.left;
}

int WGLWindowRenderTarget::getHeight() const{
	RECT rect;
	GetClientRect(mWnd,&rect);
	return rect.bottom-rect.top;
}

}
}
