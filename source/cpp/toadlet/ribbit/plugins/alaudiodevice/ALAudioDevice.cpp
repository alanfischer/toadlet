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

#include "ALAudioDevice.h"
#include "ALAudio.h"
#include "ALAudioBuffer.h"
#include <toadlet/egg/System.h>
#include <toadlet/egg/Extents.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/EndianConversion.h>
#include <stdlib.h>

#if defined(TOADLET_PLATFORM_WIN32)
	#if defined(TOADLET_LIBOPENAL_NAME)
		#pragma comment(lib,TOADLET_LIBOPENAL_NAME)
	#endif
#endif

namespace toadlet{
namespace ribbit{

#if defined(TOADLET_PLATFORM_OSX)
	using toadlet::egg::Collection;
#endif

TOADLET_C_API AudioDevice* new_ALAudioDevice(){
	return new ALAudioDevice();
}

#if defined(TOADLET_BUILD_DYNAMIC)
	TOADLET_C_API AudioDevice* new_AudioDevice(){
		return new ALAudioDevice();
	}
#endif

ALCcontext *ALAudioDevice::mContext=NULL;

ALAudioDevice::ALAudioDevice():
	mDevice(NULL),
	//mContext(NULL),
	//mAudios,
	//mSourcePool,
	//mAllSources,
	mBufferFadeTime(0),

	//mMutex,

	//mCapabilitySet,

	alBufferDataStatic(NULL)
{
	mBufferFadeTime=100;
}

ALAudioDevice::~ALAudioDevice(){
	destroy();
}

bool ALAudioDevice::create(int *options){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"creating ALAudioDevice");

	if(mContext!=NULL){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"only 1 OpenAL context available at a time");
		return false;
	}

	if(options!=NULL){
		int i=0;
		while(options[i]!=0){
			switch(options[i++]){
				case Option_BUFFER_FADE_TIME:
					mBufferFadeTime=options[i++];
					Logger::alert(Categories::TOADLET_PEEPER,
						String("Setting BufferFadeTime to:")+mBufferFadeTime);
					break;
			}
		}
	}

	#if defined(TOADLET_HAS_ALEM)
		if(alem_getInitialized()==false){
			int alemInitResult=alem_init(ALEM_ACCELERATED_IF_AVAILABLE);
			if(alemInitResult<=0){
				Error::unknown(Categories::TOADLET_PEEPER,
					"alem_init failed");
				return false;
			}
		}

		int alemAcceleratedResult=alem_getAccelerated();
		Logger::alert(Categories::TOADLET_RIBBIT,
			String("alem_getAccelerated result:")+alemAcceleratedResult);
	#endif

	// Initialize Open AL manually
	mDevice=alcOpenDevice(NULL);
	TOADLET_CHECK_ALERROR("alcOpenDevice");

	if(mDevice==NULL){
		Error::unknown(Categories::TOADLET_RIBBIT,
			"invalid device");
		return false;
	}

	alBufferDataStatic=(proc_alBufferDataStatic)alcGetProcAddress(mDevice,(ALCchar*)"alBufferDataStatic");
	if(alBufferDataStatic!=NULL){
		Logger::alert(Categories::TOADLET_RIBBIT,
			"using alBufferDataStatic extension");
	}

	mContext=alcCreateContext(mDevice,NULL);
	TOADLET_CHECK_ALERROR("alcCreateContext");

	if(mContext==NULL){
		alcCloseDevice(mDevice);
		mDevice=NULL;
		Error::unknown(Categories::TOADLET_RIBBIT,
			"invalid context");
		return false;
	}

	alcMakeContextCurrent(mContext);
	TOADLET_CHECK_ALERROR("alcMakeContextCurrent");

	/// @todo: Change this to a loop with alGenSources till it returns an error
	mCaps.maxSources=8;
	mCaps.streaming=true;
	mCaps.positional=true;

	alListenerf(AL_GAIN,1.0);
	TOADLET_CHECK_ALERROR("alListenerf");

	alDopplerFactor(1.0);
	TOADLET_CHECK_ALERROR("alDopplerFactor");
	alDopplerVelocity(340.0); /// @todo: Move the doppler parameters to the interface
	TOADLET_CHECK_ALERROR("alDopplerVelocity");

	mAllSources.resize(mCaps.maxSources);
	alGenSources(mAllSources.size(),&mAllSources[0]);
	TOADLET_CHECK_ALERROR("alGenSources");
	mSourcePool=mAllSources;

	int i;
	for(i=0;i<mAllSources.size();++i){
		alSourcei(mAllSources[i],AL_LOOPING,false);
		alSourcef(mAllSources[i],AL_PITCH,1.0);
		alSourcef(mAllSources[i],AL_GAIN,1.0);
		alSourcef(mAllSources[i],AL_ROLLOFF_FACTOR,1.0);
		ALfloat zero[]={0,0,0};
		alSourcefv(mAllSources[i],AL_POSITION,zero);
	}
	TOADLET_CHECK_ALERROR("Initialize sources");

	return true;
}

