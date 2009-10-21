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
#include "GLTexturePeer.h"
#include <toadlet/egg/Error.h>
#include <toadlet/egg/Logger.h>

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

GLTexturePeer::GLTexturePeer(GLRenderer *renderer,Texture *texture):
	renderer(NULL),
	textureHandle(0),
	textureTarget(0),
	//textureMatrix,
	manuallyBuildMipMaps(false)
{
	this->renderer=renderer;
	const CapabilitySet &capabilitySet=renderer->getCapabilitySet();

	GLuint width=texture->getWidth();
	GLuint height=texture->getHeight();
	GLuint depth=texture->getDepth();
	textureTarget=getGLTarget(texture,capabilitySet);
	GLuint format=getGLFormat(texture->getFormat());
	GLuint internalFormat=format;
	GLuint type=getGLType(texture->getFormat());

	glGenTextures(1,&textureHandle);
	glBindTexture(textureTarget,textureHandle);

	#if defined(TOADLET_HAS_GLES)
		glTexParameterx(textureTarget,GL_TEXTURE_WRAP_S,getGLWrap(texture->getSAddressMode()));
		glTexParameterx(textureTarget,GL_TEXTURE_WRAP_T,getGLWrap(texture->getTAddressMode()));

		glTexParameterx(textureTarget,GL_TEXTURE_MIN_FILTER,getGLMinFilter(texture->getMinFilter(),texture->getMipFilter()));
		glTexParameterx(textureTarget,GL_TEXTURE_MAG_FILTER,getGLMagFilter(texture->getMagFilter()));
	#else
		bool hasClampToEdge=renderer->gl_version>=12;
		glTexParameteri(textureTarget,GL_TEXTURE_WRAP_S,getGLWrap(texture->getSAddressMode(),hasClampToEdge));
		glTexParameteri(textureTarget,GL_TEXTURE_WRAP_T,getGLWrap(texture->getTAddressMode(),hasClampToEdge));
		#if defined(TOADLET_HAS_GL_12)
		if(renderer->gl_version>=12){
			glTexParameteri(textureTarget,GL_TEXTURE_WRAP_R,getGLWrap(texture->getRAddressMode(),hasClampToEdge));
		}
		#endif

		glTexParameteri(textureTarget,GL_TEXTURE_MIN_FILTER,getGLMinFilter(texture->getMinFilter(),texture->getMipFilter()));
		glTexParameteri(textureTarget,GL_TEXTURE_MAG_FILTER,getGLMagFilter(texture->getMagFilter()));
	#endif

	manuallyBuildMipMaps=texture->getAutoGenerateMipMaps();
	if(manuallyBuildMipMaps){
		#if defined(TOADLET_HAS_GLES)
			#if defined(TOADLET_HAS_GL_11)
			if(renderer->gl_version>=11){
				glTexParameterx(textureTarget,GL_GENERATE_MIPMAP,GL_TRUE);
				manuallyBuildMipMaps=false;
			}
			#endif
		#else
			#if defined(TOADLET_HAS_GL_14)
			if(renderer->gl_version>=14){
				glTexParameteri(textureTarget,GL_GENERATE_MIPMAP,GL_TRUE);
				manuallyBuildMipMaps=false;
			}
			#endif
		#endif
	}

	Image *image=texture->getImage();
	if(image!=NULL){
		#if !defined(TOADLET_HAS_GLES)
			if(textureTarget==GL_TEXTURE_CUBE_MAP){
				glTexParameteri(textureTarget,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
				glTexParameteri(textureTarget,GL_TEXTURE_MIN_FILTER,GL_LINEAR);

				int targets[6]={
					GL_TEXTURE_CUBE_MAP_POSITIVE_X,
					GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
					GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
					GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
					GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
					GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
				};

				int i;
				for(i=0;i<6;++i){
					uint8 *data=image->getData()+(image->getWidth()*image->getHeight()*image->getPixelSize()*i);
					glTexImage2D(targets[i],0,internalFormat,width,height,0,format,type,data);
				}
			}
			else if(textureTarget==GL_TEXTURE_RECTANGLE_ARB){
				glTexImage2D(textureTarget,0,internalFormat,width,height,0,format,type,image->getData());

				Math::setMatrix4x4FromScale(textureMatrix,width,height,Math::ONE);
			}
			else
		#endif
		{
			if(	(math::Math::isPowerOf2(width)==false || math::Math::isPowerOf2(height)==false || math::Math::isPowerOf2(depth)==false) &&
				capabilitySet.textureNonPowerOf2==false)
			{
				Error::unknown(Categories::TOADLET_PEEPER,
					"GLTexturePeer: Cannot load a non power of 2 texture");
				return;
			}
			else{
				switch(textureTarget){
					#if !defined(TOADLET_HAS_GLES)
						case GL_TEXTURE_1D:
							glTexImage1D(textureTarget,0,internalFormat,width,0,format,type,image->getData());
						break;
					#endif
					case GL_TEXTURE_2D:
						glTexImage2D(textureTarget,0,internalFormat,width,height,0,format,type,image->getData());
					break;
					#if !defined(TOADLET_HAS_GLES)
						case GL_TEXTURE_3D:
							glTexImage3D(textureTarget,0,internalFormat,width,height,depth,0,format,type,image->getData());
						break;
					#endif
				}
			}

			if(manuallyBuildMipMaps){
				generateMipMaps();
			}
		}
	}

	TOADLET_CHECK_GLERROR("GLTexturePeer::GLTexturePeer");
}

GLTexturePeer::~GLTexturePeer(){
	if(textureHandle!=0){
		glDeleteTextures(1,&textureHandle);
		textureHandle=0;
	}

	// Checking of proper deletion removed to avoid errors when a resource is destroyed after the context is destroyed
	//  There should be no leaks in this case, since the context should clean up any resources it has
	//TOADLET_CHECK_GLERROR("GLTexturePeer::~GLTexturePeer");
}

void GLTexturePeer::generateMipMaps(){
glEnable(GL_TEXTURE_2D);
glGenerateMipmapEXT(GL_TEXTURE_2D);
	// TODO: Get this working without using GLEW to check for the object, for now we'll just rely on GL_GENERATE_MIPMAPS
	#if 0 && defined(GL_EXT_framebuffer_object)
	if(GLEW_EXT_framebuffer_object){
		// Set some items to make ATI cards happier
		glEnable(textureTarget);
		glTexParameteri(textureTarget,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(textureTarget,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

		glBindTexture(textureTarget,textureHandle);
		glGenerateMipmap(textureTarget);
		glBindTexture(textureTarget,0);
	}
	else
	#endif
	{
		Logger::log(Categories::TOADLET_PEEPER,Logger::Level_WARNING,
			"GLTexturePeer::generateMipMaps: Not implemented");
	}
}

GLuint GLTexturePeer::getGLTarget(Texture *texture,const CapabilitySet &capabilitySet){
	Texture::Dimension dimension=texture->getDimension();
	#if !defined(TOADLET_HAS_GLES)
		int width=texture->getWidth();
		int	height=texture->getHeight();
	#endif
	GLuint target=0;

	switch(dimension){
		#if !defined(TOADLET_HAS_GLES)
			case Texture::Dimension_D1:
				target=GL_TEXTURE_1D;
			break;
		#endif
			case Texture::Dimension_D2:
			#if !defined(TOADLET_HAS_GLES)
				if(	(math::Math::isPowerOf2(width)==false || math::Math::isPowerOf2(height))==false &&
					capabilitySet.textureNonPowerOf2==false && capabilitySet.textureNonPowerOf2Restricted==true &&
					texture->getMipFilter()==Texture::Filter_NONE &&
					texture->getSAddressMode()==Texture::AddressMode_CLAMP_TO_EDGE &&
					texture->getTAddressMode()==Texture::AddressMode_CLAMP_TO_EDGE)
				{
					target=GL_TEXTURE_RECTANGLE_ARB;
				}
				else{
			#endif
					target=GL_TEXTURE_2D;
			#if !defined(TOADLET_HAS_GLES)
				}
			#endif
		break;
		#if !defined(TOADLET_HAS_GLES)
			case Texture::Dimension_D3:
				target=GL_TEXTURE_3D;
			break;
			case Texture::Dimension_CUBEMAP:
				target=GL_TEXTURE_CUBE_MAP;
			break;
		#endif
		default:
		break;
	}
	
	return target;
}

GLuint GLTexturePeer::getGLFormat(int textureFormat){
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
			"GLTexturePeer::getGLFormat: Invalid format");
		return 0;
	}

	return format;
}

GLuint GLTexturePeer::getGLType(int textureFormat){
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
			"GLTexturePeer::getGLType: Invalid type");
		return 0;
	}

	return type;
}

