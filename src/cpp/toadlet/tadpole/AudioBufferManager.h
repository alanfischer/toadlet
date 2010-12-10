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

#ifndef TOADLET_TADPOLE_AUDIOBUFFERMANAGER_H
#define TOADLET_TADPOLE_AUDIOBUFFERMANAGER_H

#include <toadlet/tadpole/ResourceManager.h>
#include <toadlet/ribbit/AudioPlayer.h>
#include <toadlet/tadpole/Types.h>

namespace toadlet{
namespace tadpole{

class Engine;

class TOADLET_API AudioBufferManager:public ResourceManager{
public:
	AudioBufferManager(Engine *engine);

	ribbit::AudioBuffer::ptr createAudioBuffer(ribbit::AudioStream::ptr stream);

	ribbit::AudioStream::ptr findAudioStream(const egg::String &name);
	ribbit::AudioBuffer::ptr findAudioBuffer(const egg::String &name){return egg::shared_static_cast<ribbit::AudioBuffer>(ResourceManager::find(name));}

	ribbit::AudioPlayer *getAudioPlayer();

protected:
	Engine *mEngine;
};

}
}

#endif
