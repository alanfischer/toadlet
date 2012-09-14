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

#ifndef TOADLET_PAD_CONSOLEAPPLICATION_H
#define TOADLET_PAD_COSOLEAPPLICATION_H

#include <toadlet/pad/BaseApplication.h>

namespace toadlet{
namespace pad{

class TOADLET_API ConsoleApplication:public BaseApplication{
public:
	ConsoleApplication();

	void start();
	void stop();
	bool isRunning() const{return mRun;}

	void activate(){}
	void deactivate(){}
	bool isActive() const{return true;}

	void setTitle(const String &title){}
	String getTitle() const{return (char*)NULL;}

	void setPosition(int x,int y){}
	int getPositionX() const{return 0;}
	int getPositionY() const{return 0;}

	void setSize(int width,int height){}
	int getWidth() const{return 0;}
	int getHeight() const{return 0;}

	void setFullscreen(bool fullscreen){}
	bool getFullscreen() const{return false;}

	void setDifferenceMouse(bool difference){}
	bool getDifferenceMouse() const{return false;}

protected:
	bool mRun;
};

}
}

#endif
