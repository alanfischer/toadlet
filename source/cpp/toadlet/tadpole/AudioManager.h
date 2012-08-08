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

	AudioStream::ptr findAudioStream(const String &name,int track=0){return findAudioStream(name,AudioData::ptr(new AudioData(track)));}
	AudioBuffer::ptr findAudioBuffer(const String &name,int track=0){return shared_static_cast<AudioBuffer>(ResourceManager::find(name,AudioData::ptr(new AudioData(track))));}

	AudioStream::ptr findAudioStream(const String &name,ResourceData *data);

	AudioDevice *getAudioDevice();
};

}
}

#endif
