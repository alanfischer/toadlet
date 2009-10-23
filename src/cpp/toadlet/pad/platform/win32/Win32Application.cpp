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

#if defined(TOADLET_PLATFORM_WINCE)
	#include <aygshell.h>
	#pragma comment(lib,"aygshell.lib")
#endif

#ifndef WM_MOUSEWHEEL
	#define WM_MOUSEWHEEL 0x020A
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
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::tadpole;

#if defined(TOADLET_HAS_OPENGL)
	#pragma comment(lib,"toadlet_peeper_glrenderer" TOADLET_LIBRARY_EXTENSION)
	extern "C" Renderer *new_GLRenderer();
	#if defined(TOADLET_PLATFORM_WINCE)
		extern "C" RenderTarget *new_EGLRenderTarget(void *nativeSurface,const Visual &visual);
	#else
		extern "C" RenderTarget *new_WGLWindowRenderTarget(HWND wnd,const Visual &visual);
	#endif
#endif
#if defined(TOADLET_HAS_DIRECT3DMOBILE)
	#pragma comment(lib,"toadlet_peeper_d3dmrenderer" TOADLET_LIBRARY_EXTENSION)
	extern "C" Renderer *new_D3DMRenderer();
	extern "C" RenderTarget *new_D3DMWindowRenderTarget(HWND wnd,const Visual &visual);
#elif defined(TOADLET_HAS_DIRECT3D9)
	#pragma comment(lib,"toadlet_peeper_d3d9renderer" TOADLET_LIBRARY_EXTENSION)
	extern "C" Renderer *new_D3D9Renderer();
//	extern "C" RenderTarget *new_D3D9WindowRenderTarget(HWND wnd,const Visual &visual);
#endif
#if defined(TOADLET_PLATFORM_WIN32)
	#pragma comment(lib,"toadlet_ribbit_win32player" TOADLET_LIBRARY_EXTENSION)
	extern "C" AudioPlayer *new_Win32Player();
#endif
#if defined(TOADLET_HAS_OPENAL)
	#pragma comment(lib,"toadlet_ribbit_alplayer" TOADLET_LIBRARY_EXTENSION)
	extern "C" AudioPlayer *new_ALPlayer();
#endif

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
	mFullscreen(true),
	#if defined(TOADLET_PLATFORM_WINCE)
		mVisual(ImageDefinitions::Format_RGB_5_6_5,16),
	#else
		mVisual(ImageDefinitions::Format_RGB_8,16),
	#endif
	mApplicationListener(NULL),

	mEngine(NULL),
	mRenderer(NULL),
	mRendererPlugin(RendererPlugin_NONE),
	mChangeRendererPlugin(RendererPlugin_NONE),
	mRendererOptions(NULL),
	mAudioPlayer(NULL),

	mRun(false),
	#if defined(TOADLET_PLATFORM_WINCE)
		mAutoActivate(true),
	#else
		mAutoActivate(false),
	#endif
	mActive(false),
	mDestroyed(false),
	win32(NULL)
{
	win32=new Win32Attributes();
	win32->mInstance=0;
	win32->mWnd=0;
	win32->mIcon=0;

	changeRendererPlugin(RendererPlugin_OPENGL); // OpenGL By default

	win32->mInstance=GetModuleHandle(NULL);
}

Win32Application::~Win32Application(){
	delete[] mRendererOptions;

	delete win32;
}

void Win32Application::create(){
	mEngine=new Engine();

	#if defined(TOADLET_HAS_OPENAL)
		if(mAudioPlayer==NULL){
			mAudioPlayer=new_ALPlayer();
			bool result=false;
			TOADLET_TRY
				result=mAudioPlayer->startup(NULL);
			TOADLET_CATCH(const Exception &){result=false;}
			if(result==false){
				delete mAudioPlayer;
				mAudioPlayer=NULL;
			}
		}
	#endif
	#if defined(TOADLET_PLATFORM_WIN32)
		if(mAudioPlayer==NULL){
			mAudioPlayer=new_Win32Player();
			bool result=false;
			TOADLET_TRY
				result=mAudioPlayer->startup(NULL);
			TOADLET_CATCH(const Exception &){result=false;}
			if(result==false){
				delete mAudioPlayer;
				mAudioPlayer=NULL;
			}
		}
	#endif
	if(mAudioPlayer!=NULL){
		mEngine->setAudioPlayer(mAudioPlayer);
	}

//HACK
createWindow();
mRendererPlugin=mChangeRendererPlugin;
createContextAndRenderer(mRendererPlugin);
mActive=true;
}

