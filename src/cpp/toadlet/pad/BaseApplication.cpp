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
#include <string.h> // memcpy

using namespace toadlet;
using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::peeper;
using namespace toadlet::ribbit;
using namespace toadlet::flick;
using namespace toadlet::tadpole;

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
	mListener(NULL),

	//mRendererPlugins,
	//mCurrentRendererPlugin,
	//mNewRendererPlugin,
	mRendererOptions(0),
	//mAudioPlayerPlugins,
	mAudioPlayerOptions(0),
	//mMotionDetctorPlugins,

	mEngine(NULL),
	mRenderTarget(NULL),
	mRenderer(NULL),
	mAudioPlayer(NULL),
	mMotionDetector(NULL)
{
	mapKeyNames(mKeyToName,mNameToKey);
	
	mFormat=WindowRenderTargetFormat::ptr(new WindowRenderTargetFormat());
	mFormat->pixelFormat=Texture::Format_RGBA_8;
	mFormat->depthBits=16;
	mFormat->multisamples=2;
	mFormat->threads=2;
	#if defined(TOADLET_DEBUG)
		mFormat->debug=true;
	#else
		mFormat->debug=false;
	#endif
}

void BaseApplication::create(String renderer,String audioPlayer,String motionDetector){
	int i;

	mEngine=new Engine(mBackable);

	/// @todo: Unify the plugin framework a bit so we dont have as much code duplication for this potion, and the creating of the plugin
	mNewRendererPlugin=mCurrentRendererPlugin=renderer;
	if(renderer!="null"){
		if(renderer!=(char*)NULL || mRendererPlugins.size()==0){
			createContextAndRenderer(renderer);
		}
		else{
			for(i=0;i<mRendererPreferences.size();++i){
				if(createContextAndRenderer(mRendererPreferences[i])) break;
			}
		}
	}
	if(audioPlayer!="null"){
		if(audioPlayer!=(char*)NULL || mAudioPlayerPlugins.size()==0){
			createAudioPlayer(audioPlayer);
		}
		else{
			for(i=0;i<mAudioPlayerPreferences.size();++i){
				if(createAudioPlayer(mAudioPlayerPreferences[i])) break;
			}
		}
	}
	if(motionDetector!="null"){
		if(motionDetector!=(char*)NULL || mMotionDetectorPlugins.size()==0){
			createMotionDetector(motionDetector);
		}
		else{
			for(i=0;i<mMotionDetectorPreferences.size();++i){
				if(createMotionDetector(mMotionDetectorPreferences[i])) break;
			}
		}
	}

	activate();
}

void BaseApplication::destroy(){
	if(mEngine!=NULL){
		mEngine->destroy();
	}

	deactivate();

	destroyRendererAndContext();
	destroyAudioPlayer();
	destroyMotionDetector();

	if(mEngine!=NULL){
		delete mEngine;
		mEngine=NULL;
	}
}

void BaseApplication::setRendererOptions(int *options,int length){
	if(mRendererOptions!=NULL){
		delete[] mRendererOptions;
	}

	mRendererOptions=new int[length];
	memcpy(mRendererOptions,options,length*sizeof(int));
}

void BaseApplication::setAudioPlayerOptions(int *options,int length){
	if(mAudioPlayerOptions!=NULL){
		delete[] mAudioPlayerOptions;
	}

	mAudioPlayerOptions=new int[length];
	memcpy(mAudioPlayerOptions,options,length*sizeof(int));
}

