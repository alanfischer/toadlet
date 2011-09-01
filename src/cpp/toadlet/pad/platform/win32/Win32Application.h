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

#include <toadlet/tadpole/handler/platform/win32/Win32ResourceArchive.h>
#include <toadlet/pad/BaseApplication.h>

namespace toadlet{
namespace pad{

struct Win32Attributes;

class TOADLET_API Win32Application:public BaseApplication{
public:
	Win32Application();
	virtual ~Win32Application();

	virtual void create(String renderDevice=(char*)NULL,String audioPlayer=(char*)NULL,String motionDevice=(char*)NULL);
	virtual void destroy();

	virtual void start();
	virtual void runEventLoop();
	virtual void stepEventLoop();
	virtual void stop();
	virtual bool isRunning() const{return mRun;}

	virtual void setAutoActivate(bool activate){mAutoActivate=activate;}
	virtual bool getAutoActivate() const{return mAutoActivate;}
	virtual void activate();
	virtual void deactivate();
	virtual bool active() const{return mActive;}

	virtual void setTitle(const String &title);
	virtual const String &getTitle() const;

	virtual void setPosition(int x,int y);
	virtual int getPositionX() const;
	virtual int getPositionY() const;

	virtual void setSize(int width,int height);
	virtual int getWidth() const;
	virtual int getHeight() const;

	virtual void setFullscreen(bool fullscreen);
	virtual bool getFullscreen() const;

	virtual void setStopOnDeactivate(bool stopOnDeactivate){mStopOnDeactivate=stopOnDeactivate;}
	virtual bool getStopOnDeactivate() const{return mStopOnDeactivate;}

	virtual void setDifferenceMouse(bool difference);
	virtual bool getDifferenceMouse() const{return mDifferenceMouse;}

	tadpole::handler::Win32ResourceArchive::ptr getResourceArchive() const{return mResourceArchive;}

	virtual void *getWindowHandle(){return getHWND();}
	void changeRenderDevicePlugin(const String &plugin){mNewRenderDevicePlugin=plugin;}
	void setIcon(void *icon);
	void *getHINSTANCE() const;
	void *getHWND() const;
	bool changeVideoMode(int width,int height,int colorBits);

	void internal_resize(int width,int height);
	void internal_mouseMoved(int x,int y);

	typedef Map<void*,Win32Application*> ApplicationMap;
	static int mWindowCount;
	static ApplicationMap mApplicationMap;
	static int translateKey(int key);
	static scalar joyToScalar(int joy);

protected:
	bool createWindow();
	void destroyWindow();

	String mTitle;
	int mPositionX,mPositionY;
	int mWidth,mHeight;
	bool mFullscreen;
	bool mDifferenceMouse;
	int mLastXMouse,mLastYMouse;
	bool mSkipNextMove;
	
	tadpole::handler::Win32ResourceArchive::ptr mResourceArchive;

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