void Win32Application::destroy(){
	if(mDestroyed){
		return;
	}

	mDestroyed=true;

	if(mAudioPlayer!=NULL){
		mEngine->setAudioPlayer(NULL);
		mAudioPlayer->shutdown();
		delete mAudioPlayer;
		mAudioPlayer=NULL;
	}

	deactivate();

	destroyWindow();

	if(mEngine!=NULL){
		delete mEngine;
		mEngine=NULL;
	}
}

bool Win32Application::start(bool runEventLoop){
// HACK
//	destroyWindow();

//	createWindow();
resized(mWidth,mHeight);

	mRun=true;

	activate();

	uint64 lastTime=System::mtime();
	while(runEventLoop && mRun){
		stepEventLoop();

		if(mActive){
			uint64 currentTime=System::mtime();
			update(currentTime-lastTime);
			if(mRenderer!=NULL){
				render(mRenderer);
			}
			lastTime=currentTime;
		}

		System::msleep(10);
	}

	deactivate();

	return true;
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

	if(mChangeRendererPlugin!=mRendererPlugin){
		mRendererPlugin=mChangeRendererPlugin;

		destroyRendererAndContext();

		createContextAndRenderer(mRendererPlugin);
	}
}

void Win32Application::stop(){
	mRun=false;
}

void Win32Application::setAutoActivate(bool autoActivate){
	mAutoActivate=autoActivate;
}

void Win32Application::activate(){
	if(mActive==false){
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

		createContextAndRenderer(mRendererPlugin);
	}
}

