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

#ifndef TOADLET_RIBBIT_AUDIOFORMATCONVERSION_H
#define TOADLET_RIBBIT_AUDIOFORMATCONVERSION_H

#include <toadlet/ribbit/Types.h>
#include <toadlet/ribbit/AudioStream.h>

namespace toadlet{
namespace ribbit{

class TOADLET_API AudioFormatConversion{
public:
	static bool decode(egg::io::Stream *stream,tbyte *&finalBuffer,int &finalLength);
	static bool decode(AudioStream *stream,tbyte *&finalBuffer,int &finalLength);
	static bool convert(tbyte *src,int srcChannels,int srcBitsPerSample,int srcSamplesPerSecond,tbyte *dst,int dstChannels,int dstBitsPerSample,int dstSamplesPerSecond,int length);
	static void fade(tbyte *buffer,int length,int channels,int bitsPerSample,int samplesPerSecond,int fadeTime);
	static int findConvertedLength(int length,int srcChannels,int srcBitsPerSample,int srcSamplesPerSecond,int dstChannels,int dstBitsPerSample,int dstSamplesPerSecond);
};

}
}

#endif
