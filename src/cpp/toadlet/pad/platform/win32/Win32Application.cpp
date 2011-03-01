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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/peeper/CapabilitySet.h>
#include <toadlet/pad/platform/win32/Win32Application.h>
#include <toadlet/pad/ApplicationListener.h>
#include <windows.h>
#pragma comment(lib,"winmm.lib")
#if defined(TOADLET_PLATFORM_WINCE)
	#include <aygshell.h>
	#pragma comment(lib,"aygshell.lib")
#endif

#include <toadlet/peeper/plugins/glrenderer/GLRenderTarget.h>

#ifndef WM_MOUSEWHEEL
	#define WM_MOUSEWHEEL 0x020A
#endif

#ifndef D3DCREATE_MULTITHREADED
	#define D3DCREATE_MULTITHREADED 0x00000004L
#endif

#if defined(TOADLET_DEBUG)
	#if defined(TOADLET_BUILD_STATIC)
		#define TOADLET_LIBRARY_EXTENSION "_sd"
	#else
		#define TOADLET_LIBRARY_EXTENSION "_d"
	#endif
#else
	#if defined(TOADLET_BUILD_STATIC)
		#define TOADLET_LIBRARY_EXTENSION "_s"
	#else
		#define TOADLET_LIBRARY_EXTENSION ""
	#endif
#endif

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::image;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::flick;
using namespace toadlet::tadpole;
using namespace toadlet::tadpole::handler;

#if defined(TOADLET_HAS_OPENGL)
	#pragma comment(lib,"toadlet_peeper_glrenderer" TOADLET_LIBRARY_EXTENSION)
	extern "C" Renderer *new_GLRenderer();
	#if defined(TOADLET_PLATFORM_WINCE)
		extern "C" RenderTarget *new_EGLWindowRenderTarget(void *window,WindowRenderTargetFormat *format);
	#else
		extern "C" RenderTarget *new_WGLWindowRenderTarget(void *window,WindowRenderTargetFormat *format);
	#endif
#endif
#if defined(TOADLET_HAS_D3DM)
	#pragma comment(lib,"toadlet_peeper_d3dmrenderer" TOADLET_LIBRARY_EXTENSION)
	extern "C" Renderer *new_D3DMRenderer();
	extern "C" RenderTarget *new_D3DMWindowRenderTarget(void *window,WindowRenderTargetFormat *format);
#endif
#if defined(TOADLET_HAS_D3D9)
	#pragma comment(lib,"toadlet_peeper_d3d9renderer" TOADLET_LIBRARY_EXTENSION)
	extern "C" Renderer *new_D3D9Renderer();
	extern "C" RenderTarget *new_D3D9WindowRenderTarget(void *window,WindowRenderTargetFormat *format);
#endif
#if defined(TOADLET_HAS_D3D10)
	#pragma comment(lib,"toadlet_peeper_d3d10renderer" TOADLET_LIBRARY_EXTENSION)
	extern "C" Renderer *new_D3D10Renderer();
	extern "C" RenderTarget *new_D3D10WindowRenderTarget(void *window,WindowRenderTargetFormat *format);
#endif
#if defined(TOADLET_HAS_D3D11)
	#pragma comment(lib,"toadlet_peeper_d3d11renderer" TOADLET_LIBRARY_EXTENSION)
	extern "C" Renderer *new_D3D11Renderer();
	extern "C" RenderTarget *new_D3D11WindowRenderTarget(void *window,WindowRenderTargetFormat *format);
#endif
#if defined(TOADLET_PLATFORM_WIN32)
	#pragma comment(lib,"toadlet_ribbit_mmplayer" TOADLET_LIBRARY_EXTENSION)
	extern "C" AudioPlayer *new_MMPlayer();
#endif
#if defined(TOADLET_HAS_OPENAL)
	#pragma comment(lib,"toadlet_ribbit_alplayer" TOADLET_LIBRARY_EXTENSION)
	extern "C" AudioPlayer *new_ALPlayer();
#endif
#if defined(TOADLET_PLATFORM_WINCE)
	#pragma comment(lib,"toadlet_flick_htcmotiondetector" TOADLET_LIBRARY_EXTENSION)
	extern "C" MotionDetector *new_HTCMotionDetector();
	#pragma comment(lib,"toadlet_flick_samsungmotiondetector" TOADLET_LIBRARY_EXTENSION)
	extern "C" MotionDetector *new_SamsungMotionDetector();
#endif

namespace toadlet{
namespace pad{

struct Win32Attributes{
	HINSTANCE mInstance;
	egg::String mClassName;
	HWND mWnd;
	HICON mIcon;
	int mJoyID;
	JOYINFOEX mJoyInfo,mLastJoyInfo;
};

LRESULT CALLBACK wndProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam);

int Win32Application::mWindowCount=0;
Win32Application::ApplicationMap Win32Application::mApplicationMap;

Win32Application::Win32Application():
	//mTitle,
	mPositionX(0),
	mPositionY(0),
	mWidth(-1),
	mHeight(-1),
	mFullscreen(false),
	#if defined(TOADLET_PLATFORM_WINCE)
		mVisual(ImageDefinitions::Format_RGB_5_6_5,16,0),
	#else
		mVisual(ImageDefinitions::Format_RGBA_8,16,2),
	#endif
	mApplicationListener(NULL),
	mDifferenceMouse(false),
	mLastXMouse(0),mLastYMouse(0),
	mSkipNextMove(false),

