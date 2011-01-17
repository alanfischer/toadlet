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

/// @todo: We get a crash on WinCE when rotating the screen.  Not specifically a crash, but a hang 2 renders afterwords.  I'm wondering if not all resources are being destroyed when we redo the surface on rotation.

class TOADLET_API Win32Application:public BaseApplication{
public:
	enum RendererPlugin{
		RendererPlugin_NONE=-1,
		RendererPlugin_ANY,
		RendererPlugin_GL,
		RendererPlugin_D3DM,
		RendererPlugin_D3D9,
		RendererPlugin_D3D10,
		RendererPlugin_D3D11,
		RendererPlugin_MAX,
	};

	enum AudioPlayerPlugin{
		AudioPlayerPlugin_NONE=-1,
		AudioPlayerPlugin_ANY,
		AudioPlayerPlugin_AL,
		AudioPlayerPlugin_MM,
		AudioPlayerPlugin_MAX
	};

	enum MotionDetectorPlugin{
		MotionDetectorPlugin_NONE=-1,
		MotionDetectorPlugin_ANY,
		MotionDetectorPlugin_HTC,
		MotionDetectorPlugin_SAMSUNG,
		MotionDetectorPlugin_MAX,
	};

	Win32Application();
	virtual ~Win32Application();

	virtual void create(int renderer=0,int audioPlayer=0,int motionDetector=0);
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

	virtual void setVisual(const peeper::Visual &visual);
	virtual const peeper::Visual &getVisual() const;

	virtual void setApplicationListener(ApplicationListener *listener);
	virtual ApplicationListener *getApplicationListener() const;

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

	void changeRendererPlugin(int index);
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
	peeper::RenderTarget *makeRenderTarget(int plugin);
	peeper::Renderer *makeRenderer(int plugin);
	bool createContextAndRenderer(int plugin);
	bool destroyRendererAndContext();
	bool changeVideoMode(int width,int height,int colorBits);

	ribbit::AudioPlayer *makeAudioPlayer(int plugin);
	bool createAudioPlayer(int plugin);
	bool destroyAudioPlayer();

	flick::MotionDetector *makeMotionDetector(int plugin);
	bool createMotionDetector(int plugin);
	bool destroyMotionDetector();

	egg::String mTitle;
	int mPositionX,mPositionY;
	int mWidth,mHeight;
	bool mFullscreen;
	peeper::Visual mVisual;
	ApplicationListener *mApplicationListener;
	bool mDifferenceMouse;
	int mLastXMouse,mLastYMouse;
	bool mSkipNextMove;
	
	tadpole::Engine *mEngine;
	peeper::RenderTarget *mRenderTarget;
	peeper::Renderer *mRenderer;
	int mRendererPlugin;
	int mChangeRendererPlugin;
	int *mRendererOptions;
	ribbit::AudioPlayer *mAudioPlayer;
	int *mAudioPlayerOptions;
	flick::MotionDetector *mMotionDetector;
	tadpole::handler::Win32ResourceArchive::ptr mResourceArchive;

	bool mRun;
	bool mAutoActivate;
	bool mActive;
	bool mDestroyed;
	bool mStopOnDeactivate;

	Win32Attributes *win32;
};

}
}

#endif
