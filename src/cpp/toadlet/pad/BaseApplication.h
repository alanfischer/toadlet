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
#include <toadlet/peeper/WindowRenderTargetFormat.h>
#include <toadlet/ribbit/AudioPlayer.h>
#include <toadlet/flick/MotionDetector.h>
#include <toadlet/tadpole/Engine.h>
#include <toadlet/pad/ApplicationListener.h>
//#include <cctype>

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

	static void mapKeyNames(egg::Map<int,egg::String> &keyToName,egg::Map<egg::String,int> &nameToKey);

	BaseApplication();
	virtual ~BaseApplication(){}

	virtual void create(egg::String renderer,egg::String audioPlayer,egg::String motionDetector);
	virtual void destroy();

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

	virtual void setStopOnDeactivate(bool stopOnDeactivate)=0;
	virtual bool getStopOnDeactivate() const=0;

	virtual void setDifferenceMouse(bool difference)=0;
	virtual bool getDifferenceMouse() const=0;

	virtual peeper::RenderTarget *getRootRenderTarget(){return mRenderTarget;}
	virtual bool isPrimary() const{return mRenderTarget->isPrimary();}
	virtual bool isValid() const{return mRenderTarget->isValid();}

	virtual void setWindowRenderTargetFormat(const peeper::WindowRenderTargetFormat::ptr format){mFormat=format;}
	virtual peeper::WindowRenderTargetFormat::ptr getWindowRenderTargetFormat() const{return mFormat;}

	virtual void setApplicationListener(ApplicationListener *listener){mListener=listener;}
	virtual ApplicationListener *getApplicationListener() const{return mListener;}

	virtual void changeRendererPlugin(const egg::String &plugin)=0;
	virtual void setRendererOptions(int *options,int length);
	virtual void setAudioPlayerOptions(int *options,int length);

	virtual void resized(int width,int height)		{if(mListener!=NULL){mListener->resized(width,height);}}
	virtual void focusGained()						{if(mListener!=NULL){mListener->focusGained();}}
	virtual void focusLost()						{if(mListener!=NULL){mListener->focusLost();}}
	virtual void keyPressed(int key)				{if(mListener!=NULL){mListener->keyPressed(key);}}
	virtual void keyReleased(int key)				{if(mListener!=NULL){mListener->keyReleased(key);}}
	virtual void mousePressed(int x,int y,int button){if(mListener!=NULL){mListener->mousePressed(x,y,button);}}
	virtual void mouseMoved(int x,int y)			{if(mListener!=NULL){mListener->mouseMoved(x,y);}}
	virtual void mouseReleased(int x,int y,int button){if(mListener!=NULL){mListener->mouseReleased(x,y,button);}}
	virtual void mouseScrolled(int x,int y,int scroll){if(mListener!=NULL){mListener->mouseScrolled(x,y,scroll);}}
	virtual void joyPressed(int button)				{if(mListener!=NULL){mListener->joyPressed(button);}}
	virtual void joyMoved(scalar x,scalar y,scalar z,scalar r,scalar u,scalar v){if(mListener!=NULL){mListener->joyMoved(x,y,z,r,u,v);}}
	virtual void joyReleased(int button)			{if(mListener!=NULL){mListener->joyReleased(button);}}
	virtual void update(int dt)						{if(mListener!=NULL){mListener->update(dt);}}
	virtual void render(peeper::Renderer *renderer)	{if(mListener!=NULL){mListener->render(renderer);}}

	virtual tadpole::Engine *getEngine() const{return mEngine;}
	virtual peeper::Renderer *getRenderer() const{return mRenderer;}
	virtual ribbit::AudioPlayer *getAudioPlayer() const{return mAudioPlayer;}
	virtual flick::MotionDetector *getMotionDetector() const{return mMotionDetector;}

	virtual egg::String getKeyName(int key){egg::Map<int,egg::String>::iterator it=mKeyToName.find(key);return it!=mKeyToName.end()?it->second:(char*)NULL;}
	virtual int getKeyValue(const egg::String &name){egg::Map<egg::String,int>::iterator it=mNameToKey.find(name);return it!=mNameToKey.end()?it->second:0;}

	virtual void setBackable(bool backable){
		if(mEngine!=NULL){
			egg::Error::unknown(egg::Categories::TOADLET_PAD,"can not change backable once engine is created");
			return;
		}

		mBackable=backable;
	}
	virtual bool getBackable() const{return mBackable;}

	virtual void *getWindowHandle()=0;

protected:
	class RendererPlugin{
	public:
		RendererPlugin(
			peeper::RenderTarget *(*renderTarget)(void *,peeper::WindowRenderTargetFormat *)=NULL,
			peeper::Renderer *(*renderer)()=NULL
		):createRenderTarget(renderTarget),createRenderer(renderer){}

		peeper::RenderTarget *(*createRenderTarget)(void *,peeper::WindowRenderTargetFormat *);
		peeper::Renderer *(*createRenderer)();
	};

	class AudioPlayerPlugin{
	public:
		AudioPlayerPlugin(
			ribbit::AudioPlayer *(*audioPlayer)()=NULL
		):createAudioPlayer(audioPlayer){}

		ribbit::AudioPlayer *(*createAudioPlayer)();
	};

	class MotionDetectorPlugin{
	public:
		MotionDetectorPlugin(
			flick::MotionDetector *(*motionDetector)()=NULL
		):createMotionDetector(motionDetector){}

		flick::MotionDetector *(*createMotionDetector)();
	};

	peeper::RenderTarget *makeRenderTarget(const egg::String &plugin);
	peeper::Renderer *makeRenderer(const egg::String &plugin);
	bool createContextAndRenderer(const egg::String &plugin);
	bool destroyRendererAndContext();
	bool changeVideoMode(int width,int height,int colorBits);

	ribbit::AudioPlayer *makeAudioPlayer(const egg::String &plugin);
	bool createAudioPlayer(const egg::String &plugin);
	bool destroyAudioPlayer();

	flick::MotionDetector *makeMotionDetector(const egg::String &plugin);
	bool createMotionDetector(const egg::String &plugin);
	bool destroyMotionDetector();

	egg::Map<egg::String,RendererPlugin> mRendererPlugins;
	egg::String mCurrentRendererPlugin;
	egg::String mNewRendererPlugin;
	int *mRendererOptions;
	egg::Map<egg::String,AudioPlayerPlugin> mAudioPlayerPlugins;
	int *mAudioPlayerOptions;
	egg::Map<egg::String,MotionDetectorPlugin> mMotionDetectorPlugins;

	egg::Map<egg::String,int> mNameToKey;
	egg::Map<int,egg::String> mKeyToName;

	bool mBackable;
	peeper::WindowRenderTargetFormat::ptr mFormat;
	ApplicationListener *mListener;
	bool mDifferenceMouse;

	tadpole::Engine *mEngine;
	peeper::RenderTarget *mRenderTarget;
	peeper::Renderer *mRenderer;
	ribbit::AudioPlayer *mAudioPlayer;
	flick::MotionDetector *mMotionDetector;
};

}
}

#endif
