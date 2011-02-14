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

#ifndef TOADLET_PAD_X11APPLICATION_H
#define TOADLET_PAD_X11APPLICATION_H

#include <toadlet/pad/BaseApplication.h>
#include <toadlet/egg/Thread.h>

namespace toadlet{
namespace pad{

struct X11Attributes;

class X11Application:public BaseApplication{
public:
	enum{
		RendererPlugin_NONE=-1,
		AudioPlayerPlugin_NONE=-1,
		MotionDetectorPlugin_NONE=-1,
	};

	X11Application();
	virtual ~X11Application();

	virtual void create(egg::String renderer=(char*)NULL,egg::String audioPlayer=(char*)NULL,egg::String motionDetector=(char*)NULL);
	virtual void destroy();

	virtual void start();
	virtual void runEventLoop();
	virtual void stepEventLoop();
	virtual void stop(){mRun=false;}
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
	virtual void update(int dt);
	virtual void render(peeper::Renderer *renderer);

	void setDifferenceMouse(bool difference);
	bool getDifferenceMouse() const{return mDifferenceMouse;}

	void setRendererOptions(int *options,int length);

	void *getDisplay() const;
	void *getWindow() const;
	void *getVisualInfo() const;

	void internal_mouseMoved(int x,int y);

protected:
	bool createWindow();
	void destroyWindow();
	peeper::RenderTarget *makeRenderTarget();
	peeper::Renderer *makeRenderer();
	bool createContextAndRenderer();
	bool destroyRendererAndContext();
	void originalResolution();
	void originalEnv();

	bool createAudioPlayer();
	bool destroyAudioPlayer();
	
	bool createMotionDetector();
	bool destroyMotionDetector();

	void configured(int x,int y,int width,int height);

	static int translateKey(int key);

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
	int *mRendererOptions;
	ribbit::AudioPlayer *mAudioPlayer;
	flick::MotionDetector *mMotionDetector;

	bool mRun;
	bool mAutoActivate;
	bool mActive;
	bool mDestroyed;

	X11Attributes *x11;
};

}
}

#endif
