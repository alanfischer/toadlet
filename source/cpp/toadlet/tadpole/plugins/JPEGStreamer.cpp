/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine
 *
 * Copyright 2009, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer, Andrew Fischer
 *
 * This file is part of The Toadlet Engine.
 *
 * The Toadlet Engine is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * The Toadlet Engine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with The Toadlet Engine.  If not, see <http://www.gnu.org/licenses/>.
 *
 ********** Copyright header - do not remove **********/

#include "JPEGStreamer.h"
#include <setjmp.h>
#include <stdio.h>
#if defined(TOADLET_PLATFORM_WIN32)
	#include <windows.h>
#endif
extern "C"{
	#include <jpeglib.h>
}

namespace toadlet{
namespace tadpole{

typedef struct{
	struct jpeg_source_mgr pub;
	Stream *stream;
	JOCTET * buffer;
	bool start_of_file;
	int bufsize;
} toadlet_src_mgr;

#define TOADLET_JPEG_BUFFER_SIZE 4096
#define JMESSAGE(code,string)	code ,
#define ERREXIT(cinfo,code)  \
  ((cinfo)->err->msg_code = (code), \
   (*(cinfo)->err->error_exit) ((j_common_ptr) (cinfo)))
#define WARNMS(cinfo,code)  \
  ((cinfo)->err->msg_code = (code), \
   (*(cinfo)->err->emit_message) ((j_common_ptr) (cinfo), -1))

enum{
	JMESSAGE(JERR_INPUT_EMPTY, "Empty input file")
	JMESSAGE(JWRN_JPEG_EOF, "Premature end of JPEG file")
};

void toadlet_init_source(j_decompress_ptr cinfo){
	toadlet_src_mgr *src=(toadlet_src_mgr*)cinfo->src;
	src->start_of_file=true;
}

boolean toadlet_fill_input_buffer(j_decompress_ptr cinfo){
	toadlet_src_mgr *src=(toadlet_src_mgr*)cinfo->src;
	size_t nbytes;

	nbytes=src->stream->read(src->buffer,src->bufsize);

	if(nbytes<=0){
		if(src->start_of_file){
			/* Treat empty input file as fatal error */
			ERREXIT(cinfo,JERR_INPUT_EMPTY);
		}
		WARNMS(cinfo,JWRN_JPEG_EOF);

		/* Insert a fake EOI marker */
		src->buffer[0]=(JOCTET)0xFF;
		src->buffer[1]=(JOCTET)JPEG_EOI;
		nbytes=2;
	}

	src->pub.next_input_byte=src->buffer;
	src->pub.bytes_in_buffer=nbytes;
	src->start_of_file=false;

	return TRUE;
}

void toadlet_skip_input_data(j_decompress_ptr cinfo,long num_bytes){
	toadlet_src_mgr *src=(toadlet_src_mgr*)cinfo->src;

	if(num_bytes>0){
		while(num_bytes>(long)src->pub.bytes_in_buffer){
			num_bytes-=(long)src->pub.bytes_in_buffer;
			toadlet_fill_input_buffer(cinfo);
			/* note we assume that fill_input_buffer will never return FALSE,
			* so suspension need not be handled.
			*/
	    }

		src->pub.next_input_byte+=(size_t)num_bytes;
		src->pub.bytes_in_buffer-=(size_t)num_bytes;
	}
}

void toadlet_term_source(j_decompress_ptr cinfo){
	toadlet_src_mgr *src;
	src=(toadlet_src_mgr*)cinfo->src;

//	if(src->pin!=NULL){
//		src->pin->unread((char*)src->pub.next_input_byte,src->pub.bytes_in_buffer);
//	}
}

void toadlet_InputStream_src(j_decompress_ptr cinfo,io::Stream *stream){
	toadlet_src_mgr *src;

	/* The source object and input buffer are made permanent so that a series
	* of JPEG images can be read from the same file by calling toadlet_InputStream_src
	* only before the first one.  (If we discarded the buffer at the end of
	* one image, we'd likely lose the start of the next one.)
	* This makes it unsafe to use this manager and a different source
	* manager serially with the same JPEG object.  Caveat programmer.
	*/
	if(cinfo->src==NULL){	/* first time for this JPEG object? */
		cinfo->src=(struct jpeg_source_mgr*)new toadlet_src_mgr();
		src=(toadlet_src_mgr*)cinfo->src;

		src->bufsize=TOADLET_JPEG_BUFFER_SIZE*sizeof(JOCTET);

		src->buffer=(JOCTET*)(*cinfo->mem->alloc_small)((j_common_ptr)cinfo,JPOOL_PERMANENT,src->bufsize);
	}

	src=(toadlet_src_mgr*)cinfo->src;
	src->pub.init_source=toadlet_init_source;
	src->pub.fill_input_buffer=toadlet_fill_input_buffer;
	src->pub.skip_input_data=toadlet_skip_input_data;
	src->pub.resync_to_restart=jpeg_resync_to_restart; /* use default method */
	src->pub.term_source=toadlet_term_source;
	src->stream=stream;
	src->pub.bytes_in_buffer=0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte=NULL; /* until buffer loaded */
}

struct toadlet_error_mgr{
	struct jpeg_error_mgr pub; /* public fields */

	jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct toadlet_error_mgr * toadlet_error_ptr;

void toadlet_error_exit(j_common_ptr cinfo){
	/* cinfo->err really points to a toadlet_error_mgr struct, so coerce pointer */
	toadlet_error_ptr toadleterr=(toadlet_error_ptr)cinfo->err;

	/* ignore message for now */
	//(*cinfo->err->output_message) (cinfo);

	/* Return control to the setjmp point */
	longjmp(toadleterr->setjmp_buffer,1);
}

Resource::ptr JPEGStreamer::load(Stream::ptr stream,ResourceData *data){
	int usage=(data!=NULL)?((TextureData*)data)->usage:0;

	struct jpeg_decompress_struct cinfo;
	struct toadlet_error_mgr jerr;
	JSAMPARRAY buffer;
	int row_stride;	

	cinfo.err=jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit=toadlet_error_exit;
	if(setjmp(jerr.setjmp_buffer)){
		jpeg_destroy_decompress(&cinfo);

		Error::unknown(Categories::TOADLET_TADPOLE,
			"JPEGStreamer::loadImage: error in jpeg loading");
		return NULL;
	}

	jpeg_create_decompress(&cinfo);

	toadlet_InputStream_src(&cinfo,stream);

	jpeg_read_header(&cinfo,true);

	jpeg_start_decompress(&cinfo);

	row_stride=cinfo.output_width * cinfo.output_components;

	/* Make a one-row-high sample array that will go away when done with image */
	buffer=(*cinfo.mem->alloc_sarray)
		((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	if(cinfo.output_components!=1 && cinfo.output_components!=3){
		jpeg_destroy_decompress(&cinfo);

		Error::unknown(Categories::TOADLET_TADPOLE,
			"JPEGStreamer::loadImage: Not 1 or 3 components");
		return NULL;
	}

	int pixelFormat=TextureFormat::Format_UNKNOWN;
	if(cinfo.output_components==1){
		pixelFormat=TextureFormat::Format_L_8;
	}
	else if(cinfo.output_components==3){
		pixelFormat=TextureFormat::Format_RGB_8;
	}
	else{
		Error::unknown(Categories::TOADLET_TADPOLE,
			"JPEGStreamer::loadImage: Not 1 or 3 output_components");
		return NULL;
	}

	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,pixelFormat,cinfo.output_width,cinfo.output_height,1,0);
	tbyte *textureData=new tbyte[textureFormat->getDataSize()];

	/* while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	* loop counter, so that we don't have to keep track ourselves.
	*/
	while (cinfo.output_scanline < cinfo.output_height){
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		jpeg_read_scanlines(&cinfo, buffer, 1);

		memcpy(textureData+row_stride*(cinfo.output_height-cinfo.output_scanline),*buffer,row_stride);
	}

	jpeg_finish_decompress(&cinfo);

	jpeg_destroy_decompress(&cinfo);

	Texture::ptr texture=mTextureManager->createTexture(usage,textureFormat,textureData);
	
	delete[] textureData;
	
	return texture;
}

}
}

