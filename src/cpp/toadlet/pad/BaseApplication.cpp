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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/System.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/pad/BaseApplication.h>
#include <ctype.h> // isalpha

namespace toadlet{
namespace pad{

void BaseApplication::mapKeyNames(Map<int,String> &keyToName,Map<String,int> &nameToKey){
	Map<int,String> &map=keyToName;

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
			map.add(key,String()+(char)key);
		}
	}

	Map<int,String>::iterator it;
	for(it=keyToName.begin();it!=keyToName.end();++it){
		nameToKey.add(it->second,it->first);
	}
}

BaseApplication::BaseApplication():
	mBackable(false),
	//mFormat,
	mApplet(NULL),

	//mRenderDevicePlugins,
	//mCurrentRenderDevicePlugin,
	//mNewRenderDevicePlugin,
	mRenderDeviceOptions(0),
	//mAudioDevicePlugins,
	mAudioDeviceOptions(0),
	//mMotionDetctorPlugins,

	mEngine(NULL),
	mRenderTarget(NULL),
	mRenderDevice(NULL),
	mAudioDevice(NULL),
	mMotionDevice(NULL)
{
	mapKeyNames(mKeyToName,mNameToKey);
	
	mFormat=WindowRenderTargetFormat::ptr(new WindowRenderTargetFormat());
	mFormat->pixelFormat=TextureFormat::Format_RGBA_8;
	mFormat->depthBits=16;
	mFormat->multisamples=2;
	mFormat->threads=2;
	#if defined(TOADLET_DEBUG)
		mFormat->debug=true;
	#else
		mFormat->debug=false;
	#endif
}

void BaseApplication::create(String renderDevice,String audioDevice,String motionDevice){
	int i;

	mEngine=new Engine(mBackable);

	/// @todo: Unify the plugin framework a bit so we dont have as much code duplication for this potion, and the creating of the plugin
	mNewRenderDevicePlugin=mCurrentRenderDevicePlugin=renderDevice;
	if(renderDevice!="null"){
		if(renderDevice!=(char*)NULL || mRenderDevicePlugins.size()==0){
			createContextAndRenderDevice(renderDevice);
		}
		else{
			for(i=0;i<mRenderDevicePreferences.size();++i){
				if(createContextAndRenderDevice(mRenderDevicePreferences[i])) break;
			}
		}
	}
	if(audioDevice!="null"){
		if(audioDevice!=(char*)NULL || mAudioDevicePlugins.size()==0){
			createAudioDevice(audioDevice);
		}
		else{
			for(i=0;i<mAudioDevicePreferences.size();++i){
				if(createAudioDevice(mAudioDevicePreferences[i])) break;
			}
		}
	}
	if(motionDevice!="null"){
		if(motionDevice!=(char*)NULL || mMotionDevicePlugins.size()==0){
			createMotionDevice(motionDevice);
		}
		else{
			for(i=0;i<mMotionDevicePreferences.size();++i){
				if(createMotionDevice(mMotionDevicePreferences[i])) break;
			}
		}
	}

	mEngine->installHandlers();

	if(mApplet!=NULL){
		mApplet->create();
	}

	activate();
}

void BaseApplication::destroy(){
	if(mEngine!=NULL){
		mEngine->destroy();
	}

	deactivate();

	if(mApplet!=NULL){
		mApplet->destroy();
		mApplet=NULL;
	}

	destroyRenderDeviceAndContext();
	destroyAudioDevice();
	destroyMotionDevice();

	if(mEngine!=NULL){
		delete mEngine;
		mEngine=NULL;
	}
}

void BaseApplication::setRenderDeviceOptions(int *options,int length){
	if(mRenderDeviceOptions!=NULL){
		delete[] mRenderDeviceOptions;
	}

	mRenderDeviceOptions=new int[length];
	memcpy(mRenderDeviceOptions,options,length*sizeof(int));
}

void BaseApplication::setAudioDeviceOptions(int *options,int length){
	if(mAudioDeviceOptions!=NULL){
		delete[] mAudioDeviceOptions;
	}

	mAudioDeviceOptions=new int[length];
	memcpy(mAudioDeviceOptions,options,length*sizeof(int));
}

RenderTarget *BaseApplication::makeRenderTarget(const String &plugin){
	RenderTarget *target=NULL;

	Map<String,RenderDevicePlugin>::iterator it=mRenderDevicePlugins.find(plugin);
	if(it!=mRenderDevicePlugins.end()){
		TOADLET_TRY
			target=it->second.createRenderTarget(getWindowHandle(),mFormat);
		TOADLET_CATCH(const Exception &){target=NULL;}
	}
	if(target!=NULL && target->isValid()==false){
		delete target;
		target=NULL;
	}
	return target;
}

RenderDevice *BaseApplication::makeRenderDevice(const String &plugin){
	RenderDevice *renderDevice=NULL;
	Map<String,RenderDevicePlugin>::iterator it=mRenderDevicePlugins.find(plugin);
	if(it!=mRenderDevicePlugins.end()){
		TOADLET_TRY
			renderDevice=it->second.createRenderDevice();
		TOADLET_CATCH(const Exception &){renderDevice=NULL;}
	}
	return renderDevice;
}

