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

#ifndef TOADLET_RIBBIT_WIN32AUDIOBUFFER_H
#define TOADLET_RIBBIT_WIN32AUDIOBUFFER_H

#include "Win32Player.h"
#include <toadlet/egg/BaseResource.h>
#include <toadlet/ribbit/AudioBuffer.h>

namespace toadlet{
namespace ribbit{

class TOADLET_API Win32AudioBuffer:protected egg::BaseResource,public AudioBuffer{
	TOADLET_BASERESOURCE_PASSTHROUGH(AudioBuffer);
public:
	Win32AudioBuffer(Win32Player *player);
	virtual ~Win32AudioBuffer();

	AudioBuffer *getRootAudioBuffer(){return this;}

	bool create(egg::io::Stream::ptr stream,const egg::String &mimeType);
	void destroy();

	void setLoopCount(int loopCount);
	int getLoopCount() const;

	inline WAVEFORMATEX *getLPWAVEFORMATEX(){return &mWaveFormat;}
	inline WAVEHDR *getLPWAVEHDR(){return &mWaveHDR;}
	inline int getTime() const{return mTime;}

protected:
	bool prepareHeader();
	void unprepareHeader();

	Win32Player *mAudioPlayer;
	WAVEFORMATEX mWaveFormat;
	WAVEHDR mWaveHDR;
	int mTime;
};

}
}

#endif
