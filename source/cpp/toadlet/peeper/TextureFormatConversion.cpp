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

#include <toadlet/peeper/TextureFormatConversion.h>
#include <toadlet/egg/Error.h>

namespace toadlet{
namespace peeper{

inline uint32 convertYCrCbToRGBA(int y,int cr,int cb){
	uint32 rgba=0;
	int c=y-16;
	int d=cb-128;
	int e=cr-128;
	rgba|=Math::intClamp(0,255,(298*c      +409*e+128)>>8);
	rgba|=Math::intClamp(0,255,(298*c-100*d-208*e+128)>>8)<<8;
	rgba|=Math::intClamp(0,255,(298*c+516*d      +128)>>8)<<16;
	return rgba;
}

inline uint32 convertYCrCbToBGRA(int y,int cr,int cb){
	uint32 rgba=0;
	int c=y-16;
	int d=cb-128;
	int e=cr-128;
	rgba|=Math::intClamp(0,255,(298*c      +409*e+128)>>8)<<16;
	rgba|=Math::intClamp(0,255,(298*c-100*d-208*e+128)>>8)<<8;
	rgba|=Math::intClamp(0,255,(298*c+516*d      +128)>>8);
	return rgba;
}

bool TextureFormatConversion::convert(tbyte *src,TextureFormat *srcFormat,tbyte *dst,TextureFormat *dstFormat){
	int i,j,k;

	/// @todo: Allow scaling and subsections
	TOADLET_ASSERT(srcFormat->getWidth()==dstFormat->getWidth() && srcFormat->getHeight()==dstFormat->getHeight() && srcFormat->getDepth()==dstFormat->getDepth());

	int srcPixelFormat=srcFormat->getPixelFormat(),dstPixelFormat=dstFormat->getPixelFormat();
	int width=srcFormat->getWidth(),height=srcFormat->getHeight(),depth=srcFormat->getDepth();
	int srcXPitch=srcFormat->getXPitch(),srcYPitch=srcFormat->getYPitch(),dstXPitch=dstFormat->getXPitch(),dstYPitch=dstFormat->getYPitch();

	if(srcPixelFormat==TextureFormat::Format_L_8 && dstPixelFormat==TextureFormat::Format_LA_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcYPitch+j*srcXPitch+i);
					uint16 *d=(uint16*)(dst+k*dstYPitch+j*dstXPitch+i*2);
					*d=((*s)<<8)|0xFF;
				}
			}
		}
	}
	else if(srcPixelFormat==TextureFormat::Format_LA_8 && dstPixelFormat==TextureFormat::Format_L_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint16 *s=(uint16*)(src+k*srcYPitch+j*srcXPitch+i*2);
					uint8 *d=(uint8*)(dst+k*dstYPitch+j*dstXPitch+i);
					*d=((*s)>>8);
				}
			}
		}
	}
	if(srcPixelFormat==TextureFormat::Format_A_8 && dstPixelFormat==TextureFormat::Format_LA_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcYPitch+j*srcXPitch+i);
					uint16 *d=(uint16*)(dst+k*dstYPitch+j*dstXPitch+i*2);
					*d=((*s)|0xFF00);
				}
			}
		}
	}
	else if(srcPixelFormat==TextureFormat::Format_LA_8 && dstPixelFormat==TextureFormat::Format_A_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint16 *s=(uint16*)(src+k*srcYPitch+j*srcXPitch+i*2);
					uint8 *d=(uint8*)(dst+k*dstYPitch+j*dstXPitch+i);
					*d=((*s)&0xFF);
				}
			}
		}
	}
	else if((srcPixelFormat==TextureFormat::Format_L_8 || srcPixelFormat==TextureFormat::Format_R_8) && (dstPixelFormat==TextureFormat::Format_RGBA_8 || dstPixelFormat==TextureFormat::Format_BGRA_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcYPitch+j*srcXPitch+i);
					uint32 *d=(uint32*)(dst+k*dstYPitch+j*dstXPitch+i*4);
					*d=(*s)|((*s)<<8)|((*s)<<16)|(0xFF<<24);
				}
			}
		}
	}
	else if((srcPixelFormat==TextureFormat::Format_RGBA_8 || srcPixelFormat==TextureFormat::Format_BGRA_8) && dstPixelFormat==TextureFormat::Format_L_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint32 *s=(uint32*)(src+k*srcYPitch+j*srcXPitch+i*4);
					uint8 *d=(uint8*)(dst+k*dstYPitch+j*dstXPitch+i);
					*d=(*s)&0xFF;
				}
			}
		}
	}
	else if((srcPixelFormat==TextureFormat::Format_L_8 || srcPixelFormat==TextureFormat::Format_R_8) && (dstPixelFormat==TextureFormat::Format_RGB_8 || dstPixelFormat==TextureFormat::Format_BGR_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcYPitch+j*srcXPitch+i);
					uint8 *d=(uint8*)(dst+k*dstYPitch+j*dstXPitch+i*3);
					*d=*s;
					*(d+1)=*s;
					*(d+2)=*s;
				}
			}
		}
	}
	else if((srcPixelFormat==TextureFormat::Format_RGB_8 || srcPixelFormat==TextureFormat::Format_BGR_8) && (dstPixelFormat==TextureFormat::Format_L_8 || dstPixelFormat==TextureFormat::Format_R_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcYPitch+j*srcXPitch+i*3);
					uint8 *d=(uint8*)(dst+k*dstYPitch+j*dstXPitch+i);
					*d=*s;
				}
			}
		}
	}
	else if(srcPixelFormat==TextureFormat::Format_A_8 && (dstPixelFormat==TextureFormat::Format_RGBA_8 || dstPixelFormat==TextureFormat::Format_BGRA_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcYPitch+j*srcXPitch+i);
					uint32 *d=(uint32*)(dst+k*dstYPitch+j*dstXPitch+i*4);
					*d=0xFFFFFF|((*s)<<24);
				}
			}
		}
	}
	else if((srcPixelFormat==TextureFormat::Format_RGBA_8 || srcPixelFormat==TextureFormat::Format_BGRA_8) && dstPixelFormat==TextureFormat::Format_A_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint32 *s=(uint32*)(src+k*srcYPitch+j*srcXPitch+i*4);
					uint8 *d=(uint8*)(dst+k*dstYPitch+j*dstXPitch+i);
					*d=((*s)&0xFF000000)>>24;
				}
			}
		}
	}
	else if(srcPixelFormat==TextureFormat::Format_LA_8 && (dstPixelFormat==TextureFormat::Format_RGBA_8 || dstPixelFormat==TextureFormat::Format_BGRA_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint16 *s=(uint16*)(src+k*srcYPitch+j*srcXPitch+i*2);
					uint32 *d=(uint32*)(dst+k*dstYPitch+j*dstXPitch+i*4);
					*d=((*s)&0xFF)|(((*s)&0xFF)<<8)|(((*s)&0xFF)<<16)|(((*s)&0xFF00)<<16);
				}
			}
		}
	}
	else if((srcPixelFormat==TextureFormat::Format_RGBA_8 || srcPixelFormat==TextureFormat::Format_BGRA_8) && dstPixelFormat==TextureFormat::Format_LA_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint32 *s=(uint32*)(src+k*srcYPitch+j*srcXPitch+i*4);
					uint16 *d=(uint16*)(dst+k*dstYPitch+j*dstXPitch+i*2);
					*d=((*s)&0xFF)|(((*s)&0xFF000000)>>16);
				}
			}
		}
	}
	else if((srcPixelFormat==TextureFormat::Format_RGB_8 && dstPixelFormat==TextureFormat::Format_RGBA_8) || (srcPixelFormat==TextureFormat::Format_BGR_8 && dstPixelFormat==TextureFormat::Format_BGRA_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcYPitch+j*srcXPitch+i*3);
					uint32 *d=(uint32*)(dst+k*dstYPitch+j*dstXPitch+i*4);
					*d=(0xFF<<24) | (*(s+2)<<16) | (*(s+1)<<8) | (*(s+0)<<0);
				}
			}
		}
	}
	else if((srcPixelFormat==TextureFormat::Format_RGBA_8 && dstPixelFormat==TextureFormat::Format_RGB_8) || (srcPixelFormat==TextureFormat::Format_BGRA_8 && dstPixelFormat==TextureFormat::Format_BGR_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint32 *s=(uint32*)(src+k*srcYPitch+j*srcXPitch+i*4);
					uint8 *d=(uint8*)(dst+k*dstYPitch+j*dstXPitch+i*3);
					*(d+0)=(*s)>>0;
					*(d+1)=(*s)>>8;
					*(d+2)=(*s)>>16;
				}
			}
		}
	}
	else if((srcPixelFormat==TextureFormat::Format_RGB_8 && dstPixelFormat==TextureFormat::Format_BGRA_8) || (srcPixelFormat==TextureFormat::Format_BGR_8 && dstPixelFormat==TextureFormat::Format_RGBA_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcYPitch+j*srcXPitch+i*3);
					uint32 *d=(uint32*)(dst+k*dstYPitch+j*dstXPitch+i*4);
					*d=(0xFF<<24) | (*(s+0)<<16) | (*(s+1)<<8) | (*(s+2)<<0);
				}
			}
		}
	}
	else if((srcPixelFormat==TextureFormat::Format_BGRA_8 && dstPixelFormat==TextureFormat::Format_RGB_8) || (srcPixelFormat==TextureFormat::Format_RGBA_8 && dstPixelFormat==TextureFormat::Format_BGR_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint32 *s=(uint32*)(src+k*srcYPitch+j*srcXPitch+i*4);
					uint8 *d=(uint8*)(dst+k*dstYPitch+j*dstXPitch+i*3);
					*(d+0)=(*s)>>16;
					*(d+1)=(*s)>>8;
					*(d+2)=(*s)>>0;
				}
			}
		}
	}
	else if((srcPixelFormat==TextureFormat::Format_RGB_8 && dstPixelFormat==TextureFormat::Format_BGR_8) || (srcPixelFormat==TextureFormat::Format_BGR_8 && dstPixelFormat==TextureFormat::Format_RGB_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcYPitch+j*srcXPitch+i*3);
					uint8 *d=(uint8*)(dst+k*dstYPitch+j*dstXPitch+i*3);
					*(d+0)=*(s+2);
					*(d+1)=*(s+1);
					*(d+2)=*(s+0);
				}
			}
		}
	}
	else if((srcPixelFormat==TextureFormat::Format_RGBA_8 && dstPixelFormat==TextureFormat::Format_BGRA_8) || (srcPixelFormat==TextureFormat::Format_BGRA_8 && dstPixelFormat==TextureFormat::Format_RGBA_8)){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				for(i=0;i<width;++i){
					uint8 *s=(uint8*)(src+k*srcYPitch+j*srcXPitch+i*4);
					uint8 *d=(uint8*)(dst+k*dstYPitch+j*dstXPitch+i*4);
					*(d+0)=*(s+2);
					*(d+1)=*(s+1);
					*(d+2)=*(s+0);
					*(d+3)=*(s+3);
				}
			}
		}
	}
	else if(srcPixelFormat==TextureFormat::Format_YUY2 && dstPixelFormat==TextureFormat::Format_RGBA_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				uint16 *srcPel=(uint16*)(src+k*srcYPitch+j*srcXPitch);
				uint32 *dstPel=(uint32*)(dst+k*dstYPitch+j*dstXPitch);
				for(i=0;i<width;i+=2){
					// Byte order is U0 Y0 V0 Y1
					int y0=(int)(srcPel[i]&0xFF);
					int u0=(int)(srcPel[i]>>8);
					int y1=(int)(srcPel[i+1]&0xFF);
					int v0=(int)(srcPel[i+1]>>8);
	
					dstPel[i]=convertYCrCbToRGBA(y0,v0,u0);
					dstPel[i+1]=convertYCrCbToRGBA(y1,v0,u0);
				}
			}
        }
	}
	else if(srcPixelFormat==TextureFormat::Format_YUY2 && dstPixelFormat==TextureFormat::Format_BGRA_8){
		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				uint16 *srcPel=(uint16*)(src+k*srcYPitch+j*srcXPitch);
				uint32 *dstPel=(uint32*)(dst+k*dstYPitch+j*dstXPitch);
				for(i=0;i<width;i+=2){
					// Byte order is U0 Y0 V0 Y1
					int y0=(int)(srcPel[i]&0xFF);
					int u0=(int)(srcPel[i]>>8);
					int y1=(int)(srcPel[i+1]&0xFF);
					int v0=(int)(srcPel[i+1]>>8);
	
					dstPel[i]=convertYCrCbToBGRA(y0,v0,u0);
					dstPel[i+1]=convertYCrCbToBGRA(y1,v0,u0);
				}
			}
        }
	}
	else if(srcPixelFormat==dstPixelFormat){
		// See if dst pitches are multiples of the src, in which case just use the src pitch.
		// (Can happen in compressed texture formats with D3D, since it will specify pitch in row-multiples of 4)
		if(dstXPitch/srcXPitch>1 && dstXPitch%srcXPitch==0){
			dstXPitch=srcXPitch;
		}
		if(dstYPitch/srcYPitch>1 && dstYPitch%srcYPitch==0){
			dstYPitch=srcYPitch;
		}

		for(k=0;k<depth;++k){
			for(j=0;j<height;++j){
				memcpy(dst+k*dstYPitch+j*dstXPitch,src+k*srcYPitch+j*srcXPitch,dstXPitch);
			}
		}
	}
	else{
		Error::unknown(Categories::TOADLET_EGG,
			"no format conversion available"
		);
		return false;
	}

	return true;
}

