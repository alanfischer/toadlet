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

#ifndef TOADLET_PAD_OSXAPPLICATION_H
#define TOADLET_PAD_OSXAPPLICATION_H

#include <toadlet/pad/BaseApplication.h>

namespace toadlet{
namespace pad{

class OSXApplication:public BaseApplication{
public:
	enum{
		RendererPlugin_NONE=-1,
		AudioPlayerPlugin_NONE=-1,
		MotionDetectorPlugin_NONE=-1,
	};

	OSXApplication();
	virtual ~OSXApplication();

	virtual void create(void *window,int renderer=0,int audioPlayer=0,int motionDetector=0);
	virtual void create(int renderer=0,int audioPlayer=0,int motionDetector=0);
	virtual void destroy();
	
	virtual void start();
	virtual void runEventLoop(){}
	virtual void stepEventLoop(){}
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

	void setRendererOptions(int *options,int length);

	static int translateKey(int key);

	inline void *getWindow() const{return mWindow;}
	inline void *getView() const{return mView;}
	
protected:
	peeper::RenderTarget *makeRenderTarget();
	peeper::Renderer *makeRenderer();
	bool createContextAndRenderer();
	bool destroyRendererAndContext();

	bool createAudioPlayer();
	bool destroyAudioPlayer();

	bool createMotionDetector();
	bool destroyMotionDetector();

	egg::String mTitle;
	int mPositionX,mPositionY;
	int mWidth,mHeight;
	bool mFullscreen;
	peeper::Visual mVisual;
	ApplicationListener *mApplicationListener;

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
	void *mWindow;
	void *mView;
};

}
}

#endif
