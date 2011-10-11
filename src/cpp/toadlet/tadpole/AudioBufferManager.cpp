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

#include <toadlet/tadpole/AudioBufferManager.h>
#include <toadlet/tadpole/handler/AudioBufferHandler.h>
#include <toadlet/tadpole/Engine.h>

namespace toadlet{
namespace tadpole{

AudioBufferManager::AudioBufferManager(Engine *engine):ResourceManager(engine->getArchiveManager()),
	mEngine(NULL)
{
	mEngine=engine;
}

AudioBuffer::ptr AudioBufferManager::createAudioBuffer(AudioStream::ptr stream){
	Logger::debug(Categories::TOADLET_TADPOLE,"AudioBufferManager::createAudioBuffer");

	AudioDevice *audioDevice=getAudioDevice();

	if(audioDevice==NULL){
		Error::nullPointer("createAudioBuffer called without a valid AudioDevice");
		return NULL;
	}
	
	if(stream==NULL){
		Error::nullPointer("createAudioBuffer called with a null stream");
		return NULL;
	}
	
	AudioBuffer::ptr audioBuffer=AudioBuffer::ptr(audioDevice->createAudioBuffer());
	if(audioBuffer!=NULL && audioBuffer->create(stream)==false){
		audioBuffer=NULL;
	}
	return audioBuffer;
}

AudioStream::ptr AudioBufferManager::findAudioStream(const String &name){
	Logger::debug(Categories::TOADLET_TADPOLE,
		"finding audio stream:"+name);

	/// @todo: Unify this with ResourceManager::findFromFile
	String filename=cleanFilename(name);
	String extension;
	int i=filename.rfind('.');
	if(i!=String::npos){
		extension=filename.substr(i+1,filename.length()).toLower();
	}
	else if(mDefaultExtension.length()>0){
		extension=mDefaultExtension;
		filename+="."+extension;
	}

	ResourceStreamer::ptr streamer=getStreamer(extension);
	if(streamer==NULL){
		streamer=mDefaultStreamer;
	}
	if(streamer!=NULL){
		Stream::ptr stream=mArchive->openStream(filename);
		if(stream!=NULL){
			return shared_static_cast<AudioBufferHandler>(streamer)->createAudioStream(stream);
		}
		else{
			Error::unknown(Categories::TOADLET_TADPOLE,
				"file "+filename+" not found");
			return NULL;
		}
	}
	else{
		Error::unknown(Categories::TOADLET_TADPOLE,
			"handler for \""+name+"\" not found");
		return NULL;
	}
}

AudioDevice *AudioBufferManager::getAudioDevice(){return mEngine->getAudioDevice();}

}
}

