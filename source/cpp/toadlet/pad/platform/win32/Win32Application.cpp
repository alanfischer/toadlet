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
#include <toadlet/pad/platform/win32/Win32Application.h>
#include <windows.h>
#if defined(TOADLET_PLATFORM_WINCE)
	#include <aygshell.h>
	#pragma comment(lib,"aygshell.lib")
#endif

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

using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::flick;
using namespace toadlet::tadpole::handler;

#if defined(TOADLET_HAS_OPENGL)
	#pragma comment(lib,"toadlet_peeper_glrenderdevice" TOADLET_LIBRARY_EXTENSION)
	extern "C" RenderDevice *new_GLRenderDevice();
	#if defined(TOADLET_PLATFORM_WINCE)
		extern "C" RenderTarget *new_EGLWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
	#else
		extern "C" RenderTarget *new_WGLWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
	#endif
#endif
#if defined(TOADLET_HAS_D3DM)
	#pragma comment(lib,"toadlet_peeper_d3dmrenderdevice" TOADLET_LIBRARY_EXTENSION)
	extern "C" RenderDevice *new_D3DMRenderDevice();
	extern "C" RenderTarget *new_D3DMWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
#endif
#if defined(TOADLET_HAS_D3D9)
	#pragma comment(lib,"toadlet_peeper_d3d9renderdevice" TOADLET_LIBRARY_EXTENSION)
	extern "C" RenderDevice *new_D3D9RenderDevice();
	extern "C" RenderTarget *new_D3D9WindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
#endif
#if defined(TOADLET_HAS_D3D10)
	#pragma comment(lib,"toadlet_peeper_d3d10renderdevice" TOADLET_LIBRARY_EXTENSION)
	extern "C" RenderDevice *new_D3D10RenderDevice();
	extern "C" RenderTarget *new_D3D10WindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
#endif
#if defined(TOADLET_HAS_D3D11)
	#pragma comment(lib,"toadlet_peeper_d3d11renderdevice" TOADLET_LIBRARY_EXTENSION)
	extern "C" RenderDevice *new_D3D11RenderDevice();
	extern "C" RenderTarget *new_D3D11WindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
#endif

#if defined(TOADLET_HAS_OPENAL)
	#pragma comment(lib,"toadlet_ribbit_alaudiodevice" TOADLET_LIBRARY_EXTENSION)
	extern "C" AudioDevice *new_ALAudioDevice();
#endif

#pragma comment(lib,"toadlet_ribbit_mmaudiodevice" TOADLET_LIBRARY_EXTENSION)
extern "C" AudioDevice *new_MMAudioDevice();

#pragma comment(lib,"toadlet_flick_win32joydevice" TOADLET_LIBRARY_EXTENSION)
extern "C" InputDevice *new_Win32JoyDevice();


namespace toadlet{
namespace pad{

struct Win32Attributes{
	HINSTANCE mInstance;
	egg::String mClassName;
	HWND mWnd;
	HICON mIcon;
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
	mDifferenceMouse(false),
	mLastXMouse(0),mLastYMouse(0),
	mSkipNextMove(false),

