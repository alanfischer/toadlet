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

#include <toadlet/tadpole/AudioManager.h>
#include <toadlet/tadpole/AudioStreamer.h>
#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{

AudioManager::AudioManager(Engine *engine):ResourceManager(engine){
}

AudioBuffer::ptr AudioManager::createAudioBuffer(AudioStream *stream){
	AudioDevice *audioDevice=getAudioDevice();

	if(audioDevice==NULL){
		Error::nullPointer("createAudioBuffer called without a valid AudioDevice");
		return NULL;
	}
	
	if(stream==NULL){
		Error::nullPointer("createAudioBuffer called with a null stream");
		return NULL;
	}

	AudioBuffer::ptr audioBuffer=audioDevice->createAudioBuffer();
	if(audioBuffer!=NULL && audioBuffer->create(stream)==false){
		audioBuffer=NULL;
	}
	return audioBuffer;
}

Audio::ptr AudioManager::createAudio(){
	AudioDevice *audioDevice=getAudioDevice();

	if(audioDevice==NULL){
		Error::nullPointer("createAudioBuffer called without a valid AudioDevice");
		return NULL;
	}
	
	return audioDevice->createAudio();
}

bool AudioManager::findAudioStream(const String &name,StreamRequest *request,ResourceData *data){
	Log::debug(Categories::TOADLET_TADPOLE,
		"finding audio stream:"+name);

	String filename=checkDefaultExtension(cleanPath(name));
	String extension=findExtension(filename);

	ResourceStreamer::ptr streamer=getStreamer(extension);
	if(streamer==NULL){
		streamer=mDefaultStreamer;
	}
	if(streamer!=NULL){
		return mEngine->getArchiveManager()->openStream(filename,new AudioStreamRequest(request,static_pointer_cast<AudioStreamer>(streamer),data));
	}
	else{
		Error::unknown(Categories::TOADLET_TADPOLE,
			"streamer for \""+name+"\" not found");
		return false;
	}
}

AudioDevice *AudioManager::getAudioDevice(){return mEngine->getAudioDevice();}

AudioManager::AudioStreamRequest::AudioStreamRequest(StreamRequest *request,ResourceStreamer *streamer,ResourceData *data){
	mRequest=request;
	mStreamer=streamer;
	mData=data;
}

void AudioManager::AudioStreamRequest::streamReady(Stream *stream){
	AudioStream::ptr audioStream=static_pointer_cast<AudioStreamer>(mStreamer)->createAudioStream(stream,mData);
	if(audioStream!=NULL){
		mRequest->streamReady(audioStream);
	}
	else{
		mRequest->streamException(Exception(Errorer::Type_UNKNOWN));
	}
}

void AudioManager::AudioStreamRequest::streamException(const Exception &ex){
	mRequest->streamException(ex);
}

}
}

