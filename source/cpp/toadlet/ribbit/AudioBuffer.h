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

#ifndef TOADLET_RIBBIT_AUDIOBUFFER_H
#define TOADLET_RIBBIT_AUDIOBUFFER_H

#include <toadlet/egg/Resource.h>
#include <toadlet/ribbit/Types.h>
#include <toadlet/ribbit/AudioStream.h>

namespace toadlet{
namespace ribbit{

class AudioBuffer:public Resource{
public:
	TOADLET_INTERFACE(AudioBuffer);

	virtual ~AudioBuffer(){}

	virtual AudioBuffer *getRootAudioBuffer()=0;

	virtual bool create(AudioStream *stream)=0;
	virtual void destroy()=0;

	virtual int getLengthTime() const=0;
};

}
}

#endif

