#ifndef TOADLET_TADPOLE_HANDLER_FFMPEGVIDEOHANDLER_H
#define TOADLET_TADPOLE_HANDLER_FFMPEGVIDEOHANDLER_H

#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/WaitCondition.h>
#include <toadlet/peeper/Texture.h>
#include <toadlet/ribbit/Audio.h>
#include <toadlet/tadpole/VideoHandler.h>
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

class FFmpegAudioStream;
class FFmpegVideoStream;

class FFmpegController:protected Object,public VideoController{
public:
	TOADLET_OBJECT(FFmpegController);

	class PacketQueue{
	public:
		TOADLET_SPTR(PacketQueue);

		enum{
			QueueResult_ERROR=-1,
			QueueResult_EMPTY=0,
			QueueResult_AVAILABLE=1,
			QueueResult_FLUSH=2,
		};

		PacketQueue():
		  first(NULL),last(NULL),
		  count(0),
		  size(0),
		  quit(false),
		  didFlush(false){}

		int put(AVPacket *packet);
		int get(AVPacket *packet,int block);
		int flush();
		bool flushed(){return didFlush;}

		AVPacketList *first,*last;
		int count;
		int size;
		bool quit;
		bool didFlush;
		Mutex mutex;
		WaitCondition cond;
	};

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

	FFmpegController(Engine *engine);
	virtual ~FFmpegController();

	bool open(Stream::ptr stream);
	void destroy();

	TextureFormat::ptr getVideoFormat(){return mVideoFormat;}
	void setTexture(Texture::ptr texture);
	void setAudio(Audio::ptr audio);

	void start();
	void pause();
	bool paused(){return mState==State_STOP || mState==State_PAUSE;}
	void stop();
	void update(int dt);

	int64 currentTime(){return avtimeToMilli(mTime);}
	int64 maxTime(){return avtimeToMilli(mMaxTime);}
	bool seek(int64 time);

	Resource::ptr getResource(){return mTexture;}

	AVFormatContext *getFormatCtx(){return mFormatCtx;}
	StreamData *getStreamData(int type){return &mStreams[type];}
	int64 rawTime(){return mTime;}

	inline uint64 milliToAVTime(uint64 t){return t*(AV_TIME_BASE/1000);}
	inline uint64 avtimeToMilli(uint64 t){return t*1000/AV_TIME_BASE;}

protected:
	enum State{
		State_STOP,
		State_PLAY,
		State_PAUSE,
	};

	void updateDecode(int dt);
	void updateVideo(int dt);

	Texture::ptr mTexture;
	SharedPointer<FFmpegVideoStream> mVideoStream;
	Audio::ptr mAudio;
	SharedPointer<FFmpegAudioStream> mAudioStream;

	Engine *mEngine;
	Stream::ptr mStream;
	ByteIOContext *mIOCtx;
	tbyte *mIOBuffer;
    AVFormatContext *mFormatCtx;
	StreamData mStreams[AVMEDIA_TYPE_NB];
	TextureFormat::ptr mVideoFormat;

	State mState;

	uint64 mTime,mMaxTime;
};

class FFmpegAudioStream:public AudioStream{
public:
	TOADLET_SPTR(FFmpegAudioStream);

	FFmpegAudioStream(FFmpegController *controller,FFmpegController::StreamData *streamData);
	virtual ~FFmpegAudioStream();

	void close();
	bool closed(){return mStreamData!=NULL;}

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
	AVPacket mPkt;
	FFmpegController *mController;
	FFmpegController::StreamData *mStreamData;
	AudioFormat::ptr mAudioFormat;
	tbyte *mDecodeBuffer;
	int mDecodeLength,mDecodeOffset;

	friend class FFmpegController;
};

class FFmpegVideoStream{
public:
	TOADLET_SPTR(FFmpegVideoStream);

	FFmpegVideoStream(FFmpegController *controller,FFmpegController::StreamData *streamData,Texture::ptr texture);
	virtual ~FFmpegVideoStream();

	void close();

	void update(int dt);

protected:
	AVPacket mPkt;
	FFmpegController *mController;
	FFmpegController::StreamData *mStreamData;
	Texture::ptr mTexture;

	SwsContext *mSwsCtx;
	AVFrame *mVideoFrame,*mDeinterlacedFrame,*mTextureFrame;

	uint64 mPtsTime;

	friend class FFmpegController;
};

class FFmpegVideoHandler:protected Object,public VideoHandler{
public:
	TOADLET_OBJECT(FFmpegVideoHandler);

	FFmpegVideoHandler(Engine *engine);
	virtual ~FFmpegVideoHandler();

	void destroy();

	VideoController::ptr open(Stream::ptr stream);

	static PixelFormat getPixelFormat(int textureFormat);

protected:
	Engine *mEngine;
};

}
}
}

#endif
