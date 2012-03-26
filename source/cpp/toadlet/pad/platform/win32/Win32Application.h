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

#include <toadlet/tadpole/platform/win32/Win32ResourceArchive.h>
#include <toadlet/pad/BaseApplication.h>

namespace toadlet{
namespace pad{

struct Win32Attributes;

class TOADLET_API Win32Application:public BaseApplication{
public:
	Win32Application();
	virtual ~Win32Application();

	void preEngineCreate();
	void postEngineCreate();
	void destroy();

	void start();
	void runEventLoop();
	void stepEventLoop();
	void stop();
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

	tadpole::Win32ResourceArchive::ptr getResourceArchive() const{return mResourceArchive;}

	void *getDisplay(){return 0;}
	void *getWindow(){return getHWND();}
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

protected:
	bool createWindow();
	void destroyWindow();

	mutable String mTitle;
	mutable int mPositionX,mPositionY;
	mutable int mWidth,mHeight;
	mutable bool mFullscreen;
	mutable bool mDifferenceMouse;
	int mLastXMouse,mLastYMouse;
	bool mSkipNextMove;
	
	tadpole::Win32ResourceArchive::ptr mResourceArchive;

	bool mRun;
	bool mActive;
	bool mDestroyed;
	bool mContextActive;

	Win32Attributes *win32;
};

}
}

#endif
