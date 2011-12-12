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

#ifndef TOADLET_PAD_APPLICATION_H
#define TOADLET_PAD_APPLICATION_H

#include <toadlet/egg/String.h>
#include <toadlet/peeper/RenderTarget.h>
#include <toadlet/peeper/RenderDevice.h>
#include <toadlet/peeper/WindowRenderTargetFormat.h>
#include <toadlet/ribbit/AudioDevice.h>
#include <toadlet/flick/InputDevice.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/pad/Types.h>
#include <toadlet/pad/Applet.h>

namespace toadlet{
namespace pad{

class TOADLET_API Application{
public:
	TOADLET_SHARED_POINTERS(Application);

	enum Key{
		Key_ENTER=10,
		Key_TAB=8,
		Key_SPACE=32,

		Key_LEFT=1024,
		Key_RIGHT,
		Key_UP,
		Key_DOWN,

		// Keyboard keys
		Key_ESC=2048,
		Key_PAUSE,
		Key_SHIFT,
		Key_CTRL,
		Key_ALT,
		Key_SPECIAL,
		Key_BACKSPACE,
		Key_DELETE,

		// Cellphone keys
		Key_SOFTLEFT=4096,
		Key_SOFTRIGHT,
		Key_ACTION,
		Key_BACK,
		Key_MENU,
		Key_SEARCH,
	};

	virtual ~Application(){}

	virtual bool create(String renderDevice,String audioDevice)=0;
	virtual void destroy()=0;

	virtual void start()=0;
	virtual void stop()=0;
	virtual bool isRunning() const=0;

	virtual void activate()=0;
	virtual void deactivate()=0;
	virtual bool isActive() const=0;

	virtual void setTitle(const String &title)=0;
	virtual String getTitle() const=0;

	virtual void setPosition(int x,int y)=0;
	virtual int getPositionX() const=0;
	virtual int getPositionY() const=0;

	virtual void setSize(int width,int height)=0;
	virtual int getWidth() const=0;
	virtual int getHeight() const=0;

	virtual void setFullscreen(bool fullscreen)=0;
	virtual bool getFullscreen() const=0;

	virtual void setDifferenceMouse(bool difference)=0;
	virtual bool getDifferenceMouse() const=0;

	virtual void setApplet(Applet *applet)=0;
	virtual Applet *getApplet() const=0;

	virtual Engine *getEngine() const=0;
	virtual RenderDevice *getRenderDevice() const=0;
	virtual AudioDevice *getAudioDevice() const=0;
	virtual InputDevice *getInputDevice(InputDevice::InputType type) const=0;
};

}
}

#endif
