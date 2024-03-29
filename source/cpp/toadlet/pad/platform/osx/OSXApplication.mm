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

#include <toadlet/Types.h>

#if defined(TOADLET_PLATFORM_IOS)
	#define TOADLET_HAS_UIKIT
#endif
 
#include <toadlet/egg/System.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/platform/osx/OSXBundleArchive.h>
#include <toadlet/pad/platform/osx/OSXApplication.h>

#import <QuartzCore/QuartzCore.h>
#if defined(TOADLET_HAS_UIKIT)
	#import <UIKit/UIKit.h>
#else
	#import <AppKit/AppKit.h>
#endif

using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::flick;
using namespace toadlet::tadpole;
using namespace toadlet::pad;

#if defined(TOADLET_HAS_OPENGL)
	#if defined(TOADLET_HAS_UIKIT)
		extern "C" RenderDevice *new_GLES1RenderDevice();
		extern "C" RenderDevice *new_GLES2RenderDevice();
		extern "C" RenderTarget *new_EAGL1RenderTarget(void *display,void *layer,WindowRenderTargetFormat *format,RenderTarget *shareTarget);
		extern "C" RenderTarget *new_EAGL2RenderTarget(void *display,void *layer,WindowRenderTargetFormat *format,RenderTarget *shareTarget);
	#else
		extern "C" RenderDevice *new_GLRenderDevice();
		extern "C" RenderTarget *new_NSGLRenderTarget(void *display,void *view,WindowRenderTargetFormat *format,RenderTarget *shareTarget);
	#endif
#endif
#if defined(TOADLET_HAS_OPENAL)
	extern "C" AudioDevice *new_ALAudioDevice();
#endif
#if defined(TOADLET_PLATFORM_IOS)
	extern "C" InputDevice *new_IOSLinearDevice();
#endif

@interface ApplicationView:
#if defined(TOADLET_HAS_UIKIT)
	UIView
#else
	NSView<NSWindowDelegate>
#endif
{
@private
	OSXApplication *mApplication;
	NSTimer *mTimer;
	toadlet::uint64 mLastTime;
	CGPoint mLastLocation;
}

- (id) initWithApplication:(OSXApplication*)application frame:
	#if defined(TOADLET_HAS_UIKIT)
		(CGRect)
	#else
		(NSRect)
	#endif
	rect;
- (void) update;
@end

//! An ApplicationView handles translating the objective c ui messages to and from the pad Application class
@implementation ApplicationView

#if defined(TOADLET_HAS_UIKIT)
+ (Class) layerClass{
	return [CAEAGLLayer class];
}
#endif

- (id) initWithApplication:(OSXApplication*)application frame:
#if defined(TOADLET_HAS_UIKIT)
	(CGRect)
#else
	(NSRect)
#endif
rect{
	if((self=[super initWithFrame:rect])){
		#if defined(TOADLET_HAS_UIKIT)
			self.opaque=YES;
		#endif

		mLastTime=System::mtime();

		mApplication=application;
	}
		
	return self;
}

#if !defined(TOADLET_HAS_UIKIT)
- (void) viewDidMoveToWindow{
	[[NSNotificationCenter defaultCenter] addObserver:self
		 selector:@selector(windowResized:) name:NSWindowDidResizeNotification object:[self window]];
}
#endif

- (void) dealloc{
	[mTimer invalidate];

#if !defined(TOADLET_HAS_UIKIT)
	[[NSNotificationCenter defaultCenter] removeObserver:self];
#endif

	[super dealloc];
}

- (void) start{
	if(mTimer==nil){
		mTimer=[NSTimer scheduledTimerWithTimeInterval:0 target:self selector:@selector(update) userInfo:nil repeats:YES];
		[[NSRunLoop currentRunLoop] addTimer:mTimer forMode:NSDefaultRunLoopMode];
	}
}

- (void) stop{
	if(mTimer!=nil){
		[mTimer invalidate];
		mTimer=nil;
	}
}

- (void) update{
	toadlet::uint64 currentTime=System::mtime();
	int dt=currentTime-mLastTime;
	if(mApplication->isActive()){
		mApplication->update(dt);
		if(mApplication->getRenderDevice()!=NULL){
			mApplication->render();
		}
		if(mApplication->getAudioDevice()!=NULL){
			mApplication->getAudioDevice()->update(dt);
		}
	}
	mLastTime=currentTime;
}

