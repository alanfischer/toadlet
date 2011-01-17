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
#include <toadlet/egg/Map.h>
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

/// @todo: Move more common items to this, and have it be a base class, and not an interface.
///  I can't think of any situations where it would be useful for BaseApplication to be an interface
class TOADLET_API BaseApplication:public peeper::RenderTarget{
public:
	TOADLET_SHARED_POINTERS(BaseApplication);

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
	};

	static void mapKeyNames(egg::Map<int,egg::String> &keyToName,egg::Map<egg::String,int> &nameToKey){
		egg::Map<int,egg::String> &map=keyToName;

		map.add(Key_ENTER,		"enter");
		map.add(Key_TAB,		"tab");
		map.add(Key_SPACE,		"space");

		map.add(Key_LEFT,		"left");
		map.add(Key_RIGHT,		"right");
		map.add(Key_UP,			"up");
		map.add(Key_DOWN,		"down");

		map.add(Key_ESC,		"esc");
		map.add(Key_PAUSE,		"pause");
		map.add(Key_SHIFT,		"shift");
		map.add(Key_CTRL,		"ctrl");
		map.add(Key_ALT,		"alt");
		map.add(Key_SPECIAL,	"special");
		map.add(Key_BACKSPACE,	"backspace");
		map.add(Key_DELETE,		"delete");

		map.add(Key_SOFTLEFT,	"softleft");
		map.add(Key_SOFTRIGHT,	"softright");
		map.add(Key_ACTION,		"action");
		map.add(Key_BACK,		"back");

		int key;
		for(key=0;key<256;++key){
			if(isalpha(key) || isdigit(key)){
				map.add(key,egg::String()+(char)key);
			}
		}

		egg::Map<int,egg::String>::iterator it;
		for(it=keyToName.begin();it!=keyToName.end();++it){
			nameToKey.add(it->second,it->first);
		}
	}

	BaseApplication(){
		mapKeyNames(mKeyToName,mNameToKey);
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

	virtual egg::String getKeyName(int key){egg::Map<int,egg::String>::iterator it=mKeyToName.find(key);return it!=mKeyToName.end()?it->second:(char*)NULL;}
	virtual int getKeyValue(const egg::String &name){egg::Map<egg::String,int>::iterator it=mNameToKey.find(name);return it!=mNameToKey.end()?it->second:0;}

protected:
	egg::Map<egg::String,int> mNameToKey;
	egg::Map<int,egg::String> mKeyToName;
};

}
}

#endif
