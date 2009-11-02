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
#if defined(TOADLET_HAS_OPENGL)
	#include <toadlet/peeper/plugins/glrenderer/GLRenderer.h>
	#include <toadlet/peeper/plugins/glrenderer/platform/glx/GLXWindowRenderTarget.h>
#endif
#if defined(TOADLET_HAS_OPENAL)
	#include <toadlet/ribbit/plugins/alplayer/ALPlayer.h>
#endif
#include <toadlet/pad/platform/x11/X11Application.h>
#include <toadlet/pad/ApplicationListener.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/extensions/xf86vmode.h>
#include <stdlib.h>
#if defined(TOADLET_PLATFORM_POSIX)
	#include <string.h>
#endif

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::tadpole;

namespace toadlet{
namespace pad{

struct X11Attributes{
	Display *mDisplay;
	Window mWindow;
	XVisualInfo *mVisualInfo;
	int mScrnum;
	Atom mDeleteWindow;

	struct{
		char *antialiasing;
		char *vblank;
	}mOriginalEnv;

	// Mode switching
	struct{
		XF86VidModeModeInfo mode;
		int viewX,viewY;
	}mOriginalMode;
};

X11Application::X11Application():
	//mTitle,
	mPositionX(0),
	mPositionY(0),
	mWidth(-1),
	mHeight(-1),
	mFullscreen(false),
	//mVisual(),
	mApplicationListener(NULL),

	mEngine(NULL),
	mRenderer(NULL),
	mRendererOptions(NULL),
	mAudioPlayer(NULL),

