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
	X11Application();
	virtual ~X11Application();

	virtual void create(egg::String renderer=(char*)NULL,egg::String audioDevice=(char*)NULL,egg::String motionDevice=(char*)NULL);
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

	virtual void setStopOnDeactivate(bool stop){}
	virtual bool getStopOnDeactivate() const{return false;}

	virtual void setDifferenceMouse(bool difference);
	virtual bool getDifferenceMouse() const{return mDifferenceMouse;}

	virtual void changeRendererPlugin(const egg::String &plugin){}

	virtual void *getWindowHandle(){return getWindow();}
	void *getDisplay() const;
	void *getWindow() const;
	void *getVisualInfo() const;

	void internal_mouseMoved(int x,int y);

protected:
	bool createWindow();
	void destroyWindow();
	void originalResolution();
	void originalEnv();
	void configured(int x,int y,int width,int height);

	// Keeping our custom versions of these until we can unify the GLXRenderTargetWindow's constructor
	peeper::RenderTarget *makeRenderTarget();
	peeper::Renderer *makeRenderer();
	bool createContextAndRenderer(const egg::String &plugin);
	bool destroyRendererAndContext();

	static int translateKey(int key);

	egg::String mTitle;
	int mPositionX,mPositionY;
	int mWidth,mHeight;
	bool mFullscreen;
	bool mDifferenceMouse;
	int mLastXMouse,mLastYMouse;
	bool mSkipNextMove;

	bool mRun;
	bool mAutoActivate;
	bool mActive;
	bool mDestroyed;

	X11Attributes *x11;
};

}
}

#endif
