#include "FFmpegStreamContext.h"

using namespace toadlet::egg;
using namespace toadlet::egg::io;

int toadlet_read_packet(void *opaque,uint8_t *buf,int buf_size){
	Stream *stream=(Stream*)opaque;
	return stream->read(buf,buf_size);
}

int toadlet_write_packet(void *opaque,uint8_t *buf,int buf_size){
	Stream *stream=(Stream*)opaque;
	return stream->write(buf,buf_size);
}

int64_t toadlet_seek(void *opaque,int64_t offset,int whence){
	Stream *stream=(Stream*)opaque;

	bool result=false;
	if(whence==SEEK_SET){
		result=stream->seek(offset);
	}
	else if(whence==SEEK_CUR){
		int position=stream->position();
		result=stream->seek(position+offset);
	}
	else if(whence==SEEK_END){
		int length=stream->length();
		result=stream->seek(length+offset);
	}
	else if(whence==AVSEEK_SIZE){
		return stream->length()-stream->position();
	}

	return result?0:-1;
}