	mBackable(false),
	mEngine(NULL),
	mRenderTarget(NULL),
	mRenderer(NULL),
	mAudioPlayer(NULL),
	mMotionDetector(NULL),

	//mRendererPlugins,
	//mCurrentRendererPlugin,
	//mNewRendererPlugin,
	mRendererOptions(0),
	//mAudioPlayerPlugins,
	mAudioPlayerOptions(0),
	//mMotionDetctorPlugins,

	mRun(false),
	#if defined(TOADLET_PLATFORM_WINCE)
		mAutoActivate(true),
	#else
		mAutoActivate(false),
	#endif
	mActive(false),
	mDestroyed(false),
	#if defined(TOADLET_PLATFORM_WINCE)
		mStopOnDeactivate(true),
	#else
		mStopOnDeactivate(false),
	#endif
	mContextActive(false),
	win32(NULL)
{
	win32=new Win32Attributes();
	win32->mInstance=0;
	win32->mWnd=0;
	win32->mIcon=0;
	win32->mJoyID=0;
	memset(&win32->mJoyInfo,0,sizeof(JOYINFOEX));
	memset(&win32->mLastJoyInfo,0,sizeof(JOYINFOEX));

	win32->mInstance=GetModuleHandle(NULL);
	
	#if defined(TOADLET_PLATFORM_WINCE)
		HRSRC res=FindResource((HMODULE)win32->mInstance,TEXT("HI_RES_AWARE"),TEXT("CEUX"));
		if(res==NULL){
			Logger::error(Categories::TOADLET_PAD,"No resource of type CEUX with name HI_RES_AWARE, may not render on all devices");
		}
	#endif

	#if defined(TOADLET_HAS_OPENGL)
		mRendererPlugins.add("gl",RendererPlugin(
			#if defined(TOADLET_PLATFORM_WINCE)
				new_EGLWindowRenderTarget
			#else
				new_WGLWindowRenderTarget
			#endif
			,new_GLRenderer
		));
	#endif
	#if defined(TOADLET_HAS_D3DM)
		mRendererPlugins.add("d3dm",RendererPlugin(new_D3DMWindowRenderTarget,new_D3DMRenderer));
	#endif
	#if defined(TOADLET_HAS_D3D9)
		mRendererPlugins.add("d3d9",RendererPlugin(new_D3D9WindowRenderTarget,new_D3D9Renderer));
	#endif
	#if defined(TOADLET_HAS_D3D10)
		mRendererPlugins.add("d3d10",RendererPlugin(new_D3D10WindowRenderTarget,new_D3D10Renderer));
	#endif
	#if defined(TOADLET_HAS_D3D11)
		mRendererPlugins.add("d3d11",RendererPlugin(new_D3D11WindowRenderTarget,new_D3D11Renderer));
	#endif

	#if defined(TOADLET_PLATFORM_WIN32)
		mAudioPlayerPlugins.add("mm",AudioPlayerPlugin(new_MMPlayer));
	#endif
	#if defined(TOADLET_HAS_OPENAL)
		mAudioPlayerPlugins.add("al",AudioPlayerPlugin(new_ALPlayer));
	#endif

	#if defined(TOADLET_PLATFORM_WINCE)
		mMotionDetectorPlugins.add("htc",MotionDetectorPlugin(new_HTCMotionDetector));
		mMotionDetectorPlugins.add("samsung",MotionDetectorPlugin(new_SamsungMotionDetector));
	#endif
}

Win32Application::~Win32Application(){
	destroy();

	delete[] mRendererOptions;
	delete[] mAudioPlayerOptions;

	delete win32;
}

void Win32Application::create(String renderer,String audioPlayer,String motionDetector){
	mEngine=new Engine(mBackable);

	mResourceArchive=Win32ResourceArchive::ptr(new Win32ResourceArchive(mEngine->getTextureManager()));
	mResourceArchive->open(win32->mInstance);
	mEngine->getArchiveManager()->manage(shared_static_cast<Archive>(mResourceArchive));
	mEngine->getTextureManager()->addResourceArchive(mResourceArchive);

	/// @todo: The Joystick/Keyboard/Mouse input should be moved to an input abstraction class useabout outside of pad or at least the Application class
	win32->mJoyInfo.dwSize=sizeof(JOYINFOEX);
	win32->mJoyInfo.dwFlags=JOY_RETURNALL;
	memcpy(&win32->mLastJoyInfo,&win32->mJoyInfo,sizeof(JOYINFOEX));
	int numJoys=0;
	int joyID=JOYSTICKID1;
	HRESULT result=S_OK;
	while((result=joyGetPosEx(joyID++,&win32->mJoyInfo))!=JOYERR_PARMS){
		if(result==JOYERR_NOERROR){numJoys++;}
	}
	if(numJoys>0){
		win32->mJoyID=JOYSTICKID1;
	}
	else{
		win32->mJoyID=-1;
	}
	Logger::alert(Categories::TOADLET_PAD,String("detected ")+numJoys+" joysticks");

	createWindow();
	activate();
	
	/// @todo: Unify the plugin framework a bit so we dont have as much code duplication for this potion, and the creating of the plugin
	mContextActive=true;
	mNewRendererPlugin=mCurrentRendererPlugin=renderer;
	if(renderer!="null"){
		if(renderer!=(char*)NULL){
			createContextAndRenderer(renderer);
		}
		else{
			Map<String,RendererPlugin>::iterator it;
			for(it=mRendererPlugins.begin();it!=mRendererPlugins.end();++it){
				if(createContextAndRenderer(it->first)) break;
			}
		}
	}
	if(audioPlayer!="null"){
		if(audioPlayer!=(char*)NULL){
			createAudioPlayer(audioPlayer);
		}
		else{
			Map<String,AudioPlayerPlugin>::iterator it;
			for(it=mAudioPlayerPlugins.begin();it!=mAudioPlayerPlugins.end();++it){
				if(createAudioPlayer(it->first)) break;
			}
		}
	}
	if(motionDetector!="null"){
		if(motionDetector!=(char*)NULL){
			createMotionDetector(motionDetector);
		}
		else{
			Map<String,MotionDetectorPlugin>::iterator it;
			for(it=mMotionDetectorPlugins.begin();it!=mMotionDetectorPlugins.end();++it){
				if(createMotionDetector(it->first)) break;
			}
		}
		createMotionDetector(motionDetector);
	}
}

void Win32Application::destroy(){
	if(mDestroyed){
		return;
	}

	mDestroyed=true;

	if(mEngine!=NULL){
		mEngine->destroy();
	}

	deactivate();
	
	destroyRendererAndContext();
	destroyAudioPlayer();
	destroyMotionDetector();
	destroyWindow();

	if(mEngine!=NULL){
		delete mEngine;
		mEngine=NULL;
	}
}

void Win32Application::start(){
	mRun=true;
	resized(mWidth,mHeight);
	runEventLoop();
}

void Win32Application::runEventLoop(){
	uint64 lastTime=System::mtime();
	while(mRun){
		stepEventLoop();

		if(mActive){
			uint64 currentTime=System::mtime();
			int dt=currentTime-lastTime;

			update(dt);

			if(mRenderer!=NULL){
				if(mRenderer->getStatus()==Renderer::RendererStatus_NEEDSRESET){
					mEngine->contextReset(mRenderer);
				}

				if(mWidth>0 && mHeight>0){
					render(mRenderer);
				}
			}

			if(mAudioPlayer!=NULL){
				mAudioPlayer->update(dt);
			}

			lastTime=currentTime;
		}

		System::msleep(0);
	}
}

void Win32Application::stepEventLoop(){
	MSG msg;
	while(PeekMessage(&msg,NULL,0,0,PM_REMOVE)){
		if(msg.message==WM_QUIT){
			mRun=false;
		}
		else{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	if(win32->mJoyID>=JOYSTICKID1){
		JOYINFOEX *joyInfo=&win32->mJoyInfo,*lastJoyInfo=&win32->mLastJoyInfo;
		MMRESULT result=joyGetPosEx(win32->mJoyID,joyInfo);
		if(	joyInfo->dwXpos!=lastJoyInfo->dwXpos || joyInfo->dwYpos!=lastJoyInfo->dwYpos || joyInfo->dwZpos!=lastJoyInfo->dwZpos ||
			joyInfo->dwRpos!=lastJoyInfo->dwRpos || joyInfo->dwUpos!=lastJoyInfo->dwUpos || joyInfo->dwVpos!=lastJoyInfo->dwVpos){
			joyMoved(joyToScalar(joyInfo->dwXpos),joyToScalar(joyInfo->dwYpos),joyToScalar(joyInfo->dwZpos),joyToScalar(joyInfo->dwRpos),joyToScalar(joyInfo->dwUpos),joyToScalar(joyInfo->dwVpos));
		}
		if(joyInfo->dwButtons!=lastJoyInfo->dwButtons){
			int pressedButtons=(joyInfo->dwButtons^lastJoyInfo->dwButtons)&joyInfo->dwButtons;
			int releasedButtons=(joyInfo->dwButtons^lastJoyInfo->dwButtons)&lastJoyInfo->dwButtons;
			int button=0;
			while(pressedButtons>0 || releasedButtons>0){
				if((pressedButtons&1)>0){
					joyPressed(button);
				}
				if((releasedButtons&1)>0){
					joyReleased(button);
				}
				pressedButtons>>=1;
				releasedButtons>>=1;
				button++;
			}
		}
		memcpy(lastJoyInfo,joyInfo,sizeof(JOYINFOEX));
	}

	if(mCurrentRendererPlugin!=mNewRendererPlugin){
		destroyRendererAndContext();
		mCurrentRendererPlugin=mNewRendererPlugin;
		createContextAndRenderer(mCurrentRendererPlugin);
	}
}

void Win32Application::stop(){
	Logger::debug(Categories::TOADLET_PAD,"Win32Application::stop");
	mRun=false;
}

void Win32Application::activate(){
	if(mActive==false){
		Logger::debug(Categories::TOADLET_PAD,"Win32Application::activate");
		mActive=true;

		if(mFullscreen){
			#if defined(TOADLET_PLATFORM_WINCE)
				// On WinCE, we need to hide the extra screen items
				SHFullScreen(win32->mWnd,SHFS_HIDESIPBUTTON|SHFS_HIDETASKBAR|SHFS_HIDESTARTICON);
			#endif
		}
		#if !defined(TOADLET_PLAFORM_WINCE)
			// On Win32, we need to call resized, since it is apparently not called
			resized(mWidth,mHeight);
		#endif

		// Context activation notes:		
		// We do not handle render or audio resources contexts here anymore
		//  We did just handle render, but we should do either all or none, and only the d3d9 resources handle context restore currently
	}
}

void Win32Application::deactivate(){
	if(mActive==true){
		Logger::debug(Categories::TOADLET_PAD,"Win32Application::deactivate");
		mActive=false;

		if(mFullscreen){
			#if defined(TOADLET_PLATFORM_WINCE)
				// On WinCE, we need to show the extra screen items
				SHFullScreen(win32->mWnd,SHFS_SHOWSIPBUTTON|SHFS_SHOWTASKBAR|SHFS_SHOWSTARTICON);
			#endif
		}

		// See Win32Application::activate notes
		
		if(mStopOnDeactivate){
			stop();
		}
	}
}

bool Win32Application::createWindow(){
	int screenWidth=GetSystemMetrics(SM_CXSCREEN);
	int screenHeight=GetSystemMetrics(SM_CYSCREEN);
	bool adjustSize=true;
	bool adjustPosition=false;

	if(mWidth==-1 || mHeight==-1 || mFullscreen){
		if(mWidth==-1){
			adjustSize=false;
			mWidth=screenWidth;
		}
		if(mHeight==-1){
			adjustSize=false;
			mHeight=screenHeight;
		}
		if(mFullscreen){
			adjustSize=true;
			adjustPosition=true;
		}
	}

	if(mWidth>screenWidth || mHeight>screenHeight){
		Error::unknown(Categories::TOADLET_PAD,
			"size cannot be greater than screen size");
		return false;
	}

	if(mPositionX>screenWidth || mPositionY>screenHeight){
		Error::unknown(Categories::TOADLET_PAD,
			"position cannot be greater than screen size");
		return false;
	}

	win32->mClassName=String("toadlet::pad::Win32Application:") + (mWindowCount++);

	WNDCLASS wndClass={0};
	wndClass.lpfnWndProc=wndProc;
	wndClass.hInstance=win32->mInstance;
	wndClass.hCursor=LoadCursor(NULL,IDC_ARROW);
	wndClass.lpszClassName=win32->mClassName;
	wndClass.hIcon=win32->mIcon;

	if(!RegisterClass(&wndClass)){
		Error::unknown(Categories::TOADLET_PAD,
			"failed to register window class");
		return false;
	}

	if(mFullscreen){
		int format=mVisual.pixelFormat;
		int redBits=ImageFormatConversion::getRedBits(format);
		int greenBits=ImageFormatConversion::getGreenBits(format);
		int blueBits=ImageFormatConversion::getBlueBits(format);
		int alphaBits=ImageFormatConversion::getAlphaBits(format);
		bool result=changeVideoMode(mWidth,mHeight,redBits+greenBits+blueBits+alphaBits);

		if(result==false){
			UnregisterClass(win32->mClassName,win32->mInstance);
			win32->mClassName=String();

			Error::unknown(Categories::TOADLET_PAD,
				String("fullscreen size not available:")+mWidth+","+mHeight+" bits:"+(redBits+greenBits+blueBits));
			return false;
		}
	}

	DWORD style=WS_VISIBLE,exStyle=0;
	if(mFullscreen==false){
		#if defined(TOADLET_PLATFORM_WINCE)
			style|=WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
		#else
			style|=WS_OVERLAPPEDWINDOW;
		#endif
	}
	else{
		#if !defined(TOADLET_PLATFORM_WINCE)
			style|=WS_POPUP;
			exStyle|=WS_EX_APPWINDOW;
		#endif
	}

	RECT rect;
	rect.left=mPositionX;
	rect.right=mPositionX+mWidth;
	rect.top=mPositionY;
	rect.bottom=mPositionY+mHeight;
	if(adjustPosition || adjustSize){ // If the specified size is the size of the client area, otherwise its the window and decorations
		AdjustWindowRectEx(&rect,style,false,exStyle);
		if(adjustPosition==false){
			rect.right=mPositionX+(rect.right-rect.left);
			rect.bottom=mPositionY+(rect.bottom-rect.top);
			rect.left=mPositionX;
			rect.top=mPositionY;
		}
		else if(adjustSize==false){
			rect.right=mWidth+rect.left;
			rect.bottom=mHeight+rect.top;
		}
	}

	win32->mWnd=CreateWindowEx(exStyle,win32->mClassName,mTitle,style,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,NULL,NULL,win32->mInstance,NULL);
	if(win32->mWnd==0){
		UnregisterClass(win32->mClassName,win32->mInstance);
		win32->mClassName=String();

		Error::unknown(Categories::TOADLET_PAD,
			"window creation error");
		return false;
	}

	// Use client rect for size, the one that matters
	GetClientRect(win32->mWnd,&rect);
	mHeight=rect.bottom-rect.top;
	mWidth=rect.right-rect.left;

	mApplicationMap.add(win32->mWnd,this);

	ShowWindow(win32->mWnd,SW_SHOW);
	SetForegroundWindow(win32->mWnd);
	SetFocus(win32->mWnd);

	return true;
}

void Win32Application::destroyWindow(){
	if(win32->mWnd!=0){
		DestroyWindow(win32->mWnd);
		ApplicationMap::iterator it=mApplicationMap.find(win32->mWnd);
		if(it!=mApplicationMap.end()){
			mApplicationMap.erase(it);
		}
		win32->mWnd=0;
	}

	if(win32->mClassName!=(char*)NULL){
		UnregisterClass(win32->mClassName,win32->mInstance);
		win32->mClassName=String();
	}
}

void Win32Application::setTitle(const String &title){
	mTitle=title;
	if(win32->mWnd!=0){
		SetWindowText(win32->mWnd,mTitle);
	}
}

const String &Win32Application::getTitle() const{
	return mTitle;
}

void Win32Application::setSize(int width,int height){
	mWidth=width;
	mHeight=height;
}

void Win32Application::setPosition(int x,int y){
	mPositionX=x;
	mPositionY=y;
}

int Win32Application::getPositionX() const{
	return mPositionX;
}

int Win32Application::getPositionY() const{
	return mPositionY;
}

int Win32Application::getWidth() const{
	return mWidth;
}

int Win32Application::getHeight() const{
	return mHeight;
}

void Win32Application::setFullscreen(bool fullscreen){
	mFullscreen=fullscreen;
}

bool Win32Application::getFullscreen() const{
	return mFullscreen;
}

void Win32Application::setVisual(const Visual &visual){
	mVisual=visual;
}

const Visual &Win32Application::getVisual() const{
	return mVisual;
}

void Win32Application::setApplicationListener(ApplicationListener *listener){
	mApplicationListener=listener;
}

ApplicationListener *Win32Application::getApplicationListener() const{
	return mApplicationListener;
}

void Win32Application::resized(int width,int height){
	if(mApplicationListener!=NULL){
		mApplicationListener->resized(width,height);
	}
}

void Win32Application::focusGained(){
	if(mApplicationListener!=NULL){
		mApplicationListener->focusGained();
	}
}

void Win32Application::focusLost(){
	if(mApplicationListener!=NULL){
		mApplicationListener->focusLost();
	}
}

void Win32Application::keyPressed(int key){
	if(mApplicationListener!=NULL){
		mApplicationListener->keyPressed(key);
	}
}

void Win32Application::keyReleased(int key){
	if(mApplicationListener!=NULL){
		mApplicationListener->keyReleased(key);
	}
}

void Win32Application::mousePressed(int x,int y,int button){
	if(mApplicationListener!=NULL){
		mApplicationListener->mousePressed(x,y,button);
	}
}

void Win32Application::mouseMoved(int x,int y){
	if(mApplicationListener!=NULL){
		mApplicationListener->mouseMoved(x,y);
	}
}

void Win32Application::mouseReleased(int x,int y,int button){
	if(mApplicationListener!=NULL){
		mApplicationListener->mouseReleased(x,y,button);
	}
}

void Win32Application::mouseScrolled(int x,int y,int scroll){
	if(mApplicationListener!=NULL){
		mApplicationListener->mouseScrolled(x,y,scroll);
	}
}

void Win32Application::joyPressed(int button){
	if(mApplicationListener!=NULL){
		mApplicationListener->joyPressed(button);
	}
}

void Win32Application::joyMoved(scalar x,scalar y,scalar z,scalar r,scalar u,scalar v){
	if(mApplicationListener!=NULL){
		mApplicationListener->joyMoved(x,y,z,r,u,v);
	}
}

void Win32Application::joyReleased(int button){
	if(mApplicationListener!=NULL){
		mApplicationListener->joyReleased(button);
	}
}

void Win32Application::update(int dt){
	if(mApplicationListener!=NULL){
		mApplicationListener->update(dt);
	}
}

void Win32Application::render(Renderer *renderer){
	if(mApplicationListener!=NULL){
		mApplicationListener->render(renderer);
	}
}

void Win32Application::setDifferenceMouse(bool difference){
	mDifferenceMouse=difference;
	mSkipNextMove=true;

	ShowCursor(!mDifferenceMouse);
}

void Win32Application::setBackable(bool backable){
	if(mEngine!=NULL){
		Error::unknown(Categories::TOADLET_PAD,"can not change backable once engine is created");
		return;
	}

	mBackable=backable;
}

void Win32Application::changeRendererPlugin(const String &plugin){
	mNewRendererPlugin=plugin;
}

void Win32Application::setRendererOptions(int *options,int length){
	if(mRendererOptions!=NULL){
		delete[] mRendererOptions;
	}

	mRendererOptions=new int[length];
	memcpy(mRendererOptions,options,length*sizeof(int));
}

void Win32Application::setAudioPlayerOptions(int *options,int length){
	if(mAudioPlayerOptions!=NULL){
		delete[] mAudioPlayerOptions;
	}

	mAudioPlayerOptions=new int[length];
	memcpy(mAudioPlayerOptions,options,length*sizeof(int));
}

void Win32Application::setIcon(void *icon){
	win32->mIcon=(HICON)icon;
	if(win32->mWnd!=0){
		SendMessage(win32->mWnd,WM_SETICON,ICON_SMALL,(LPARAM)win32->mIcon);
	}
}

void *Win32Application::getHINSTANCE() const{return win32->mInstance;}
void *Win32Application::getHWND() const{return win32->mWnd;}

RenderTarget *Win32Application::makeRenderTarget(const String &plugin){
	RenderTarget *target=NULL;
	DWORD flags=D3DCREATE_MULTITHREADED;

	#if defined(TOADLET_DEBUG)
		bool debug=true;
	#else
		bool debug=false;
	#endif
	WindowRenderTargetFormat::ptr format(new WindowRenderTargetFormat(mVisual,2,debug,0));

	Map<String,RendererPlugin>::iterator it=mRendererPlugins.find(plugin);
	if(it!=mRendererPlugins.end()){
		TOADLET_TRY
			target=it->second.createRenderTarget(win32->mWnd,format);
		TOADLET_CATCH(const Exception &){target=NULL;}
	}
	if(target!=NULL && target->isValid()==false){
		delete target;
		target=NULL;
	}
	return target;
}

Renderer *Win32Application::makeRenderer(const String &plugin){
	Renderer *renderer=NULL;
	Map<String,RendererPlugin>::iterator it=mRendererPlugins.find(plugin);
	if(it!=mRendererPlugins.end()){
		TOADLET_TRY
			renderer=it->second.createRenderer();
		TOADLET_CATCH(const Exception &){renderer=NULL;}
	}
	return renderer;
}

bool Win32Application::createContextAndRenderer(const String &plugin){
	Logger::debug(Categories::TOADLET_PAD,
		"Win32Application: creating RenderTarget and Renderer:"+plugin);

	bool result=false;
	mRenderTarget=makeRenderTarget(plugin);
	if(mRenderTarget!=NULL){
		mRenderer=makeRenderer(plugin);
		TOADLET_TRY
			result=mRenderer->create(this,mRendererOptions);
		TOADLET_CATCH(const Exception &){result=false;}
		if(result==false){
			delete mRenderer;
			mRenderer=NULL;
		}
	}
	if(result==false){
		delete mRenderTarget;
		mRenderTarget=NULL;
	}

	if(result==false){
		Logger::error(Categories::TOADLET_PAD,
			"error starting Renderer");
		return false;
	}
	else if(mRenderer==NULL){
		Logger::error(Categories::TOADLET_PAD,
			"error creating Renderer");
		return false;
	}

	if(mRenderer!=NULL){
		mRenderer->setRenderTarget(this);
		mEngine->setRenderer(mRenderer);
	}

	return mRenderer!=NULL;
}

bool Win32Application::destroyRendererAndContext(){
	Logger::debug(Categories::TOADLET_PAD,
		"Win32Application: destroying context and renderer");

	if(mRenderer!=NULL){
		mEngine->setRenderer(NULL);
		mRenderer->destroy();
		delete mRenderer;
		mRenderer=NULL;
	}

	if(mRenderTarget!=NULL){
		delete mRenderTarget;
		mRenderTarget=NULL;
	}

	return true;
}

bool Win32Application::changeVideoMode(int width,int height,int colorBits){
	bool result=false;

	#if defined(TOADLET_PLATFORM_WINCE)
		result=(width==GetSystemMetrics(SM_CXSCREEN) && height==GetSystemMetrics(SM_CYSCREEN));
	#else
		DEVMODE dmScreenSettings={0};
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth=width;
		dmScreenSettings.dmPelsHeight=height;
		dmScreenSettings.dmBitsPerPel=colorBits;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		// Try to set mode and get results.  CDS_FULLSCREEN gets rid of start bar.
		LONG dispResult=ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN);
		result=(dispResult==DISP_CHANGE_SUCCESSFUL);
	#endif

	return result;
}

AudioPlayer *Win32Application::makeAudioPlayer(const String &plugin){
	AudioPlayer *audioPlayer=NULL;
	Map<String,AudioPlayerPlugin>::iterator it=mAudioPlayerPlugins.find(plugin);
	if(it!=mAudioPlayerPlugins.end()){
		TOADLET_TRY
			audioPlayer=it->second.createAudioPlayer();
		TOADLET_CATCH(const Exception &){audioPlayer=NULL;}
	}
	return audioPlayer;
}

bool Win32Application::createAudioPlayer(const String &plugin){
	Logger::debug(Categories::TOADLET_PAD,
		"Win32Application: creating AudioPlayer:"+plugin);

	bool result=false;
	mAudioPlayer=makeAudioPlayer(plugin);
	if(mAudioPlayer!=NULL){
		TOADLET_TRY
			result=mAudioPlayer->create(mAudioPlayerOptions);
		TOADLET_CATCH(const Exception &){result=false;}
		if(result==false){
			delete mAudioPlayer;
			mAudioPlayer=NULL;
		}
	}

	if(result==false){
		Logger::error(Categories::TOADLET_PAD,
			"error starting AudioPlayer");
		return false;
	}
	else if(mAudioPlayer==NULL){
		Logger::error(Categories::TOADLET_PAD,
			"error creating AudioPlayer");
		return false;
	}

	if(mAudioPlayer!=NULL){
		mEngine->setAudioPlayer(mAudioPlayer);
	}
	return true;
}

bool Win32Application::destroyAudioPlayer(){
	if(mAudioPlayer!=NULL){
		mEngine->setAudioPlayer(NULL);
		mAudioPlayer->destroy();
		delete mAudioPlayer;
		mAudioPlayer=NULL;
	}
	return true;
}

MotionDetector *Win32Application::makeMotionDetector(const String &plugin){
	MotionDetector *motionDetector=NULL;
	Map<String,MotionDetectorPlugin>::iterator it=mMotionDetectorPlugins.find(plugin);
	if(it!=mMotionDetectorPlugins.end()){
		TOADLET_TRY
			motionDetector=it->second.createMotionDetector();
		TOADLET_CATCH(const Exception &){motionDetector=NULL;}
	}
	return motionDetector;
}

bool Win32Application::createMotionDetector(const String &plugin){
	Logger::debug(Categories::TOADLET_PAD,
		"Win32Application: creating MotionDetector:"+plugin);

	bool result=false;
	mMotionDetector=makeMotionDetector(plugin);
	if(mMotionDetector!=NULL){
		TOADLET_TRY
			result=mMotionDetector->create();
		TOADLET_CATCH(const Exception &){result=false;}
		if(result==false){
			delete mMotionDetector;
			mMotionDetector=NULL;
		}
	}

	if(result==false){
		Logger::error(Categories::TOADLET_PAD,
			"error starting MotionDetector");
		return false;
	}
	else if(mMotionDetector==NULL){
		Logger::error(Categories::TOADLET_PAD,
			"error creating MotionDetector");
		return false;
	}
	return true;
}

bool Win32Application::destroyMotionDetector(){
	if(mMotionDetector!=NULL){
		mMotionDetector->destroy();
		delete mMotionDetector;
		mMotionDetector=NULL;
	}
	return true;
}

void Win32Application::internal_resize(int width,int height){
	// We don't want any commands to be issued when we are destroyed
	if(mDestroyed){
		return;
	}

	mWidth=width;
	mHeight=height;

	resized(width,height);

	bool visible=(width>0 && height>0);
	if(mActive && mRenderer!=NULL){
		if(mRenderer->getCapabilitySet().resetOnResize){
			if(mContextActive==false && visible==true){
				mEngine->contextDeactivate(mRenderer);
				mContextActive=true;
			}
			else if(mContextActive==true && visible==false){
				mEngine->contextDeactivate(mRenderer);
				mContextActive=false;
			}
			else if(visible){
				mEngine->contextReset(mRenderer);
			}
		}
		
		update(0);

		if(width>0 && height>0){
			render(mRenderer);
		}
	}
}

void Win32Application::internal_mouseMoved(int x,int y){
	if(mSkipNextMove){
		mLastXMouse=x;mLastYMouse=y;
		mSkipNextMove=false;
		return;
	}

	if(mDifferenceMouse){
		int dx=x-mLastXMouse,dy=y-mLastYMouse;
		mLastXMouse=x;mLastYMouse=y;

		#if !defined(TOADLET_PLATFORM_WINCE)
			POINT pt={getWidth()/2,getHeight()/2};
			ClientToScreen((HWND)getHWND(),&pt);
			SetCursorPos(pt.x,pt.y);
			mSkipNextMove=true;
		#endif

		x=dx;y=dy;
	}

	mouseMoved(x,y);
}

LRESULT CALLBACK wndProc(HWND wnd,UINT msg,WPARAM wParam,LPARAM lParam){
	Win32Application *application;
	Win32Application::ApplicationMap::iterator it=Win32Application::mApplicationMap.find(wnd);
	if(it!=Win32Application::mApplicationMap.end()){
		application=it->second;
	}
	else{
		return DefWindowProc(wnd,msg,wParam,lParam);
	}

	int key=0;
	switch(msg){
		case WM_SETFOCUS:
			if(application->getAutoActivate()){
				application->activate();
			}
			application->focusGained();
			return 0;
		case WM_KILLFOCUS:
			application->focusLost();
			if(application->getAutoActivate()){
				application->deactivate();
			}
			return 0;
		case WM_SETTINGCHANGE:
			#if defined(TOADLET_PLATFORM_WINCE)
				// We have to re-hide the items when it issues the SETTINGCHANGE, but only if we're not in any sub windows
				if(application->getFullscreen() && application->active()){
					ShowWindow(wnd,SW_SHOWNORMAL);
					SetFocus(wnd);
					SHFullScreen(wnd,SHFS_HIDESIPBUTTON|SHFS_HIDETASKBAR|SHFS_HIDESTARTICON);
				}
			#endif
			return 0;
		case WM_SIZE:
			application->internal_resize(LOWORD(lParam),HIWORD(lParam));
			return 0;
		// WinCE does not call WM_CLOSE on an X press, instead it minimizes windows
		// However in WinCE 6, WM_CLOSE can be sent from the application closer
		case WM_CLOSE:
		case WM_QUIT:
		case WM_DESTROY:
			application->stop();
			return 0;
		case WM_SYSKEYDOWN:
			application->keyPressed(Win32Application::translateKey(wParam));
			// Pass it on so we can alt-f4
			break;
		case WM_SYSKEYUP:
			application->keyReleased(Win32Application::translateKey(wParam));
			// Pass it on so we can alt-f4
			break;
		#if defined(TOADLET_PLATFORM_WINCE)
			case WM_CHAR:
				application->keyPressed(wParam);
				application->keyReleased(wParam);
				return 0;
		#endif
		case WM_KEYDOWN:
			// Just ignore several of these
			if(wParam==0 || wParam==16 || wParam==132 || wParam==134 || wParam==190 || wParam>=200){
				return 0;
			}

			// Do not send repeated keys
			if((lParam&(1<<30))==0){
				key=Win32Application::translateKey(wParam);
				if(key==wParam){
					if(GetKeyState(VK_SHIFT)<0 || GetKeyState(VK_CAPITAL)!=0){
						key=toupper(key);
					}
					else{
						key=tolower(key);
					}
				}
				application->keyPressed(key);
			}
			return 0;
		case WM_COMMAND: // So the command buttons can be heard
		case WM_KEYUP:
			// Just ignore several of these
			if(wParam==0 || wParam==16 || wParam==132 || wParam==134 || wParam==190 || wParam>=200){
				return 0;
			}
			if(wParam==112){
				application->keyPressed(BaseApplication::Key_SOFTLEFT);
				application->keyReleased(BaseApplication::Key_SOFTLEFT);
			}
			else if(wParam==113){
				application->keyPressed(BaseApplication::Key_SOFTRIGHT);
				application->keyReleased(BaseApplication::Key_SOFTRIGHT);
			}
			else if(wParam==27){
				application->keyPressed(BaseApplication::Key_BACK);
				application->keyReleased(BaseApplication::Key_BACK);
			}
			else{
				key=Win32Application::translateKey(wParam);
				if(key==wParam){
					if(GetKeyState(VK_SHIFT)<0 || GetKeyState(VK_CAPITAL)!=0){
						key=toupper(key);
					}
					else{
						key=tolower(key);
					}
				}
				application->keyReleased(key);
			}
			return 0;
		case WM_MOUSEMOVE:
			application->internal_mouseMoved(LOWORD(lParam),HIWORD(lParam));
			return 0;
		case WM_LBUTTONDOWN:
			application->mousePressed(LOWORD(lParam),HIWORD(lParam),0);
			return 0;
		case WM_MBUTTONDOWN:
			application->mousePressed(LOWORD(lParam),HIWORD(lParam),1);
			return 0;
		case WM_RBUTTONDOWN:
			application->mousePressed(LOWORD(lParam),HIWORD(lParam),2);
			return 0;
		case WM_LBUTTONUP:
			application->mouseReleased(LOWORD(lParam),HIWORD(lParam),0);
			return 0;
		case WM_MBUTTONUP:
			application->mouseReleased(LOWORD(lParam),HIWORD(lParam),1);
			return 0;
		case WM_RBUTTONUP:
			application->mouseReleased(LOWORD(lParam),HIWORD(lParam),2);
			return 0;
		case WM_MOUSEWHEEL:
			application->mouseScrolled(LOWORD(lParam),HIWORD(lParam),wParam);
			return 0;
	}

	return DefWindowProc(wnd,msg,wParam,lParam);
}

int Win32Application::translateKey(int key){
	switch(key){
		case VK_ESCAPE:
			return Key_ESC;
		case VK_LEFT:
			return Key_LEFT;
		case VK_UP:
			return Key_UP;
		case VK_RIGHT:
			return Key_RIGHT;
		case VK_DOWN:
			return Key_DOWN;
		case VK_TAB:
			return Key_TAB;
		case VK_MENU:
			return Key_ALT;
		case VK_PAUSE:
			return Key_PAUSE;
		case VK_SPACE:
			return Key_SPACE;
		case VK_SHIFT:
			return Key_SHIFT;
		case VK_CONTROL:
			return Key_CTRL;
		case VK_LWIN:
		case VK_RWIN:
			return Key_SPECIAL;
		case VK_RETURN:
			return Key_ENTER;
		case VK_BACK:
			return Key_BACKSPACE;
		case VK_DELETE:
			return Key_DELETE;
	}

	return key;
}

scalar Win32Application::joyToScalar(int joy){
	return Math::fromFloat(((float)joy/(float)65536)*2-1);
}

}
}
