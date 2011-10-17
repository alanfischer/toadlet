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

static Logger::Level ffmpegGetLogLevel(int level){
	switch(level){
		case AV_LOG_DEBUG:
			return Logger::Level_EXCESS;
		case AV_LOG_VERBOSE:
			return Logger::Level_DEBUG;
		case AV_LOG_INFO:
			return Logger::Level_ALERT;
		case AV_LOG_WARNING:
			return Logger::Level_WARNING;
		case AV_LOG_ERROR:
		case AV_LOG_FATAL:
		case AV_LOG_PANIC:
			return Logger::Level_ERROR;
		default:
			return Logger::Level_ALERT;
	}
}

static void ffmpegLogCallback(void *ptr,int level,const char *fmt,va_list vl){
	char buffer[1024];
	int amt=vsnprintf(buffer,sizeof(buffer),fmt,vl);
	if(amt>0 && buffer[amt-1]=='\n'){
		buffer[amt-1]=0; // Trim off the trailing newline
	}
	Logger::getInstance()->addLogEntry("ffmpeg",ffmpegGetLogLevel(level),buffer);
}


FFmpegAudioStream::FFmpegAudioStream(FFmpegTextureController *controller,FFmpegTextureController::StreamData *streamData):
	mDecodeLength(0)
{
	mController=controller;
	mStreamData=streamData;
	av_init_packet(&mPkt);
	mPkt.data=NULL;

	AVCodecContext *ctx=mStreamData->codecCtx;
	mAudioFormat=AudioFormat::ptr(new AudioFormat(16,ctx->channels,ctx->sample_rate));
}

FFmpegAudioStream::~FFmpegAudioStream(){
	close();
}

void FFmpegAudioStream::close(){
	mStreamData=NULL;
	av_free_packet(&mPkt);
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
				int result=mStreamData->queue->get(&mPkt,0);
				if(result==FFmpegTextureController::PacketQueue::QueueResult_FLUSH){
					avcodec_flush_buffers(mStreamData->codecCtx);
					continue;
				}
				else if(result!=FFmpegTextureController::PacketQueue::QueueResult_AVAILABLE){
					break;
				}
			}

			mDecodeLength=AVCODEC_MAX_AUDIO_FRAME_SIZE;
			result=avcodec_decode_audio3(mStreamData->codecCtx,(int16_t*)mDecodeBuffer,&mDecodeLength,&mPkt);
			if(result<0){
				break;
			}

			av_free_packet(&mPkt);
			mPkt.data=NULL;
		}
	}

	return total;
}

