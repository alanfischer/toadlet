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
#include <toadlet/egg/io/InputStream.h>
#include <toadlet/egg/String.h>
#include <toadlet/egg/Resource.h>

namespace toadlet{
namespace ribbit{

class AudioPlayer;

class AudioBufferPeer{
public:
	virtual ~AudioBufferPeer(){}
};

class TOADLET_API AudioBuffer:public egg::Resource{
public:
	TOADLET_SHARED_POINTERS(AudioBuffer,egg::Resource);

	AudioBuffer();
	AudioBuffer(AudioPlayer *player,egg::io::InputStream::ptr in,const egg::String &mimeType);

	virtual ~AudioBuffer();

	void setStream(egg::io::InputStream::ptr in,const egg::String &mimeType);
	egg::io::InputStream::ptr getInputStream() const;
	const egg::String &getMimeType() const;

	virtual void internal_setAudioBufferPeer(AudioBufferPeer *audioBufferPeer,bool owns);
	inline AudioBufferPeer *internal_getAudioBufferPeer() const{return mAudioBufferPeer;}
	inline bool internal_ownsAudioBufferPeer() const{return mOwnsAudioBufferPeer;}

protected:
	egg::io::InputStream::ptr mIn;
	egg::String mMimeType;

	AudioBufferPeer *mAudioBufferPeer;
	bool mOwnsAudioBufferPeer;
};

}
}

#endif

