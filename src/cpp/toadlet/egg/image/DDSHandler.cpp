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

#include <toadlet/egg/image/DDSHandler.h>
#include <toadlet/egg/io/DataStream.h>
#include <toadlet/egg/math/Math.h>
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/EndianConversion.h>

using namespace toadlet::egg::math;

namespace toadlet{
namespace egg{
namespace image{

#define DDS_MAGIC 0x20534444

#define DDSD_CAPS                   0x00000001 
#define DDSD_HEIGHT                 0x00000002 
#define DDSD_WIDTH                  0x00000004 
#define DDSD_PITCH                  0x00000008 
#define DDSD_PIXELFORMAT            0x00001000 
#define DDSD_MIPMAPCOUNT            0x00020000 
#define DDSD_LINEARSIZE             0x00080000 
#define DDSD_DEPTH                  0x00800000 

#define DDPF_ALPHAPIXELS            0x00000001 
#define DDPF_FOURCC                 0x00000004 
#define DDPF_INDEXED                0x00000020 
#define DDPF_RGB                    0x00000040 

#define DDSCAPS_COMPLEX             0x00000008 
#define DDSCAPS_TEXTURE             0x00001000 
#define DDSCAPS_MIPMAP              0x00400000 

#define DDSCAPS2_CUBEMAP            0x00000200 
#define DDSCAPS2_CUBEMAP_POSITIVEX  0x00000400 
#define DDSCAPS2_CUBEMAP_NEGATIVEX  0x00000800 
#define DDSCAPS2_CUBEMAP_POSITIVEY  0x00001000 
#define DDSCAPS2_CUBEMAP_NEGATIVEY  0x00002000 
#define DDSCAPS2_CUBEMAP_POSITIVEZ  0x00004000 
#define DDSCAPS2_CUBEMAP_NEGATIVEZ  0x00008000 
#define DDSCAPS2_VOLUME             0x00200000 

#define D3DFMT_DXT1                 827611204   // '1TXD'
#define D3DFMT_DXT2                 844388420   // '2TXD'
#define D3DFMT_DXT3                 861165636   // '3TXD'
#define D3DFMT_DXT4                 877942852   // '4TXD'
#define D3DFMT_DXT5                 894720068   // '5TXD'

#define PF_IS_BGRA8(pf) \
  ((pf.dwFlags & DDPF_RGB) && \
   (pf.dwFlags & DDPF_ALPHAPIXELS) && \
   (pf.dwRGBBitCount == 32) && \
   (pf.dwRBitMask == 0xff0000) && \
   (pf.dwGBitMask == 0xff00) && \
   (pf.dwBBitMask == 0xff) && \
   (pf.dwAlphaBitMask == 0xff000000U))

#define PF_IS_BGR8(pf) \
  ((pf.dwFlags & DDPF_ALPHAPIXELS) && \
  !(pf.dwFlags & DDPF_ALPHAPIXELS) && \
   (pf.dwRGBBitCount == 24) && \
   (pf.dwRBitMask == 0xff0000) && \
   (pf.dwGBitMask == 0xff00) && \
   (pf.dwBBitMask == 0xff))

#define PF_IS_BGR5A1(pf) \
  ((pf.dwFlags & DDPF_RGB) && \
   (pf.dwFlags & DDPF_ALPHAPIXELS) && \
   (pf.dwRGBBitCount == 16) && \
   (pf.dwRBitMask == 0x00007c00) && \
   (pf.dwGBitMask == 0x000003e0) && \
   (pf.dwBBitMask == 0x0000001f) && \
   (pf.dwAlphaBitMask == 0x00008000))

#define PF_IS_BGR565(pf) \
  ((pf.dwFlags & DDPF_RGB) && \
  !(pf.dwFlags & DDPF_ALPHAPIXELS) && \
   (pf.dwRGBBitCount == 16) && \
   (pf.dwRBitMask == 0x0000f800) && \
   (pf.dwGBitMask == 0x000007e0) && \
   (pf.dwBBitMask == 0x0000001f))

#define PF_IS_INDEX8(pf) \
  ((pf.dwFlags & DDPF_INDEXED) && \
   (pf.dwRGBBitCount == 8))

typedef struct{
	uint32 dwSize;
	uint32 dwFlags;
	uint32 dwFourCC;
	uint32 dwRGBBitCount;
	uint32 dwRBitMask;
	uint32 dwGBitMask;
	uint32 dwBBitMask;
	uint32 dwAlphaBitMask;
} DDS_PIXELFORMAT;

typedef struct{
	uint32 dwSize;
	uint32 dwHeaderFlags;
	uint32 dwHeight;
	uint32 dwWidth;
	uint32 dwPitchOrLinearSize;
	uint32 dwDepth;
	uint32 dwMipMapCount;
	uint32 dwReserved1[11];
	DDS_PIXELFORMAT ddspf;
	uint32 dwSurfaceFlags;
	uint32 dwCubemapFlags;
	uint32 dwReserved2[3];
} DDS_HEADER;

DDSHandler::DDSHandler(){
}

DDSHandler::~DDSHandler(){
}

bool DDSHandler::loadImage(Stream *stream,Collection<Image::ptr> &mipLevels){
	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_EGG,
			"Stream is NULL");
		return false;
	}