#if 0
/// @todo: Fix the mipmap generation.  It's just nearest neighbor right now.  The format conversion, mipmap generation, and scaling should all be moved into egg::image, and work with raw memory, not Image objects.
bool BackableTexture::convertAndScale(TextureFormat *srcFormat,tbyte *src,TextureFormat *dstFormat,tbyte *dst){
	int srcPitch=ImageFormatConversion::getRowPitch(srcFormat,srcWidth);

	if(srcWidth==dstWidth && srcHeight==dstHeight && srcDepth==dstDepth){
		int dstPitch=ImageFormatConversion::getRowPitch(dstFormat,dstWidth);

		ImageFormatConversion::convert(src,srcFormat,srcPitch,srcPitch*srcHeight,dst,dstFormat,dstPitch,dstPitch*dstHeight,srcWidth,srcHeight,srcDepth);
	}
	else{
		int srcPitch=ImageFormatConversion::getRowPitch(srcFormat,srcWidth);
		int srcPitch2=ImageFormatConversion::getRowPitch(dstFormat,srcWidth);

		/// @todo: Merge these methods so we can do the conversion & scaling in 1 pass, and not this unoptimal mess we have now
		Image::ptr converted(Image::createAndReallocate(Image::Dimension_D2,dstFormat,srcWidth,srcHeight,srcDepth));

		ImageFormatConversion::convert(src,srcFormat,srcPitch,srcPitch*srcHeight,converted->getData(),dstFormat,srcPitch2,srcPitch2*srcHeight,srcWidth,srcHeight,srcDepth);

		Image::ptr scaled(Image::createAndReallocate(Image::Dimension_D2,dstFormat,dstWidth,dstHeight,dstDepth));
		
		Pixel<uint8> pixel;
		int x,y,z;
		for(z=0;z<dstDepth;++z){
			for(y=0;y<dstHeight;++y){
				for(x=0;x<dstWidth;++x){
					converted->getPixel(pixel,x*srcWidth/dstWidth + srcWidth/dstWidth/2,y*srcHeight/dstHeight + srcHeight/dstHeight/2,z*srcDepth/dstDepth + srcDepth/dstDepth/2);
					scaled->setPixel(pixel,x,y,z);
				}
			}
		}

		memcpy(dst,scaled->getData(),ImageFormatConversion::getRowPitch(dstFormat,dstWidth)*dstHeight*dstDepth);
	}
}
/*
	int pitch=ImageFormatConversion::getRowPitch(format,width);
	int closePitch=ImageFormatConversion::getRowPitch(closeFormat,width);
	tbyte *closeData=new tbyte[closePitch * height];
	ImageFormatConversion::convert(mipDatas[0],format,pitch,pitch*height,closeData,closeFormat,closePitch,closePitch*height,width,height,depth);

	Image::ptr image;
	if((hasAutogen==false || hasNonPowerOf2==false) && (Math::isPowerOf2(width)==false || Math::isPowerOf2(height)==false || (dimension!=Image::Dimension_CUBE && Math::isPowerOf2(depth)==false))){
		Logger::debug(Categories::TOADLET_TADPOLE,"making image power of 2");

		int dwidth=width>1?(Math::nextPowerOf2(width)>>1):1;
		int dheight=height>1?(Math::nextPowerOf2(height)>>1):1;
		int ddepth=depth>1?(Math::nextPowerOf2(depth)>>1):1;

		image=Image::ptr(Image::createAndReallocate(dimension,format,dwidth,dheight,ddepth));
		if(image==NULL){
			return false;
		}

		ImageFormatConversion::scale(
		ImageFormatConversion::convert(mData,mFormat,mRowPitch,mSlicePitch,backData,backFormat,backPitch,backPitch * mHeight,mWidth,mHeight,mDepth);
		Pixel<uint8> pixel;
		int x,y,z;
		for(z=0;z<ddepth;++z){
			for(y=0;y<dheight;++y){
				for(x=0;x<dwidth;++x){
					image->getPixel(pixel,x*width/dwidth,y*height/dheight,z*depth/ddepth);
					finalImage->setPixel(pixel,x,y,z);
				}
			}
		}

		width=dwidth;
		height=dheight;
		depth=ddepth;
	}

	Image::ptr finalImage=image;

	if(format!=closestFormat){
if(closestFormat==0)closestFormat=Texture::Format_RGBA_8;
		Logger::debug(Categories::TOADLET_TADPOLE,String("converting image from:")+format+" to:"+closestFormat);

		Image::ptr convertedImage(Image::createAndReallocate(dimension,closestFormat,width,height,depth));
		if(convertedImage==NULL){
			return NULL;
		}

		ImageFormatConversion::convert(
			finalImage->getData(),finalImage->getFormat(),finalImage->getRowPitch(),finalImage->getSlicePitch(),
			convertedImage->getData(),convertedImage->getFormat(),convertedImage->getRowPitch(),convertedImage->getSlicePitch(),width,height,depth);
		finalImage=convertedImage;
		format=closestFormat;
	}

	Collection<Image::ptr> mipImages;
	Collection<tbyte*> mipDatas;

	mipImages.add(finalImage);
	mipDatas.add(finalImage->getData());

	if(hasAutogen==false && wantsAutogen==true){
		Logger::debug(Categories::TOADLET_TADPOLE,"simulating mipmap generation");

		if(mipLevels==0){
			int hwidth=width,hheight=height,hdepth=depth;
			while(hwidth>1 || hheight>1 || hdepth>1){
				mipLevels++;
				hwidth/=2;hheight/=2;hdepth/=2;
				hwidth=hwidth>0?hwidth:1;hheight=hheight>0?hheight:1;hdepth=hdepth>0?hdepth:1;
				int xoff=width/(hwidth+1),yoff=height/(hheight+1),zoff=depth/(hdepth+1);

				Image::ptr mipImage(Image::createAndReallocate(dimension,format,hwidth,hheight,hdepth));
				if(mipImage==NULL){
					return NULL;
				}

				Pixel<uint8> pixel;
				int x,y,z;
				for(z=0;z<hdepth;++z){
					for(y=0;y<hheight;++y){
						for(x=0;x<hwidth;++x){
							finalImage->getPixel(pixel,xoff+x*(1<<mipLevels),yoff+y*(1<<mipLevels),zoff+z*(1<<mipLevels));
							mipImage->setPixel(pixel,x,y,z);
						}
					}
				}

				mipImages.add(mipImage);
				mipDatas.add(mipImage->getData());
			}
		}
	}
*/
#endif

}
}

