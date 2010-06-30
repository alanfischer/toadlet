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

#include <toadlet/egg/image/PNGHandler.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <stdlib.h>
extern "C"{
	#include <png.h>
}

#if defined(TOADLET_PLATFORM_WIN32) && defined(TOADLET_LIBPNG_NAME)
	#pragma comment(lib,TOADLET_LIBPNG_NAME)
#endif

#if defined(TOADLET_PLATFORM_WIN32) && defined(TOADLET_ZLIB_NAME)
	#pragma comment(lib,TOADLET_ZLIB_NAME)
#endif

using namespace toadlet::egg::io;

namespace toadlet{
namespace egg{
namespace image{
	
PNGHandler::PNGHandler(){
}

PNGHandler::~PNGHandler(){
}

void toadlet_png_read_data(png_structp png_ptr,png_bytep data,png_size_t length){
	png_size_t check; 

	Stream *stream=(Stream*)png_get_io_ptr(png_ptr);
	check=(png_size_t)stream->read(data,length);

	if(check!=length){
		png_error(png_ptr,"Data read length mismatch");
	}
}

Image *PNGHandler::loadImage(Stream *stream){
	png_byte header[8];
	int y;

	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_EGG,
			"Stream is NULL");
		return NULL;
	}

	int width, height;
	png_byte color_type;
	png_byte bit_depth;

	png_structp png_ptr;
	png_infop info_ptr;
	int number_of_passes;
	png_bytep *row_pointers;
	
	stream->read(header,8);
	if(png_sig_cmp(header,0,8)){
		Error::loadingImage(Categories::TOADLET_EGG,
			"PNGHandler::loadImage: Not a PNG file");
		return NULL;
	}
		
	png_ptr=png_create_read_struct(PNG_LIBPNG_VER_STRING,NULL,NULL,NULL);
	if(png_ptr==NULL){
		Error::loadingImage(Categories::TOADLET_EGG,
			"PNGHandler::loadImage: png_create_read_struct failed");
		return NULL;
	}
	
	info_ptr=png_create_info_struct(png_ptr);
	if(info_ptr==NULL){
		png_destroy_read_struct(&png_ptr,NULL,NULL);

		Error::loadingImage(Categories::TOADLET_EGG,
			"PNGHandler::loadImage: png_create_info_struct failed");
		return NULL;
	}

	if(setjmp(png_jmpbuf(png_ptr))){
		png_destroy_read_struct(&png_ptr,&info_ptr,NULL);

		Error::loadingImage(Categories::TOADLET_EGG,
			"PNGHandler::loadImage: error in png loading");
		return NULL;
	}

	png_set_read_fn(png_ptr,stream,toadlet_png_read_data);
	png_set_sig_bytes(png_ptr,8);
	png_read_info(png_ptr,info_ptr);

	width=png_get_image_width(png_ptr,info_ptr);
	height=png_get_image_height(png_ptr,info_ptr);
	color_type=png_get_color_type(png_ptr,info_ptr);
	bit_depth=png_get_bit_depth(png_ptr,info_ptr);

	number_of_passes=png_set_interlace_handling(png_ptr);
	png_read_update_info(png_ptr,info_ptr);

	row_pointers=(png_bytep*) malloc(sizeof(png_bytep)*height);
	for(y=0;y<height;++y){
		row_pointers[y]=(png_byte*) malloc(png_get_rowbytes(png_ptr,info_ptr));
	}
	
	png_read_image(png_ptr,row_pointers);

	Image *image=NULL;

	#if 0 // This code is removed until I have time to update it to all the non-depreciated & new trans stuff
	if(color_type==PNG_COLOR_TYPE_PALETTE){
		#define PNGHANDLER_GET_INDEX \
			(row[x/a]>>((8-bit_depth)-bit_depth*(x%a)))&b

		png_colorp palette=info_ptr->palette;
		png_bytep trans=info_ptr->trans;
		int a=(8/bit_depth);
		int b=(255>>(8-bit_depth));

		if(png_get_valid(png_ptr,info_ptr,PNG_INFO_tRNS)){
			if(info_ptr->num_trans==1){
				image=new Image(Image::Dimension_D2,Image::Format_RGBA_5_5_5_1,width,height);
			}
			else{
				image=new Image(Image::Dimension_D2,Image::Format_RGBA_4_4_4_4,width,height);
			}

			uint16 *data=(uint16*)image->getData();

			int x;
			if(info_ptr->num_trans==1){
				for(y=0;y<height;++y){
					byte *row=row_pointers[y];
					for(x=0;x<width;++x){
						int index=PNGHANDLER_GET_INDEX;
						data[y*width+x]=ImageFormatConversion::makeRGBA5551(palette[index].red,palette[index].green,palette[index].blue,(info_ptr->trans_values.index==index)?0:255);
					}
				}
			}
			else{
				for(y=0;y<height;++y){
					byte *row=row_pointers[y];
					for(x=0;x<width;++x){
						int index=PNGHANDLER_GET_INDEX;
						data[y*width+x]=ImageFormatConversion::makeRGBA4444(palette[index].red,palette[index].green,palette[index].blue,trans[index]);
					}
				}
			}
		}
		else{
			image=new Image(Image::Dimension_D2,Image::Format_RGB_5_6_5,width,height);

			uint16 *data=(uint16*)image->getData();

			int x;
			for(y=0;y<height;++y){
				byte *row=row_pointers[y];
				for(x=0;x<width;++x){
					int index=PNGHANDLER_GET_INDEX;
					data[y*width+x]=ImageFormatConversion::makeRGB565(palette[index].red,palette[index].green,palette[index].blue);
				}
			}
		}
	}
	#endif
	int format=0;
	switch(color_type){
		case(PNG_COLOR_TYPE_GRAY):
			format=Image::Format_L_8;
		break;
		case(PNG_COLOR_TYPE_GRAY_ALPHA):
			format=Image::Format_LA_8;
		break;
		case(PNG_COLOR_TYPE_RGB):
			format=Image::Format_RGB_8;
		break;
		case(PNG_COLOR_TYPE_RGB_ALPHA):
			format=Image::Format_RGBA_8;
		break;
		default:
			png_destroy_read_struct(&png_ptr,&info_ptr,NULL);
			Error::loadingImage(Categories::TOADLET_EGG,
				String("PNGHandler::loadImage: Unknown PNG type:")+color_type);
		break;
	}

	image=Image::ptr(Image::createAndReallocate(Image::Dimension_D2,format,width,height));
	if(image==NULL){
		return NULL;
	}
	
	int bytewidth=width*image->getPixelSize();
	tbyte *data=image->getData();
	for(y=0;y<height;++y){
		// Flip image in this copy
		memcpy(data+bytewidth*(height-y-1),row_pointers[y],bytewidth);
	}
	
	for(y=0;y<height;++y){
		free(row_pointers[y]);
	}
	free(row_pointers);

	png_destroy_read_struct(&png_ptr,&info_ptr,NULL);

	return image;
}

bool PNGHandler::saveImage(Image *image,Stream *stream){
	Error::unimplemented(Categories::TOADLET_EGG,
		"PNGHandler::saveImage: Not implemented");
	return false;
}

}
}
}