	DataStream::ptr dataStream(new DataStream(stream));

	uint32 magic=dataStream->readLUInt32();
	if(magic!=DDS_MAGIC){
		Error::unknown(Categories::TOADLET_EGG,
			"bad magic number");
		return false;
	}

	DDS_HEADER hdr={0};
	int i;

	dataStream->read((tbyte*)&hdr,sizeof(hdr));
	#if defined(TOADLET_BIG_ENDIAN)
		littleUInt32InPlace(hdr.dwSize);
		littleUInt32InPlace(hdr.dwHeaderFlags);
		littleUInt32InPlace(hdr.dwHeight);
		littleUInt32InPlace(hdr.dwWidth);
		littleUInt32InPlace(hdr.dwPitchOrLinearSize);
		littleUInt32InPlace(hdr.dwDepth);
		littleUInt32InPlace(hdr.dwMipMapCount);
		for(i=0;i<11;++i){
			littleUInt32InPlace(hdr.dwReserved1[i]);
		}

		littleUInt32InPlace(hdr.ddspf.dwSize);
		littleUInt32InPlace(hdr.ddspf.dwFlags);
		littleUInt32InPlace(hdr.ddspf.dwFourCC);
		littleUInt32InPlace(hdr.ddspf.dwRGBBitCount);
		littleUInt32InPlace(hdr.ddspf.dwRBitMask);
		littleUInt32InPlace(hdr.ddspf.dwGBitMask);
		littleUInt32InPlace(hdr.ddspf.dwBBitMask);
		littleUInt32InPlace(hdr.ddspf.dwAlphaBitMask);

		littleUInt32InPlace(hdr.dwSurfaceFlags);
		littleUInt32InPlace(hdr.dwCubemapFlags);
		for(i=0;i<3;++i){
			littleUInt32InPlace(hdr.dwReserved2[i]);
		}
	#endif

	if(!(hdr.dwHeaderFlags & DDSD_PIXELFORMAT) || !(hdr.dwHeaderFlags & DDSD_CAPS)){\
		Error::unknown(Categories::TOADLET_EGG,
			"invalid format or caps");
		return false;
	}

	int width=hdr.dwWidth;
	int height=hdr.dwHeight;
	int depth=hdr.dwDepth;
	int mipCount=(hdr.dwHeaderFlags&DDSD_MIPMAPCOUNT) ? hdr.dwMipMapCount : 1;

	mipLevels.resize(mipCount);

	if((hdr.ddspf.dwFlags&DDPF_FOURCC)!=0 && (
		hdr.ddspf.dwFourCC==D3DFMT_DXT1 ||
		hdr.ddspf.dwFourCC==D3DFMT_DXT3 ||
		hdr.ddspf.dwFourCC==D3DFMT_DXT5
	)){
		int divSize=4;
		int blockBytes=(hdr.ddspf.dwFourCC==D3DFMT_DXT1) ? 8 : 16;

		int format=0;
		switch(hdr.ddspf.dwFourCC){
			case D3DFMT_DXT1:
				format=Image::Format_RGB_DXT1;
			break;
			case D3DFMT_DXT2:
				format=Image::Format_RGBA_DXT2;
			break;
			case D3DFMT_DXT3:
				format=Image::Format_RGBA_DXT3;
			break;
			case D3DFMT_DXT4:
				format=Image::Format_RGBA_DXT4;
			break;
			case D3DFMT_DXT5:
				format=Image::Format_RGBA_DXT5;
			break;
		}

	    for(i=0;i<mipCount;++i){
			Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,format,width,height,depth));
			int size=Math::maxVal(divSize,width)/divSize * Math::maxVal(divSize,height)/divSize * blockBytes;

