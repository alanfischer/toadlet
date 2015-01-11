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

#ifndef TOADLET_TADPOLE_AUDIOMANAGER_H
#define TOADLET_TADPOLE_AUDIOMANAGER_H

#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/tadpole/AudioData.h>
#include <toadlet/ribbit/AudioDevice.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API AudioManager:public ResourceManager{
public:
	TOADLET_OBJECT(AudioManager);

	AudioManager(Engine *engine);

	AudioBuffer::ptr createAudioBuffer(AudioStream *stream);

	Audio::ptr createAudio();

	AudioBuffer::ptr getAudioBuffer(const String &name){return static_pointer_cast<AudioBuffer>(get(name));}

	bool findAudioBuffer(const String &name,ResourceRequest *request,int track=0){return ResourceManager::find(name,request,new AudioData(track));}
	bool findAudioStream(const String &name,StreamRequest *request,int track=0){return findAudioStream(name,request,new AudioData(track));}
	bool findAudioStream(const String &name,StreamRequest *request,ResourceData *data);

	AudioDevice *getAudioDevice();

	ResourceData *createResourceData(){return new AudioData();}

protected:
	class AudioStreamRequest:public Object,public StreamRequest{
	public:
		TOADLET_IOBJECT(AudioStreamRequest);

		AudioStreamRequest(StreamRequest *request,ResourceStreamer *streamer,ResourceData *data);

		void streamReady(Stream *stream);
		void streamException(const Exception &ex);
		void streamProgress(float progress){}

	protected:
		StreamRequest::ptr mRequest;
		ResourceStreamer::ptr mStreamer;
		ResourceData::ptr mData;
	};
};

}
}

#endif
