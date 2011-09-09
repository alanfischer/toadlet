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

#include "GLRenderDevice.h"
#include "GLTexture.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>
#include <toadlet/egg/image/ImageFormatConversion.h>

namespace toadlet{
namespace peeper{

GLTexture::GLTexture(GLRenderDevice *renderDevice):BaseResource(),
	mDevice(NULL),

	mUsage(0),
	//mFormat,

	mHandle(0),
	mTarget(0),
	//mMatrix,
	mManuallyGenerateMipLevels(false)
	//mBuffers
{
	mDevice=renderDevice;
}

GLTexture::~GLTexture(){
	destroy();
}

bool GLTexture::create(int usage,TextureFormat::ptr format,tbyte *mipDatas[]){
	format->width=format->width>0?format->width:1;format->height=format->height>0?format->height:1;format->depth=format->depth>0?format->depth:1;

	if((Math::isPowerOf2(format->width)==false || Math::isPowerOf2(format->height)==false || (format->dimension!=TextureFormat::Dimension_CUBE && Math::isPowerOf2(format->depth)==false)) &&
		mDevice->mNPOT==false && (mDevice->mNPOTR==false || (usage&Usage_BIT_NPOT_RESTRICTED)==0))
	{
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexture: Cannot load a non power of 2 texture");
		return false;
	}

	mUsage=usage;
	mFormat=format;

	bool result=createContext(format->mipLevels,mipDatas);

	return result;
}

void GLTexture::destroy(){
	destroyContext();

	mBuffers.clear();
}

bool GLTexture::createContext(int mipLevels,tbyte *mipDatas[]){
	// Create context data
	mTarget=getGLTarget();
	glGenTextures(1,&mHandle);
	glBindTexture(mTarget,mHandle);

	// GL specific states
	glTexParameteri(mTarget,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(mTarget,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(mTarget,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(mTarget,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	bool onlyFirstLevel=false;
	mManuallyGenerateMipLevels=(mUsage&Usage_BIT_AUTOGEN_MIPMAPS)!=0;
	if(mManuallyGenerateMipLevels &&
		#if defined(TOADLET_HAS_GLES)
			mDevice->gl_version>=11
		#else
			mDevice->gl_version>=14
		#endif
		#if defined(TOADLET_HAS_GLEW) && defined(GL_EXT_framebuffer_object)
			&& GLEW_EXT_framebuffer_object==false
		#endif
	){
		glTexParameteri(mTarget,GL_GENERATE_MIPMAP,GL_TRUE);
		mManuallyGenerateMipLevels=false;
		// RenderTargets need all levels specified
		if((mUsage&Usage_BIT_RENDERTARGET)==0){
			onlyFirstLevel=true;
		}
	}

	int totalMipLevels=Math::intLog2(Math::maxVal(mFormat->width,Math::maxVal(mFormat->height,mFormat->depth)));
	int specifiedMipLevels=mipLevels>0?mipLevels:totalMipLevels;

	// If we don't support partial miplevel specification, then calculate the amount of levels we'll need
	#if !defined(TOADLET_HAS_GLES)
		if(mDevice->gl_version>=12){
			glTexParameteri(mTarget,GL_TEXTURE_BASE_LEVEL,0);
			glTexParameteri(mTarget,GL_TEXTURE_MAX_LEVEL,specifiedMipLevels-1);
		}
		else
	#endif
	/// @todo: Determine if GLES will let us do partial specifications anyway, and if so, enable that
	if(totalMipLevels!=mFormat->mipLevels && mFormat->mipLevels>0){
		specifiedMipLevels=totalMipLevels;

		Logger::debug(Categories::TOADLET_PEEPER,
			String("partial mipmap specification not supported.  calculated:")+totalMipLevels+" requested:"+mFormat->mipLevels);
	}

	// Always need to specify at least 1 level
	if(specifiedMipLevels==0){
		specifiedMipLevels=1;
	}

	int pixelFormat=mFormat->pixelFormat;
	GLint glinternalFormat=GLRenderDevice::getGLFormat(pixelFormat,true);
	GLint glformat=GLRenderDevice::getGLFormat(pixelFormat,false);
	GLint gltype=ImageFormatConversion::isFormatCompressed(pixelFormat)==false?GLRenderDevice::getGLType(pixelFormat):0;

	if(glinternalFormat==0 || glformat==0){
		return false;
	}

	// Allocate texture memory
	int level=0,width=mFormat->width,height=mFormat->height,depth=mFormat->depth;
	for(level=0;level<specifiedMipLevels;++level,width/=2,height/=2,depth/=2){
		int rowPitch=ImageFormatConversion::getRowPitch(mFormat->pixelFormat,width);
 		int slicePitch=rowPitch*height;
		TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(slicePitch);

		tbyte *data=NULL;
		if(mipDatas!=NULL && (level==0 || level<mipLevels)){
			data=mipDatas[level];
			int alignment=1,pitch=rowPitch;
			while(pitch>0 && (pitch&1)==0){alignment<<=1;pitch>>=1;}
			glPixelStorei(GL_UNPACK_ALIGNMENT,alignment<8?alignment:8);
		}
		else{
			if(level>0 && onlyFirstLevel) break;
		}

		if(ImageFormatConversion::isFormatCompressed(mFormat->pixelFormat)==false){
			switch(mTarget){
				#if !defined(TOADLET_HAS_GLES)
					case GL_TEXTURE_1D:
						glTexImage1D(mTarget,level,glinternalFormat,width,0,glformat,gltype,data);
					break;
				#endif
				case GL_TEXTURE_2D:
					glTexImage2D(mTarget,level,glinternalFormat,width,height,0,glformat,gltype,data);
				break;
				#if !defined(TOADLET_HAS_GLES) || defined(TOADLET_HAS_GL_20)
					case GL_TEXTURE_CUBE_MAP:{
						glTexParameteri(mTarget,GL_TEXTURE_WRAP_S,GL_CLAMP);
						glTexParameteri(mTarget,GL_TEXTURE_WRAP_T,GL_CLAMP);
						glTexParameteri(mTarget,GL_TEXTURE_WRAP_R,GL_CLAMP);
						glTexParameteri(mTarget,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
						glTexParameteri(mTarget,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
					
						int i;
						for(i=0;i<6;++i){
							glTexImage2D(GLRenderDevice::GLCubeFaces[i],level,glinternalFormat,width,height,0,glformat,gltype,data+slicePitch*i);
						}
					}break;
				#endif
				#if !defined(TOADLET_HAS_GLES)
					case GL_TEXTURE_3D:
						glTexImage3D(mTarget,level,glinternalFormat,width,height,depth,0,glformat,gltype,data);
					break;
				#endif
				#if !defined(TOADLET_HAS_GLES)
					case GL_TEXTURE_RECTANGLE_ARB:
						// Set up rectangle scale matrix
						Math::setMatrix4x4FromScale(mMatrix,mFormat->width,mFormat->height,Math::ONE);

						glTexImage2D(mTarget,level,glinternalFormat,width,height,0,glformat,gltype,data);
					break;
				#endif
			}
		}
		else{
			TOADLET_ASSERT(data!=NULL);
			
			switch(mTarget){
				#if !defined(TOADLET_HAS_GLES)
					case GL_TEXTURE_1D:
						glCompressedTexImage1D(mTarget,level,glinternalFormat,width,0,slicePitch,data);
					break;
				#endif
				case GL_TEXTURE_2D:
					glCompressedTexImage2D(mTarget,level,glinternalFormat,width,height,0,slicePitch,data);
				break;
				#if !defined(TOADLET_HAS_GLES) || defined(TOADLET_HAS_GL_20)
					case GL_TEXTURE_CUBE_MAP:{
						glTexParameteri(mTarget,GL_TEXTURE_WRAP_S,GL_CLAMP);
						glTexParameteri(mTarget,GL_TEXTURE_WRAP_T,GL_CLAMP);
						glTexParameteri(mTarget,GL_TEXTURE_WRAP_R,GL_CLAMP);
						glTexParameteri(mTarget,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
						glTexParameteri(mTarget,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
					
						int i;
						for(i=0;i<6;++i){
							glCompressedTexImage2D(GLRenderDevice::GLCubeFaces[i],level,glinternalFormat,width,height,0,slicePitch,data+slicePitch*i);
						}
					}break;
				#endif
				#if !defined(TOADLET_HAS_GLES)
					case GL_TEXTURE_3D:
						glCompressedTexImage3D(mTarget,level,glinternalFormat,width,height,depth,0,slicePitch,data);
					break;
				#endif
				#if !defined(TOADLET_HAS_GLES)
					case GL_TEXTURE_RECTANGLE_ARB:
						// Set up rectangle scale matrix
						Math::setMatrix4x4FromScale(mMatrix,mFormat->width,mFormat->height,Math::ONE);

						glCompressedTexImage2D(mTarget,level,glinternalFormat,width,height,0,slicePitch,data);
					break;
				#endif
			}
		}
	}

	TOADLET_CHECK_GLERROR("GLTexture::createContext");

	if(mipDatas!=NULL){
		generateMipLevels();
	}

	if(mHandle==0){
		Error::unknown("error in createContext");
		return false;
	}

	return true;
}

bool GLTexture::destroyContext(){
	int i;
	for(i=0;i<mBuffers.size();++i){
		mBuffers[i]->destroy();
	}

	if(mHandle!=0){
		glDeleteTextures(1,&mHandle);
		mHandle=0;
		mTarget=0;

		// Check this only if we had a handle, to eliminate errors at shutdown
		TOADLET_CHECK_GLERROR("GLTexture::destroy");
	}

	return true;
}

PixelBuffer::ptr GLTexture::getMipPixelBuffer(int level,int cubeSide){
	if(mHandle==0){
		return NULL;
	}

	int index=level;
	if(mFormat->dimension==TextureFormat::Dimension_CUBE){
		index=level*6+cubeSide;
	}

	if(mBuffers.size()<=index){
		mBuffers.resize(index+1);
	}

	if(mBuffers[index]==NULL){
		PixelBuffer::ptr buffer(new GLTextureMipPixelBuffer(const_cast<GLTexture*>(this),index,cubeSide));
		mBuffers[index]=buffer;
	}

	return mBuffers[index];
}

bool GLTexture::load(int width,int height,int depth,int mipLevel,tbyte *mipData){
	if(mHandle==0){
		return false;
	}

	width=width>0?width:1;height=height>0?height:1;depth=depth>0?depth:1;

	if(mipLevel==0 && (width!=mFormat->width || height!=mFormat->height || depth!=mFormat->depth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexture: Texture data of incorrect dimensions");
		return false;
	}

	glBindTexture(mTarget,mHandle);

	int pixelFormat=mFormat->pixelFormat;
	int rowPitch=ImageFormatConversion::getRowPitch(pixelFormat,width);
	int slicePitch=rowPitch*height;
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(slicePitch);
	GLint glformat=GLRenderDevice::getGLFormat(pixelFormat,false);
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(glformat);
	GLint gltype=ImageFormatConversion::isFormatCompressed(pixelFormat)==false?GLRenderDevice::getGLType(pixelFormat):0;
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(gltype);

	int alignment=1,pitch=rowPitch;
	while((pitch&1)==0){alignment<<=1;pitch>>=1;}
	glPixelStorei(GL_UNPACK_ALIGNMENT,alignment<8?alignment:8);

	if(ImageFormatConversion::isFormatCompressed(pixelFormat)==false){
		switch(mTarget){
			#if !defined(TOADLET_HAS_GLES)
				case GL_TEXTURE_1D:
					glTexSubImage1D(mTarget,mipLevel,0,width,glformat,gltype,mipData);
				break;
			#endif
			case GL_TEXTURE_2D:
				glTexSubImage2D(mTarget,mipLevel,0,0,width,height,glformat,gltype,mipData);
			break;
			#if !defined(TOADLET_HAS_GLES)
				case GL_TEXTURE_3D:
					glTexSubImage3D(mTarget,mipLevel,0,0,0,width,height,depth,glformat,gltype,mipData);
				break;
				case GL_TEXTURE_CUBE_MAP:
					for(int i=0;i<6;++i){
						glTexSubImage2D(GLRenderDevice::GLCubeFaces[i],mipLevel,0,0,width,height,glformat,gltype,mipData+slicePitch*i);
					}
				break;
				case GL_TEXTURE_RECTANGLE_ARB:
					glTexSubImage2D(mTarget,mipLevel,0,0,width,height,glformat,gltype,mipData);
				break;
			#endif
		}
	}
	else{
		switch(mTarget){
			#if !defined(TOADLET_HAS_GLES)
				case GL_TEXTURE_1D:
					glCompressedTexSubImage1D(mTarget,mipLevel,0,width,glformat,slicePitch,mipData);
				break;
			#endif
			case GL_TEXTURE_2D:
				glCompressedTexSubImage2D(mTarget,mipLevel,0,0,width,height,glformat,slicePitch,mipData);
			break;
			#if !defined(TOADLET_HAS_GLES)
				case GL_TEXTURE_3D:
					glCompressedTexSubImage3D(mTarget,mipLevel,0,0,0,width,height,depth,glformat,slicePitch,mipData);
				break;
				case GL_TEXTURE_CUBE_MAP:
					for(int i=0;i<6;++i){
						glCompressedTexSubImage2D(GLRenderDevice::GLCubeFaces[i],mipLevel,0,0,width,height,glformat,slicePitch,mipData+slicePitch*i);
					}
				break;
				case GL_TEXTURE_RECTANGLE_ARB:
					glCompressedTexSubImage2D(mTarget,mipLevel,0,0,width,height,glformat,slicePitch,mipData);
				break;
			#endif
		}
	}
	
	TOADLET_CHECK_GLERROR("GLTexture::load");

	generateMipLevels();

	return true;
}

bool GLTexture::read(int width,int height,int depth,int mipLevel,tbyte *mipData){
	if(mHandle==0){
		return false;
	}

	width=width>0?width:1;height=height>0?height:1;depth=depth>0?depth:1;

	if(mipLevel==0 && (width!=mFormat->width || height!=mFormat->height || depth!=mFormat->depth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexture: Texture data of incorrect dimensions");
		return false;
	}

	glBindTexture(mTarget,mHandle);

	int pixelFormat=mFormat->pixelFormat;
	int rowPitch=ImageFormatConversion::getRowPitch(pixelFormat,width);
	int slicePitch=rowPitch*height;
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(slicePitch);
	GLint glformat=GLRenderDevice::getGLFormat(pixelFormat,false);
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(glformat);
	GLint gltype=ImageFormatConversion::isFormatCompressed(pixelFormat)==false?GLRenderDevice::getGLType(pixelFormat):0;
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(gltype);

	int alignment=1,pitch=rowPitch;
	while((pitch&1)==0){alignment<<=1;pitch>>=1;}
	glPixelStorei(GL_PACK_ALIGNMENT,alignment<8?alignment:8);

	#if !defined(TOADLET_HAS_GLES)
		if(mTarget!=GL_TEXTURE_CUBE_MAP){
			glGetTexImage(mTarget,mipLevel,glformat,gltype,mipData);
		}
		else{
			int i;
			for(i=0;i<6;++i){
				glGetTexImage(GLRenderDevice::GLCubeFaces[i],mipLevel,glformat,gltype,mipData+slicePitch*i);
			}
		}

		TOADLET_CHECK_GLERROR("GLTexture::read");

		return true;
	#else
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"GLTexture::read is not supported");
		return false;
	#endif
} 

bool GLTexture::generateMipLevels(){
	if(mHandle==0 || mManuallyGenerateMipLevels==false){
		return false;
	}

	#if defined(TOADLET_HAS_GLEW) && defined(GL_EXT_framebuffer_object)
	if(GLEW_EXT_framebuffer_object){
		glBindTexture(mTarget,mHandle);
		// Set some items to make ATI cards happier
		glEnable(mTarget);
		glTexParameteri(mTarget,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(mTarget,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		glGenerateMipmap(mTarget);
		glDisable(mTarget);
		glBindTexture(mTarget,0);

		TOADLET_CHECK_GLERROR("GLTexture::generateMipLevels");

		return true;
	}
	else
	#endif
	{
		Logger::warning(Categories::TOADLET_PEEPER,
			"GLTexture::generateMipMaps: Not implemented");
		return false;
	}
}

GLuint GLTexture::getGLTarget(){
	switch(mFormat->dimension){
		#if !defined(TOADLET_HAS_GLES)
			case TextureFormat::Dimension_D1:
				return GL_TEXTURE_1D;
		#endif
		case TextureFormat::Dimension_D2:
			#if !defined(TOADLET_HAS_GLES)
				if((mUsage&Usage_BIT_NPOT_RESTRICTED)>0){
					return GL_TEXTURE_RECTANGLE_ARB;
				}
			#endif
			return GL_TEXTURE_2D;
		break;
		#if !defined(TOADLET_HAS_GLES)
			case TextureFormat::Dimension_D3:
				return GL_TEXTURE_3D;
			case TextureFormat::Dimension_CUBE:
				return GL_TEXTURE_CUBE_MAP;
		#endif
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"GLTexture::getGLTarget: Invalid target");
			return 0;
	}
}

}
}
