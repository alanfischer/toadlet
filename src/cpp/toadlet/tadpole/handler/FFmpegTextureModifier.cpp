#include "FFmpegTextureModifier.h"
#include "FFmpegStreamContext.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/Mutex.h>

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

FFmpegTextureController::FFmpegTextureController():
	mIOCtx(NULL),
	mIOBuffer(NULL),
    mFormatCtx(NULL),
    mSwsCtx(NULL){}

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
			stream.codec=avcodec_find_decoder(stream.codecCtx->codec_id);
			stream.index=i;

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

	mStreams[AVMEDIA_TYPE_VIDEO].maxSize=(10 * 256 * 1024);
	mStreams[AVMEDIA_TYPE_AUDIO].maxSize=(10 * 16 * 1024);

	AVCodecContext *videoCodecCtx=mStreams[AVMEDIA_TYPE_VIDEO].codecCtx;
	if(videoCodecCtx==NULL){
		Error::unknown(
			"unable to find a video codec");
		return false;
	}

	mSwsCtx=sws_getContext(
		videoCodecCtx->width,videoCodecCtx->height,
		videoCodecCtx->pix_fmt,
		textureFormat->width,textureFormat->height,
		PIX_FMT_RGBA, /// @todo: Convert from textureFormat->pixelFormat to PIX_FMT
		SWS_FAST_BILINEAR,
		NULL,NULL,NULL
	);

	mVideoFrame=avcodec_alloc_frame();
	mTextureFrame=avcodec_alloc_frame();

	int size=avpicture_get_size(PIX_FMT_RGBA,videoCodecCtx->width,videoCodecCtx->height);
	mTextureBuffer=(tbyte*)av_malloc(size);

	avpicture_fill((AVPicture*)mTextureFrame,mTextureBuffer,PIX_FMT_RGBA,textureFormat->width,textureFormat->height);

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
	queuePackets();

	mPtsTime=0;
	mTime=0;
	mOffsetTime=av_gettime();

	mFrameCount=0;
	mFrameFlush=0;
}

void FFmpegTextureController::queuePackets(){ 
	int i;
	for(i=0;i<AVMEDIA_TYPE_NB;++i){
		if(mStreams[i].codec!=NULL && mStreams[i].size<mStreams[i].maxSize){
			break;
		}
	}
	if(i==AVMEDIA_TYPE_NB){
		return;
	}

	AVPacket packet;
	AVPacketList *packetList;

	while(av_read_frame(mFormatCtx,&packet)>=0){
		StreamData *stream=NULL;
		for(i=0;i<AVMEDIA_TYPE_NB;++i){
			if(mStreams[i].index==packet.stream_index){
				stream=&mStreams[i];
				break;
			}
		}
		if(stream!=NULL){
			if(av_dup_packet(&packet)==0){
				packetList=(AVPacketList*) av_malloc( sizeof(AVPacketList) );
				packetList->pkt=packet;
				packetList->next=NULL;

				if(stream->last==NULL){
					stream->first=packetList;
				}
				else{
					stream->last->next=packetList;
				}
				stream->last=packetList;
				stream->size+=packet.size+sizeof(*packetList);
				stream->count++;
			}
		}
		else{
			av_free_packet(&packet);
		}

		for(i=0;i<AVMEDIA_TYPE_NB;++i){
			if(mStreams[i].codec!=NULL && mStreams[i].size<mStreams[i].maxSize){
				break;
			}
		}
		if(i==AVMEDIA_TYPE_NB){
			return;
		}
	}
}

int FFmpegTextureController::nextPacket(AVPacket *result,StreamData *stream){
    AVPacketList *packetList=stream->first;
	if(packetList){
		stream->first=packetList->next;
		if(stream->first==NULL){
			stream->last=NULL;
		}

		stream->count--;
		stream->size-=(packetList->pkt.size + sizeof(*packetList));
		*result=packetList->pkt;
		av_free(packetList);
		return 1;
	}
	return 0;
}

void FFmpegTextureController::decode(){
	queuePackets();

	mTime=av_gettime() - mOffsetTime + mStartTime;

	double pts=0.0f;
	AVPacket packet;
	int frameFinished = 0;

	// decode new video frame
	while(mTime>mPtsTime || mPtsTime==0){
		StreamData *stream=&mStreams[AVMEDIA_TYPE_VIDEO];

		if(!nextPacket(&packet,stream)){
            break;
        }

		avcodec_decode_video2(stream->codecCtx,mVideoFrame,&frameFinished,&packet);

		if(packet.dts != AV_NOPTS_VALUE){
            pts=packet.dts;
        }
		else{
            pts = 0;
        }

        pts *= av_q2d( mFormatCtx->streams[stream->index]->time_base );
        mPtsTime = pts * AV_TIME_BASE;

		if( frameFinished && ( mPtsTime >= mTime || mPtsTime == 0 ) ){
            sws_scale(
                mSwsCtx,
                mVideoFrame->data,
                mVideoFrame->linesize,
                0,
                mStreams[AVMEDIA_TYPE_VIDEO].codecCtx->height,
                mTextureFrame->data,
                mTextureFrame->linesize
            );

            mFrameCount++;
        }

        av_free_packet(&packet);
    }
}

void FFmpegTextureController::update(int dt){
	decode();

	if(mFrameCount==mFrameFlush){
        return;
    }

	StreamData *videoStream=&mStreams[AVMEDIA_TYPE_VIDEO];

	if(videoStream->codecCtx){
		mTexture->load(videoStream->codecCtx->width,videoStream->codecCtx->height,0,0,mTextureFrame->data[0]);
	}

    mFrameFlush=mFrameCount;
}


FFmpegTextureModifier::FFmpegTextureModifier(){
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
	FFmpegTextureController::ptr controller(new FFmpegTextureController());

	if(controller->open(stream,resource)==false){
		Error::unknown(Categories::TOADLET_TADPOLE,
			"unable to open stream");
		return NULL;
	}

	return controller;
}

}
}
}
