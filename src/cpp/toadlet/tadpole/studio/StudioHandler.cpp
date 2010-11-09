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

#include <toadlet/egg/Error.h>
#include <toadlet/tadpole/studio/StudioHandler.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace studio{

StudioHandler::StudioHandler(Engine *engine){
	mEngine=engine;
}

StudioHandler::~StudioHandler(){}

Resource::ptr StudioHandler::load(Stream::ptr stream,const ResourceHandlerData *handlerData){
	DataStream::ptr dataStream(new DataStream(stream));
	int id=dataStream->readLittleInt32();
	dataStream->reset();

	if(id!=IDST && id!=IDSQ){
		Error::unknown(String("incorrect studio id:")+id);
		return NULL;
	}

	StudioModel::ptr model(new StudioModel());

	int length=dataStream->length();
	model->data=new tbyte[length];

	dataStream->read(model->data,length);

	model->header=(studiohdr*)model->data;
	if(model->header->textureindex>0 && model->header->numtextures<=MAXSTUDIOSKINS){
		studiotexture *ptexture=(studiotexture*)(model->data+model->header->textureindex);
		int i;
		for(i=0;i<model->header->numtextures;i++){
			model->textures.add(createTexture(&ptexture[i],model->data+ptexture[i].index,model->data+ptexture[i].width*ptexture[i].height+ptexture[i].index));
		}
	}

	return model;
}

Texture::ptr StudioHandler::createTexture(studiotexture *ptexture,tbyte *data,tbyte *pal){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_RGB_8,ptexture->width,ptexture->height));
	tbyte *imageData=image->getData();

	int i,j;
	for(j=0;j<ptexture->height;++j){
		for(i=0;i<ptexture->width;++i){
			tbyte *c=pal+data[(j*ptexture->width+i)*3];
				
			imageData[(j*ptexture->height+i)*3+0]=c[0];
			imageData[(j*ptexture->height+i)*3+1]=c[1];
			imageData[(j*ptexture->height+i)*3+2]=c[2];
		}
	}
	
	Texture::ptr texture=mEngine->getTextureManager()->createTexture(image);
	texture->setName(ptexture->name);
	return texture;
}

}
}
}