void Win32Application::deactivate(){
	if(mActive==true){
		mActive=false;

		destroyRendererAndContext();

		if(mFullscreen){
			#if defined(TOADLET_PLATFORM_WINCE)
				// On WinCE, we need to show the extra screen items
				SHFullScreen(win32->mWnd,SHFS_SHOWSIPBUTTON|SHFS_SHOWTASKBAR|SHFS_SHOWSTARTICON);
			#endif
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
			"Size cannot be greater than screen size");
		return false;
	}

	if(mPositionX>screenWidth || mPositionY>screenHeight){
		Error::unknown(Categories::TOADLET_PAD,
			"Position cannot be greater than screen size");
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
			"Failed to register window class");
		return false;
	}

	if(mFullscreen){
		int format=mVisual.pixelFormat;
		int redBits=ImageFormatConversion::getRedBits(format);
		int greenBits=ImageFormatConversion::getGreenBits(format);
		int blueBits=ImageFormatConversion::getBlueBits(format);
		bool result=changeVideoMode(mWidth,mHeight,redBits+greenBits+blueBits);

		if(result==false){
			UnregisterClass(win32->mClassName,win32->mInstance);
			win32->mClassName=String();

			Error::unknown(Categories::TOADLET_PAD,
				String("Fullscreen size not available:")+mWidth+","+mHeight);
			return false;
		}
	}

	DWORD style=WS_VISIBLE;
	if(mFullscreen==false){
		#if defined(TOADLET_PLATFORM_WINCE)
			style|=WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX;
		#else
			style|=WS_OVERLAPPEDWINDOW;
		#endif
	}

	RECT rect;
	rect.left=mPositionX;
	rect.right=mPositionX+mWidth;
	rect.top=mPositionY;
	rect.bottom=mPositionY+mHeight;
	if(adjustPosition || adjustSize){ // If the specified size is the size of the client area, otherwise its the window and decorations
		AdjustWindowRectEx(&rect,style,false,0);
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

	win32->mWnd=CreateWindow(win32->mClassName,mTitle,style,rect.left,rect.top,rect.right-rect.left,rect.bottom-rect.top,NULL,NULL,win32->mInstance,NULL);
	if(win32->mWnd==0){
		UnregisterClass(win32->mClassName,win32->mInstance);
		win32->mClassName=String();

		Error::unknown(Categories::TOADLET_PAD,
			"Window creation error");
		return false;
	}

	mApplicationMap[win32->mWnd]=this;

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

void Win32Application::changeRendererPlugin(int rendererPlugin){
	mChangeRendererPlugin=rendererPlugin;
}

void Win32Application::setRendererOptions(int *options,int length){
	if(mRendererOptions!=NULL){
		delete mRendererOptions;
	}

	mRendererOptions=new int[length];
	memcpy(mRendererOptions,options,length*sizeof(int));
}

void Win32Application::setIcon(void *icon){
	win32->mIcon=(HICON)icon;
	if(win32->mWnd!=0){
		SendMessage(win32->mWnd,WM_SETICON,ICON_SMALL,(LPARAM)win32->mIcon);
	}
}

void *Win32Application::getHINSTANCE() const{return win32->mInstance;}
void *Win32Application::getHWND() const{return win32->mWnd;}

RenderTarget *Win32Application::makeRenderTarget(int rendererPlugin){
	RenderTarget *target=NULL;
	if(rendererPlugin==RendererPlugin_OPENGL){
		#if defined(TOADLET_HAS_OPENGL)
			#if defined(TOADLET_PLATFORM_WINCE)
				target=new_EGLRenderTarget(win32->mWnd,mVisual);
			#else
				target=new_WGLWindowRenderTarget(win32->mWnd,mVisual);
			#endif
		#endif
	}
	else if(rendererPlugin==RendererPlugin_DIRECT3D){
		#if defined(TOADLET_HAS_DIRECT3DMOBILE)
			target=new_D3DMWindowRenderTarget(win32->mWnd,mVisual);
		#elif defined(TOADLET_HAS_DIRECT3D9)
//			target=new_D3D9WindowRenderTarget(win32->mWnd,mVisual);
		#endif
	}
	if(target!=NULL && target->isValid()==false){
		delete target;
		target=NULL;
	}
	return target;
}

Renderer *Win32Application::makeRenderer(int plugin){
	Renderer *renderer=NULL;
	if(plugin==RendererPlugin_OPENGL){
		#if defined(TOADLET_HAS_OPENGL)
			renderer=new_GLRenderer();
		#endif
	}
	else if(plugin==RendererPlugin_DIRECT3D){
		#if defined(TOADLET_HAS_DIRECT3DMOBILE)
			renderer=new_D3DMRenderer();
		#elif defined(TOADLET_HAS_DIRECT3D9)
			renderer=new_D3D9Renderer();
		#endif
	}
	return renderer;
}

bool Win32Application::createContextAndRenderer(int plugin){
	if(plugin==RendererPlugin_NONE){
		return false;
	}

	Logger::log(Categories::TOADLET_PAD,Logger::Level_DEBUG,
		"Win32Application: creating context and renderer");

	RenderTarget *renderTarget=makeRenderTarget(plugin);
	if(renderTarget!=NULL){
		mRenderTarget=renderTarget;

		mRenderer=makeRenderer(plugin);
		if(mRenderer!=NULL){
			if(mRenderer->startup(this,mRendererOptions)==false){
				delete mRenderer;
				mRenderer=NULL;

				Error::unknown(Categories::TOADLET_PAD,
					"Error starting Renderer");
				return false;
			}
		}
		else{
			Error::unknown(Categories::TOADLET_PAD,
				"Error creating Renderer");
			return false;
		}

		if(mRenderer==NULL){
			delete mRenderTarget;
			mRenderTarget=NULL;
		}
	}
	else{
		Error::unknown(Categories::TOADLET_PAD,
			"Error creating RenderTargetPeer");
		return false;
	}

	if(mRenderer!=NULL){
		mRenderer->setRenderTarget(this);
		mEngine->setRenderer(mRenderer);
	}

	return mRenderer!=NULL;
}

bool Win32Application::destroyRendererAndContext(){
	Logger::log(Categories::TOADLET_PAD,Logger::Level_DEBUG,
		"Win32Application: destroying context and renderer");

	if(mRenderer!=NULL){
		mEngine->setRenderer(NULL);

		mRenderer->shutdown();
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
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth=width;
		dmScreenSettings.dmPelsHeight=height;
		dmScreenSettings.dmBitsPerPel=colorBits;
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
		// Try to set mode and get results.  CDS_FULLSCREEN gets rid of start bar.
		result=(ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)==DISP_CHANGE_SUCCESSFUL);
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

	if(mActive && mRenderer!=NULL){
		if(mRenderer->getCapabilitySet().resetOnResize){
			mEngine->setRenderer(NULL);

			mRenderer->reset();

			mEngine->setRenderer(mRenderer);
		}
		render(mRenderer);
	}
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
				if(application->getFullscreen() && application->isActive()){
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
		case WM_KEYUP:
		case WM_COMMAND: // So the command buttons can be heard
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
			application->mouseMoved(LOWORD(lParam),HIWORD(lParam));
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
