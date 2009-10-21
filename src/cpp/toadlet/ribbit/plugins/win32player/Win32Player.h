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

#ifndef TOADLET_RIBBIT_WIN32PLAYER_H
#define TOADLET_RIBBIT_WIN32PLAYER_H

#include <toadlet/ribbit/Audio.h>
#include <toadlet/ribbit/AudioPlayer.h>
#include <toadlet/ribbit/AudioBuffer.h>
#include <toadlet/ribbit/AudioStream.h>
#include <toadlet/ribbit/CapabilitySet.h>
#include <toadlet/egg/Collection.h>
#include <windows.h>
#include <mmsystem.h>

namespace toadlet{
namespace ribbit{

class Win32AudioPeer;
class Win32AudioBufferPeer;

class TOADLET_API Win32Player:public AudioPlayer{
public:
	Win32Player();
	virtual ~Win32Player();

	bool startup(int *options);
	bool shutdown();
	bool isStarted() const{return true;}

	AudioBufferPeer *createAudioBufferPeer(AudioBuffer *audioBuffer);
	AudioPeer *createBufferedAudioPeer(Audio *audio,AudioBuffer::ptr buffer);
	AudioPeer *createStreamingAudioPeer(Audio *audio,egg::io::InputStream::ptr in,const egg::String &mimeType);

	void update(int dt){} // milliseconds

	void suspend(){}
	void resume(){}

	void setListenerTranslate(const Vector3 &translate){}
	void setListenerRotate(const Matrix3x3 &rotate){}
	void setListenerVelocity(const Vector3 &velocity){}
	void setListenerGain(scalar gain);
	scalar getListenerGain() const;

	void setDopplerFactor(scalar factor){}
	void setDopplerVelocity(scalar velocity){}

	void setDefaultRolloffFactor(scalar factor){}
	scalar getDefaultRolloffFactor(){return 0;}

	void setGroupGain(const egg::String &group,scalar gain){}
	scalar getGroupGain(const egg::String &group){return 0;}
	void removeGroup(const egg::String &group){}

	const CapabilitySet &getCapabilitySet(){return mCapabilitySet;}

	AudioStream::ptr startAudioStream(egg::io::InputStream::ptr in,const egg::String &mimeType);

	HWAVEOUT getWaveOut();
	bool canPlaySound();
	void playedSound(int time);
	void internal_audioPeerDestroy(Win32AudioPeer *audioPeer);

protected:
	void decodeStream(AudioStream *decoder,char *&finalBuffer,int &finalLength);

	CapabilitySet mCapabilitySet;

	egg::Collection<Win32AudioPeer*> mAudioPeers;
	egg::Collection<Win32AudioBufferPeer*> mAudioBufferPeers;

	HWAVEOUT mWaveOut;
	int64 mNextSoundTime;
};

}
}

#endif
