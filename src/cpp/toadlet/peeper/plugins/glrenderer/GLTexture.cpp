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

GLTexture::GLTexture(GLRenderer *renderer):
	mRenderer(NULL),

	mUsageFlags(UsageFlags_NONE),
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
	//mSurfaces
{
	mRenderer=renderer;
}

GLTexture::~GLTexture(){
	if(mHandle!=0){
		destroy();
	}
}

bool GLTexture::create(int usageFlags,Dimension dimension,int format,int width,int height,int depth,int mipLevels){
	destroy();

	if((Math::isPowerOf2(width)==false || Math::isPowerOf2(height)==false || Math::isPowerOf2(depth)==false) &&
		mRenderer->getCapabilitySet().textureNonPowerOf2==false &&
		(mRenderer->getCapabilitySet().textureNonPowerOf2==false || (usageFlags&UsageFlags_NPOT_RESTRICTED)==0))
	{
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexture: Cannot load a non power of 2 texture");
		return false;
	}

	if(mipLevels==0){
		int w=width,h=height;
		while(w>0 && h>0){
			mipLevels++;
			w/=2; h/=2;
		}
	}

	mUsageFlags=usageFlags;
	mDimension=dimension;
	mFormat=format;
	mWidth=width;
	mHeight=height;
	mDepth=depth;
	mMipLevels=mipLevels;

	// Create texture
	mTarget=getGLTarget();
	glGenTextures(1,&mHandle);
	glBindTexture(mTarget,mHandle);

	// Set defaults for now
	// TODO: See if we want to add these as texture parameters to the texture?  Or just leave them to the textureStages?
	glTexParameteri(mTarget,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(mTarget,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(mTarget,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
	glTexParameteri(mTarget,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

	// Allocate space for texture
	GLint glformat=getGLFormat(format);
	GLint glinternalFormat=glformat;
	GLint gltype=getGLType(format);
	switch(mTarget){
		#if !defined(TOADLET_HAS_GLES)
			case GL_TEXTURE_1D:
				glTexImage1D(mTarget,0,glinternalFormat,width,0,glformat,gltype,NULL);
			break;
		#endif
		case GL_TEXTURE_2D:
			glTexImage2D(mTarget,0,glinternalFormat,width,height,0,glformat,gltype,NULL);
		break;
		#if !defined(TOADLET_HAS_GLES)
			case GL_TEXTURE_3D:
				glTexImage3D(mTarget,0,glinternalFormat,width,height,depth,0,glformat,gltype,NULL);
			break;
			case GL_TEXTURE_CUBE_MAP:{
				// TODO: Is the following required?
				glTexParameteri(mTarget,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(mTarget,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

				int i;
				for(i=0;i<6;++i){
					glTexImage2D(GLCubeFaces[i],0,glinternalFormat,width,height,0,glformat,gltype,NULL);
				}
			}break;
			case GL_TEXTURE_RECTANGLE_ARB:
				// Set up rectangle scale matrix
				Math::setMatrix4x4FromScale(mMatrix,width,height,Math::ONE);

				glTexImage2D(mTarget,0,glinternalFormat,width,height,0,glformat,gltype,NULL);
			break;
		#endif
	}

	mManuallyGenerateMipLevels=(mUsageFlags&UsageFlags_AUTOGEN_MIPMAPS)>0;
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

	TOADLET_CHECK_GLERROR("GLTexture::create");

	return true;
}

void GLTexture::destroy(){
	if(mHandle!=0){
		glDeleteTextures(1,&mHandle);
		mHandle=0;

		TOADLET_CHECK_GLERROR("GLTexture::destroy");
	}
}

Surface::ptr GLTexture::getMipSuface(int i) const{
	return Surface::ptr(new GLTextureMipSurface(const_cast<GLTexture*>(this),i));
}

bool GLTexture::load(int format,int width,int height,int depth,uint8 *data){
	if(width!=mWidth || height!=mHeight || depth!=mDepth){
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexture: Texture data of incorrect dimensions");
		return false;
	}

	glBindTexture(mTarget,mHandle);

	GLint glformat=getGLFormat(format);
	GLint gltype=getGLType(format);
	switch(mTarget){
		#if !defined(TOADLET_HAS_GLES)
			case GL_TEXTURE_1D:
				glTexSubImage1D(mTarget,0,0,width,glformat,gltype,data);
			break;
		#endif
		case GL_TEXTURE_2D:
			glTexSubImage2D(mTarget,0,0,0,width,height,glformat,gltype,data);
		break;
		#if !defined(TOADLET_HAS_GLES)
			case GL_TEXTURE_3D:
				glTexSubImage3D(mTarget,0,0,0,0,width,height,depth,glformat,gltype,data);
			break;
			case GL_TEXTURE_CUBE_MAP:
				for(int i=0;i<6;++i){
					glTexSubImage2D(GLCubeFaces[i],0,0,0,width,height,glformat,gltype,
						data+(width*height*ImageFormatConversion::getPixelSize(format)*i));
				}
			break;
			case GL_TEXTURE_RECTANGLE_ARB:
				glTexSubImage2D(mTarget,0,0,0,width,height,glformat,gltype,data);
			break;
		#endif
	}

	if(mManuallyGenerateMipLevels){
		generateMipLevels();
	}

	return true;
}

bool GLTexture::read(int format,int width,int height,int depth,uint8 *data){
	#if !defined(TOADLET_HAS_GLES)
		GLint glformat=getGLFormat(format);
		GLint gltype=getGLType(format);

		glGetTexImage(mTarget,0,glformat,gltype,data);

		return true;
	#else
		Error::unimplemented(Categories::TOADLET_PEEPER,
			"GLTexture::read is not supported");
		return false;
	#endif
}

void GLTexture::generateMipLevels(){
	#if defined(TOADLET_HAS_GLEW) && defined(GL_EXT_framebuffer_object)
	if(GLEW_EXT_framebuffer_object){
		// Set some items to make ATI cards happier
		glEnable(mTarget);
		glTexParameteri(mTarget,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(mTarget,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		glBindTexture(mTarget,mHandle);
		glGenerateMipmap(mTarget);
		glBindTexture(mTarget,0);
	}
	else
	#endif
		Logger::log(Categories::TOADLET_PEEPER,Logger::Level_WARNING,
			"GLTexture::generateMipMaps: Not implemented");
}

GLuint GLTexture::getGLTarget(){
	switch(mDimension){
		#if !defined(TOADLET_HAS_GLES)
			case Texture::Dimension_D1:
				return GL_TEXTURE_1D;
			break;
		#endif
		case Texture::Dimension_D2:
			#if !defined(TOADLET_HAS_GLES)
				if((mUsageFlags&UsageFlags_NPOT_RESTRICTED)>0){
					return GL_TEXTURE_RECTANGLE_ARB;
				}
			#endif
			return GL_TEXTURE_2D;
		break;
		#if !defined(TOADLET_HAS_GLES)
			case Texture::Dimension_D3:
				return GL_TEXTURE_3D;
			break;
			case Texture::Dimension_CUBEMAP:
				return GL_TEXTURE_CUBE_MAP;
			break;
		#endif
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"GLTexture::getGLTarget: Invalid target");
			return 0;
	}
}

GLuint GLTexture::getGLFormat(int textureFormat){
	GLuint format=0;
	
	if((textureFormat&Texture::Format_BIT_L)>0){
		format=GL_LUMINANCE;
	}
	else if((textureFormat&Texture::Format_BIT_A)>0){
		format=GL_ALPHA;
	}
	else if((textureFormat&Texture::Format_BIT_LA)>0){
		format=GL_LUMINANCE_ALPHA;
	}
	else if((textureFormat&Texture::Format_BIT_RGB)>0){
		format=GL_RGB;
	}
	else if((textureFormat&Texture::Format_BIT_RGBA)>0){
		format=GL_RGBA;
	}
	#if !defined(TOADLET_HAS_GLES)
		else if((textureFormat&Texture::Format_BIT_DEPTH)>0){
			format=GL_DEPTH_COMPONENT;
			// We just leave the internalFormat as GL_DEPTH_COMPONENT, otherwise it seems it needs to do
			// a conversion if the formats dont match up, and that gets really slow in a copy to texture
		}
	#endif

	if(format==0){
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexture::getGLFormat: Invalid format");
		return 0;
	}

	return format;
}

GLuint GLTexture::getGLType(int textureFormat){
	GLuint type=0;

	if((textureFormat&Texture::Format_BIT_UINT_8)>0){
		type=GL_UNSIGNED_BYTE;
	}
	else if((textureFormat&Texture::Format_BIT_FLOAT_32)>0){
		type=GL_FLOAT;
	}
	else if((textureFormat&Texture::Format_BIT_UINT_5_6_5)>0){
		type=GL_UNSIGNED_SHORT_5_6_5;
	}
	else if((textureFormat&Texture::Format_BIT_UINT_5_5_5_1)>0){
		type=GL_UNSIGNED_SHORT_5_5_5_1;
	}
	else if((textureFormat&Texture::Format_BIT_UINT_4_4_4_4)>0){
		type=GL_UNSIGNED_SHORT_4_4_4_4;
	}

	if(type==0){
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexture::getGLType: Invalid type");
		return 0;
	}

	return type;
}

GLuint GLTexture::getGLWrap(TextureStage::AddressMode addressMode,bool hasClampToEdge){
	switch(addressMode){
		case TextureStage::AddressMode_REPEAT:
			return GL_REPEAT;
		#if !defined(TOADLET_HAS_GLES)
			case TextureStage::AddressMode_CLAMP_TO_BORDER:
				return GL_CLAMP_TO_BORDER;
			case TextureStage::AddressMode_MIRRORED_REPEAT:
				return GL_MIRRORED_REPEAT;
			case TextureStage::AddressMode_CLAMP_TO_EDGE:
				return hasClampToEdge?GL_CLAMP_TO_EDGE:GL_CLAMP;
		#else
			case TextureStage::AddressMode_CLAMP_TO_EDGE:
				return GL_CLAMP_TO_EDGE;
		#endif
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"GLTexture::getGLWrap: Invalid address mode");
			return 0;
	}
}

GLuint GLTexture::getGLMinFilter(TextureStage::Filter minFilter,TextureStage::Filter mipFilter){
	GLuint filter=0;

	switch(mipFilter){
		case TextureStage::Filter_NONE:
			switch(minFilter){
				case TextureStage::Filter_NEAREST:
					filter=GL_NEAREST;
				break;
				case TextureStage::Filter_LINEAR:
					filter=GL_LINEAR;
				break;
				default:
				break;
			}
		break;
		case TextureStage::Filter_NEAREST:
			switch(minFilter){
				case TextureStage::Filter_NEAREST:
					filter=GL_NEAREST_MIPMAP_NEAREST;
				break;
				case TextureStage::Filter_LINEAR:
					filter=GL_NEAREST_MIPMAP_LINEAR;
				break;
				default:
				break;
			}
		break;
		case TextureStage::Filter_LINEAR:
			switch(minFilter){
				case TextureStage::Filter_NEAREST:
					filter=GL_LINEAR_MIPMAP_NEAREST;
				break;
				case TextureStage::Filter_LINEAR:
					filter=GL_LINEAR_MIPMAP_LINEAR;
				break;
				default:
				break;
			}
		break;
	}

	if(filter==0){
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexture::getGLMinFilter: Invalid filter");
		return 0;
	}

	return filter;
}

GLuint GLTexture::getGLMagFilter(TextureStage::Filter magFilter){
	switch(magFilter){
		case TextureStage::Filter_NEAREST:
			return GL_NEAREST;
		case TextureStage::Filter_LINEAR:
			return GL_LINEAR;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"GLTexture::getGLMagFilter: Invalid filter");
			return 0;
	}
}

GLuint GLTexture::getGLTextureBlendSource(TextureBlend::Source blend){
	switch(blend){
		case TextureBlend::Source_PREVIOUS:
			return GL_PREVIOUS;
		case TextureBlend::Source_TEXTURE:
			return GL_TEXTURE;
		case TextureBlend::Source_PRIMARY_COLOR:
			return GL_PRIMARY_COLOR;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"GLTexture::getGLTextureBlendSource: Invalid source");
			return 0;
	}
}

#if !defined(TOADLET_HAS_GLES)
	GLuint GLTexture::GLCubeFaces[6]={
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
#endif

}
}
