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

#include "GLRenderer.h"
#include "GLTexture.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

GLTexture::GLTexture(GLRenderer *renderer):BaseResource(),
	mRenderer(NULL),

	mUsage(0),
	mDimension(Dimension_UNKNOWN),
	mFormat(0),
	mWidth(0),
	mHeight(0),
	mDepth(0),
	mMipLevels(0),

	mHandle(0),
	mTarget(0),
	//mMatrix,
	mManuallyGenerateMipLevels(false)
	//mBuffers
{
	mRenderer=renderer;
}

GLTexture::~GLTexture(){
	destroy();
}

bool GLTexture::create(int usage,Dimension dimension,int format,int width,int height,int depth,int mipLevels,tbyte *mipDatas[]){
	if((Math::isPowerOf2(width)==false || Math::isPowerOf2(height)==false || (dimension!=Dimension_CUBE && Math::isPowerOf2(depth)==false)) &&
		mRenderer->getCapabilitySet().textureNonPowerOf2==false &&
		(mRenderer->getCapabilitySet().textureNonPowerOf2==false || (usage&Usage_BIT_NPOT_RESTRICTED)==0))
	{
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexture: Cannot load a non power of 2 texture");
		return false;
	}

	if(format!=mRenderer->getClosestTextureFormat(format)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexture: Invalid texture format");
		return false;
	}

	mUsage=usage;
	mDimension=dimension;
	mFormat=format;
	mWidth=width;
	mHeight=height;
	mDepth=depth;
	mMipLevels=mipLevels;

	createContext(mipLevels,mipDatas);

	return true;
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

	mManuallyGenerateMipLevels=(mUsage&Usage_BIT_AUTOGEN_MIPMAPS)>0;
	if(mManuallyGenerateMipLevels &&
		#if defined(TOADLET_HAS_GLES)
			mRenderer->gl_version>=11
		#else
			mRenderer->gl_version>=14
		#endif
	){
		glTexParameteri(mTarget,GL_GENERATE_MIPMAP,GL_TRUE);
		mManuallyGenerateMipLevels=false;
	}

	// If we don't support partial miplevel specification, then calculate the amount of levels we'll need
	int specifiedMipLevels=0;
	if(mMipLevels>0){
		/// @todo: Determine if GLES will let us do partial specifications anyway, and if so, enable that
		#if !defined(TOADLET_HAS_GLES)
			if(mRenderer->gl_version>=12){
				glTexParameteri(mTarget,GL_TEXTURE_BASE_LEVEL,0);
				glTexParameteri(mTarget,GL_TEXTURE_MAX_LEVEL,mMipLevels-1);
				specifiedMipLevels=mMipLevels;
			}
			else
		#endif
		{
			int hwidth=mWidth,hheight=mHeight,hdepth=mDepth;
			while(hwidth>1 || hheight>1 || hdepth>1){
				specifiedMipLevels++;
				hwidth/=2;hheight/=2;hdepth/=2;
				hwidth=hwidth>0?hwidth:1;hheight=hheight>0?hheight:1;hdepth=hdepth>0?hdepth:1;
			}

			if(specifiedMipLevels!=mMipLevels){
				Logger::debug(Categories::TOADLET_PEEPER,
					String("partial mipmap specification not supported.  calculated:")+specifiedMipLevels+" requested:"+mMipLevels);
			}
		}
	}

	GLint glinternalFormat=GLRenderer::getGLFormat(mFormat);
	GLint glformat=GLRenderer::getGLFormat(mFormat);
	GLint gltype=GLRenderer::getGLType(mFormat);

	// Allocate texture memory
	int level=0,width=mWidth,height=mHeight,depth=mDepth;
	for(level=0;level<=specifiedMipLevels;++level,width/=2,height/=2,depth/=2){
		int rowPitch=ImageFormatConversion::getRowPitch(mFormat,width);
 		int slicePitch=rowPitch*height;
		TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(slicePitch);

		tbyte *data=NULL;
		if(mipDatas!=NULL && (level==0 || level<mMipLevels)){
			data=mipDatas[level];
			int alignment=1,pitch=rowPitch;
			while(pitch>0 && (pitch&1)==0){alignment<<=1;pitch>>=1;}
			glPixelStorei(GL_PACK_ALIGNMENT,alignment<8?alignment:8);
		}

		if(ImageFormatConversion::isFormatCompressed(mFormat)==false){
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
							glTexImage2D(GLRenderer::GLCubeFaces[i],level,glinternalFormat,width,height,0,glformat,gltype,data+slicePitch*i);
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
						Math::setMatrix4x4FromScale(mMatrix,mWidth,mHeight,Math::ONE);

						glTexImage2D(mTarget,level,glinternalFormat,width,height,0,glformat,gltype,data);
					break;
				#endif
			}
		}
		else{
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
							glCompressedTexImage2D(GLRenderer::GLCubeFaces[i],level,glinternalFormat,width,height,0,slicePitch,data+slicePitch*i);
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
						Math::setMatrix4x4FromScale(mMatrix,mWidth,mHeight,Math::ONE);

						glCompressedTexImage2D(mTarget,level,glinternalFormat,width,height,0,slicePitch,data);
					break;
				#endif
			}
		}
	}

	if(mipDatas!=NULL && mManuallyGenerateMipLevels){
		generateMipLevels();
	}

	TOADLET_CHECK_GLERROR("GLTexture::createContext");
	
	return mHandle!=0;
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
	}

	TOADLET_CHECK_GLERROR("GLTexture::destroy");
	
	return true;
}

