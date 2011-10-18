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

#include <toadlet/pad/Application.h>

namespace toadlet{
namespace pad{

class TOADLET_API BaseApplication:public Application,public RenderTarget{
public:
	TOADLET_SHARED_POINTERS(BaseApplication);

	static void mapKeyNames(Map<int,String> &keyToName,Map<String,int> &nameToKey);

	BaseApplication();
	virtual ~BaseApplication(){}

	virtual bool defaultCreate(){return create("","","","");}
	virtual bool create(String renderDevice,String audioDevice,String motionDevice,String joyDevice);
	virtual void destroy();

	virtual RenderTarget *getRootRenderTarget(){return mRenderTarget;}
	virtual bool isPrimary() const{return mRenderTarget->isPrimary();}
	virtual bool isValid() const{return mRenderTarget->isValid();}

	virtual void setWindowRenderTargetFormat(const WindowRenderTargetFormat::ptr format){mFormat=format;}
	virtual WindowRenderTargetFormat::ptr getWindowRenderTargetFormat() const{return mFormat;}

	virtual void setApplet(Applet *applet){mApplet=applet;}
	virtual Applet *getApplet() const{return mApplet;}

	virtual void changeRenderDevicePlugin(const String &plugin)=0;
	virtual void setRenderDeviceOptions(int *options,int length);
	virtual void setAudioDeviceOptions(int *options,int length);

	virtual void resized(int width,int height)		{if(mApplet!=NULL){mApplet->resized(width,height);}}
	virtual void focusGained()						{if(mApplet!=NULL){mApplet->focusGained();}}
	virtual void focusLost()						{if(mApplet!=NULL){mApplet->focusLost();}}
	virtual void update(int dt)						{if(mApplet!=NULL){mApplet->update(dt);}}
	virtual void render(RenderDevice *renderDevice)	{if(mApplet!=NULL){mApplet->render(renderDevice);}}

	virtual void keyPressed(int key)				{if(mApplet!=NULL){mApplet->keyPressed(key);}}
	virtual void keyReleased(int key)				{if(mApplet!=NULL){mApplet->keyReleased(key);}}

	virtual void mousePressed(int x,int y,int button){if(mApplet!=NULL){mApplet->mousePressed(x,y,button);}}
	virtual void mouseMoved(int x,int y)			{if(mApplet!=NULL){mApplet->mouseMoved(x,y);}}
	virtual void mouseReleased(int x,int y,int button){if(mApplet!=NULL){mApplet->mouseReleased(x,y,button);}}
	virtual void mouseScrolled(int x,int y,int scroll){if(mApplet!=NULL){mApplet->mouseScrolled(x,y,scroll);}}

	virtual Engine *getEngine() const{return mEngine;}
	virtual RenderDevice *getRenderDevice() const{return mRenderDevice;}
	virtual AudioDevice *getAudioDevice() const{return mAudioDevice;}
	virtual MotionDevice *getMotionDevice() const{return mMotionDevice;}
	virtual JoyDevice *getJoyDevice() const{return mJoyDevice;}

	virtual String getKeyName(int key){Map<int,String>::iterator it=mKeyToName.find(key);return it!=mKeyToName.end()?it->second:(char*)NULL;}
	virtual int getKeyValue(const String &name){Map<String,int>::iterator it=mNameToKey.find(name);return it!=mNameToKey.end()?it->second:0;}

	virtual void setBackable(bool backable){
		if(mEngine!=NULL){
			Error::unknown(Categories::TOADLET_PAD,"can not change backable once engine is created");
			return;
		}

		mBackable=backable;
	}
	virtual bool getBackable() const{return mBackable;}

	virtual void *getWindowHandle()=0;

protected:
	class RenderDevicePlugin{
	public:
		RenderDevicePlugin(
			RenderTarget *(*renderTarget)(void *,WindowRenderTargetFormat *)=NULL,
			RenderDevice *(*renderDevice)()=NULL
		):createRenderTarget(renderTarget),createRenderDevice(renderDevice){}

		RenderTarget *(*createRenderTarget)(void *,WindowRenderTargetFormat *);
		RenderDevice *(*createRenderDevice)();
	};

	class AudioDevicePlugin{
	public:
		AudioDevicePlugin(
			AudioDevice *(*audioDevice)()=NULL
		):createAudioDevice(audioDevice){}

		AudioDevice *(*createAudioDevice)();
	};

	class MotionDevicePlugin{
	public:
		MotionDevicePlugin(
			MotionDevice *(*motionDevice)()=NULL
		):createMotionDevice(motionDevice){}

		MotionDevice *(*createMotionDevice)();
	};

	class JoyDevicePlugin{
	public:
		JoyDevicePlugin(
			JoyDevice *(*joyDevice)()=NULL
		):createJoyDevice(joyDevice){}

		JoyDevice *(*createJoyDevice)();
	};

	virtual RenderTarget *makeRenderTarget(const String &plugin);
	virtual RenderDevice *makeRenderDevice(const String &plugin);
	virtual bool createContextAndRenderDevice(const String &plugin);
	virtual bool destroyRenderDeviceAndContext();

	virtual AudioDevice *makeAudioDevice(const String &plugin);
	virtual bool createAudioDevice(const String &plugin);
	virtual bool destroyAudioDevice();

	virtual MotionDevice *makeMotionDevice(const String &plugin);
	virtual bool createMotionDevice(const String &plugin);
	virtual bool destroyMotionDevice();

	virtual JoyDevice *makeJoyDevice(const String &plugin);
	virtual bool createJoyDevice(const String &plugin);
	virtual bool destroyJoyDevice();

	bool mBackable;
	WindowRenderTargetFormat::ptr mFormat;
	Applet *mApplet;

	Map<String,RenderDevicePlugin> mRenderDevicePlugins;
	Collection<String> mRenderDevicePreferences;
	String mCurrentRenderDevicePlugin;
	String mNewRenderDevicePlugin;
	int *mRenderDeviceOptions;
	Map<String,AudioDevicePlugin> mAudioDevicePlugins;
	Collection<String> mAudioDevicePreferences;
	int *mAudioDeviceOptions;
	Map<String,MotionDevicePlugin> mMotionDevicePlugins;
	Collection<String> mMotionDevicePreferences;
	Map<String,JoyDevicePlugin> mJoyDevicePlugins;
	Collection<String> mJoyDevicePreferences;

	Map<String,int> mNameToKey;
	Map<int,String> mKeyToName;

	Engine *mEngine;
	RenderTarget *mRenderTarget;
	RenderDevice *mRenderDevice;
	AudioDevice *mAudioDevice;
	MotionDevice *mMotionDevice;
	JoyDevice *mJoyDevice;
};

}
}

#endif