bool BaseApplication::createContextAndRenderDevice(const String &plugin){
	Logger::debug(Categories::TOADLET_PAD,
		"BaseApplication: creating RenderTarget and RenderDevice:"+plugin);

	bool result=false;
	mRenderTarget=makeRenderTarget(plugin);
	if(mRenderTarget!=NULL){
		mRenderDevice=makeRenderDevice(plugin);
		TOADLET_TRY
			result=mRenderDevice->create(this,mRenderDeviceOptions);
		TOADLET_CATCH(const Exception &){result=false;}
		if(result==false){
			delete mRenderDevice;
			mRenderDevice=NULL;
		}
	}
	if(result==false){
		mRenderTarget->destroy();
		delete mRenderTarget;
		mRenderTarget=NULL;

		Logger::error(Categories::TOADLET_PAD,
			"error starting RenderDevice");
		return false;
	}
	else if(mRenderDevice==NULL){
		Logger::error(Categories::TOADLET_PAD,
			"error creating RenderDevice");
		return false;
	}

	if(mRenderDevice!=NULL){
		mRenderDevice->setRenderTarget(this);
		mEngine->setRenderDevice(mRenderDevice);
	}

	return mRenderDevice!=NULL;
}

bool BaseApplication::destroyRenderDeviceAndContext(){
	Logger::debug(Categories::TOADLET_PAD,
		"BaseApplication: destroying context and renderDevice");

	if(mRenderDevice!=NULL){
		mEngine->setRenderDevice(NULL);
		mRenderDevice->destroy();
		delete mRenderDevice;
		mRenderDevice=NULL;
	}

	if(mRenderTarget!=NULL){
		mRenderTarget->destroy();
		delete mRenderTarget;
		mRenderTarget=NULL;
	}

	return true;
}

AudioDevice *BaseApplication::makeAudioDevice(const String &plugin){
	AudioDevice *audioDevice=NULL;
	Map<String,AudioDevicePlugin>::iterator it=mAudioDevicePlugins.find(plugin);
	if(it!=mAudioDevicePlugins.end()){
		TOADLET_TRY
			audioDevice=it->second.createAudioDevice();
		TOADLET_CATCH(const Exception &){audioDevice=NULL;}
	}
	return audioDevice;
}

bool BaseApplication::createAudioDevice(const String &plugin){
	Logger::debug(Categories::TOADLET_PAD,
		"BaseApplication: creating AudioDevice:"+plugin);

	bool result=false;
	mAudioDevice=makeAudioDevice(plugin);
	if(mAudioDevice!=NULL){
		TOADLET_TRY
			result=mAudioDevice->create(mAudioDeviceOptions);
		TOADLET_CATCH(const Exception &){result=false;}
		if(result==false){
			delete mAudioDevice;
			mAudioDevice=NULL;
		}
	}

	if(result==false){
		Logger::error(Categories::TOADLET_PAD,
			"error starting AudioDevice");
		return false;
	}
	else if(mAudioDevice==NULL){
		Logger::error(Categories::TOADLET_PAD,
			"error creating AudioDevice");
		return false;
	}

	if(mAudioDevice!=NULL){
		mEngine->setAudioDevice(mAudioDevice);
	}
	return true;
}

bool BaseApplication::destroyAudioDevice(){
	if(mAudioDevice!=NULL){
		mEngine->setAudioDevice(NULL);
		mAudioDevice->destroy();
		delete mAudioDevice;
		mAudioDevice=NULL;
	}
	return true;
}

MotionDevice *BaseApplication::makeMotionDevice(const String &plugin){
	MotionDevice *motionDevice=NULL;
	Map<String,MotionDevicePlugin>::iterator it=mMotionDevicePlugins.find(plugin);
	if(it!=mMotionDevicePlugins.end()){
		TOADLET_TRY
			motionDevice=it->second.createMotionDevice();
		TOADLET_CATCH(const Exception &){motionDevice=NULL;}
	}
	return motionDevice;
}

bool BaseApplication::createMotionDevice(const String &plugin){
	Logger::debug(Categories::TOADLET_PAD,
		"BaseApplication: creating MotionDevice:"+plugin);

	bool result=false;
	mMotionDevice=makeMotionDevice(plugin);
	if(mMotionDevice!=NULL){
		TOADLET_TRY
			result=mMotionDevice->create();
		TOADLET_CATCH(const Exception &){result=false;}
		if(result==false){
			delete mMotionDevice;
			mMotionDevice=NULL;
		}
	}

	if(result==false){
		Logger::error(Categories::TOADLET_PAD,
			"error starting MotionDevice");
		return false;
	}
	else if(mMotionDevice==NULL){
		Logger::error(Categories::TOADLET_PAD,
			"error creating MotionDevice");
		return false;
	}
	return true;
}

bool BaseApplication::destroyMotionDevice(){
	if(mMotionDevice!=NULL){
		mMotionDevice->destroy();
		delete mMotionDevice;
		mMotionDevice=NULL;
	}
	return true;
}
	
}
}
