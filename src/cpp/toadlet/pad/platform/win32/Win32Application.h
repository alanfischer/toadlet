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

#ifndef TOADLET_PAD_WIN32APPLICATION_H
#define TOADLET_PAD_WIN32APPLICATION_H

#include <toadlet/egg/Thread.h>
#include <toadlet/tadpole/handler/platform/win32/Win32ResourceArchive.h>
#include <toadlet/pad/BaseApplication.h>

namespace toadlet{
namespace pad{

struct Win32Attributes;

class TOADLET_API Win32Application:public BaseApplication{
public:
	Win32Application();
	virtual ~Win32Application();

	virtual void create(egg::String renderer=(char*)NULL,egg::String audioPlayer=(char*)NULL,egg::String motionDetector=(char*)NULL);
	virtual void destroy();

	virtual void start();
	virtual void runEventLoop();
	virtual void stepEventLoop();
	virtual void stop();
	virtual bool isRunning() const{return mRun;}

	virtual void setAutoActivate(bool autoActivate){mAutoActivate=autoActivate;}
	virtual bool getAutoActivate() const{return mAutoActivate;}
	virtual void activate();
	virtual void deactivate();
	virtual bool active() const{return mActive;}

	virtual void setTitle(const egg::String &title);
	virtual const egg::String &getTitle() const;

	virtual void setPosition(int x,int y);
	virtual int getPositionX() const;
	virtual int getPositionY() const;

	virtual void setSize(int width,int height);
	virtual int getWidth() const;
	virtual int getHeight() const;

	virtual void setFullscreen(bool fullscreen);
	virtual bool getFullscreen() const;

	virtual void setWindowRenderTargetFormat(const peeper::WindowRenderTargetFormat::ptr format){mFormat=format;}
	virtual peeper::WindowRenderTargetFormat::ptr getWindowRenderTargetFormat() const{return mFormat;}

	virtual void setApplicationListener(ApplicationListener *listener){mListener=listener;}
	virtual ApplicationListener *getApplicationListener() const{return mListener;}

	virtual tadpole::Engine *getEngine() const{return mEngine;}
	virtual peeper::Renderer *getRenderer() const{return mRenderer;}
	virtual ribbit::AudioPlayer *getAudioPlayer() const{return mAudioPlayer;}
	virtual flick::MotionDetector *getMotionDetector() const{return mMotionDetector;}
	tadpole::handler::Win32ResourceArchive::ptr getResourceArchive() const{return mResourceArchive;}

	virtual peeper::RenderTarget *getRootRenderTarget(){return mRenderTarget;}
	virtual bool isPrimary() const{return mRenderTarget->isPrimary();}
	virtual bool isValid() const{return mRenderTarget->isValid();}

	virtual void resized(int width,int height);
	virtual void focusGained();
	virtual void focusLost();
	virtual void keyPressed(int key);
	virtual void keyReleased(int key);
	virtual void mousePressed(int x,int y,int button);
	virtual void mouseMoved(int x,int y);
	virtual void mouseReleased(int x,int y,int button);
	virtual void mouseScrolled(int x,int y,int scroll);
	virtual void joyPressed(int button);
	virtual void joyMoved(scalar x,scalar y,scalar z,scalar r,scalar u,scalar v);
	virtual void joyReleased(int button);
	virtual void update(int dt);
	virtual void render(peeper::Renderer *renderer);

	virtual void setDifferenceMouse(bool difference);
	virtual bool getDifferenceMouse() const{return mDifferenceMouse;}

	virtual void setStopOnDeactivate(bool stopOnDeactivate){mStopOnDeactivate=stopOnDeactivate;}
	virtual bool getStopOnDeactivate(){return mStopOnDeactivate;}

	void setBackable(bool backable);
	void changeRendererPlugin(const egg::String &plugin);
	void setRendererOptions(int *options,int length);
	void setAudioPlayerOptions(int *options,int length);

	void setIcon(void *icon);
	void *getHINSTANCE() const;
	void *getHWND() const;

	void internal_resize(int width,int height);
	void internal_mouseMoved(int x,int y);

	typedef egg::Map<void*,Win32Application*> ApplicationMap;
	static int mWindowCount;
	static ApplicationMap mApplicationMap;
	static int translateKey(int key);
	static scalar joyToScalar(int joy);

protected:
	bool createWindow();
	void destroyWindow();
	peeper::RenderTarget *makeRenderTarget(const egg::String &plugin);
	peeper::Renderer *makeRenderer(const egg::String &plugin);
	bool createContextAndRenderer(const egg::String &plugin);
	bool destroyRendererAndContext();
	bool changeVideoMode(int width,int height,int colorBits);

	ribbit::AudioPlayer *makeAudioPlayer(const egg::String &plugin);
	bool createAudioPlayer(const egg::String &plugin);
	bool destroyAudioPlayer();

	flick::MotionDetector *makeMotionDetector(const egg::String &plugin);
	bool createMotionDetector(const egg::String &plugin);
	bool destroyMotionDetector();

	egg::String mTitle;
	int mPositionX,mPositionY;
	int mWidth,mHeight;
	bool mFullscreen;
	peeper::WindowRenderTargetFormat::ptr mFormat;
	ApplicationListener *mListener;
	bool mDifferenceMouse;
	int mLastXMouse,mLastYMouse;
	bool mSkipNextMove;
	
	bool mBackable;
	tadpole::Engine *mEngine;
	peeper::RenderTarget *mRenderTarget;
	peeper::Renderer *mRenderer;
	ribbit::AudioPlayer *mAudioPlayer;
	flick::MotionDetector *mMotionDetector;
	tadpole::handler::Win32ResourceArchive::ptr mResourceArchive;

	class RendererPlugin{
	public:
		RendererPlugin(
			peeper::RenderTarget *(*renderTarget)(void *,peeper::WindowRenderTargetFormat *)=NULL,
			peeper::Renderer *(*renderer)()=NULL
		):createRenderTarget(renderTarget),createRenderer(renderer){}

		peeper::RenderTarget *(*createRenderTarget)(void *,peeper::WindowRenderTargetFormat *);
		peeper::Renderer *(*createRenderer)();
	};

	class AudioPlayerPlugin{
	public:
		AudioPlayerPlugin(
			ribbit::AudioPlayer *(*audioPlayer)()=NULL
		):createAudioPlayer(audioPlayer){}

		ribbit::AudioPlayer *(*createAudioPlayer)();
	};

	class MotionDetectorPlugin{
	public:
		MotionDetectorPlugin(
			flick::MotionDetector *(*motionDetector)()=NULL
		):createMotionDetector(motionDetector){}

		flick::MotionDetector *(*createMotionDetector)();
	};

	egg::Map<egg::String,RendererPlugin> mRendererPlugins;
	egg::String mCurrentRendererPlugin;
	egg::String mNewRendererPlugin;
	int *mRendererOptions;
	egg::Map<egg::String,AudioPlayerPlugin> mAudioPlayerPlugins;
	int *mAudioPlayerOptions;
	egg::Map<egg::String,MotionDetectorPlugin> mMotionDetectorPlugins;

	bool mRun;
	bool mAutoActivate;
	bool mActive;
	bool mDestroyed;
	bool mStopOnDeactivate;
	bool mContextActive;

	Win32Attributes *win32;
};

}
}

#endif
