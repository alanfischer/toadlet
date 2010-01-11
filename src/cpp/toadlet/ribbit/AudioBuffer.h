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

#include <toadlet/ribbit/Types.h>
#include <toadlet/egg/io/Stream.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/Resource.h>

namespace toadlet{
namespace ribbit{

class AudioBuffer:public egg::Resource{
public:
	TOADLET_SHARED_POINTERS(AudioBuffer);

	virtual ~AudioBuffer(){}

	virtual AudioBuffer *getRootAudioBuffer()=0;

	virtual bool create(egg::io::Stream::ptr stream,const egg::String &mimeType)=0;
	virtual void destroy()=0;
};

}
}

#endif

