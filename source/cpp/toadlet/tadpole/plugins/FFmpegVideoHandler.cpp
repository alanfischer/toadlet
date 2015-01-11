#include "FFmpegVideoHandler.h"
#include "FFmpegStreamContext.h"
#include <toadlet/egg/Mutex.h>
#include <toadlet/egg/System.h>

namespace toadlet{
namespace tadpole{

AVRational avTimeBaseQ={1,AV_TIME_BASE};

TOADLET_C_API int toadlet_ffmpegLockManager(void **mtx,enum AVLockOp op){
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

TOADLET_C_API Logger::Level toadlet_ffmpegGetLogLevel(int level){
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

TOADLET_C_API void toadlet_ffmpegLogCallback(void *ptr,int level,const char *fmt,va_list vl){
	char buffer[1024];
	int amt=vsnprintf(buffer,sizeof(buffer),fmt,vl);
	if(amt>0 && buffer[amt-1]=='\n'){
		buffer[amt-1]=0; // Trim off the trailing newline
	}
	Log::getInstance()->addLogEntry("ffmpeg",toadlet_ffmpegGetLogLevel(level),buffer);
}

TOADLET_C_API void toadlet_ffmpegRegisterLogCallback(){
	av_log_set_callback(toadlet_ffmpegLogCallback);
}

TOADLET_C_API void toadlet_ffmpegUnregisterLogCallback(){
	av_log_set_callback(NULL);
}

TOADLET_C_API int toadlet_ffmpegRegisterLockManager(){
	return av_lockmgr_register(toadlet_ffmpegLockManager);
}

TOADLET_C_API int toadlet_ffmpegUnregisterLockManager(){
	return av_lockmgr_register(NULL);
}

FFmpegAudioStream::FFmpegAudioStream(FFmpegController *controller,FFmpegController::StreamData *streamData):
	mDecodeLength(0),
	mDecodeOffset(0)
{
	mController=controller;
	mStreamData=streamData;
	av_init_packet(&mPkt);
	mPkt.data=NULL;

	AVCodecContext *ctx=mStreamData->codecCtx;
	mAudioFormat=new AudioFormat(16,ctx->channels,ctx->sample_rate);
	mAudioFrame=(AVFrame*)av_mallocz(sizeof(AVFrame));
}

FFmpegAudioStream::~FFmpegAudioStream(){
	close();

	av_free(mAudioFrame);
}

void FFmpegAudioStream::close(){
	mStreamData=NULL;

	if(mPkt.data!=NULL){
		av_free_packet(&mPkt);
		mPkt.data=NULL;
	}
}

int FFmpegAudioStream::read(tbyte *buffer,int length){
	int total=0,amt=0,result=0;
	int64 time=mController->rawTime();

	while(length>0){
		if(mDecodeLength>0){
			amt=mDecodeLength<length?mDecodeLength:length;
			memcpy(buffer,mAudioFrame->data[0]+mDecodeOffset,amt);
			mDecodeLength-=amt;
			mDecodeOffset+=amt;
			length-=amt;
			buffer+=amt;
			total+=amt;
		}
		else{
			if(mPkt.data==NULL){
				result=mStreamData->queue->get(&mPkt,0);
				if(result==FFmpegController::PacketQueue::QueueResult_FLUSH){
					avcodec_flush_buffers(mStreamData->codecCtx);
					continue;
				}
				else if(result!=FFmpegController::PacketQueue::QueueResult_AVAILABLE){
					break;
				}
			}

			result=-1;
			int64 ptsTime=av_rescale_q(mPkt.dts!=AV_NOPTS_VALUE?mPkt.dts:0,mController->getFormatCtx()->streams[mPkt.stream_index]->time_base,avTimeBaseQ);
			if(ptsTime<=time){
				mDecodeOffset=0;
				mDecodeLength=avcodec_decode_audio4(mStreamData->codecCtx,mAudioFrame,&result,&mPkt);
			}

			if(ptsTime<=time || mStreamData->queue->flushed()){
				av_free_packet(&mPkt);
				mPkt.data=NULL;
			}

			if(result<0){
				break;
			}
		}
	}

	return total;
}

FFmpegVideoStream::FFmpegVideoStream(FFmpegController *controller,FFmpegController::StreamData *streamData,Texture::ptr texture):
	mSwsCtx(NULL),
	mVideoFrame(NULL),mDeinterlacedFrame(NULL),mTextureFrame(NULL),

	mPtsTime(0)
{
	mController=controller;
	mStreamData=streamData;
	mTexture=texture;
	av_init_packet(&mPkt);
	mPkt.data=NULL;

	AVCodecContext *ctx=mStreamData->codecCtx;
	TextureFormat::ptr textureFormat=mTexture->getFormat();
	mSwsCtx=sws_getContext(
		ctx->width,ctx->height,
		ctx->pix_fmt,
		textureFormat->getWidth(),textureFormat->getHeight(),
		FFmpegVideoHandler::getPixelFormat(textureFormat->getPixelFormat()),
		SWS_FAST_BILINEAR,
		NULL,NULL,NULL
	);

	mVideoFrame=(AVFrame*)av_mallocz(sizeof(AVFrame));
	mDeinterlacedFrame=(AVFrame*)av_mallocz(sizeof(AVFrame));
	mTextureFrame=(AVFrame*)av_mallocz(sizeof(AVFrame));

	avpicture_alloc((AVPicture*)mDeinterlacedFrame,ctx->pix_fmt,ctx->width,ctx->height);
	avpicture_alloc((AVPicture*)mTextureFrame,FFmpegVideoHandler::getPixelFormat(textureFormat->getPixelFormat()),textureFormat->getWidth(),textureFormat->getHeight());
}

FFmpegVideoStream::~FFmpegVideoStream(){
	close();
}

void FFmpegVideoStream::close(){
	if(mDeinterlacedFrame!=NULL){
		avpicture_free((AVPicture*)mDeinterlacedFrame);
		av_free(mDeinterlacedFrame);
		mDeinterlacedFrame=NULL;
	}

	if(mTextureFrame!=NULL){
		avpicture_free((AVPicture*)mTextureFrame);
		av_free(mTextureFrame);
		mTextureFrame=NULL;
	}

	if(mVideoFrame!=NULL){
		av_free(mVideoFrame);
		mVideoFrame=NULL;
	}

	if(mSwsCtx!=NULL){
		sws_freeContext(mSwsCtx);
		mSwsCtx=NULL;
	}

	if(mPkt.data!=NULL){
		av_free_packet(&mPkt);
		mPkt.data=NULL;
	}
}

void FFmpegVideoStream::update(int dt){
	int64 time=mController->rawTime();
	int frameFinished=0;

	while(time>mPtsTime || mPtsTime==0 || mStreamData->queue->flushed()){
		int result=mStreamData->queue->get(&mPkt,0);
		if(result==FFmpegController::PacketQueue::QueueResult_FLUSH){
			mPtsTime=0;
			avcodec_flush_buffers(mStreamData->codecCtx);
			continue;
		}
		else if(result!=FFmpegController::PacketQueue::QueueResult_AVAILABLE){
			break;
		}

		avcodec_decode_video2(mStreamData->codecCtx,mVideoFrame,&frameFinished,&mPkt);

		int64 ptsTime=av_rescale_q(mPkt.dts!=AV_NOPTS_VALUE?mPkt.dts:0,mController->getFormatCtx()->streams[mPkt.stream_index]->time_base,avTimeBaseQ);

		if(frameFinished && (ptsTime>=time || ptsTime==0)){
			mPtsTime=ptsTime;

			AVFrame *frame=mVideoFrame;
			result=avpicture_deinterlace((AVPicture*)mDeinterlacedFrame,(AVPicture*)mVideoFrame,
				mStreamData->codecCtx->pix_fmt, 
				mStreamData->codecCtx->width, 
				mStreamData->codecCtx->height);
			if(result>=0){
				frame=mDeinterlacedFrame;
			}

			sws_scale(
				mSwsCtx,
				frame->data,
				frame->linesize,
				0,
				mStreamData->codecCtx->height,
				mTextureFrame->data,
				mTextureFrame->linesize
			);
		}

	    av_free_packet(&mPkt);
	}

	mTexture->load(mTexture->getFormat(),mTextureFrame->data[0]);
}


FFmpegController::FFmpegController(Engine *engine):
	mEngine(NULL),
	mIOCtx(NULL),
    mFormatCtx(NULL),

	mState(State_STOP),
	mTime(0)
{
	mEngine=engine;
}

bool FFmpegController::open(Stream::ptr stream){
	Log::alert(Categories::TOADLET_TADPOLE,"FFmpegController::open");

	if(stream->closed()){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"stream not opened");
		return false;
	}

	String name;

	mStream=stream;
	int bufferSize=4096;
	unsigned char *buffer=(unsigned char*)av_mallocz(bufferSize+FF_INPUT_BUFFER_PADDING_SIZE);
	mIOCtx=avio_alloc_context(buffer,bufferSize,0,stream,toadlet_read_packet,toadlet_write_packet,toadlet_seek);
	
	mFormatCtx=avformat_alloc_context();
	if(mFormatCtx==NULL){
		Error::unknown("avformat_alloc_context failed");
        return false;
    }

	mFormatCtx->pb=mIOCtx;

	int result=av_probe_input_buffer(mIOCtx,&mFormatCtx->iformat,name,NULL,0,0);
	if(result<0){
		Error::unknown("av_probe_input_buffer failed");
        return false;
	}

	result=avformat_open_input(&mFormatCtx,name,mFormatCtx->iformat,NULL);
	if(result<0){
		Error::unknown("avformat_open_input failed");
		return false;
	}

	result=avformat_find_stream_info(mFormatCtx,NULL);
	if(result<0){
		Error::unknown("av_find_stream_info failed");
		return false;
	}

	av_dump_format(mFormatCtx,0,name,0);

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

			if(avcodec_open2(stream.codecCtx,stream.codec,NULL)<0){
				Error::unknown(
					String("unable to open stream codec for type:")+type);
				return false;
			}

			mStreams[type]=stream;
		}
	}