	mRun(false),
	mActive(false),
	mDestroyed(false),
	mContextActive(false),
	win32(NULL)
{
	win32=new Win32Attributes();
	win32->mInstance=0;
	win32->mWnd=0;
	win32->mIcon=0;
	win32->mInstance=GetModuleHandle(NULL);
	
	#if defined(TOADLET_PLATFORM_WINCE)
		HRSRC res=FindResource((HMODULE)win32->mInstance,TEXT("HI_RES_AWARE"),TEXT("CEUX"));
		if(res==NULL){
			Logger::error(Categories::TOADLET_PAD,"No resource of type CEUX with name HI_RES_AWARE, may not render on all devices");
		}
	#endif

	#if defined(TOADLET_HAS_D3DM)
		mRenderDevicePlugins.add("d3dm",RenderDevicePlugin(new_D3DMWindowRenderTarget,new_D3DMRenderDevice));
	#endif
	#if defined(TOADLET_HAS_D3D9)
		mRenderDevicePlugins.add("d3d9",RenderDevicePlugin(new_D3D9WindowRenderTarget,new_D3D9RenderDevice));
	#endif
	#if defined(TOADLET_HAS_D3D10)
		mRenderDevicePlugins.add("d3d10",RenderDevicePlugin(new_D3D10WindowRenderTarget,new_D3D10RenderDevice));
	#endif
	#if defined(TOADLET_HAS_D3D11)
		mRenderDevicePlugins.add("d3d11",RenderDevicePlugin(new_D3D11WindowRenderTarget,new_D3D11RenderDevice));
	#endif
	#if defined(TOADLET_HAS_OPENGL)
		mRenderDevicePlugins.add("gl",RenderDevicePlugin(
			#if defined(TOADLET_PLATFORM_WINCE)
				new_EGLWindowRenderTarget
			#else
				new_WGLWindowRenderTarget
			#endif
			,new_GLRenderDevice
		));
	#endif
	mRenderDevicePreferences.add("d3d9");
	mRenderDevicePreferences.add("d3dm");
	mRenderDevicePreferences.add("d3d10");
	mRenderDevicePreferences.add("d3d11");
	mRenderDevicePreferences.add("gl");

	#if defined(TOADLET_HAS_OPENAL)
		mAudioDevicePlugins.add("al",AudioDevicePlugin(new_ALAudioDevice));
	#endif
	#if defined(TOADLET_PLATFORM_WIN32)
		mAudioDevicePlugins.add("mm",AudioDevicePlugin(new_MMAudioDevice));
	#endif
	mAudioDevicePreferences.add("al");
	mAudioDevicePreferences.add("mm");
}

Win32Application::~Win32Application(){
	destroy();

	delete[] mRenderDeviceOptions;
	delete[] mAudioDeviceOptions;

	delete win32;
}

void Win32Application::preEngineCreate(){
	mContextActive=true;

	createWindow();
}

void Win32Application::postEngineCreate(){
	InputDevice *joyDevice=new_Win32JoyDevice();
	if(joyDevice->create()==false){
		delete joyDevice;
		joyDevice=NULL;
	}
	else{
		mInputDevices[joyDevice->getType()]=joyDevice;
	}

	mResourceArchive=Win32ResourceArchive::ptr(new Win32ResourceArchive(mEngine->getTextureManager()));
	mResourceArchive->open(win32->mInstance);
	mEngine->getArchiveManager()->manage(shared_static_cast<Archive>(mResourceArchive));
	mEngine->getTextureManager()->addResourceArchive(mResourceArchive);
}

void Win32Application::destroy(){
	if(mDestroyed){
		return;
	}

	mDestroyed=true;

	BaseApplication::destroy();

	destroyWindow();
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

			if(mRenderDevice!=NULL){
				if(mRenderDevice->activate()==RenderDevice::DeviceStatus_NEEDSRESET){
					mEngine->contextReset(mRenderDevice);
				}

				if(mWidth>0 && mHeight>0){
					render();
				}
			}

			if(mAudioDevice!=NULL){
				mAudioDevice->update(dt);
			}

			int i;
			for(i=0;i<mInputDevices.size();++i){
				InputDevice *device=mInputDevices[i];
				if(device!=NULL){
					device->update(dt);
				}
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

	if(mCurrentRenderDevicePlugin!=mNewRenderDevicePlugin){
		destroyRenderDeviceAndContext();
		mCurrentRenderDevicePlugin=mNewRenderDevicePlugin;
		createContextAndRenderDevice(mCurrentRenderDevicePlugin);
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
		int format=mFormat->pixelFormat;
		int redBits=TextureFormat::getRedBits(format);
		int greenBits=TextureFormat::getGreenBits(format);
		int blueBits=TextureFormat::getBlueBits(format);
		int alphaBits=TextureFormat::getAlphaBits(format);
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

	if(win32->mClassName!=NULL){
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

String Win32Application::getTitle() const{
	if(win32->mWnd!=0){
		TCHAR title[1024];
		GetWindowText(win32->mWnd,title,1024);
		mTitle=title;
	}
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

void Win32Application::setDifferenceMouse(bool difference){
	mDifferenceMouse=difference;
	mSkipNextMove=true;

	ShowCursor(!mDifferenceMouse);
}

void Win32Application::setIcon(void *icon){
	win32->mIcon=(HICON)icon;
	if(win32->mWnd!=0){
		SendMessage(win32->mWnd,WM_SETICON,ICON_SMALL,(LPARAM)win32->mIcon);
	}
}

void *Win32Application::getHINSTANCE() const{return win32->mInstance;}
void *Win32Application::getHWND() const{return win32->mWnd;}

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

void Win32Application::internal_resize(int width,int height){
	// We don't want any commands to be issued when we are destroyed
	if(mDestroyed){
		return;
	}

	mWidth=width;
	mHeight=height;

	resized(width,height);

	bool visible=(width>0 && height>0);
	if(mActive && mRenderDevice!=NULL){
		if(mEngine->getRenderCaps().resetOnResize){
			if(mContextActive==false && visible==true){
				mEngine->contextDeactivate(mRenderDevice);
				mContextActive=true;
			}
			else if(mContextActive==true && visible==false){
				mEngine->contextDeactivate(mRenderDevice);
				mContextActive=false;
			}
			else if(visible){
				mEngine->contextReset(mRenderDevice);
			}
		}
		
		update(0);

		if(width>0 && height>0){
			render();
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
			application->focusGained();
			return 0;
		case WM_KILLFOCUS:
			application->focusLost();
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

}
}