PixelBuffer::ptr GLTexture::getMipPixelBuffer(int level,int cubeSide){
	if(mHandle==0){
		return NULL;
	}

	int index=level;
	if(mDimension==Dimension_CUBE){
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

	if(mipLevel==0 && (width!=mWidth || height!=mHeight || depth!=mDepth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexture: Texture data of incorrect dimensions");
		return false;
	}

	glBindTexture(mTarget,mHandle);

	int format=mFormat;
	int rowPitch=ImageFormatConversion::getRowPitch(format,width);
	int slicePitch=rowPitch*height;
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(slicePitch);

	GLint glformat=GLRenderer::getGLFormat(format);
	GLint gltype=GLRenderer::getGLType(format);

	int alignment=1,pitch=rowPitch;
	while((pitch&1)==0){alignment<<=1;pitch>>=1;}
	glPixelStorei(GL_PACK_ALIGNMENT,alignment<8?alignment:8);

	if(ImageFormatConversion::isFormatCompressed(mFormat)){
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
						glTexSubImage2D(GLRenderer::GLCubeFaces[i],mipLevel,0,0,width,height,glformat,gltype,mipData+slicePitch*i);
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
						glCompressedTexSubImage2D(GLRenderer::GLCubeFaces[i],mipLevel,0,0,width,height,glformat,slicePitch,mipData+slicePitch*i);
					}
				break;
				case GL_TEXTURE_RECTANGLE_ARB:
					glCompressedTexSubImage2D(mTarget,mipLevel,0,0,width,height,glformat,slicePitch,mipData);
				break;
			#endif
		}
	}

	if(mManuallyGenerateMipLevels){
		generateMipLevels();
	}

	TOADLET_CHECK_GLERROR("GLTexture::load");

	return true;
}

bool GLTexture::read(int width,int height,int depth,int mipLevel,tbyte *mipData){
	if(mHandle==0){
		return false;
	}

	width=width>0?width:1;height=height>0?height:1;depth=depth>0?depth:1;

	if(mipLevel==0 && (width!=mWidth || height!=mHeight || depth!=mDepth)){
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexture: Texture data of incorrect dimensions");
		return false;
	}

	glBindTexture(mTarget,mHandle);

	int format=mFormat;
	int rowPitch=ImageFormatConversion::getRowPitch(format,width);
	int slicePitch=rowPitch*height;
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(slicePitch);
	GLint glformat=GLRenderer::getGLFormat(format);
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(glformat);
	GLint gltype=GLRenderer::getGLType(format);
	TOADLET_IGNORE_UNUSED_VARIABLE_WARNING(gltype);

	int alignment=1,pitch=rowPitch;
	while((pitch&1)==0){alignment<<=1;pitch>>=1;}
	glPixelStorei(GL_UNPACK_ALIGNMENT,alignment<8?alignment:8);

	#if !defined(TOADLET_HAS_GLES)
		if(mTarget!=GL_TEXTURE_CUBE_MAP){
			glGetTexImage(mTarget,mipLevel,glformat,gltype,mipData);
		}
		else{
			int i;
			for(i=0;i<6;++i){
				glGetTexImage(GLRenderer::GLCubeFaces[i],mipLevel,glformat,gltype,mipData+slicePitch*i);
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
	if(mHandle==0){
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
	switch(mDimension){
		#if !defined(TOADLET_HAS_GLES)
			case Texture::Dimension_D1:
				return GL_TEXTURE_1D;
		#endif
		case Texture::Dimension_D2:
			#if !defined(TOADLET_HAS_GLES)
				if((mUsage&Usage_BIT_NPOT_RESTRICTED)>0){
					return GL_TEXTURE_RECTANGLE_ARB;
				}
			#endif
			return GL_TEXTURE_2D;
		break;
		#if !defined(TOADLET_HAS_GLES)
			case Texture::Dimension_D3:
				return GL_TEXTURE_3D;
			case Texture::Dimension_CUBE:
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