	AVCodecContext *videoCtx=mStreams[AVMEDIA_TYPE_VIDEO].codecCtx;
	if(videoCtx!=NULL){
		mVideoFormat=new TextureFormat(TextureFormat::Dimension_D2,0,videoCtx->width,videoCtx->height,1,0);
	}
	else{
		mVideoFormat=NULL;
	}

	AVStream *avstream=mStreams[AVMEDIA_TYPE_VIDEO].index>=0?mFormatCtx->streams[mStreams[AVMEDIA_TYPE_VIDEO].index]:NULL;
	if(avstream!=NULL){
		mMaxTime=av_rescale_q(avstream->duration,avstream->time_base,avTimeBaseQ);
	}
	else{
		mMaxTime=0;
	}

	return true;
}

void FFmpegController::destroy(){
	Log::alert(Categories::TOADLET_TADPOLE,"FFmpegController::destroy");

	mTexture=NULL;

	if(mAudio!=NULL){
		mAudio->stop();
		mAudio=NULL;
	}

	if(mVideoStream!=NULL){
		mVideoStream->close();
		mVideoStream=NULL;
	}

	if(mAudioStream!=NULL){
		mAudioStream->close();
		mAudioStream=NULL;
	}

	if(mFormatCtx!=NULL){
		int i;
		for(i=0;i<mFormatCtx->nb_streams;++i){
			int type=mFormatCtx->streams[i]->codec->codec_type;
			if(type>=0 && type<AVMEDIA_TYPE_NB){
				StreamData *stream=&mStreams[type];
				if(stream->codecCtx!=NULL){
					avcodec_close(stream->codecCtx);
					stream->codecCtx=NULL;
				}
				if(stream->queue!=NULL){
					stream->queue->flush();
				}
			}
		}

		avformat_close_input(&mFormatCtx);
		mFormatCtx=NULL;
	}

	if(mIOCtx!=NULL){
		av_free(mIOCtx);
		mIOCtx=NULL;
	}

	mStream=NULL;
}

void FFmpegController::setTexture(Texture::ptr texture){
	mTexture=texture;

	StreamData *videoStreamData=&mStreams[AVMEDIA_TYPE_VIDEO];
	if(videoStreamData->codecCtx!=NULL && mVideoStream==NULL){
		mVideoStream=new FFmpegVideoStream(this,videoStreamData,mTexture);
	}
}

void FFmpegController::setAudio(Audio::ptr audio){
	mAudio=audio;

	StreamData *audioStreamData=&mStreams[AVMEDIA_TYPE_AUDIO];
	if(audioStreamData->codecCtx!=NULL && mAudioStream==NULL){
		mAudioStream=new FFmpegAudioStream(this,audioStreamData);

		if(mAudio!=NULL){
			mAudio->create(static_pointer_cast<AudioStream>(mAudioStream));
		}
	}
}

void FFmpegController::start(){
	mState=State_PLAY;

	if(mAudio!=NULL){
		mAudio->play();
	}
}

void FFmpegController::pause(){
	mState=State_PAUSE;

	if(mAudio!=NULL){
		mAudio->stop();
	}
}

void FFmpegController::stop(){
	seek(0);
	mState=State_STOP;

	if(mAudio!=NULL){
		mAudio->stop();
	}
}

void FFmpegController::updateDecode(int dt){
	int i;
	AVPacket pkt;
	PacketQueue *videoQueue=mStreams[AVMEDIA_TYPE_VIDEO].queue;
	PacketQueue *audioQueue=mStreams[AVMEDIA_TYPE_AUDIO].queue;
	int maxQueueSize=100;

	if((videoQueue==NULL || videoQueue->count>=4) && (audioQueue==NULL || audioQueue->count>=1)){
		return;
	}

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

		if((videoQueue==NULL || videoQueue->count>=4) && (audioQueue==NULL || audioQueue->count>=1)){
			break;
		}
	}

