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

	void preEngineCreate();
	void destroy();

	void start();
	void runEventLoop();
	void stepEventLoop();
	void stop(){mRun=false;}
	bool isRunning() const{return mRun;}

	void activate();
	void deactivate();
	bool isActive() const{return mActive;}

	void setTitle(const String &title);
	String getTitle() const;

	void setPosition(int x,int y);
	int getPositionX() const;
	int getPositionY() const;

	void setSize(int width,int height);
	int getWidth() const;
	int getHeight() const;

	void setFullscreen(bool fullscreen);
	bool getFullscreen() const;

	void setDifferenceMouse(bool difference);
	bool getDifferenceMouse() const{return mDifferenceMouse;}

	void changeRenderDevicePlugin(const String &plugin){}

	void *getDisplay();
	void *getWindow();
	void *getVisualInfo();

	void internal_mouseMoved(int x,int y);

protected:
	bool createWindow();
	void destroyWindow();
	void originalResolution();
	void originalEnv();
	void configured(int x,int y,int width,int height);

	static int translateKey(int key);

	String mTitle;
	int mPositionX,mPositionY;
	int mWidth,mHeight;
	bool mFullscreen;
	bool mDifferenceMouse;
	int mLastXMouse,mLastYMouse;
	bool mSkipNextMove;

	bool mRun;
	bool mActive;
	bool mDestroyed;

	X11Attributes *x11;
};

}
}

#endif
