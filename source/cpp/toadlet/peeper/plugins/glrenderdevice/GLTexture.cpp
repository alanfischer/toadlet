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
	if(format->isPowerOf2()==false &&
		mDevice->mNPOT==false && (mDevice->mNPOTR==false || (usage&Usage_BIT_NPOT_RESTRICTED)==0))
	{
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexture: Cannot load a non power of 2 texture");
		return false;
	}

	mUsage=usage;
	mFormat=format;

	bool result=createContext(format->getMipMax(),mipDatas);

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

	int specifiedMipLevels=mipLevels>0?mipLevels:mFormat->getMipMaxPossible();

	// If we don't support partial miplevel specification, then calculate the amount of levels we'll need
	#if !defined(TOADLET_HAS_GLES)
		if(mDevice->gl_version>=12){
			glTexParameteri(mTarget,GL_TEXTURE_BASE_LEVEL,0);
			glTexParameteri(mTarget,GL_TEXTURE_MAX_LEVEL,specifiedMipLevels-1);
		}
	#endif
/*
	/// @todo: Determine if GLES will let us do partial specifications anyway, and if so, enable that
	if(totalMipLevels!=mFormat->mipLevels && mFormat->mipLevels>0){
		specifiedMipLevels=totalMipLevels;

		Logger::debug(Categories::TOADLET_PEEPER,
			String("partial mipmap specification not supported.  calculated:")+totalMipLevels+" requested:"+mFormat->mipLevels);
	}
*/
	// Always need to specify at least 1 level
	if(specifiedMipLevels==0){
		specifiedMipLevels=1;
	}

	int pixelFormat=mFormat->getPixelFormat();
	GLint glinternalFormat=GLRenderDevice::getGLFormat(pixelFormat,true);
	GLint glformat=GLRenderDevice::getGLFormat(pixelFormat,false);
	GLint gltype=GLRenderDevice::getGLTextureCompressed(pixelFormat)==false?GLRenderDevice::getGLType(pixelFormat):0;

	if(glinternalFormat==0 || glformat==0){
		return false;
	}

	// Allocate texture memory
	int level=0;
	for(level=0;level<specifiedMipLevels;++level){
		TextureFormat::ptr format(new TextureFormat(mFormat,level));
		int width=format->getWidth(),height=format->getHeight(),depth=format->getDepth();
		int xPitch=format->getXPitch();
		int yPitch=format->getYPitch();

		tbyte *data=NULL;
		if(mipDatas!=NULL && (level==0 || level<mipLevels)){
			data=mipDatas[level];
			int alignment=1,pitch=xPitch;
			while(pitch>0 && (pitch&1)==0){alignment<<=1;pitch>>=1;}
			glPixelStorei(GL_UNPACK_ALIGNMENT,alignment<8?alignment:8);
		}
		else{
			if(level>0 && onlyFirstLevel) break;
		}

		if(GLRenderDevice::getGLTextureCompressed(mFormat->getPixelFormat())==false){
			switch(mTarget){
				#if defined(GL_TEXTURE_1D)
					case GL_TEXTURE_1D:
						glTexImage1D(mTarget,level,glinternalFormat,width,0,glformat,gltype,data);
					break;
				#endif
				case GL_TEXTURE_2D:
					glTexImage2D(mTarget,level,glinternalFormat,width,height,0,glformat,gltype,data);
				break;
				#if defined(GL_TEXTURE_CUBE_MAP)
					case GL_TEXTURE_CUBE_MAP:{
						glTexParameteri(mTarget,GL_TEXTURE_WRAP_S,GL_CLAMP);
						glTexParameteri(mTarget,GL_TEXTURE_WRAP_T,GL_CLAMP);
						glTexParameteri(mTarget,GL_TEXTURE_WRAP_R,GL_CLAMP);
						glTexParameteri(mTarget,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
						glTexParameteri(mTarget,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
					
						int i;
						for(i=0;i<6;++i){
							glTexImage2D(GLRenderDevice::GLCubeFaces[i],level,glinternalFormat,width,height,0,glformat,gltype,data+yPitch*i);
						}
					}break;
				#endif
				#if defined(GL_TEXTURE_3D)
					case GL_TEXTURE_3D:
						glTexImage3D(mTarget,level,glinternalFormat,width,height,depth,0,glformat,gltype,data);
					break;
				#endif
				#if defined(GL_TEXTURE_RECTANGLE_ARB)
					case GL_TEXTURE_RECTANGLE_ARB:
						glTexImage2D(mTarget,level,glinternalFormat,width,height,0,glformat,gltype,data);
					break;
				#endif
			}
		}
		else{
			TOADLET_ASSERT(data!=NULL);
			
			switch(mTarget){
				#if defined(GL_TEXTURE_1D)
					case GL_TEXTURE_1D:
						glCompressedTexImage1D(mTarget,level,glinternalFormat,width,0,yPitch,data);
					break;
				#endif
				case GL_TEXTURE_2D:
					glCompressedTexImage2D(mTarget,level,glinternalFormat,width,height,0,yPitch,data);
				break;
				#if defined(GL_TEXTURE_CUBE_MAP)
					case GL_TEXTURE_CUBE_MAP:{
						glTexParameteri(mTarget,GL_TEXTURE_WRAP_S,GL_CLAMP);
						glTexParameteri(mTarget,GL_TEXTURE_WRAP_T,GL_CLAMP);
						glTexParameteri(mTarget,GL_TEXTURE_WRAP_R,GL_CLAMP);
						glTexParameteri(mTarget,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
						glTexParameteri(mTarget,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
					
						int i;
						for(i=0;i<6;++i){
							glCompressedTexImage2D(GLRenderDevice::GLCubeFaces[i],level,glinternalFormat,width,height,0,yPitch,data+yPitch*i);
						}
					}break;
				#endif
				#if defined(GL_TEXTURE_3D)
					case GL_TEXTURE_3D:
						glCompressedTexImage3D(mTarget,level,glinternalFormat,width,height,depth,0,yPitch,data);
					break;
				#endif
				#if defined(GL_TEXTURE_RECTANGLE_ARB)
					case GL_TEXTURE_RECTANGLE_ARB:
						glCompressedTexImage2D(mTarget,level,glinternalFormat,width,height,0,yPitch,data);
					break;
				#endif
			}
		}
	}

	#if defined(GL_TEXTURE_RECTANGLE_ARB)
		if(mTarget==GL_TEXTURE_RECTANGLE_ARB){
			// Set up rectangle scale matrix
			Math::setMatrix4x4FromScale(mMatrix,mFormat->getWidth(),mFormat->getHeight(),mFormat->getDepth());
		}
	#endif

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
	if(mFormat->getDimension()==TextureFormat::Dimension_CUBE){
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

bool GLTexture::load(TextureFormat *format,tbyte *data){
	mDevice->activate(); // Not ideal, but lets us use GLTexture::load with multiple contexts freely, perhaps should be rethought

	if(mHandle==0){
		return false;
	}

	TOADLET_ASSERT(format->isSectionOf(mFormat));

	glBindTexture(mTarget,mHandle);

	int pixelFormat=format->getPixelFormat();
	int x=format->getXMin(),y=format->getYMin(),z=format->getZMin();
	int width=format->getWidth(),height=format->getHeight(),depth=format->getDepth();
	int xPitch=format->getXPitch();
	int yPitch=format->getYPitch();
	int mipLevel=format->getMipMin();
	GLint glformat=GLRenderDevice::getGLFormat(pixelFormat,false);
	GLint gltype=GLRenderDevice::getGLTextureCompressed(pixelFormat)==false?GLRenderDevice::getGLType(pixelFormat):0;

	int alignment=1,pitch=xPitch;
	while((pitch&1)==0){alignment<<=1;pitch>>=1;}
	glPixelStorei(GL_UNPACK_ALIGNMENT,alignment<8?alignment:8);

	if(GLRenderDevice::getGLTextureCompressed(pixelFormat)==false){
		switch(mTarget){
			#if !defined(TOADLET_HAS_GLES)
				case GL_TEXTURE_1D:
					glTexSubImage1D(mTarget,mipLevel,x,width,glformat,gltype,data);
				break;
			#endif
			case GL_TEXTURE_2D:
				glTexSubImage2D(mTarget,mipLevel,x,y,width,height,glformat,gltype,data);
			break;
			#if !defined(TOADLET_HAS_GLES)
				case GL_TEXTURE_3D:
					glTexSubImage3D(mTarget,mipLevel,x,y,z,width,height,depth,glformat,gltype,data);
				break;
				case GL_TEXTURE_CUBE_MAP:
					for(int i=0;i<6;++i){
						glTexSubImage2D(GLRenderDevice::GLCubeFaces[i],mipLevel,x,y,width,height,glformat,gltype,data+yPitch*i);
					}
				break;
				case GL_TEXTURE_RECTANGLE_ARB:
					glTexSubImage2D(mTarget,mipLevel,x,y,width,height,glformat,gltype,data);
				break;
			#endif
		}
	}
	else{
		switch(mTarget){
			#if !defined(TOADLET_HAS_GLES)
				case GL_TEXTURE_1D:
					glCompressedTexSubImage1D(mTarget,mipLevel,x,width,glformat,yPitch,data);
				break;
			#endif
			case GL_TEXTURE_2D:
				glCompressedTexSubImage2D(mTarget,mipLevel,x,y,width,height,glformat,yPitch,data);
			break;
			#if !defined(TOADLET_HAS_GLES)
				case GL_TEXTURE_3D:
					glCompressedTexSubImage3D(mTarget,mipLevel,x,y,z,width,height,depth,glformat,yPitch,data);
				break;
				case GL_TEXTURE_CUBE_MAP:
					for(int i=0;i<6;++i){
						glCompressedTexSubImage2D(GLRenderDevice::GLCubeFaces[i],mipLevel,x,y,width,height,glformat,yPitch,data+yPitch*i);
					}
				break;
				case GL_TEXTURE_RECTANGLE_ARB:
					glCompressedTexSubImage2D(mTarget,mipLevel,x,y,width,height,glformat,yPitch,data);
				break;
			#endif
		}
	}
	
	TOADLET_CHECK_GLERROR("GLTexture::load");

	generateMipLevels();

	return true;
}

bool GLTexture::read(TextureFormat *format,tbyte *data){
	mDevice->activate(); // Not ideal, but lets us use GLTexture::load with multiple contexts freely, perhaps should be rethought

	if(mHandle==0){
		return false;
	}

	if(format->getXMin()!=0 || format->getYMin()!=0 || format->getZMin()!=0){
		Error::invalidParameters(Categories::TOADLET_PEEPER,
			"can not read sub sections");
		return false;
	}

	glBindTexture(mTarget,mHandle);

	int pixelFormat=format->getPixelFormat();
	int xPitch=format->getXPitch();
	int yPitch=format->getYPitch();
	int mipLevel=format->getMipMin();
	GLint glformat=GLRenderDevice::getGLFormat(pixelFormat,false);
	GLint gltype=GLRenderDevice::getGLTextureCompressed(pixelFormat)==false?GLRenderDevice::getGLType(pixelFormat):0;

	int alignment=1,pitch=xPitch;
	while((pitch&1)==0){alignment<<=1;pitch>>=1;}
	glPixelStorei(GL_PACK_ALIGNMENT,alignment<8?alignment:8);

	#if !defined(TOADLET_HAS_GLES)
		if(mTarget!=GL_TEXTURE_CUBE_MAP){
			glGetTexImage(mTarget,mipLevel,glformat,gltype,data);
		}
		else{
			int i;
			for(i=0;i<6;++i){
				glGetTexImage(GLRenderDevice::GLCubeFaces[i],mipLevel,glformat,gltype,data+yPitch*i);
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
	switch(mFormat->getDimension()){
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