FFmpegVideoStream::FFmpegVideoStream(FFmpegTextureController *controller,FFmpegTextureController::StreamData *streamData,Texture::ptr texture):
	mSwsCtx(NULL),
	mVideoFrame(NULL),
	mTextureFrame(NULL),
	mTextureBuffer(NULL),

	mPtsTime(0)
{
	mController=controller;
	mStreamData=streamData;
	mTexture=texture;
	av_init_packet(&mPkt);

	AVCodecContext *ctx=mStreamData->codecCtx;
	TextureFormat::ptr textureFormat=mTexture->getFormat();
	mSwsCtx=sws_getContext(
		ctx->width,ctx->height,
		ctx->pix_fmt,
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

FFmpegVideoStream::~FFmpegVideoStream(){
	close();
}

void FFmpegVideoStream::close(){
	av_free_packet(&mPkt);
}

void FFmpegVideoStream::update(int dt){
	int64 time=mController->rawTime();
	double pts=0.0f;
	int frameFinished=0;

	while(time>mPtsTime || mPtsTime==0 || mStreamData->queue->flushed()){
		int result=mStreamData->queue->get(&mPkt,0);
		if(result==FFmpegTextureController::PacketQueue::QueueResult_FLUSH){
			mPtsTime=0;
			avcodec_flush_buffers(mStreamData->codecCtx);
			continue;
		}
		else if(result!=FFmpegTextureController::PacketQueue::QueueResult_AVAILABLE){
			break;
		}

		avcodec_decode_video2(mStreamData->codecCtx,mVideoFrame,&frameFinished,&mPkt);

		if(mPkt.dts!=AV_NOPTS_VALUE){
			pts=mPkt.dts;
		}
		else{
			pts=0;
		}

		pts*=av_q2d(mController->getFormatCtx()->streams[mStreamData->index]->time_base);
		int64 ptsTime=pts*AV_TIME_BASE;

		if(frameFinished && (ptsTime>=time || ptsTime==0)){
			mPtsTime=ptsTime;

			sws_scale(
				mSwsCtx,
				mVideoFrame->data,
				mVideoFrame->linesize,
				0,
				mStreamData->codecCtx->height,
				mTextureFrame->data,
				mTextureFrame->linesize
			);
		} 

	    av_free_packet(&mPkt);
	}

	mTexture->load(mTexture->getFormat()->width,mTexture->getFormat()->height,0,0,mTextureFrame->data[0]);
}


FFmpegTextureController::FFmpegTextureController(Engine *engine):
	mEngine(NULL),
	mIOCtx(NULL),
	mIOBuffer(NULL),
    mFormatCtx(NULL),

	mTime(0),
	mState(State_STOP)
{
	mEngine=engine;
}

bool FFmpegTextureController::open(Stream::ptr stream,Resource::ptr resource){
	if(stream->closed()){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"stream not opened");
		return false;
	}

	mTexture=shared_static_cast<Texture>(resource);
	if(mTexture==NULL){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"no Texture specified");
		return false;
	}

	Logger::alert(Categories::TOADLET_TADPOLE,
		"reading stream for "+resource->getName());

	String name=resource->getName();

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

//	if(mFormatCtx->start_time!=AV_NOPTS_VALUE){
//		mStartTime=mFormatCtx->start_time;
//	}
//	else{
//		mStartTime=0;
//	}

	int i;
	for(i=0;i<mFormatCtx->nb_streams;++i){
		int type=mFormatCtx->streams[i]->codec->codec_type;
		if(type>=0 && type<AVMEDIA_TYPE_NB && mStreams[type].codec==NULL){
			StreamData stream;
			stream.codecCtx=mFormatCtx->streams[i]->codec;
			stream.codecCtx->opaque=this;
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

	StreamData *videoStreamData=&mStreams[AVMEDIA_TYPE_VIDEO];
	if(videoStreamData->codecCtx!=NULL){
		mVideoStream=FFmpegVideoStream::ptr(new FFmpegVideoStream(this,videoStreamData,mTexture));
	}

	StreamData *audioStreamData=&mStreams[AVMEDIA_TYPE_AUDIO];
	if(audioStreamData->codecCtx!=NULL){
		mAudioStream=FFmpegAudioStream::ptr(new FFmpegAudioStream(this,audioStreamData));

		if(mEngine->getAudioDevice()!=NULL){
			mAudio=Audio::ptr(mEngine->getAudioDevice()->createAudio());
		}
		if(mAudio!=NULL){
			mAudio->create(shared_static_cast<AudioStream>(mAudioStream));
		}
	};

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

	if(mAudio!=NULL){
		mAudio->play();
	}
}

void FFmpegTextureController::pause(){
	mState=State_PAUSE;
}

void FFmpegTextureController::stop(){
	seek(0);
	mState=State_STOP;

	if(mAudio!=NULL){
		mAudio->stop();
	}
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

void FFmpegTextureController::update(int dt){
	updateDecode(dt);

	if(mState==State_PLAY){
		mTime+=dt*1000;
		if(mTime>maxTime()*1000){
			mTime=maxTime()*1000;
			mState=State_STOP;
		}
	}

	mVideoStream->update(dt);
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

	mTime=time;

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
		}
	}

	return true;
}


FFmpegTextureModifier::FFmpegTextureModifier(Engine *engine){
	mEngine=engine;

	av_log_set_callback(ffmpegLogCallback);

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

	av_log_set_callback(NULL);
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
//		case TextureFormat::Format_LA_8:
//			return PIX_FMT_GRAY8A;
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
	if(av_dup_packet(pkt)<0){
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
			ret=QueueResult_ERROR;
			break;
		}
		if(didFlush){
			didFlush=false;
			ret=QueueResult_FLUSH;
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
			ret=QueueResult_AVAILABLE;
			break;
		}
		else if (!block){
			ret=QueueResult_EMPTY;
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
	didFlush=true;

	mutex.unlock();

	return 1;
}

}
}
}