RenderTarget *BaseApplication::makeRenderTarget(const String &plugin){
	RenderTarget *target=NULL;

	Map<String,RendererPlugin>::iterator it=mRendererPlugins.find(plugin);
	if(it!=mRendererPlugins.end()){
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

Renderer *BaseApplication::makeRenderer(const String &plugin){
	Renderer *renderer=NULL;
	Map<String,RendererPlugin>::iterator it=mRendererPlugins.find(plugin);
	if(it!=mRendererPlugins.end()){
		TOADLET_TRY
			renderer=it->second.createRenderer();
		TOADLET_CATCH(const Exception &){renderer=NULL;}
	}
	return renderer;
}

bool BaseApplication::createContextAndRenderer(const String &plugin){
	Logger::debug(Categories::TOADLET_PAD,
		"BaseApplication: creating RenderTarget and Renderer:"+plugin);

	bool result=false;
	mRenderTarget=makeRenderTarget(plugin);
	if(mRenderTarget!=NULL){
		mRenderer=makeRenderer(plugin);
		TOADLET_TRY
			result=mRenderer->create(this,mRendererOptions);
		TOADLET_CATCH(const Exception &){result=false;}
		if(result==false){
			delete mRenderer;
			mRenderer=NULL;
		}
	}
	if(result==false){
		delete mRenderTarget;
		mRenderTarget=NULL;
	}

	if(result==false){
		Logger::error(Categories::TOADLET_PAD,
			"error starting Renderer");
		return false;
	}
	else if(mRenderer==NULL){
		Logger::error(Categories::TOADLET_PAD,
			"error creating Renderer");
		return false;
	}

	if(mRenderer!=NULL){
		mRenderer->setRenderTarget(this);
		mEngine->setRenderer(mRenderer);
	}

	return mRenderer!=NULL;
}

bool BaseApplication::destroyRendererAndContext(){
	Logger::debug(Categories::TOADLET_PAD,
		"BaseApplication: destroying context and renderer");

	if(mRenderer!=NULL){
		mEngine->setRenderer(NULL);
		mRenderer->destroy();
		delete mRenderer;
		mRenderer=NULL;
	}

	if(mRenderTarget!=NULL){
		delete mRenderTarget;
		mRenderTarget=NULL;
	}

	return true;
}

AudioPlayer *BaseApplication::makeAudioPlayer(const String &plugin){
	AudioPlayer *audioPlayer=NULL;
	Map<String,AudioPlayerPlugin>::iterator it=mAudioPlayerPlugins.find(plugin);
	if(it!=mAudioPlayerPlugins.end()){
		TOADLET_TRY
			audioPlayer=it->second.createAudioPlayer();
		TOADLET_CATCH(const Exception &){audioPlayer=NULL;}
	}
	return audioPlayer;
}

bool BaseApplication::createAudioPlayer(const String &plugin){
	Logger::debug(Categories::TOADLET_PAD,
		"BaseApplication: creating AudioPlayer:"+plugin);

	bool result=false;
	mAudioPlayer=makeAudioPlayer(plugin);
	if(mAudioPlayer!=NULL){
		TOADLET_TRY
			result=mAudioPlayer->create(mAudioPlayerOptions);
		TOADLET_CATCH(const Exception &){result=false;}
		if(result==false){
			delete mAudioPlayer;
			mAudioPlayer=NULL;
		}
	}

	if(result==false){
		Logger::error(Categories::TOADLET_PAD,
			"error starting AudioPlayer");
		return false;
	}
	else if(mAudioPlayer==NULL){
		Logger::error(Categories::TOADLET_PAD,
			"error creating AudioPlayer");
		return false;
	}

	if(mAudioPlayer!=NULL){
		mEngine->setAudioPlayer(mAudioPlayer);
	}
	return true;
}

bool BaseApplication::destroyAudioPlayer(){
	if(mAudioPlayer!=NULL){
		mEngine->setAudioPlayer(NULL);
		mAudioPlayer->destroy();
		delete mAudioPlayer;
		mAudioPlayer=NULL;
	}
	return true;
}

MotionDetector *BaseApplication::makeMotionDetector(const String &plugin){
	MotionDetector *motionDetector=NULL;
	Map<String,MotionDetectorPlugin>::iterator it=mMotionDetectorPlugins.find(plugin);
	if(it!=mMotionDetectorPlugins.end()){
		TOADLET_TRY
			motionDetector=it->second.createMotionDetector();
		TOADLET_CATCH(const Exception &){motionDetector=NULL;}
	}
	return motionDetector;
}

bool BaseApplication::createMotionDetector(const String &plugin){
	Logger::debug(Categories::TOADLET_PAD,
		"BaseApplication: creating MotionDetector:"+plugin);

	bool result=false;
	mMotionDetector=makeMotionDetector(plugin);
	if(mMotionDetector!=NULL){
		TOADLET_TRY
			result=mMotionDetector->create();
		TOADLET_CATCH(const Exception &){result=false;}
		if(result==false){
			delete mMotionDetector;
			mMotionDetector=NULL;
		}
	}

	if(result==false){
		Logger::error(Categories::TOADLET_PAD,
			"error starting MotionDetector");
		return false;
	}
	else if(mMotionDetector==NULL){
		Logger::error(Categories::TOADLET_PAD,
			"error creating MotionDetector");
		return false;
	}
	return true;
}

bool BaseApplication::destroyMotionDetector(){
	if(mMotionDetector!=NULL){
		mMotionDetector->destroy();
		delete mMotionDetector;
		mMotionDetector=NULL;
	}
	return true;
}
	
}
}
