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

#include <toadlet/egg/io/Archive.h>
#include <toadlet/pad/BaseApplication.h>

namespace toadlet{
namespace pad{

class TOADLET_API OSXApplication:public BaseApplication{
public:
	OSXApplication();
	virtual ~OSXApplication();

	virtual void setWindow(void *window);
	virtual void preEngineCreate();
	virtual void postEngineCreate();
	virtual void destroy();
	
	virtual void start();
	virtual void runEventLoop(){}
	virtual void stepEventLoop(){}
	virtual void stop();
	virtual bool isRunning() const{return mRun;}

	virtual void activate();
	virtual void deactivate();
	virtual bool isActive() const{return mActive;}

	virtual void setTitle(const String &title);
	virtual String getTitle() const;

	virtual void setPosition(int x,int y);
	virtual int getPositionX() const;
	virtual int getPositionY() const;

	virtual void setSize(int width,int height);
	virtual int getWidth() const;
	virtual int getHeight() const;

	virtual void setFullscreen(bool fullscreen);
	virtual bool getFullscreen() const;

	virtual void setDifferenceMouse(bool difference);
	virtual bool getDifferenceMouse() const{return mDifferenceMouse;}

	static int translateKey(int key);

	virtual void changeRenderDevicePlugin(const String &plugin){}

	void *getDisplay(){return 0;}
	void *getWindow();
	void *getView(){return mView;}

	void internal_mouseMoved(int x,int y);
	
protected:
	String mTitle;
	int mPositionX,mPositionY;
	int mWidth,mHeight;
	bool mFullscreen;
	bool mDifferenceMouse;

	Archive::ptr mBundleArchive;
	
	bool mRun;
	bool mActive;
	bool mDestroyed;
	void *mWindow;
	void *mView;
	void *mPool;
};

}
}

#endif
