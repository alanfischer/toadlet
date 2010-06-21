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

#ifndef TOADLET_PAD_BASEAPPLICATION_H
#define TOADLET_PAD_BASEAPPLICATION_H

#include <toadlet/egg/Runnable.h>
#include <toadlet/egg/String.h>
#include <toadlet/peeper/RenderTarget.h>
#include <toadlet/peeper/Renderer.h>
#include <toadlet/peeper/Visual.h>
#include <toadlet/ribbit/AudioPlayer.h>
#include <toadlet/flick/MotionDetector.h>
#include <toadlet/tadpole/Engine.h>
#include <cctype>

namespace toadlet{
namespace pad{

class ApplicationListener;

class BaseApplication:public peeper::RenderTarget{
public:
	TOADLET_SHARED_POINTERS(BaseApplication);

	enum Key{
		Key_ENTER=10,
		Key_TAB=8,
		Key_SPACE=32,

		Key_LEFT=1000,
		Key_RIGHT,
		Key_UP,
		Key_DOWN,

		// Keyboard keys
		Key_ESC=2000,
		Key_PAUSE,
		Key_SHIFT,
		Key_CTRL,
		Key_ALT,
		Key_SPECIAL,
		Key_BACKSPACE,
		Key_DELETE,

		// Cellphone keys
		Key_SOFTLEFT=3000,
		Key_SOFTRIGHT,
		Key_ACTION,
		Key_BACK,
	};

	static egg::String getKeyName(int key){
		switch(key){
			case(Key_ENTER):return		"enter";
			case(Key_TAB):return		"tab";
			case(Key_SPACE):return		"space";

			case(Key_LEFT):return		"left";
			case(Key_RIGHT):return		"right";
			case(Key_UP):return			"up";
			case(Key_DOWN):return		"down";

			case(Key_ESC):return		"esc";
			case(Key_PAUSE):return		"pause";
			case(Key_SHIFT):return		"shift";
			case(Key_CTRL):return		"ctrl";
			case(Key_ALT):return		"alt";
			case(Key_SPECIAL):return	"special";
			case(Key_BACKSPACE):return	"backspace";
			case(Key_DELETE):return		"delete";

			case(Key_SOFTLEFT):return	"softleft";
			case(Key_SOFTRIGHT):return	"softright";
			case(Key_ACTION):return		"action";
			case(Key_BACK):return		"back";
			default:
				if(isalpha(key) || isdigit(key)){
					return egg::String()+(char)key;
				}
				else{
					return (char*)NULL;
				}
		}
	}

	virtual ~BaseApplication(){}

	virtual void create(int renderer,int audioPlayer,int motionDetector)=0;
	virtual void destroy()=0;

	virtual void start()=0;
	virtual void runEventLoop()=0;
	virtual void stepEventLoop()=0;
	virtual void stop()=0;
	virtual bool isRunning() const=0;

	virtual void setAutoActivate(bool autoActivate)=0;
	virtual bool getAutoActivate() const=0;
	virtual void activate()=0;
	virtual void deactivate()=0;
	virtual bool active() const=0;

	virtual void setTitle(const egg::String &title)=0;
	virtual const egg::String &getTitle() const=0;

	virtual void setPosition(int x,int y)=0;
	virtual int getPositionX() const=0;
	virtual int getPositionY() const=0;

	virtual void setSize(int width,int height)=0;
	virtual int getWidth() const=0;
	virtual int getHeight() const=0;

	virtual void setFullscreen(bool fullscreen)=0;
	virtual bool getFullscreen() const=0;

	virtual void setVisual(const peeper::Visual &visual)=0;
	virtual const peeper::Visual &getVisual() const=0;

	virtual void setApplicationListener(ApplicationListener *listener)=0;
	virtual ApplicationListener *getApplicationListener() const=0;

	virtual tadpole::Engine *getEngine() const=0;
	virtual peeper::Renderer *getRenderer() const=0;
	virtual ribbit::AudioPlayer *getAudioPlayer() const=0;
	virtual flick::MotionDetector *getMotionDetector() const=0;

	virtual void resized(int width,int height)=0;
	virtual void focusGained()=0;
	virtual void focusLost()=0;
	virtual void keyPressed(int key)=0;
	virtual void keyReleased(int key)=0;
	virtual void mousePressed(int x,int y,int button)=0;
	virtual void mouseMoved(int x,int y)=0;
	virtual void mouseReleased(int x,int y,int button)=0;
	virtual void mouseScrolled(int x,int y,int scroll)=0;
	virtual void update(int dt)=0;
	virtual void render(peeper::Renderer *renderer)=0;
};

}
}

#endif
