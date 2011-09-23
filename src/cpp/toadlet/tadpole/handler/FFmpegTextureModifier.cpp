#include "FFmpegTextureModifier.h"
#include "FFmpegStreamContext.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/Random.h>

namespace toadlet{
namespace tadpole{
namespace handler{

static int ffmpegLockManager(void **mtx,enum AVLockOp op){
	switch(op){
		case AV_LOCK_CREATE:
			*mtx=new Mutex();
			return !*mtx;
		case AV_LOCK_OBTAIN:
			return !((Mutex*)(*mtx))->lock();
		case AV_LOCK_RELEASE:
			return !((Mutex*)(*mtx))->unlock();
		case AV_LOCK_DESTROY:
			delete (Mutex*)(*mtx);
			return 0;
		default:
			return 1;
	}
}

static int ffmpegGetBuffer(struct AVCodecContext *c,AVFrame *pic){
	int ret=avcodec_default_get_buffer(c,pic);
	uint64_t *pts=(uint64_t*)av_malloc(sizeof(uint64_t));
	*pts=((FFmpegTextureController*)(c->opaque))->video_pkt_pts;
	pic->opaque=pts;
	return ret;
}

static void ffmpegReleaseBuffer(struct AVCodecContext *c,AVFrame *pic){
	if(pic)av_freep(&pic->opaque);
	avcodec_default_release_buffer(c,pic);
}


FFmpegAudioStream::FFmpegAudioStream(AVCodecContext *ctx,FFmpegTextureController *controller):
	mDecodeLength(0)
{
	mCtx=ctx;
	mPkt.data=NULL;
	mController=controller;
	mAudioFormat=AudioFormat::ptr(new AudioFormat(16,ctx->channels,ctx->sample_rate));
}

FFmpegAudioStream::~FFmpegAudioStream(){
	close();
}

void FFmpegAudioStream::close(){
	if(mCtx!=NULL){
		mCtx=NULL;
	}
}

int FFmpegAudioStream::read(tbyte *buffer,int length){
	int total=0,amt=0,result=0;
	while(length>0){
		if(mDecodeLength>0){
			amt=mDecodeLength<length?mDecodeLength:length;
			memcpy(buffer,mDecodeBuffer,amt);
			mDecodeLength-=amt;
			length-=amt;
			buffer+=amt;
			total+=amt;
		}
		else{
			if(mPkt.data==NULL){
				result=mController->getQueue(AVMEDIA_TYPE_AUDIO)->get(&mPkt,0);
				if(result<=0){
					break;
				}
			}
			
			mDecodeLength=AVCODEC_MAX_AUDIO_FRAME_SIZE;
			result=avcodec_decode_audio3(mCtx,(int16_t*)mDecodeBuffer,&mDecodeLength,&mPkt);
			if(result<0){
				break;
			}

			av_free_packet(&mPkt);
		}
	}

	return total;
}


FFmpegTextureController::FFmpegTextureController(Engine *engine):
	mEngine(NULL),
	mIOCtx(NULL),
	mIOBuffer(NULL),
    mFormatCtx(NULL),
    mSwsCtx(NULL),

	mTime(0),
	mState(State_STOP),

