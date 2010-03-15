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

#if defined(TOADLET_PLATFORM_IPHONE)
	#define TOADLET_HAS_UIKIT
#endif
 
#include <toadlet/egg/System.h>
#include <toadlet/egg/Error.h>
#if defined(TOADLET_HAS_OPENGL)
	#include <toadlet/peeper/plugins/glrenderer/GLRenderer.h>
	#if defined(TOADLET_HAS_UIKIT)
		#include <toadlet/peeper/plugins/glrenderer/platform/eagl/EAGLRenderTarget.h>
	#else
		#include <toadlet/peeper/plugins/glrenderer/platform/nsgl/NSGLRenderTarget.h>
	#endif
#endif
#if defined(TOADLET_HAS_OPENAL)
	#include <toadlet/ribbit/plugins/alplayer/ALPlayer.h>
#endif
#include <toadlet/pad/platform/osx/OSXApplication.h>
#include <toadlet/pad/ApplicationListener.h>

#import <QuartzCore/QuartzCore.h>
#if defined(TOADLET_HAS_UIKIT)
	#import <UIKit/UIKit.h>
#else
	#import <AppKit/AppKit.h>
#endif

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::tadpole;
using namespace toadlet::pad;

@interface ApplicationView:
#if defined(TOADLET_HAS_UIKIT)
	UIView
#else
	NSView
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

		mTimer=[NSTimer scheduledTimerWithTimeInterval:0.01 target:self selector:@selector(update) userInfo:nil repeats:YES];
		[[NSRunLoop currentRunLoop] addTimer:mTimer forMode:NSDefaultRunLoopMode];
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

- (void) update{
	toadlet::uint64 currentTime=System::mtime();
	if(mApplication->active()){
		mApplication->update(currentTime-mLastTime);
		if(mApplication->getRenderer()!=NULL){
			mApplication->render(mApplication->getRenderer());
		}
	}
	mLastTime=currentTime;
}