GLuint GLTexturePeer::getGLWrap(Texture::AddressMode addressMode,bool hasClampToEdge){
	GLuint wrap=0;

	switch(addressMode){
		case Texture::AddressMode_REPEAT:
			wrap=GL_REPEAT;
		break;
		#if !defined(TOADLET_HAS_GLES)
			case Texture::AddressMode_CLAMP_TO_BORDER:
				wrap=GL_CLAMP_TO_BORDER;
			break;
			case Texture::AddressMode_MIRRORED_REPEAT:
				wrap=GL_MIRRORED_REPEAT;
			break;
			case Texture::AddressMode_CLAMP_TO_EDGE:
				wrap=hasClampToEdge?GL_CLAMP_TO_EDGE:GL_CLAMP;
			break;
		#else
			case Texture::AddressMode_CLAMP_TO_EDGE:
				wrap=GL_CLAMP_TO_EDGE;
			break;
		#endif
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"GLTexturePeer::getGLWrap: Invalid address mode");
			return 0;
		break;
	}

	return wrap;
}

GLuint GLTexturePeer::getGLMinFilter(Texture::Filter minFilter,Texture::Filter mipFilter){
	GLuint filter=0;

	switch(mipFilter){
		case Texture::Filter_NONE:
			switch(minFilter){
				case Texture::Filter_NEAREST:
					filter=GL_NEAREST;
				break;
				case Texture::Filter_LINEAR:
					filter=GL_LINEAR;
				break;
				default:
				break;
			}
		break;
		case Texture::Filter_NEAREST:
			switch(minFilter){
				case Texture::Filter_NEAREST:
					filter=GL_NEAREST_MIPMAP_NEAREST;
				break;
				case Texture::Filter_LINEAR:
					filter=GL_NEAREST_MIPMAP_LINEAR;
				break;
				default:
				break;
			}
		break;
		case Texture::Filter_LINEAR:
			switch(minFilter){
				case Texture::Filter_NEAREST:
					filter=GL_LINEAR_MIPMAP_NEAREST;
				break;
				case Texture::Filter_LINEAR:
					filter=GL_LINEAR_MIPMAP_LINEAR;
				break;
				default:
				break;
			}
		break;
	}

	if(filter==0){
		Error::unknown(Categories::TOADLET_PEEPER,
			"GLTexturePeer::getGLMinFilter: Invalid filter");
		return 0;
	}

	return filter;
}

GLuint GLTexturePeer::getGLMagFilter(Texture::Filter magFilter){
	GLuint filter=0;

	switch(magFilter){
		case Texture::Filter_NEAREST:
			filter=GL_NEAREST;
		break;
		case Texture::Filter_LINEAR:
			filter=GL_LINEAR;
		break;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"GLTexturePeer::getGLMagFilter: Invalid filter");
			return 0;
		break;
	}

	return filter;
}

GLuint GLTexturePeer::getGLTextureBlendSource(TextureBlend::Source blend){
	switch(blend){
		case TextureBlend::Source_PREVIOUS:
			return GL_PREVIOUS;
		case TextureBlend::Source_TEXTURE:
			return GL_TEXTURE;
		case TextureBlend::Source_PRIMARY_COLOR:
			return GL_PRIMARY_COLOR;
		default:
			Error::unknown(Categories::TOADLET_PEEPER,
				"GLTexturePeer::getGLTextureBlendSource: Invalid source");
			return 0;
	}
}

}
}
