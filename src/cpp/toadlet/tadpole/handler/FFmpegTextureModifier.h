#ifndef TOADLET_TADPOLE_HANDLER_FFMPEGTEXTUREMODIFIER_H
#define TOADLET_TADPOLE_HANDLER_FFMPEGTEXTUREMODIFIER_H

#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/WaitCondition.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/ribbit/Audio.h>
#include <toadlet/tadpole/ResourceModifier.h>
#include <toadlet/tadpole/Engine.h>

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

class FFmpegTextureController;

class FFmpegAudioStream:public AudioStream{
public:
	TOADLET_SHARED_POINTERS(FFmpegAudioStream);

	FFmpegAudioStream(AVCodecContext *ctx,FFmpegTextureController *controller);
	virtual ~FFmpegAudioStream();

	void close();
	bool closed(){return mCtx!=NULL;}

	virtual bool readable(){return true;}
	virtual int read(tbyte *buffer,int length);

	virtual bool writeable(){return false;}
	virtual int write(const tbyte *buffer,int length){return -1;}

	virtual bool reset(){return false;}
	virtual int length(){return -1;}
	virtual int position(){return 0;}
	virtual bool seek(int offs){return false;}

	AudioFormat::ptr getAudioFormat() const{return mAudioFormat;}

protected:
	AVCodecContext *mCtx;
	AVPacket mPkt;
	FFmpegTextureController *mController;
	AudioFormat::ptr mAudioFormat;
	tbyte mDecodeBuffer[AVCODEC_MAX_AUDIO_FRAME_SIZE];
	int mDecodeLength;
};

class FFmpegTextureController:public ResourceController{
public:
	TOADLET_SHARED_POINTERS(FFmpegTextureController);

	class PacketQueue{
	public:
		TOADLET_SHARED_POINTERS(PacketQueue);

		PacketQueue():
		  first(NULL),last(NULL),
		  count(0),
		  size(0),
		  quit(false){}

		int put(AVPacket *packet);
		int get(AVPacket *packet,int block);
		int flush();

		AVPacketList *first,*last;
		int count;
		int size;
		bool quit;
		Mutex mutex;
		WaitCondition cond;
	};

	/// @todo: Let us be able to open a stream and query its parameters before specifying a texture
	/// @todo: Move the Audio creation out from here, and instead have it be something passed in, if Audio is desired
	FFmpegTextureController(Engine *engine);
	bool open(Stream::ptr stream,Resource::ptr resource);
	void destroy();

	void start();
	void pause();
	bool paused(){return mState==State_PAUSE;}
	void stop();
	void update(int dt);

	int64 currentTime();
	int64 maxTime();
	bool seek(int64 time);

	PacketQueue *getQueue(int type){return mStreams[type].queue;}

	void run();

	uint64 video_pkt_pts;

protected:
	class StreamData{
	public:
		StreamData():
		  index(-1),
		  codecCtx(NULL),
		  codec(NULL){}

		int index;
		AVCodecContext *codecCtx;
		AVCodec *codec;
		PacketQueue::ptr queue;
	};

	enum State{
		State_STOP,
		State_PLAY,
		State_PAUSE,
	};

	void updateDecode(int dt);
	void updateVideo(int dt);

	static uint8_t *FLUSH;

	Texture::ptr mTexture;
	Audio::ptr mAudio;

	Engine *mEngine;
	ByteIOContext *mIOCtx;
	tbyte *mIOBuffer;
    AVFormatContext *mFormatCtx;
    SwsContext *mSwsCtx;
	StreamData mStreams[AVMEDIA_TYPE_NB];
	AVPacket mFlushPkt;

	State mState;

	AVFrame *mVideoFrame;
	AVFrame *mTextureFrame;
	tbyte *mTextureBuffer;
	uint64 mPtsTime,mStartTime,mOffsetTime,mTime;
};

class FFmpegTextureModifier:public ResourceModifier{
public:
	TOADLET_SHARED_POINTERS(FFmpegTextureModifier);

	FFmpegTextureModifier(Engine *engine);
	virtual ~FFmpegTextureModifier();

	void destroy();

	ResourceController::ptr open(Stream::ptr stream,Resource::ptr resource);

	static PixelFormat getPixelFormat(int textureFormat);

protected:
	Engine *mEngine;
};

}
}
}

#endif