	video_pkt_pts(0)
{
	mEngine=engine;
}

uint8_t *FFmpegTextureController::FLUSH=(uint8_t*)"FLUSH";

bool FFmpegTextureController::open(Stream::ptr stream,Resource::ptr resource){
	Logger::alert(Categories::TOADLET_TADPOLE,
		"reading stream for "+resource->getName());

	mTexture=shared_static_cast<Texture>(resource);
	if(mTexture==NULL){
		return false;
	}

	String name=resource->getName();
	TextureFormat::ptr textureFormat=mTexture->getFormat();

	mIOBuffer=new tbyte[4096+FF_INPUT_BUFFER_PADDING_SIZE];
	mIOCtx=new ByteIOContext();
	toadlet_ffmpeg_stream_context(mIOCtx,stream,mIOBuffer,4096);

	int result=av_open_input_stream(&mFormatCtx,mIOCtx,name,av_find_input_format(name),NULL);
	if(result<0){
		Error::unknown("av_open_input_stream failed");
        return false;
    }

    result=av_find_stream_info(mFormatCtx);
	if(result<0){
		Error::unknown("av_find_stream_info failed");
        return false;
    }

    dump_format(mFormatCtx,0,name,0);

	if(mFormatCtx->start_time!=AV_NOPTS_VALUE){
		mStartTime=mFormatCtx->start_time;
	}
	else{
		mStartTime=0;
	}

	int i;
	for(i=0;i<mFormatCtx->nb_streams;++i){
		int type=mFormatCtx->streams[i]->codec->codec_type;
		if(type>=0 && type<AVMEDIA_TYPE_NB && mStreams[type].codec==NULL){
			StreamData stream;
			stream.codecCtx=mFormatCtx->streams[i]->codec;
			stream.codecCtx->opaque=this;
			stream.codecCtx->get_buffer=ffmpegGetBuffer;
			stream.codecCtx->release_buffer=ffmpegReleaseBuffer;

			stream.codec=avcodec_find_decoder(stream.codecCtx->codec_id);
			stream.index=i;
			stream.queue=PacketQueue::ptr(new PacketQueue());

			if(stream.codec==NULL){
				Error::unknown(
					String("unable to find stream codec for type:")+type);
				return false;
			}

			if(avcodec_open(stream.codecCtx,stream.codec)<0){
				Error::unknown(
					String("unable to open stream codec for type:")+type);
				return false;
			}

			mStreams[type]=stream;
		}
	}

	AVCodecContext *videoCodecCtx=mStreams[AVMEDIA_TYPE_VIDEO].codecCtx;
	if(videoCodecCtx==NULL){
//		Error::unknown(
//			"unable to find a video codec");
//		return false;
	}
	else{
		mSwsCtx=sws_getContext(
			videoCodecCtx->width,videoCodecCtx->height,
			videoCodecCtx->pix_fmt,
			textureFormat->width,textureFormat->height,
			FFmpegTextureModifier::getPixelFormat(textureFormat->pixelFormat),
			SWS_FAST_BILINEAR,
			NULL,NULL,NULL
		);

		mVideoFrame=avcodec_alloc_frame();
		mTextureFrame=avcodec_alloc_frame();

		PixelFormat pixelFormat=FFmpegTextureModifier::getPixelFormat(textureFormat->pixelFormat);
		int size=avpicture_get_size(pixelFormat,textureFormat->width,textureFormat->height);
		mTextureBuffer=(tbyte*)av_malloc(size);
		avpicture_fill((AVPicture*)mTextureFrame,mTextureBuffer,pixelFormat,textureFormat->width,textureFormat->height);
	}

	AVCodecContext *audioCodecCtx=mStreams[AVMEDIA_TYPE_AUDIO].codecCtx;
	if(audioCodecCtx==NULL){
//		Error::unknown(
//			"unable to find a audio codec");
//		return false;
	}
	else{
		FFmpegAudioStream::ptr audioStream(new FFmpegAudioStream(audioCodecCtx,this));

		mAudio=Audio::ptr(mEngine->getAudioDevice()->createStreamingAudio());
		mAudio->create(shared_static_cast<AudioStream>(audioStream));
	};

	mPtsTime=0;
	mOffsetTime=av_gettime();

	av_init_packet(&mFlushPkt);
	mFlushPkt.data=FLUSH;

	return true;
}

void FFmpegTextureController::destroy(){
	if(mIOCtx!=NULL){
		delete mIOCtx;
		mIOCtx=NULL;
	}

	if(mIOBuffer!=NULL){
		delete mIOBuffer;
		mIOBuffer=NULL;
	}
}

void FFmpegTextureController::start(){
	mState=State_PLAY;

//	mAudio->play();
}

void FFmpegTextureController::pause(){
	mState=State_PAUSE;
}

void FFmpegTextureController::stop(){
	seek(0);
	mState=State_STOP;

//	mAudio->stop();
}

void FFmpegTextureController::updateDecode(int dt){
	int i;
	AVPacket pkt;
	while(av_read_frame(mFormatCtx,&pkt)>=0){
		StreamData *stream=NULL;
		for(i=0;i<AVMEDIA_TYPE_NB;++i){
			if(mStreams[i].index==pkt.stream_index){
				stream=&mStreams[i];
				break;
			}
		}

		if(stream!=NULL && stream->queue!=NULL){
			stream->queue->put(&pkt);
		}
		else{
			av_free_packet(&pkt);
		}

		if(mStreams[AVMEDIA_TYPE_VIDEO].queue!=NULL && mStreams[AVMEDIA_TYPE_VIDEO].queue->count<4){
			continue;
		}
		if(mStreams[AVMEDIA_TYPE_AUDIO].queue!=NULL && mStreams[AVMEDIA_TYPE_AUDIO].queue->count<4){
			continue;
		}
	}
}

void FFmpegTextureController::updateVideo(int dt){
//	mTime=av_gettime() - mOffsetTime + mStartTime;
	if(mState==State_PLAY){
		mTime+=dt*1000;
	}

	double pts=0.0f;
	int frameFinished=0;
	AVPacket pkt;

	// decode new video frame
	while(mTime>mPtsTime || mPtsTime==0){
		StreamData *stream=&mStreams[AVMEDIA_TYPE_VIDEO];

		if(!stream->queue->get(&pkt,0)){
			break;
		}
		if(pkt.data==mFlushPkt.data){
			avcodec_flush_buffers(stream->codecCtx);
			continue;
		}

		video_pkt_pts=pkt.pts;
		avcodec_decode_video2(stream->codecCtx,mVideoFrame,&frameFinished,&pkt);

		if(pkt.dts!=AV_NOPTS_VALUE){
			pts=pkt.dts;
		}
		else if(pkt.dts==AV_NOPTS_VALUE && mVideoFrame->opaque && *(uint64_t*)mVideoFrame->opaque!=AV_NOPTS_VALUE){
			pts=*(uint64_t*)mVideoFrame->opaque;
		}
		else{
			pts=0;
		}

		pts*=av_q2d(mFormatCtx->streams[stream->index]->time_base);
		int64 ptsTime=pts*AV_TIME_BASE;

		if(frameFinished && (ptsTime>=mTime || ptsTime==0)){
			mPtsTime=ptsTime;

			sws_scale(
				mSwsCtx,
				mVideoFrame->data,
				mVideoFrame->linesize,
				0,
				mVideoFrame->height,
				mTextureFrame->data,
				mTextureFrame->linesize
			);
		}

	    av_free_packet(&pkt);
	}
}

void FFmpegTextureController::update(int dt){
	updateDecode(dt);
	updateVideo(dt);

	StreamData *videoStream=&mStreams[AVMEDIA_TYPE_VIDEO];

	if(videoStream->codecCtx){
		mTexture->load(mTexture->getFormat()->width,mTexture->getFormat()->height,0,0,mTextureFrame->data[0]);
	}
}

int64 FFmpegTextureController::currentTime(){
	return mTime/(AV_TIME_BASE/1000);
}

int64 FFmpegTextureController::maxTime(){
	AVRational avTimeBaseQ={1,AV_TIME_BASE};

	int64 time=0;
	AVStream *stream=mStreams[AVMEDIA_TYPE_VIDEO].index>=0?mFormatCtx->streams[mStreams[AVMEDIA_TYPE_VIDEO].index]:NULL;
	if(stream!=NULL){
		time=av_rescale_q(stream->duration,stream->time_base,avTimeBaseQ);
	}
	return time/(AV_TIME_BASE/1000);
}

bool FFmpegTextureController::seek(int64 time){
	AVRational avTimeBaseQ={1,AV_TIME_BASE};

	if(time<0){
		time=0;
	}
	if(time>maxTime()){
		time=maxTime();
	}

	int flags=AVSEEK_FLAG_BACKWARD;
	int result=0;

	int index=-1,i=0;
	for(i=0;i<AVMEDIA_TYPE_NB;++i){
		if(mStreams[i].index>=0){
			index=mStreams[i].index;
			break;
		}
	}

	time*=(AV_TIME_BASE/1000);

	mPtsTime=0;
	mTime=time;
	mOffsetTime=av_gettime()-time;

	if(index>=0){
		int64 frameTime=av_rescale_q(time,avTimeBaseQ,mFormatCtx->streams[index]->time_base);
		result=av_seek_frame(mFormatCtx,index,frameTime,flags);
		if(result<0){
//			Error::unknown(Categories::TOADLET_TADPOLE,
//				"unable to seek");
//			return false;
		}
	}

	for(i=0;i<AVMEDIA_TYPE_NB;++i){
		if(mStreams[i].queue!=NULL){
			mStreams[i].queue->flush();
			mStreams[i].queue->put(&mFlushPkt);
		}
	}

	return true;
}


FFmpegTextureModifier::FFmpegTextureModifier(Engine *engine){
	mEngine=engine;

	if(av_lockmgr_register(ffmpegLockManager)){
		Error::unknown("Unable to register lock manager");
	}

	av_register_all();
}

FFmpegTextureModifier::~FFmpegTextureModifier(){
	destroy();
}

void FFmpegTextureModifier::destroy(){
	av_lockmgr_register(NULL);
}

ResourceController::ptr FFmpegTextureModifier::open(Stream::ptr stream,Resource::ptr resource){
	FFmpegTextureController::ptr controller(new FFmpegTextureController(mEngine));

	if(controller->open(stream,resource)==false){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"unable to open stream");
		return NULL;
	}

