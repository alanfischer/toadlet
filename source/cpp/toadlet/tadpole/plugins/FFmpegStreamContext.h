#ifndef TOADLET_TADPOLE_FFMPEGSTREAMCONTEXT_H
#define TOADLET_TADPOLE_FFMPEGSTREAMCONTEXT_H

#include <toadlet/egg/io/Stream.h>

#ifndef INT64_C
#define INT64_C(c) TOADLET_MAKE_INT64(c)
#define UINT64_C(c) TOADLET_MAKE_UINT64(c)
#endif

extern "C"{
	#include <libavformat/avformat.h>
}

int toadlet_read_packet(void *opaque,uint8_t *buf,int buf_size);
int toadlet_write_packet(void *opaque,uint8_t *buf,int buf_size);
int64_t toadlet_seek(void *opaque,int64_t offset,int whence);

#endif
