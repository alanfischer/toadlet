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

#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/ribbit/AudioFormatConversion.h>
#include "GMEDecoder.h"

namespace toadlet{
namespace tadpole{

GMEDecoder::GMEDecoder():
	emu(NULL)
{
	mFormat=AudioFormat::ptr(new AudioFormat(16,2,22050));
}

GMEDecoder::~GMEDecoder(){
	if(emu!=NULL){
		gme_delete(emu);
	}
}

bool GMEDecoder::openStream(Stream *stream){
	tbyte header[4];
	int amount=stream->read(header,4);
	stream->reset();

	const char *extension=gme_identify_header(header);
	if(extension==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,"unable to identify header");
		return false;
	}

	gme_type_t type=gme_identify_extension(extension);
	emu=gme_new_emu(type,mFormat->getSamplesPerSecond());
	if(emu==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,"unable to create emu");
		return false;
	}

	int length=stream->length();
	gme_load_custom(emu,reader,length,stream);

	return true;
}

int GMEDecoder::getNumTracks(){
	return gme_track_count(emu);
}

bool GMEDecoder::startTrack(int track){
	gme_err_t result=gme_start_track(emu,track);
	return result==NULL;
}

int GMEDecoder::read(tbyte *buffer,int length){
	gme_err_t result=gme_play(emu,length/2,(short*)buffer);
	return result==NULL?length:0;
}

gme_err_t GMEDecoder::reader(void *data,void *buffer,int length){
	Stream *stream=(Stream*)data;
	if(stream->read((tbyte*)buffer,length)<length){
		return "out of data";
	}
	return NULL;
}

}
}