#if defined(TOADLET_HAS_UIKIT)
- (void) layoutSubviews{
#else
- (void) windowResized:(NSNotification*)notification{
#endif
	int width=[self bounds].size.width,height=[self bounds].size.height;
	Log::debug(Categories::TOADLET_PAD,String("resized:")+width+","+height);

	mApplication->resized(width,height);

	if(mApplication->isActive() && mApplication->getRenderDevice()!=NULL){
		if(mApplication->getEngine()->getRenderCaps().resetOnResize){
			mApplication->getEngine()->contextReset(mApplication->getRenderDevice());
		}
		mApplication->update(0);
		mApplication->render();
	}
}

#if defined(TOADLET_HAS_UIKIT)
// Handles the start of a touch
- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event{
	UITouch* touch=[[event touchesForView:self] anyObject];
	CGPoint location=[touch locationInView:self];
	mLastLocation=location;
	mApplication->internal_mousePressed(location.x,location.y,0);
}

// Handles the continuation of a touch.
- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event{  
	UITouch* touch=[[event touchesForView:self] anyObject];
	CGPoint location=[touch locationInView:self];
	mLastLocation=location;
	mApplication->internal_mouseMoved(location.x,location.y);
}

// Handles the end of a touch event when the touch is a tap.
- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event{
	UITouch* touch=[[event touchesForView:self] anyObject];
	CGPoint location=[touch locationInView:self];
	if(location.x!=mLastLocation.x || location.y!=mLastLocation.y){
		mApplication->internal_mouseMoved(location.x,location.y);
	}
	mApplication->mouseReleased(location.x,location.y,0);
}

// Handles the end of a touch event.
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event{
	UITouch* touch=[[event touchesForView:self] anyObject];
	CGPoint location=[touch locationInView:self];
	if(location.x!=mLastLocation.x || location.y!=mLastLocation.y){
		mApplication->internal_mouseMoved(location.x,location.y);
	}
	mApplication->mouseReleased(location.x,location.y,0);
}
#else
/// @todo: There is a problem where (it seems) if the application is launched without a NIB file, you have to double click to get any click events to register.
- (void) mouseDown:(NSEvent*)event{
	mApplication->internal_mousePressed(
		[event locationInWindow].x,
		[self bounds].size.height-[event locationInWindow].y,
		[event buttonNumber]
	);
}

- (void) mouseDragged:(NSEvent*)event{
	mApplication->internal_mouseMoved(
		[event locationInWindow].x,
		[self bounds].size.height-[event locationInWindow].y
	);
}

- (void) mouseUp:(NSEvent*)event{
	mApplication->mouseReleased(
		[event locationInWindow].x,
		[self bounds].size.height-[event locationInWindow].y,
		[event buttonNumber]
	);
}

// To accept keyboard events
- (BOOL)acceptsFirstResponder {
	return YES;
}

- (void) keyDown:(NSEvent*)event{
	NSString *characters=[event characters];
    unichar character;
    character=[characters characterAtIndex: 0];

	mApplication->keyPressed(mApplication->translateKey(character));
}

- (void) keyUp:(NSEvent*)event{
	NSString *characters=[event characters];
    unichar character;
    character=[characters characterAtIndex: 0];
	
	mApplication->keyReleased(mApplication->translateKey(character));
}

- (void) windowWillClose:(NSNotification*)notification{
	mApplication->stop();
}
#endif

@end

namespace toadlet{
namespace pad{

OSXApplication::OSXApplication():
	//mTitle,
	mPositionX(0),mPositionY(0),
	mWidth(-1),mHeight(-1),
	mFullscreen(false),
	mDifferenceMouse(false),
	mLastMouseX(0),mLastMouseY(0),

	mActive(false),
	mDestroyed(false),
	mWindow(nil),
	mView(nil),
	mPool(nil)
{
	#if defined(TOADLET_HAS_OPENGL)
		#if defined(TOADLET_HAS_UIKIT)
			mRenderDevicePlugins["gles1"]=RenderDevicePlugin(new_EAGL1RenderTarget,new_GLES1RenderDevice);
			mRenderDevicePlugins["gles2"]=RenderDevicePlugin(new_EAGL2RenderTarget,new_GLES2RenderDevice);
			mRenderDevicePreferences.push_back("gles2");
			mRenderDevicePreferences.push_back("gles1");
		#else
			mRenderDevicePlugins["gl"]=RenderDevicePlugin(new_NSGLRenderTarget,new_GLRenderDevice);
			mRenderDevicePreferences.push_back("gl");
		#endif
	#endif
	
	#if defined(TOADLET_HAS_OPENAL)
		mAudioDevicePlugins["al"]=AudioDevicePlugin(new_ALAudioDevice);
		mAudioDevicePreferences.push_back("al");
	#endif
}

OSXApplication::~OSXApplication(){
	destroy();

	if(mView!=nil){
		[(ApplicationView*)mView release];
	}
}

void OSXApplication::setWindow(void *window){
	mWindow=window;
	[(NSObject*)mWindow retain];
}

void OSXApplication::preEngineCreate(){
	if(mWindow==nil){
		// This programatic Window creation isn't spectacular, but it's enough to run examples.
		mPool=[[NSAutoreleasePool alloc] init];
		#if defined(TOADLET_HAS_UIKIT)
			[UIApplication sharedApplication];
			mPool=[[NSAutoreleasePool alloc] init];
			CGRect rect=[[UIScreen mainScreen] applicationFrame];
			if(mWidth==-1 && mHeight==-1){
				mWidth=rect.size.width;
				mHeight=rect.size.height;
			}
			mWindow=[[UIWindow alloc] initWithFrame:rect];
		#else
			NSApplicationLoad();
			if(mWidth==-1 && mHeight==-1){
				NSRect rect=[[NSScreen mainScreen] frame];
				mWidth=rect.size.width;
				mHeight=rect.size.height;
			}
			mWindow=[[NSWindow alloc] initWithContentRect:NSMakeRect(mPositionX,mPositionY,mWidth,mHeight)
				styleMask:NSTitledWindowMask|NSClosableWindowMask|NSMiniaturizableWindowMask|NSResizableWindowMask
				backing:NSBackingStoreBuffered defer:FALSE];
		#endif
	}
	else{
		#if defined(TOADLET_HAS_UIKIT)
			mWidth=[(UIWindow*)mWindow bounds].size.width;
			mHeight=[(UIWindow*)mWindow bounds].size.height;
		#else
			NSView *view=[(NSWindow*)mWindow contentView];
			if(view!=nil){
				mWidth=[view bounds].size.width;
				mHeight=[view bounds].size.height;
			}
		#endif
	}
	#if defined(TOADLET_HAS_UIKIT)
		CGRect rect;
	#else
		NSRect rect;
	#endif
	rect.origin.x=mPositionX;
	rect.origin.y=mPositionY;
	rect.size.width=mWidth;
	rect.size.height=mHeight;
	mView=(void*)[[ApplicationView alloc] initWithApplication:this frame:rect];
	#if defined(TOADLET_HAS_UIKIT)
		[(UIWindow*)mWindow addSubview:(ApplicationView*)mView];
		// No need to call the initial resized on iphone
	#else
		[(NSWindow*)mWindow setDelegate:(ApplicationView*)mView];
		[(NSWindow*)mWindow setContentView:(ApplicationView*)mView];
		// Need to call the initial resized on osx
		[(ApplicationView*)mView windowResized:nil];
	#endif
}

void OSXApplication::postEngineCreate(){
#if defined(TOADLET_PLATFORM_IOS)
	InputDevice::ptr linearDevice=new_IOSLinearDevice();
	if(linearDevice->create()==false){
		linearDevice=NULL;
	}
	else{
		mInputDevices[linearDevice->getType()]=linearDevice;
	}
#endif
	
	mBundleArchive=new OSXBundleArchive();
	static_pointer_cast<OSXBundleArchive>(mBundleArchive)->open([NSBundle mainBundle]);
	mEngine->getArchiveManager()->manage(static_pointer_cast<Archive>(mBundleArchive));
}

void OSXApplication::destroy(){
	BaseApplication::destroy();
	
	if(mPool!=nil){
		[(NSAutoreleasePool*)mPool release];
	}
}
	
void OSXApplication::start(){
	resized([(ApplicationView*)mView bounds].size.width,[(ApplicationView*)mView bounds].size.height);

	[(ApplicationView*)mView start];

	if(mPool!=nil){
		#if !defined(TOADLET_HAS_UIKIT)
			[(NSWindow*)mWindow makeKeyAndOrderFront:nil];
			[[NSApplication sharedApplication] run];
		#else
			[(UIWindow*)mWindow makeKeyAndVisible];
			[[UIApplication sharedApplication] run];
		#endif
	}
}

void OSXApplication::stop(){
	[(ApplicationView*)mView stop];

	#if !defined(TOADLET_HAS_UIKIT)
		[[NSApplication sharedApplication] terminate:nil];
	#endif
}

void OSXApplication::activate(){
	if(mActive==false){
		mActive=true;

		// See Win32Application::activate notes
	}
}

void OSXApplication::deactivate(){
	if(mActive==true){
		mActive=false;

		// See Win32Application::activate notes
	}
}

void OSXApplication::setTitle(const String &title){
	mTitle=title;
}

String OSXApplication::getTitle() const{
	return mTitle;
}

void OSXApplication::setSize(int width,int height){
	mWidth=width;
	mHeight=height;
}

void OSXApplication::setPosition(int x,int y){
	mPositionX=x;
	mPositionY=y;
}

int OSXApplication::getPositionX() const{
	return mPositionX;
}

int OSXApplication::getPositionY() const{
	return mPositionY;
}

int OSXApplication::getWidth() const{
	return mView!=nil?[(ApplicationView*)mView bounds].size.width:mWidth;
}

int OSXApplication::getHeight() const{
	return mView!=nil?[(ApplicationView*)mView bounds].size.height:mHeight;
}

void OSXApplication::setFullscreen(bool fullscreen){
	mFullscreen=fullscreen;
}

bool OSXApplication::getFullscreen() const{
	return mFullscreen;
}

void OSXApplication::setDifferenceMouse(bool difference){
	mDifferenceMouse=difference;
	
	#if !defined(TOADLET_PLATFORM_IOS)
		if(difference){
			CGDisplayHideCursor(kCGDirectMainDisplay);
		}
		else{
			CGDisplayShowCursor(kCGDirectMainDisplay);
		}
	#endif
}

void OSXApplication::internal_mousePressed(int x,int y,int button){
	mousePressed(x,y,button);

	mLastMouseX=x;
	mLastMouseY=y;
}

void OSXApplication::internal_mouseMoved(int x,int y){
	if(mDifferenceMouse){
		#if defined(TOADLET_PLATFORM_IOS)
			mouseMoved(mLastMouseX-x,mLastMouseY-y);
		#else
			CGGetLastMouseDelta(&x,&y);

			NSPoint npoint=NSMakePoint(getWidth()/2,getHeight()/2);
			npoint=[(ApplicationView*)mView convertPoint:npoint toView:nil];
			NSRect nrect=NSMakeRect(npoint.x,npoint.y,0,0);
			nrect=[[(ApplicationView*)mView window] convertRectToScreen:nrect];
			npoint=nrect.origin;
			npoint.y=[[NSScreen mainScreen] frame].size.height-npoint.y;
			CGPoint cpoint=CGPointMake(npoint.x,npoint.y);
			CGWarpMouseCursorPosition(cpoint);

			mouseMoved(x,y);
		#endif
	}
	else{
		mouseMoved(x,y);
	}

	mLastMouseX=x;
	mLastMouseY=y;
}

void *OSXApplication::getWindow(){
	#if defined(TOADLET_HAS_UIKIT)
		return (CAEAGLLayer*)[(UIView*)mView layer];
	#else
		return (NSView*)mView;
	#endif
}

int OSXApplication::translateKey(int key){
#if !defined(TOADLET_HAS_UIKIT)
	switch(key){
		case 27:
			return Key_ESC;
		case NSPauseFunctionKey:
			return Key_PAUSE;
		case NSLeftArrowFunctionKey:
			return Key_LEFT;
		case NSRightArrowFunctionKey:
			return Key_RIGHT;
		case NSUpArrowFunctionKey:
			return Key_UP;
		case NSDownArrowFunctionKey:
			return Key_DOWN;
		case NSEnterCharacter:
			return Key_ENTER;
		case NSTabCharacter:
			return Key_TAB;
		case NSBackspaceCharacter:
			return Key_BACKSPACE;
		case NSDeleteCharacter:
			return Key_DELETE;
		case 32:
			return Key_SPACE;
	}
#endif

	return key;
}

}
}
