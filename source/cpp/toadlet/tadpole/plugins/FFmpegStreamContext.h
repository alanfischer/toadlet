#ifndef TOADLET_TADPOLE_FFMPEGSTREAMCONTEXT_H
#define TOADLET_TADPOLE_FFMPEGSTREAMCONTEXT_H

#include <toadlet/egg/io/Stream.h>

extern "C"{
	#define __STDC_CONSTANT_MACROS
	#include <libavformat/avformat.h>
}

int toadlet_read_packet(void *opaque,uint8_t *buf,int buf_size);
int toadlet_write_packet(void *opaque,uint8_t *buf,int buf_size);
int64_t toadlet_seek(void *opaque,int64_t offset,int whence);

#endif