			if(i==0 && (hdr.dwHeaderFlags & DDSD_LINEARSIZE)!=0 && size!=(int)hdr.dwPitchOrLinearSize){
				Error::unknown(Categories::TOADLET_EGG,
					"incorrect size");
				return false;
			}

			TOADLET_ASSERT(image->getSlicePitch()==size);

			stream->read(image->getData(),size);

			mipLevels[i]=image;

			width=(width+1)>>1;
			height=(height+1)>>1;
			depth=(depth+1)>>1;
		}
	}
	else if(PF_IS_INDEX8(hdr.ddspf)){
		Logger::alert("Other pallette");
	}
	else{
		Logger::alert("Other type");
	}

	return true;
}

bool DDSHandler::saveImage(Image *image,Stream *stream){
	return false;
}

}
}
}





#if 0
/* DDS loader written by Jon Watte 2002 */
/* Permission granted to use freely, as long as Jon Watte */
/* is held harmless for all possible damages resulting from */
/* your use or failure to use this code. */
/* No warranty is expressed or implied. Use at your own risk, */
/* or not at all. */




#include <stdio.h>
#include <string.h>
#include <assert.h>

struct DdsLoadInfo {
  bool compressed;
  bool swap;
  bool palette;
  unsigned int divSize;
  unsigned int blockBytes;
  GLenum internalFormat;
  GLenum externalFormat;
  GLenum type;
};

DdsLoadInfo loadInfoDXT1 = {
  true, false, false, 4, 8, GL_COMPRESSED_RGBA_S3TC_DXT1
};
DdsLoadInfo loadInfoDXT3 = {
  true, false, false, 4, 16, GL_COMPRESSED_RGBA_S3TC_DXT3
};
DdsLoadInfo loadInfoDXT5 = {
  true, false, false, 4, 16, GL_COMPRESSED_RGBA_S3TC_DXT5
};
DdsLoadInfo loadInfoBGRA8 = {
  false, false, false, 1, 4, GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE
};
DdsLoadInfo loadInfoBGR8 = {
  false, false, false, 1, 3, GL_RGB8, GL_BGR, GL_UNSIGNED_BYTE
};
DdsLoadInfo loadInfoBGR5A1 = {
  false, true, false, 1, 2, GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV
};
DdsLoadInfo loadInfoBGR565 = {
  false, true, false, 1, 2, GL_RGB5, GL_RGB, GL_UNSIGNED_SHORT_5_6_5
};
DdsLoadInfo loadInfoIndex8 = {
  false, false, true, 1, 1, GL_RGB8, GL_BGRA, GL_UNSIGNED_BYTE
};

