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

#include <toadlet/egg/System.h>
#include <toadlet/pad/platform/x11/X11Application.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <X11/extensions/xf86vmode.h>

using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::flick;

#if defined(TOADLET_HAS_OPENGL)
	extern "C" RenderDevice *new_GLRenderDevice();
	extern "C" RenderTarget *new_GLXWindowRenderTarget(void *display,void *window,WindowRenderTargetFormat *format);
#endif
#if defined(TOADLET_HAS_OPENAL)
	extern "C" AudioDevice *new_ALAudioDevice();
#endif

namespace toadlet{
namespace pad{

struct X11Attributes{
	Display *mDisplay;
	Window mWindow;
	XVisualInfo *mVisualInfo;
	int mScrnum;
	Atom mDeleteWindow;
	Cursor mBlankCursor;

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
	mDifferenceMouse(false),
	mLastXMouse(0),mLastYMouse(0),
	mSkipNextMove(false),

	mRun(false),
	mActive(false),
	mDestroyed(false),
	x11(NULL)
{
	x11=new X11Attributes();
	memset(x11,0,sizeof(X11Attributes));

	mFormat->setPixelFormat(TextureFormat::Format_RGBA_8);

	#if defined(TOADLET_HAS_OPENGL)
		mRenderDevicePlugins.add("gl",RenderDevicePlugin(new_GLXWindowRenderTarget,new_GLRenderDevice));
	#endif

	#if defined(TOADLET_HAS_OPENAL)
		mAudioDevicePlugins.add("al",AudioDevicePlugin(new_ALAudioDevice));
	#endif
}

X11Application::~X11Application(){
	delete x11;
}

void X11Application::preEngineCreate(){
	createWindow();
}

void X11Application::destroy(){
	if(mDestroyed){
		return;
	}

	mDestroyed=true;

	BaseApplication::destroy();
}

void X11Application::start(){
	resized(mWidth,mHeight);
	mRun=true;
	runEventLoop();
}

void X11Application::runEventLoop(){
	uint64 lastTime=System::mtime();
	while(mRun){
		stepEventLoop();

		uint64 currentTime=System::mtime();
		int dt=currentTime-lastTime;
		update(dt);
		if(mRenderDevice!=NULL){
			render();
		}
		if(mAudioDevice!=NULL){
			mAudioDevice->update(dt);
		}
		lastTime=currentTime;

		System::msleep(0);
	}
}

void X11Application::stepEventLoop(){
	XEvent event;
	KeySym key;

	while(x11->mDisplay!=NULL && XPending(x11->mDisplay)){
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
				internal_mouseMoved(event.xmotion.x,event.xmotion.y);
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

void X11Application::activate(){
	if(mActive==false){
		mActive=true;

		// See Win32Application::activate notes
	}
}

void X11Application::deactivate(){
	if(mActive==true){
		mActive=false;

		// See Win32Application::activate notes
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

	// Antialiasing should work with an nvidia card and nvidia glx
	x11->mOriginalEnv.antialiasing = getenv("__GL_FSAA_MODE");
	if(x11->mOriginalEnv.antialiasing==NULL){
		if(mFormat->multisamples>1){
			setenv("__GL_FSAA_MODE","4",1);
			Log::alert(Categories::TOADLET_PAD,
				"Antialiasing attempted with __GL_FSAA_MODE = 4");
		}
		else{
			setenv("__GL_FSAA_MODE","0",1);
		}
	}

	// Setup vblank syncing for a smoother display
	x11->mOriginalEnv.vblank = getenv("__GL_SYNC_TO_VBLANK");
	if(x11->mOriginalEnv.vblank==NULL){
		if(mFormat->vsync){
			setenv("__GL_SYNC_TO_VBLANK","1",1);
		}
		else{
			setenv("__GL_SYNC_TO_VBLANK","0",1);
		}
	}

	XVisualInfo info;
	int redBits=TextureFormat::getRedBits(mFormat->pixelFormat);
	int greenBits=TextureFormat::getGreenBits(mFormat->pixelFormat);
	int blueBits=TextureFormat::getBlueBits(mFormat->pixelFormat);
	int alphaBits=TextureFormat::getAlphaBits(mFormat->pixelFormat);
	int colorBits=redBits+greenBits+blueBits+alphaBits;
	if(XMatchVisualInfo(x11->mDisplay,x11->mScrnum,colorBits,TrueColor,&info)==0){
		Error::unknown(Categories::TOADLET_PAD,
			String("failed to match visual for ")+colorBits);
		return false;
	}
	info.screen=0;
	int n=0;
	x11->mVisualInfo=XGetVisualInfo(x11->mDisplay,VisualAllMask,&info,&n);
	if(x11->mVisualInfo==None){
		Error::unknown(Categories::TOADLET_PAD,
			"failed to get visual");
		return false;
	}

	// Set the window attributes
	XSetWindowAttributes attr;
	unsigned long mask;
	attr.background_pixel=0;
	attr.border_pixel=0;
	attr.colormap=XCreateColormap(x11->mDisplay,XRootWindow(x11->mDisplay,x11->mScrnum),x11->mVisualInfo->visual,AllocNone);
	attr.event_mask=StructureNotifyMask | ExposureMask | KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | FocusChangeMask;

	// Go to fullscreen mode if requested
	if(mFullscreen){
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
			Log::alert(Categories::TOADLET_PAD,
				String("Attemping to use Mode ") + (int)mode + " : " + (int)displayModes[mode]->hdisplay +
				" x " + (int)displayModes[mode]->vdisplay + " @ " +
				(1000 * displayModes[mode]->dotclock / (displayModes[mode]->htotal * displayModes[mode]->vtotal)));

			// Only switch if requested mode is actually different
			if(displayModes[mode]->hdisplay!=x11->mOriginalMode.mode.hdisplay || displayModes[mode]->vdisplay!=x11->mOriginalMode.mode.vdisplay){
				XF86VidModeSwitchToMode(x11->mDisplay,x11->mScrnum,displayModes[mode]);
				XF86VidModeSetViewPort(x11->mDisplay,x11->mScrnum,0,0);
			}
			Log::alert(Categories::TOADLET_PAD,
				"Success");
		}
		else{
			if(displayModes){
				XFree(displayModes);
				displayModes=NULL;
			}

			Error::unknown(Categories::TOADLET_PAD,
				"fullscreen mode unavailable due to bad requested fullscreen size");
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

	static char cursorData[1]={0};
	XColor cursorColor;
	Pixmap cursorPixmap=XCreateBitmapFromData(x11->mDisplay,x11->mWindow,cursorData,1,1);	
	x11->mBlankCursor=XCreatePixmapCursor(x11->mDisplay,cursorPixmap,cursorPixmap,&cursorColor,&cursorColor,0,0);
	XFreePixmap(x11->mDisplay,cursorPixmap);

	return true;
}

void X11Application::destroyWindow(){
	if(x11->mBlankCursor){
		XFreeCursor(x11->mDisplay,x11->mBlankCursor);
		x11->mBlankCursor=None;
	}

	// Just to be safe
	if(x11->mDisplay){
		XSync(x11->mDisplay,true);
	}
	
	originalResolution();

	originalEnv();

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
}

void X11Application::originalEnv(){
	if(x11->mOriginalEnv.antialiasing==NULL){
		unsetenv("__GL_FSAA_MODE");
	}
	if(x11->mOriginalEnv.vblank==NULL){
		unsetenv("__GL_SYNC_TO_VBLANK");
	}
}

void X11Application::setTitle(const String &title){
	mTitle=title;
}

String X11Application::getTitle() const{
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

void X11Application::internal_mouseMoved(int x,int y){
	if(mSkipNextMove){
		mLastXMouse=x;
		mLastYMouse=y;
		mSkipNextMove=false;
		return;
	}

	if(mDifferenceMouse){
		// We check to see if we are at the warp-point, instead of using SkipNextMove 
		//  since it seems on X11 that user mouse-move commands can get processed before the skip, getting things confused
		// We still use SkipNextMove when the DifferenceMouse is turned on to avoid the first jump
		if(x==mWidth/2 && y==mHeight/2){
			mLastXMouse=x;
			mLastYMouse=y;
			return;
		}

		int dx=x-mLastXMouse,dy=y-mLastYMouse;

		XWarpPointer(x11->mDisplay,None,x11->mWindow,0,0,0,0,mWidth/2,mHeight/2);

		x=dx;y=dy;
	}

	mouseMoved(x,y);
}

void X11Application::configured(int x,int y,int width,int height){
	mPositionX=x;
	mPositionY=y;

	if(mWidth!=width || mHeight!=height){
		mWidth=width;
		mHeight=height;

		resized(width,height);

		if(mActive && mRenderDevice!=NULL){
			if(mEngine->getRenderCaps().resetOnResize){
				mEngine->contextReset(mRenderDevice);
			}
			update(0);
			render();
		}
	}
}

void X11Application::setDifferenceMouse(bool difference){
	mDifferenceMouse=difference;
	mSkipNextMove=true;

	if(difference){
		XDefineCursor(x11->mDisplay,x11->mWindow,x11->mBlankCursor);
	}
	else{
		XUndefineCursor(x11->mDisplay,x11->mWindow);
	}
}

void *X11Application::getDisplay(){return x11->mDisplay;}
void *X11Application::getWindow(){return (void*)x11->mWindow;}

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