	while(videoQueue!=NULL && videoQueue->count>maxQueueSize){
		videoQueue->get(&pkt,0);
		av_free_packet(&pkt);
	}
	while(audioQueue!=NULL && audioQueue->count>maxQueueSize){
		audioQueue->get(&pkt,0);
		av_free_packet(&pkt);
	}
}

void FFmpegController::update(int dt){
	updateDecode(dt);

	if(mState==State_PLAY){
		mTime+=milliToAVTime(dt);
		if(mTime>mMaxTime){
			mTime=mMaxTime;
			mState=State_STOP;

			if(mAudio!=NULL){
				mAudio->stop();
			}
		}
	}

	if(mVideoStream!=NULL){
		mVideoStream->update(dt);
	}
}

bool FFmpegController::seek(int64 time){
	time=milliToAVTime(time);
	if(time<0){
		time=0;
	}
	if(time>mMaxTime){
		time=mMaxTime;
	}

	int flags=AVSEEK_FLAG_BACKWARD; // If we have this enabled, then it always works, otherwise we can't always go forward
	int result=0;

	int index=mVideoStream!=NULL?mVideoStream->mStreamData->index:-1;

	uint64 avtime=time;
	if(index>=0){
		avtime=av_rescale_q(avtime,avTimeBaseQ,mFormatCtx->streams[index]->time_base);
	}

	if(1 /*is not mpeg with h264*/ || time<mTime){
		result=av_seek_frame(mFormatCtx,index,avtime,flags);
		if(result!=0){
			result=av_seek_frame(mFormatCtx,index,avtime,AVSEEK_FLAG_ANY);
		}
		mTime=time;

		int i;
		for(i=0;i<AVMEDIA_TYPE_NB;++i){
			if(mStreams[i].queue!=NULL){
				mStreams[i].queue->flush();
			}
		}
	}
	else{
		mTime=time;

		while(mVideoStream->mPtsTime<mTime){
			updateDecode(0);
			mVideoStream->update(0);
		}
	}

	return true;
}


