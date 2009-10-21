/********** Copyright header - do not remove **********
 *
 * The Toadlet Engine 
 *
 * Copyright (C) 2005, Lightning Toads Productions, LLC
 *
 * Author(s): Alan Fischer
 * 
 * All source code for the Toadlet Engine, including 
 * this file, is the sole property of Lightning Toads 
 * Productions, LLC. It has been developed on our own 
 * time, with our own tools, on our own hardware. None
 * of this code may be observed, used or changed 
 * without our explicit permission. 
 *
 *  - Lightning Toads Productions, LLC
 *
 ********** Copyright header - do not remove **********/

#include "NDSTexturePeer.h"
#include "../../../egg/Logger.h"

using namespace toadlet::egg;
using namespace toadlet::egg::image;

namespace toadlet{
namespace peeper{

NDSTexturePeer::NDSTexturePeer(Renderer *renderer,Texture *texture){
/*
	GLuint width=0;
	GLuint height=0;
	GLuint depth=0;
	GLuint format=0;
	GLuint internalFormat=0;
	GLuint type=0;

	const CapabilitySet &capabilitySet=renderer->getCapabilitySet();

	getTextureParameters(texture,capabilitySet,width,height,depth,textureDimension,internalFormat,format,type);

	glGenTextures(1,(GLuint*)&textureHandle);
	glBindTexture(textureDimension,textureHandle);

	Image *image=texture->getImage();
	if(image==NULL){
		TOADLET_ERROR(TOADLET_EGG_CATEGORY,Error::TYPE_UNKNOWN,
			"NDSTexturePeer: Null Image");
	}
	else{
		if(Math::isPowerOf2(width)==false || Math::isPowerOf2(height)==false){
			TOADLET_ERROR(TOADLET_EGG_CATEGORY,Error::TYPE_UNKNOWN,
				"NDSTexturePeer: Cannot load a non power of 2 texture");
		}
		else{
			glTexImage2D(textureDimension,0,internalFormat,width,height,0,format,type,image->getData());
		}
	}
*/
}

NDSTexturePeer::~NDSTexturePeer(){
/*
	if(textureHandle!=0){
		glDeleteTextures(1,&textureHandle);
		textureHandle=0;
	}
*/
}

void NDSTexturePeer::getTextureParameters(const Texture *texture,const CapabilitySet &capabilitySet,unsigned int &width,unsigned int &height,unsigned int &depth,int &dimension,int &internalFormat,int &format,int &type){
/*
	width=texture->getWidth();
	height=texture->getHeight();
	depth=texture->getDepth();

	switch(texture->getDimension()){
		case(Image::DIMENSION_2D):
			dimension=GL_TEXTURE_2D;
		break;
		default:
			TOADLET_LOG(TOADLET_PEEPER_CATEGORY,Logger::LEVEL_ERROR,
				"NDSTexturePeer: Invalid texture dimension");
	}

	const Image::Format &textureFormat=texture->getFormat();

	format=0;

	if(textureFormat&Image::FORMAT_BIT_RGB){
		format=GL_RGB;
	}
	else if(textureFormat&Image::FORMAT_BIT_RGBA){
		format=GL_RGBA;
	}

	type=0;

	if if(textureFormat&Image::FORMAT_BIT_UINT_5_6_5){
		type=GL_RGB;
	}
	else if(textureFormat&Image::FORMAT_BIT_UINT_5_5_5_1){
		type=GL_RGBA;
	}

	internalFormat=format;

	if(format==0 || type==0){
		TOADLET_ERROR(TOADLET_EGG_CATEGORY,Error::TYPE_UNKNOWN,
			"NDSTexturePeer::getTextureParameters: Invalid format or type");
	}
*/
}

}
}