bool
MyTexture::loadDds( FILE * f )
{
  DDS_header hdr;
  size_t s = 0;
  unsigned int x = 0;
  unsigned int y = 0;
  unsigned int mipMapCount = 0;
  //  DDS is so simple to read, too
  fread( &hdr, sizeof( hdr ), 1, f );
  assert( hdr.dwMagic == DDS_MAGIC );
  assert( hdr.dwSize == 124 );
  
  if( hdr.dwMagic != DDS_MAGIC || hdr.dwSize != 124 ||
    !(hdr.dwFlags & DDSD_PIXELFORMAT) || !(hdr.dwFlags & DDSD_CAPS) )
  {
    goto failure;
  }

  xSize = hdr.dwWidth;
  ySize = hdr.dwHeight;
  assert( !(xSize & (xSize-1)) );
  assert( !(ySize & (ySize-1)) );

  DdsLoadInfo * li;

  if( PF_IS_DXT1( hdr.sPixelFormat ) ) {
    li = &loadInfoDXT1;
  }
  else if( PF_IS_DXT3( hdr.sPixelFormat ) ) {
    li = &loadInfoDXT3;
  }
  else if( PF_IS_DXT5( hdr.sPixelFormat ) ) {
    li = &loadInfoDXT5;
  }
  else if( PF_IS_BGRA8( hdr.sPixelFormat ) ) {
    li = &loadInfoBGRA8;
  }
  else if( PF_IS_BGR8( hdr.sPixelFormat ) ) {
    li = &loadInfoBGR8;
  }
  else if( PF_IS_BGR5A1( hdr.sPixelFormat ) ) {
    li = &loadInfoBGR5A1;
  }
  else if( PF_IS_BGR565( hdr.sPixelFormat ) ) {
    li = &loadInfoBGR565;
  }
  else if( PF_IS_INDEX8( hdr.sPixelFormat ) ) {
    li = &loadInfoIndex8;
  }
  else {
    goto failure;
  }

  //fixme: do cube maps later
  //fixme: do 3d later
  x = xSize;
  y = ySize;
  glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
  mipMapCount = (hdr.dwFlags & DDSD_MIPMAPCOUNT) ? hdr.dwMipMapCount : 1;
  if( mipMapCount > 1 ) {
    hasMipmaps_ = true;
  }
  if( li->compressed ) {
    size_t size = max( li->divSize, x )/li->divSize * max( li->divSize, y )/li->divSize * li->blockBytes;
    assert( size == hdr.dwPitchOrLinearSize );
    assert( hdr.dwFlags & DDSD_LINEARSIZE );
    unsigned char * data = (unsigned char *)malloc( size );
    if( !data ) {
      goto failure;
    }
    format = cFormat = li->internalFormat;
    for( unsigned int ix = 0; ix < mipMapCount; ++ix ) {
      fread( data, 1, size, f );
      glCompressedTexImage2D( GL_TEXTURE_2D, ix, li->internalFormat, x, y, 0, size, data );
      gl->updateError();
      x = (x+1)>>1;
      y = (y+1)>>1;
      size = max( li->divSize, x )/li->divSize * max( li->divSize, y )/li->divSize * li->blockBytes;
    }
    free( data );
  }
  else if( li->palette ) {
    //  currently, we unpack palette into BGRA
    //  I'm not sure we always get pitch...
    assert( hdr.dwFlags & DDSD_PITCH );
    assert( hdr.sPixelFormat.dwRGBBitCount == 8 );
    size_t size = hdr.dwPitchOrLinearSize * ySize;
    //  And I'm even less sure we don't get padding on the smaller MIP levels...
    assert( size == x * y * li->blockBytes );
    format = li->externalFormat;
    cFormat = li->internalFormat;
    unsigned char * data = (unsigned char *)malloc( size );
    unsigned int palette[ 256 ];
    unsigned int * unpacked = (unsigned int *)malloc( size*sizeof( unsigned int ) );
    fread( palette, 4, 256, f );
    for( unsigned int ix = 0; ix < mipMapCount; ++ix ) {
      fread( data, 1, size, f );
      for( unsigned int zz = 0; zz < size; ++zz ) {
        unpacked[ zz ] = palette[ data[ zz ] ];
      }
      glPixelStorei( GL_UNPACK_ROW_LENGTH, y );
      glTexImage2D( GL_TEXTURE_2D, ix, li->internalFormat, x, y, 0, li->externalFormat, li->type, unpacked );
      gl->updateError();
      x = (x+1)>>1;
      y = (y+1)>>1;
      size = x * y * li->blockBytes;
    }
    free( data );
    free( unpacked );
  }
  else {
    if( li->swap ) {
      glPixelStorei( GL_UNPACK_SWAP_BYTES, GL_TRUE );
    }
    size = x * y * li->blockBytes;
    format = li->externalFormat;
    cFormat = li->internalFormat;
    unsigned char * data = (unsigned char *)malloc( size );
    //fixme: how are MIP maps stored for 24-bit if pitch != ySize*3 ?
    for( unsigned int ix = 0; ix < mipMapCount; ++ix ) {
      fread( data, 1, size, f );
      glPixelStorei( GL_UNPACK_ROW_LENGTH, y );
      glTexImage2D( GL_TEXTURE_2D, ix, li->internalFormat, x, y, 0, li->externalFormat, li->type, data );
      gl->updateError();
      x = (x+1)>>1;
      y = (y+1)>>1;
      size = x * y * li->blockBytes;
    }
    free( data );
    glPixelStorei( GL_UNPACK_SWAP_BYTES, GL_FALSE );
    gl->updateError();
  }
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, mipMapCount-1 );
  gl->updateError();

  return true;

failure:
  return false;
}

#endif