	return controller;
}

PixelFormat FFmpegTextureModifier::getPixelFormat(int textureFormat){
	switch(textureFormat){
		case TextureFormat::Format_L_8:
			return PIX_FMT_GRAY8;
		case TextureFormat::Format_LA_8:
			return PIX_FMT_GRAY8A;
		case TextureFormat::Format_RGB_8:
			return PIX_FMT_RGB24;
		case TextureFormat::Format_BGR_8:
			return PIX_FMT_BGR24;
		case TextureFormat::Format_ARGB_8:
			return PIX_FMT_ARGB;
		case TextureFormat::Format_RGBA_8:
			return PIX_FMT_RGBA;
		case TextureFormat::Format_ABGR_8:
			return PIX_FMT_ABGR;
		case TextureFormat::Format_BGRA_8:
			return PIX_FMT_BGRA;
		case TextureFormat::Format_RGB_5_6_5:
			return PIX_FMT_RGB565BE;
		case TextureFormat::Format_BGR_5_6_5:
			return PIX_FMT_BGR565BE;
		case TextureFormat::Format_RGBA_5_5_5_1:
			return PIX_FMT_RGB565BE;
		case TextureFormat::Format_BGRA_5_5_5_1:
			return PIX_FMT_BGR565BE;
		case TextureFormat::Format_RGBA_4_4_4_4:
			return PIX_FMT_RGB444BE;
		case TextureFormat::Format_BGRA_4_4_4_4:
			return PIX_FMT_BGR444BE;
		default:
			return (PixelFormat)0;
	}
}


int FFmpegTextureController::PacketQueue::put(AVPacket *pkt){
	AVPacketList *pkt1;
	if(pkt->data!=FFmpegTextureController::FLUSH && av_dup_packet(pkt)<0){
		return -1;
	}
	pkt1 = (AVPacketList*)av_malloc(sizeof(AVPacketList));
	if (!pkt1)
		return -1;
	pkt1->pkt = *pkt;
	pkt1->next = NULL;

	mutex.lock();
  
	if (!last)
		first=pkt1;
	else
		last->next = pkt1;
	last=pkt1;
	count++;
	size += pkt1->pkt.size;
	cond.notify();
  
	mutex.unlock();
	return 0;
}

int FFmpegTextureController::PacketQueue::get(AVPacket *pkt,int block){
	AVPacketList *pkt1;
	int ret;
  
	mutex.lock();
  
	for(;;){
		if(quit){
			ret=-1;
			break;
		}

		pkt1=first;
		if (pkt1) {
			first=pkt1->next;
			if (!first)
				last=NULL;
			count--;
			size -= pkt1->pkt.size;
			*pkt = pkt1->pkt;
			av_free(pkt1);
			ret = 1;
			break;
		}
		else if (!block){
			ret = 0;
			break;
		}
		else{
			cond.wait(&mutex);
		}
	}
	mutex.unlock();

	return ret;
}

int FFmpegTextureController::PacketQueue::flush(){
	AVPacketList *pkt,*pkt1;

	mutex.lock();

	for(pkt = first; pkt != NULL; pkt = pkt1) {
		pkt1 = pkt->next;
		av_free_packet(&pkt->pkt);
		av_freep(&pkt);
	}
	last=NULL;
	first=NULL;
	count=0;
	size=0;

	mutex.unlock();

	return 1;
}

}
}
}