#if defined(TOADLET_HAS_UIKIT)
- (void) layoutSubviews{
#else
- (void) windowResized:(NSNotification*)notification{
#endif
	mApplication->resized([self bounds].size.width,[self bounds].size.height);

	if(mApplication->active() && mApplication->getRenderer()!=NULL){
		if(mApplication->getRenderer()->getCapabilitySet().resetOnResize){
			mApplication->getEngine()->contextReset(mApplication->getRenderer());
		}
		mApplication->update(0);
		mApplication->render(mApplication->getRenderer());
	}
}

#if defined(TOADLET_HAS_UIKIT)
// Handles the start of a touch
- (void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event{
	CGRect bounds=[self bounds];
	UITouch* touch=[[event touchesForView:self] anyObject];
	CGPoint location=[touch locationInView:self];
	mLastLocation=location;
	mApplication->mousePressed(location.x,location.y,0);
}

// Handles the continuation of a touch.
- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event{  
	CGRect bounds=[self bounds];
	UITouch* touch=[[event touchesForView:self] anyObject];
	CGPoint location=[touch locationInView:self];
	mLastLocation=location;
	mApplication->mouseMoved(location.x,location.y);
}

// Handles the end of a touch event when the touch is a tap.
- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event{
	CGRect bounds=[self bounds];
	UITouch* touch=[[event touchesForView:self] anyObject];
	CGPoint location=[touch locationInView:self];
	if(location.x!=mLastLocation.x || location.y!=mLastLocation.y){
		mApplication->mouseMoved(location.x,location.y);
	}
	mApplication->mouseReleased(location.x,location.y,0);
}

// Handles the end of a touch event.
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event{
	CGRect bounds=[self bounds];
	UITouch* touch=[[event touchesForView:self] anyObject];
	CGPoint location=[touch locationInView:self];
	if(location.x!=mLastLocation.x || location.y!=mLastLocation.y){
		mApplication->mouseMoved(location.x,location.y);
	}
	mApplication->mouseReleased(location.x,location.y,0);
}
#else
- (void) mouseDown:(NSEvent*)event{
	mApplication->mousePressed(
		[event locationInWindow].x,
		[self bounds].size.height-[event locationInWindow].y,
		[event buttonNumber]
	);
}

- (void) mouseDragged:(NSEvent*)event{
	mApplication->mouseMoved(
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
#endif

@end

namespace toadlet{
namespace pad{

OSXApplication::OSXApplication():
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
	mWindow(nil),
	mView(nil)
{}

OSXApplication::~OSXApplication(){
	destroy();

	delete[] mRendererOptions;

	if(mView!=nil){
		[(ApplicationView*)mView release];
	}
}

void OSXApplication::create(void *window){
	mWindow=window;

	create();
}

void OSXApplication::create(){
	if(mWindow==nil){
		Error::nullPointer("invalid window");
		return;
	}

	mEngine=new Engine();

	createAudioPlayer();
	
	#if defined(TOADLET_HAS_UIKIT)
		mWidth=[(UIWindow*)mWindow bounds].size.width;
		mHeight=[(UIWindow*)mWindow bounds].size.height;
		CGRect rect;
	#else
		NSView *view=[(NSWindow*)mWindow contentView];
		if(view!=nil){
			mWidth=[view bounds].size.width;
			mHeight=[view bounds].size.height;
		}
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
		[(NSWindow*)mWindow setContentView:(ApplicationView*)mView];
		// Need to call the initial resized on osx
		[(ApplicationView*)mView windowResized:nil];
	#endif
	
	activate();
}

void OSXApplication::destroy(){
	if(mEngine!=NULL){
		mEngine->destroy();
	}

	deactivate();
	destroyAudioPlayer();

	if(mEngine!=NULL){
		delete mEngine;
		mEngine=NULL;
	}
}
	
void OSXApplication::start(){
	mRun=true;
	resized([(ApplicationView*)mView bounds].size.width,[(ApplicationView*)mView bounds].size.height);
}

void OSXApplication::stop(){
	mRun=false;

	#if !defined(TOADLET_HAS_UIKIT)
		[[NSApplication sharedApplication] terminate:nil];
	#endif
}

void OSXApplication::activate(){
	if(mActive==false){
		mActive=true;

		createContextAndRenderer();
	}
}

void OSXApplication::deactivate(){
	if(mActive==true){
		mActive=false;

		destroyRendererAndContext();
	}
}

void OSXApplication::setTitle(const String &title){
	mTitle=title;
}

const String &OSXApplication::getTitle() const{
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

void OSXApplication::setVisual(const peeper::Visual &visual){
	mVisual=visual;
}

const peeper::Visual &OSXApplication::getVisual() const{
	return mVisual;
}

void OSXApplication::setApplicationListener(ApplicationListener *listener){
	mApplicationListener=listener;
}

ApplicationListener *OSXApplication::getApplicationListener() const{
	return mApplicationListener;
}

RenderTarget *OSXApplication::makeRenderTarget(){
	#if defined(TOADLET_HAS_OPENGL)
		#if defined(TOADLET_HAS_UIKIT)
			return (GLRenderTarget*)new EAGLRenderTarget((CAEAGLLayer*)[(UIView*)mView layer],mVisual);
		#else
			return new NSGLRenderTarget((NSView*)mView,mVisual);
	#endif
	#else
		return NULL;
	#endif
}

Renderer *OSXApplication::makeRenderer(){
	#if defined(TOADLET_HAS_OPENGL)
		return new GLRenderer();
	#else
		return NULL;
	#endif
}

bool OSXApplication::createContextAndRenderer(){
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
			"Error creating RenderTargetPeer");
		return false;
	}

	if(mRenderer!=NULL){
		mEngine->setRenderer(mRenderer);
	}

	return mRenderer!=NULL;
}

bool OSXApplication::destroyRendererAndContext(){
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

bool OSXApplication::createAudioPlayer(){
	#if defined(TOADLET_HAS_OPENAL)
		mAudioPlayer=new ALPlayer();
		// Fade in buffers over 100 ms, reduces popping
		int options[]={1,100,0};
		bool result=false;
		TOADLET_TRY
			result=mAudioPlayer->create(options);
		TOADLET_CATCH(const Exception &){result=false;}
		if(result==false){
			delete mAudioPlayer;
			mAudioPlayer=NULL;
		}
	#endif
	if(mAudioPlayer!=NULL){
		mEngine->setAudioPlayer(mAudioPlayer);
	}
	return true;
}
	
bool OSXApplication::destroyAudioPlayer(){
	if(mAudioPlayer!=NULL){
		mEngine->setAudioPlayer(NULL);
		mAudioPlayer->destroy();
		delete mAudioPlayer;
		mAudioPlayer=NULL;
	}
	return true;
}

void OSXApplication::resized(int width,int height){
	if(mApplicationListener!=NULL){
		mApplicationListener->resized(width,height);
	}
}

void OSXApplication::focusGained(){
	if(mApplicationListener!=NULL){
		mApplicationListener->focusGained();
	}
}

void OSXApplication::focusLost(){
	if(mApplicationListener!=NULL){
		mApplicationListener->focusLost();
	}
}

void OSXApplication::keyPressed(int key){
	if(mApplicationListener!=NULL){
		mApplicationListener->keyPressed(key);
	}
}

void OSXApplication::keyReleased(int key){
	if(mApplicationListener!=NULL){
		mApplicationListener->keyReleased(key);
	}
}

void OSXApplication::mousePressed(int x,int y,int button){
	if(mApplicationListener!=NULL){
		mApplicationListener->mousePressed(x,y,button);
	}
}
	
void OSXApplication::mouseMoved(int x,int y){
	if(mApplicationListener!=NULL){
		mApplicationListener->mouseMoved(x,y);
	}
}

void OSXApplication::mouseReleased(int x,int y,int button){
	if(mApplicationListener!=NULL){
		mApplicationListener->mouseReleased(x,y,button);
	}
}

void OSXApplication::mouseScrolled(int x,int y,int scroll){
	if(mApplicationListener!=NULL){
		mApplicationListener->mouseScrolled(x,y,scroll);
	}
}

void OSXApplication::update(int dt){
	if(mApplicationListener!=NULL){
		mApplicationListener->update(dt);
	}
}

void OSXApplication::render(Renderer *renderer){
	if(mApplicationListener!=NULL){
		mApplicationListener->render(renderer);
	}
}

void OSXApplication::setRendererOptions(int *options,int length){
        if(mRendererOptions!=NULL){
                delete[] mRendererOptions;
        }

        mRendererOptions=new int[length];
        memcpy(mRendererOptions,options,length*sizeof(int));
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
