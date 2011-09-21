#ifndef TOADLET_TADPOLE_HANDLER_FFMPEGTEXTUREMODIFIER_H
#define TOADLET_TADPOLE_HANDLER_FFMPEGTEXTUREMODIFIER_H

#include <toadlet/tadpole/ResourceModifier.h>
#include <toadlet/peeper/Texture.h>

#ifndef INT64_C
#define INT64_C(c) TOADLET_MAKE_INT64(c)
#define UINT64_C(c) TOADLET_MAKE_UINT64(c)
#endif

extern "C"{
	#include <libavutil/avutil.h>
	#include <libavcodec/avcodec.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
}

namespace toadlet{
namespace tadpole{
namespace handler{

/// @todo: Look through the ffmpeg example and clean this up to follow that
class FFmpegTextureController:public ResourceController{
public:
	TOADLET_SHARED_POINTERS(FFmpegTextureController);

	FFmpegTextureController();
	bool open(Stream::ptr stream,Resource::ptr resource);
	void destroy();

	void start();
	void update(int dt);
	void stop(){}

	uint64 currentTime(){return 0;}
	void seek(uint64 time){}

protected:
	class StreamData{
	public:
		StreamData():
		  index(-1),
		  codecCtx(NULL),
		  codec(NULL),
		  first(NULL),last(NULL),
		  count(0),
		  size(0),maxSize(0){}

		int index;
		AVCodecContext *codecCtx;
		AVCodec *codec;
		AVPacketList *first,*last;
		int count;
		int size,maxSize;
	};

	int nextPacket(AVPacket *result,StreamData *stream);
	void queuePackets();
	void decode();

	Texture::ptr mTexture;

	ByteIOContext *mIOCtx;
	tbyte *mIOBuffer;
    AVFormatContext *mFormatCtx;
    SwsContext *mSwsCtx;
	StreamData mStreams[AVMEDIA_TYPE_NB];

	AVFrame *mVideoFrame;
	AVFrame *mTextureFrame;
	tbyte *mTextureBuffer;
	int64 mPtsTime,mTime,mStartTime,mOffsetTime;
	int mFrameCount,mFrameFlush;
};

class FFmpegTextureModifier:public ResourceModifier{
public:
	TOADLET_SHARED_POINTERS(FFmpegTextureModifier);

	FFmpegTextureModifier();
	virtual ~FFmpegTextureModifier();

	void destroy();

	ResourceController::ptr open(Stream::ptr stream,Resource::ptr resource);
};

}
}
}

#endif
