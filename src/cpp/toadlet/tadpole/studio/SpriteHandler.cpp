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
#include <toadlet/tadpole/studio/SpriteHandler.h>

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;

namespace toadlet{
namespace tadpole{
namespace studio{

SpriteHandler::SpriteHandler(Engine *engine){
	mEngine=engine;
}

SpriteHandler::~SpriteHandler(){}

Resource::ptr SpriteHandler::load(Stream::ptr stream,const ResourceHandlerData *handlerData){
	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE_STUDIO,
			"null stream");
		return NULL;
	}

	DataStream::ptr dataStream(new DataStream(stream));
	int ident=dataStream->readLInt32();
	int version=dataStream->readLInt32();
	dataStream->reset();

	if(ident!=IDSP || version!=SPRITE_VERSION){
		Error::unknown(Categories::TOADLET_TADPOLE_STUDIO,
			String("incorrect sprite ident:")+ident+" or version:"+version);
		return NULL;
	}

	SpriteModel::ptr model(new SpriteModel());

	int length=dataStream->length();
	model->data=new tbyte[length];

	dataStream->read(model->data,length);

	model->header=(spritehdr*)model->data;
	model->paletteSize=(*(uint16*)(model->data+sizeof(spritehdr)));
	model->palette=(model->data+sizeof(spritehdr)+2);

	buildTextures(model);
	buildMaterials(model);

	return model;
}

void SpriteHandler::buildTextures(SpriteModel *model){
	int i;
	for(i=0;i<model->header->numframes;++i){
		spriteframe *frame=model->frame(i);
		model->textures.add(createTexture(frame,((tbyte*)frame)+sizeof(spriteframe),model->palette));
	}
}

void SpriteHandler::buildMaterials(SpriteModel *model){
	model->materials.resize(model->header->numframes);
	int i;
	for(i=0;i<model->header->numframes;i++){
		Material::ptr material=mEngine->getMaterialManager()->createMaterial(model->textures[i],false,model->materials.size()>0?model->materials[0]:NULL);
		material->retain();
		model->materials[i]=material;
	}
}

Texture::ptr SpriteHandler::createTexture(spriteframe *f,tbyte *data,tbyte *pal){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_RGBA_8,f->width,f->height));
	tbyte *imageData=image->getData();

	int i,j;
	for(j=0;j<f->height;++j){
		for(i=0;i<f->width;++i){
			tbyte *d=imageData+(j*f->width+i)*4;
			tbyte s=data[(f->height-j-1)*f->width+i];
			tbyte *c=pal+s*3;

			d[0]=c[0];
			d[1]=c[1];
			d[2]=c[2];
			d[3]=(s==255)?0:255;
		}
	}
	
	return mEngine->getTextureManager()->createTexture(image);
}

}
}
}