void ALAudioDevice::destroy(){
	Logger::alert(Categories::TOADLET_RIBBIT,
		"destroying ALAudioDevice");

	alcMakeContextCurrent(mContext);
	TOADLET_CHECK_ALERROR("alcMakeContextCurrent");

	int i;
	for(i=0;i<mAudios.size();++i){
		mAudios[i]->mDevice=NULL;
	}

	if(mAllSources.size()>0){
		alDeleteSources(mAllSources.size(),&mAllSources[0]);
		mAllSources.clear();
	}

	if(mContext!=NULL){
		alcDestroyContext(mContext);
		mContext=NULL;
	}

	if(mDevice!=NULL){
		alcCloseDevice(mDevice);
		mDevice=NULL;
	}
}

AudioBuffer *ALAudioDevice::createAudioBuffer(){
	return new ALAudioBuffer(this);
}

Audio *ALAudioDevice::createAudio(){
	return new ALAudio(this);
}

void deactivate();

void ALAudioDevice::activate(){
	alcMakeContextCurrent(mContext);
	TOADLET_CHECK_ALERROR("alcMakeContextCurrent");

	alcProcessContext(mContext);
	TOADLET_CHECK_ALERROR("alcProcessContext");
}

void ALAudioDevice::deactivate(){
	alcSuspendContext(mContext);
	TOADLET_CHECK_ALERROR("alcSuspendContext");

	alcMakeContextCurrent(NULL);
	TOADLET_CHECK_ALERROR("alcMakeContextCurrent");
}

void ALAudioDevice::update(int dt){
	alcMakeContextCurrent(mContext);
	TOADLET_CHECK_ALERROR("alcMakeContextCurrent");

	int i;
	for(i=0;i<mAudios.size();++i){
		mAudios[i]->update(dt);
	}
}

void ALAudioDevice::setListenerTranslate(const Vector3 &translate){
	MathConversion::scalarToFloat(cacheVector3,translate);

	// Not sure if this belongs here, but we'll just just in case
	// NAN values will lock the computer
	if(Extents::isNAN(cacheVector3.x)){
		return;
	}

	alListenerfv(AL_POSITION,cacheVector3.getData());
}

void ALAudioDevice::setListenerRotate(const Quaternion &rotate){
	Matrix3x3 matrix;
	ALfloat final[6];

	Math::setMatrix3x3FromQuaternion(matrix,rotate);

	final[0]=MathConversion::scalarToFloat(matrix.at(0,1));
	final[1]=MathConversion::scalarToFloat(matrix.at(1,1));
	final[2]=MathConversion::scalarToFloat(matrix.at(2,1));

	final[3]=MathConversion::scalarToFloat(matrix.at(0,2));
	final[4]=MathConversion::scalarToFloat(matrix.at(1,2));
	final[5]=MathConversion::scalarToFloat(matrix.at(2,2));

	// Not sure if this belongs here, but we'll just just in case
	// NAN values will lock the computer
	if(Extents::isNAN(final[0])){
		return;
	}

	alListenerfv(AL_ORIENTATION,final);

	TOADLET_CHECK_ALERROR("setListenerRotate");
}

void ALAudioDevice::setListenerVelocity(const Vector3 &velocity){
	// Doppler effect doesnt work on other platforms...  bad OpenAL?
	#if defined(TOADLET_PLATFORM_WIN32)
		MathConversion::scalarToFloat(cacheVector3,velocity);

		// Not sure if this belongs here, but we'll just just in case
		// NAN values will lock the computer
		if(Extents::isNAN(cacheVector3.x)){
			return;
		}

		alListenerfv(AL_VELOCITY,cacheVector3.getData());
	#endif

	TOADLET_CHECK_ALERROR("setListenerVelocity");
}

void ALAudioDevice::setListenerGain(scalar gain){
	alListenerf(AL_GAIN,MathConversion::scalarToFloat(gain));
	
	TOADLET_CHECK_ALERROR("setListenerGain");
}

ALuint ALAudioDevice::checkoutSourceHandle(ALAudio *audio){
	mAudios.add(audio);

	ALuint handle=-1;
	if(mSourcePool.size()>0){
		handle=mSourcePool[0];
		mSourcePool.removeAt(0);
	}
	return handle;
}

void ALAudioDevice::checkinSourceHandle(ALAudio *audio,ALuint source){
	mSourcePool.add(source);

	mAudios.remove(audio);
}

void ALAudioDevice::preDestroyBuffer(ALAudioBuffer *buffer){
	int i;
	for(i=0;i<mAudios.size();++i){
		if(mAudios[i]->getAudioBuffer()==buffer){
			mAudios[i]->destroy();
		}
	}
}

ALenum ALAudioDevice::getALFormat(AudioFormat *format){
	int bps=format->getBitsPerSample(),channels=format->getChannels();
	if(bps==8 && channels==1){
		return AL_FORMAT_MONO8;
	}
	else if(bps==16 && channels==1){
		return AL_FORMAT_MONO16;
	}
	else if(bps==8 && channels==2){
		return AL_FORMAT_STEREO8;
	}
	else if(bps==16 && channels==2){
		return AL_FORMAT_STEREO16;
	}
	else{
		return 0;
	}
}

}
}