	mRun(false),
	mAutoActivate(false),
	mActive(false),
	mDestroyed(false),
	x11(NULL)
{
	x11=new X11Attributes();
	memset(x11,0,sizeof(X11Attributes));
}

X11Application::~X11Application(){
	delete[] mRendererOptions;

	delete x11;
}

void X11Application::create(){
	mEngine=new Engine();

	createAudioPlayer();
	createWindow();
	activate();
}

void X11Application::destroy(){
	if(mDestroyed){
		return;
	}

	mDestroyed=true;

	deactivate();
	destroyWindow();
	destroyAudioPlayer();

	if(mEngine!=NULL){
		delete mEngine;
		mEngine=NULL;
	}
}

bool X11Application::start(bool runEventLoop){
	resized(mWidth,mHeight);

	mRun=true;

	uint64 lastTime=System::mtime();
	while(runEventLoop && mRun){
		stepEventLoop();

		uint64 currentTime=System::mtime();
		update(currentTime-lastTime);
		if(mRenderer!=NULL){
			render(mRenderer);
		}
		lastTime=currentTime;

		System::msleep(10);
	}

	return true;
}

void X11Application::stepEventLoop(){
	XEvent event;
	KeySym key;

	while(XPending(x11->mDisplay)){
		XNextEvent(x11->mDisplay,&event);
		switch(event.type){
			case ClientMessage:
				if(event.xclient.data.l[0]==x11->mDeleteWindow){
					stop();
					break;
				}
				break;
			case DestroyNotify:
				stop();
				break;
			case Expose:
				break;
			case FocusIn:
				focusGained();
				break;
			case FocusOut:
				focusLost();
				break;
			case ConfigureNotify:
				configured(event.xconfigure.x,event.xconfigure.y,event.xconfigure.width,event.xconfigure.height);
				break;
			case KeyRelease:
				key=XKeycodeToKeysym(x11->mDisplay,event.xkey.keycode,0);
				keyReleased(translateKey(key));
				break;
			case KeyPress:
				key=XKeycodeToKeysym(x11->mDisplay,event.xkey.keycode,0);
				keyPressed(translateKey(key));
				break;
			case MotionNotify:
				mouseMoved(event.xmotion.x,event.xmotion.y);
				break;
			case ButtonPress:
				switch(event.xbutton.button){
					case Button1:
						mousePressed(event.xmotion.x,event.xmotion.y,0);
						break;
					case Button2:
						mousePressed(event.xmotion.x,event.xmotion.y,1);
						break;
					case Button3:
						mousePressed(event.xmotion.x,event.xmotion.y,2);
						break;
					case Button4:
						mouseScrolled(event.xmotion.x,event.xmotion.y,1);
						break;
					case Button5:
						mouseScrolled(event.xmotion.x,event.xmotion.y,-1);
						break;
				}
				break;
			case ButtonRelease:
				switch(event.xbutton.button){
					case Button1:
						mouseReleased(event.xmotion.x,event.xmotion.y,0);
						break;
					case Button2:
						mouseReleased(event.xmotion.x,event.xmotion.y,1);
						break;
					case Button3:
						mouseReleased(event.xmotion.x,event.xmotion.y,2);
						break;
					case Button4:
						break;
					case Button5:
						break;
				}
				break;
		}
	}
}

void X11Application::stop(){
	mRun=false;
}

void X11Application::setAutoActivate(bool autoActivate){
	mAutoActivate=autoActivate;
}

void X11Application::activate(){
	if(mActive==false){
		mActive=true;

		createContextAndRenderer();
	}
}

void X11Application::deactivate(){
	if(mActive==true){
		mActive=false;

		destroyRendererAndContext();
	}
}

bool X11Application::createWindow(){
	x11->mDisplay=XOpenDisplay(getenv("DISPLAY"));
	if(x11->mDisplay==None){
		Error::unknown(Categories::TOADLET_PAD,
			"DISPLAY bad value; couldn't open display");
		return false;
	}

	x11->mScrnum=XDefaultScreen(x11->mDisplay);

	#if !defined(TOADLET_PLATFORM_IRIX)
		if(mWidth==-1 || mHeight==-1){
			XF86VidModeModeLine currentMode;
			int tmp;
			XF86VidModeGetModeLine(x11->mDisplay,x11->mScrnum,&tmp,&currentMode);
			if(mWidth==-1){
				mWidth=currentMode.hdisplay;
			}
			if(mHeight==-1){
				mHeight=currentMode.vdisplay;
			}
		}
	#else
		if(mWidth==-1 || mHeight==-1){
			if(mWidth==-1){
				mWidth=640;
			}
			if(mHeight==-1){
				mHeight=480;
			}
		}
	#endif

	#if !defined(TOADLET_PLATFORM_IRIX)
		// Antialiasing should work with an nvidia card and nvidia glxvpenis
		x11->mOriginalEnv.antialiasing = getenv("__GL_FSAA_MODE");
		if(x11->mOriginalEnv.antialiasing==NULL){
			if(mVisual.multisamples>1){
				setenv("__GL_FSAA_MODE","4",1);
				Logger::log(Categories::TOADLET_PAD,Logger::Level_ALERT,
					"Antialiasing attempted with __GL_FSAA_MODE = 4");
			}
			else{
				setenv("__GL_FSAA_MODE","0",1);
			}
		}

		// Setup vblank syncing for a smoother display
		x11->mOriginalEnv.vblank = getenv("__GL_SYNC_TO_VBLANK");
		if(x11->mOriginalEnv.vblank==NULL){
			if(mVisual.vsync){
				setenv("__GL_SYNC_TO_VBLANK","1",1);
			}
			else{
				setenv("__GL_SYNC_TO_VBLANK","0",1);
			}
		}
	#endif

	XSetWindowAttributes attr;
	unsigned long mask;
	int attrib[] = { GLX_RGBA,
		GLX_RED_SIZE, 1,
		GLX_GREEN_SIZE, 1,
		GLX_BLUE_SIZE, 1,
		GLX_DOUBLEBUFFER,
		GLX_DEPTH_SIZE, 1,
		None };
	
	// Check for glx on the display
	int dummy;
	if(!glXQueryExtension(x11->mDisplay,&dummy,&dummy)){
		Error::unknown(Categories::TOADLET_PAD,
			"glXQueryExtension failure");
		return false; 
	}
	
	// Find an OpenGL-capable Color Index visual RGB and Double-buffer
	x11->mVisualInfo=glXChooseVisual(x11->mDisplay,x11->mScrnum,attrib);
	if(!x11->mVisualInfo){
		Error::unknown(Categories::TOADLET_PAD,
			"Couldn't get an RGB, Double-buffered visual");
		return false;
	}

	// Set the window attributes
	attr.background_pixel=0;
	attr.border_pixel=0;
	attr.colormap=XCreateColormap(x11->mDisplay,XRootWindow(x11->mDisplay,x11->mScrnum),x11->mVisualInfo->visual,AllocNone);
	attr.event_mask=StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask;

	// Go to fullscreen mode if requested
	if(mFullscreen){
		#if defined(TOADLET_PLATFORM_IRIX)
			Error::unknown(Categories::TOADLET_PAD,
				"Fullscreen mode not supported on IRIX");
			return false;
		#else
			// Get the display modes list
			XF86VidModeModeInfo **displayModes=NULL;
			int numDisplayModes;
			XF86VidModeGetAllModeLines(x11->mDisplay,x11->mScrnum,&numDisplayModes,&displayModes);
			if(displayModes==NULL){
				Error::unknown(Categories::TOADLET_PAD,
					"XF86VidModeGetAllModeLines didn't find any");
				return false;
			}
			
			// Store the current mode, converting from ModeLine to ModeInfo
			XF86VidModeModeLine currentMode;
			int dotclock=0;
			XF86VidModeGetModeLine(x11->mDisplay,x11->mScrnum,&dotclock,&currentMode);
			memcpy(((char*)(&x11->mOriginalMode.mode))+sizeof(int),&currentMode,sizeof(XF86VidModeModeLine));
			x11->mOriginalMode.mode.dotclock=dotclock;
			XF86VidModeGetViewPort(x11->mDisplay,x11->mScrnum,&x11->mOriginalMode.viewX,&x11->mOriginalMode.viewY);
			
			// See if any display modes match the requested mode and switch
			int i;
			int mode=-1;
			for(i=0; i<numDisplayModes; ++i){
				if(mWidth==displayModes[i]->hdisplay && mHeight==displayModes[i]->vdisplay){
					mode=i;
					i=numDisplayModes;
				}
			}

			if(mode!=-1){
				Logger::log(Categories::TOADLET_PAD,Logger::Level_ALERT,
					String("Attemping to use Mode ") + (int)mode + " : " + (int)displayModes[mode]->hdisplay +
					" x " + (int)displayModes[mode]->vdisplay + " @ " +
					(1000 * displayModes[mode]->dotclock / (displayModes[mode]->htotal * displayModes[mode]->vtotal)));

				// Only switch if requested mode is actually different
				if(displayModes[mode]->hdisplay!=x11->mOriginalMode.mode.hdisplay || displayModes[mode]->vdisplay!=x11->mOriginalMode.mode.vdisplay){
					XF86VidModeSwitchToMode(x11->mDisplay,x11->mScrnum,displayModes[mode]);
					XF86VidModeSetViewPort(x11->mDisplay,x11->mScrnum,0,0);
				}
				Logger::log(Categories::TOADLET_PAD,Logger::Level_ALERT,
					"Success");
			}
			else{
				if(displayModes){
					XFree(displayModes);
					displayModes=NULL;
				}

				Error::unknown(Categories::TOADLET_PAD,
					"Fullscreen mode unavailable due to bad requested fullscreen size");
				return false;
			}
			
			// Make sure we are square after mode switch
			XSync(x11->mDisplay,true);

			// Clean up
			if(displayModes){
				XFree(displayModes);
				displayModes=NULL;
			}
			
			// Set Fullscreen attributes
			mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask | CWOverrideRedirect;
			attr.border_pixmap = None;
			attr.override_redirect = true;

			// Create window
			x11->mWindow=XCreateWindow(x11->mDisplay,XRootWindow(x11->mDisplay,x11->mScrnum),mPositionX,mPositionY,mWidth,mHeight,0,x11->mVisualInfo->depth,InputOutput,x11->mVisualInfo->visual,mask,&attr);

			// Lock things to window
			XWarpPointer(x11->mDisplay,None,x11->mWindow,0,0,0,0,mWidth/2,mHeight/2);
			XMapRaised(x11->mDisplay,x11->mWindow);
			XGrabKeyboard(x11->mDisplay,x11->mWindow,true,GrabModeAsync,GrabModeAsync,CurrentTime);
			XGrabPointer(x11->mDisplay,x11->mWindow,true,ButtonPressMask,GrabModeAsync,GrabModeAsync,x11->mWindow,None,CurrentTime);
		#endif
	}
	// Otherwise use windowed mode
	else{
		// Set attribute mask
		mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;
		
		// Create window
		x11->mWindow=XCreateWindow(x11->mDisplay,XRootWindow(x11->mDisplay,x11->mScrnum),mPositionX,mPositionY,mWidth,mHeight,0,x11->mVisualInfo->depth,InputOutput,x11->mVisualInfo->visual,mask,&attr);
		
		// Set hints and properties
		XSizeHints sizehints;
		sizehints.x = 0;
		sizehints.y = 0;
		sizehints.width = mWidth;
		sizehints.height = mHeight;
		sizehints.flags = USSize | USPosition;
		XSetNormalHints(x11->mDisplay,x11->mWindow,&sizehints);
		XSetStandardProperties(x11->mDisplay,x11->mWindow,mTitle,mTitle,None,(char **)NULL,0,&sizehints);

		// Bring up the window
		XMapRaised(x11->mDisplay,x11->mWindow);
	}

	// Make sure we catch the close window event
	x11->mDeleteWindow=XInternAtom(x11->mDisplay,"WM_DELETE_WINDOW",false);
	XSetWMProtocols(x11->mDisplay,x11->mWindow,&x11->mDeleteWindow,1);

	return true;
}

void X11Application::destroyWindow(){
	// Just to be safe
	if(x11->mDisplay){
		XSync(x11->mDisplay,true);
	}
	
	originalResolution();

	originalEnv();

	if(x11->mVisualInfo){
		XFree(x11->mVisualInfo);
		x11->mVisualInfo=None;
	}
	
	if(x11->mWindow){
		XDestroyWindow(x11->mDisplay,x11->mWindow);
		x11->mWindow=None;
	}

	if(x11->mDisplay){
		XCloseDisplay(x11->mDisplay);
		x11->mDisplay=None;
	}
}

void X11Application::originalResolution(){
	#if !defined(TOADLET_PLATFORM_IRIX)
		if(mFullscreen){
			// Store the current mode
			XF86VidModeModeLine currentMode;
			int tmp;
			XF86VidModeGetModeLine(x11->mDisplay,x11->mScrnum,&tmp,&currentMode);

			// Switch if necessary
			if(currentMode.hdisplay!=x11->mOriginalMode.mode.hdisplay || currentMode.vdisplay!=x11->mOriginalMode.mode.vdisplay){
				XF86VidModeSwitchToMode(x11->mDisplay,x11->mScrnum,&x11->mOriginalMode.mode);
				XF86VidModeSetViewPort(x11->mDisplay,x11->mScrnum,x11->mOriginalMode.viewX,x11->mOriginalMode.viewY);
			}

			// Make sure we are square after mode switch
			XSync(x11->mDisplay,true);
			mFullscreen=false;

			// Release 
			XUngrabKeyboard(x11->mDisplay,CurrentTime);
			XUngrabPointer(x11->mDisplay,CurrentTime);
		}
	#endif
}

void X11Application::originalEnv(){
	#if !defined(TOADLET_PLATFORM_IRIX)
		if(x11->mOriginalEnv.antialiasing==NULL){
			unsetenv("__GL_FSAA_MODE");
		}
		if(x11->mOriginalEnv.vblank==NULL){
			unsetenv("__GL_SYNC_TO_VBLANK");
		}
	#endif
}

void X11Application::setTitle(const String &title){
	mTitle=title;
}

const String &X11Application::getTitle() const{
	return mTitle;
}

void X11Application::setSize(int width,int height){
	mWidth=width;
	mHeight=height;
}

void X11Application::setPosition(int x,int y){
	mPositionX=x;
	mPositionY=y;
}

int X11Application::getPositionX() const{
	return mPositionX;
}

int X11Application::getPositionY() const{
	return mPositionY;
}

int X11Application::getWidth() const{
	return mWidth;
}

int X11Application::getHeight() const{
	return mHeight;
}

void X11Application::setFullscreen(bool fullscreen){
	mFullscreen=fullscreen;
}

bool X11Application::getFullscreen() const{
	return mFullscreen;
}

void X11Application::setVisual(const peeper::Visual &visual){
	mVisual=visual;
}

const peeper::Visual &X11Application::getVisual() const{
	return mVisual;
}

void X11Application::setApplicationListener(ApplicationListener *listener){
	mApplicationListener=listener;
}

ApplicationListener *X11Application::getApplicationListener() const{
	return mApplicationListener;
}

RenderTarget *X11Application::makeRenderTarget(){
	#if defined(TOADLET_HAS_OPENGL)
		return new GLXWindowRenderTarget(x11->mWindow,x11->mDisplay,x11->mVisualInfo);
	#else
		return NULL;
	#endif
}

Renderer *X11Application::makeRenderer(){
	#if defined(TOADLET_HAS_OPENGL)
		return new GLRenderer();
	#else
		return NULL;
	#endif
}

bool X11Application::createContextAndRenderer(){
	RenderTarget *renderTarget=makeRenderTarget();
	if(renderTarget!=NULL){
		mRenderTarget=renderTarget;

		mRenderer=makeRenderer();
		if(mRenderer!=NULL){
			if(mRenderer->create(this,mRendererOptions)==false){
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
			"Error creating RenderTarget");
		return false;
	}

	if(mRenderer!=NULL){
		mRenderer->setRenderTarget(this);
		mEngine->setRenderer(mRenderer);
	}

	return mRenderer!=NULL;
}

bool X11Application::destroyRendererAndContext(){
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

bool X11Application::createAudioPlayer(){
	#if defined(TOADLET_HAS_OPENAL)
		mAudioPlayer=new ALPlayer();
		bool result=false;
		TOADLET_TRY
			result=mAudioPlayer->create(NULL);
		TOADLET_CATCH(const Exception &){result=false;}
		if(result=false){
			delete mAudioPlayer;
			mAudioPlayer=NULL;
		}
	#endif
	if(mAudioPlayer!=NULL){
		mEngine->setAudioPlayer(mAudioPlayer);
	}
	return true;
}

bool X11Application::destroyAudioPlayer(){
	if(mAudioPlayer!=NULL){
		mEngine->setAudioPlayer(NULL);
		mAudioPlayer->destroy();
		delete mAudioPlayer;
		mAudioPlayer=NULL;
	}
	return true;
}

void X11Application::configured(int x,int y,int width,int height){
	mPositionX=x;
	mPositionY=y;

	if(mWidth!=width || mHeight!=height){
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
}

void X11Application::resized(int width,int height){
	if(mApplicationListener!=NULL){
		mApplicationListener->resized(width,height);
	}
}

void X11Application::focusGained(){
	if(mApplicationListener!=NULL){
		mApplicationListener->focusGained();
	}
}

void X11Application::focusLost(){
	if(mApplicationListener!=NULL){
		mApplicationListener->focusLost();
	}
}

void X11Application::keyPressed(int key){
	if(mApplicationListener!=NULL){
		mApplicationListener->keyPressed(key);
	}
}

void X11Application::keyReleased(int key){
	if(mApplicationListener!=NULL){
		mApplicationListener->keyReleased(key);
	}
}

void X11Application::mousePressed(int x,int y,int button){
	if(mApplicationListener!=NULL){
		mApplicationListener->mousePressed(x,y,button);
	}
}

void X11Application::mouseMoved(int x,int y){
	if(mApplicationListener!=NULL){
		mApplicationListener->mouseMoved(x,y);
	}
}

void X11Application::mouseReleased(int x,int y,int button){
	if(mApplicationListener!=NULL){
		mApplicationListener->mouseReleased(x,y,button);
	}
}

void X11Application::mouseScrolled(int x,int y,int scroll){
	if(mApplicationListener!=NULL){
		mApplicationListener->mouseScrolled(x,y,scroll);
	}
}

void X11Application::update(int dt){
	if(mApplicationListener!=NULL){
		mApplicationListener->update(dt);
	}
}

void X11Application::render(Renderer *renderer){
	if(mApplicationListener!=NULL){
		mApplicationListener->render(renderer);
	}
}

void X11Application::setRendererOptions(int *options,int length){
	if(mRendererOptions!=NULL){
		delete[] mRendererOptions;
	}

	mRendererOptions=new int[length];
	memcpy(mRendererOptions,options,length*sizeof(int));
}

void *X11Application::getDisplay() const{return x11->mDisplay;}
void *X11Application::getWindow() const{return (void*)x11->mWindow;}
void *X11Application::getVisualInfo() const{return x11->mVisualInfo;}

int X11Application::translateKey(int key){
	switch(key){
		case XK_Escape:
			return Key_ESC;
		case XK_Pause:
			return Key_PAUSE;
		case XK_Left:
			return Key_LEFT;
		case XK_Right:
			return Key_RIGHT;
		case XK_Up:
			return Key_UP;
		case XK_Down:
			return Key_DOWN;
		case XK_Shift_L:
		case XK_Shift_R:
			return Key_SHIFT;
		case XK_Control_L:
		case XK_Control_R:
			return Key_CTRL;
		case XK_Alt_L:
		case XK_Alt_R:
			return Key_ALT;
		case XK_Super_L:
		case XK_Super_R:
			return Key_SPECIAL;
		case XK_Return:
			return Key_ENTER;
		case XK_Tab:
			return Key_TAB;
		case XK_BackSpace:
			return Key_BACKSPACE;
		case XK_Delete:
			return Key_DELETE;
		case 32:
			return Key_SPACE;
	}

	return key;
}

}
}