FFmpegVideoHandler::FFmpegVideoHandler(Engine *engine){
	mEngine=engine;

	toadlet_ffmpegRegisterLogCallback();
	toadlet_ffmpegRegisterLockManager();

	av_register_all();
}

void FFmpegVideoHandler::destroy(){
	toadlet_ffmpegUnregisterLockManager();
	toadlet_ffmpegUnregisterLogCallback();
}

VideoController::ptr FFmpegVideoHandler::open(Stream::ptr stream){
	FFmpegController::ptr controller(new FFmpegController(mEngine));

	if(controller->open(stream)==false){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"unable to open stream");
		return NULL;
	}

	return controller;
}

PixelFormat FFmpegVideoHandler::getPixelFormat(int textureFormat){
	switch(textureFormat){
		case TextureFormat::Format_L_8:
			return PIX_FMT_GRAY8;
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


int FFmpegController::PacketQueue::put(AVPacket *pkt){
	AVPacketList *pkt1;
	if(av_dup_packet(pkt)<0){
		return -1;
	}
	pkt1 = (AVPacketList*)av_mallocz(sizeof(AVPacketList));
	if (!pkt1)
		return -1;
	pkt1->pkt = *pkt;

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

int FFmpegController::PacketQueue::get(AVPacket *pkt,int block){
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

int FFmpegController::PacketQueue::flush(){
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
