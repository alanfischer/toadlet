#ifndef TOADLET_TADPOLE_HANDLER_FFMPEGSTREAMCONTEXT_H
#define TOADLET_TADPOLE_HANDLER_FFMPEGSTREAMCONTEXT_H

#include <toadlet/egg/io/Stream.h>

#ifndef INT64_C
#define INT64_C(c) TOADLET_MAKE_INT64(c)
#define UINT64_C(c) TOADLET_MAKE_UINT64(c)
#endif

extern "C"{
	#include <libavformat/avformat.h>
}

int toadlet_ffmpeg_stream_context(ByteIOContext *context,toadlet::egg::io::Stream *stream,unsigned char *buffer,int size);

#endif
