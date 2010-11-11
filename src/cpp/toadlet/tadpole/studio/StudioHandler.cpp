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
		Error::unknown(Categories::TOADLET_TADPOLE_STUDIO,
			String("incorrect studio id:")+id);
		return NULL;
	}

	StudioModel::ptr model(new StudioModel());

	int length=dataStream->length();
	model->data=new tbyte[length];

	dataStream->read(model->data,length);

	model->header=(studiohdr*)model->data;

	buildBuffers(model);
	buildTextures(model);
	buildMaterials(model);

	return model;
}

void StudioHandler::buildBuffers(StudioModel *model){
	int i,j,k,l;

	int meshCount=0;
	int vertexCount=0;
	for(i=0;i<model->header->numbodyparts;++i){
		studiobodyparts *sbodyparts=model->bodyparts(i);
		for(j=0;j<sbodyparts->nummodels;++j){
			studiomodel *smodel=model->model(sbodyparts,j);
			for(k=0;k<smodel->nummesh;++k){
				studiomesh *smesh=model->mesh(smodel,k);

				short *tricmds=(short*)(model->data+smesh->triindex);
				while(l=*(tricmds++)){
					if(l<0){
						l=-l;
					}
					for(;l>0;l--,tricmds+=4){
						vertexCount++;
					}
				}

				meshCount++;
			}
		}
	}

	model->meshdatas.resize(meshCount);

	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,mEngine->getVertexFormats().POSITION_NORMAL_TEX_COORD,vertexCount);

	VertexBufferAccessor vba;
	vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);

	meshCount=0;
	vertexCount=0;
	for(i=0;i<model->header->numbodyparts;++i){
		studiobodyparts *sbodyparts=model->bodyparts(i);
		for(j=0;j<sbodyparts->nummodels;++j){
			studiomodel *smodel=model->model(sbodyparts,j);
			Vector3 *verts=(Vector3*)(model->data+smodel->vertindex);
			Vector3 *norms=(Vector3*)(model->data+smodel->normindex);
			for(k=0;k<smodel->nummesh;++k){
				studiomesh *smesh=model->mesh(smodel,k);

				short sskin=model->skin(smesh->skinref);
				studiotexture *stexture=model->texture(sskin);
				float s=1.0f/(float)stexture->width;
				float t=1.0f/(float)stexture->height;

				short *tricmds=(short*)(model->data+smesh->triindex);
				while(l=*(tricmds++)){
					IndexData::Primitive primitive=IndexData::Primitive_TRISTRIP;
					if(l<0){
						l=-l;
						primitive=IndexData::Primitive_TRIFAN;
					}

					IndexData::ptr indexData(new IndexData(primitive,NULL,vertexCount,l));
					model->meshdatas[meshCount].indexdatas.add(indexData);

					for(;l>0;l--,tricmds+=4){
						vba.set3(vertexCount,0,verts[tricmds[0]]);
						vba.set3(vertexCount,1,norms[tricmds[1]]);
						vba.set2(vertexCount,2,tricmds[2]*s,tricmds[3]*t);
						vertexCount++;
					}
				}

				meshCount++;
			}
		}
	}

	vba.unlock();

	model->vertexdata=VertexData::ptr(new VertexData(vertexBuffer));
}

void StudioHandler::buildTextures(StudioModel *model){
	if(model->header->textureindex>0 && model->header->numtextures<=MAXSTUDIOSKINS){
		int i;
		for(i=0;i<model->header->numtextures;i++){
			studiotexture *stexture=model->texture(i);
			model->textures.add(createTexture(stexture,model->data+stexture->index,model->data+stexture->index+stexture->width*stexture->height));
		}
	}
}

void StudioHandler::buildMaterials(StudioModel *model){
	model->materials.resize(model->header->numtextures);
	int i;
	for(i=0;i<model->header->numtextures;i++){
		studiotexture *stexture=model->texture(i);

		Material::ptr material=mEngine->getMaterialManager()->createMaterial();
		material->retain();
		material->setLighting(false);
		material->setFaceCulling(Renderer::FaceCulling_FRONT);
		material->setDepthWrite(true);
		if((stexture->flags&STUDIO_NF_FLATSHADE)>0){
			material->setShading(Renderer::Shading_FLAT);
		}

		TextureStage::ptr primary=mEngine->getMaterialManager()->createTextureStage(model->textures[i]);
		material->setTextureStage(0,primary);

		model->materials[i]=material;
	}
}

Texture::ptr StudioHandler::createTexture(studiotexture *ptexture,tbyte *data,tbyte *pal){
	Image::ptr image(Image::createAndReallocate(Image::Dimension_D2,Image::Format_RGB_8,ptexture->width,ptexture->height));
	tbyte *imageData=image->getData();

	int i,j;
	for(j=0;j<ptexture->height;++j){
		for(i=0;i<ptexture->width;++i){
			tbyte *c=pal+data[j*ptexture->width+i]*3;
				
			imageData[(j*ptexture->width+i)*3+0]=c[0];
			imageData[(j*ptexture->width+i)*3+1]=c[1];
			imageData[(j*ptexture->width+i)*3+2]=c[2];
		}
	}
	
	Texture::ptr texture=mEngine->getTextureManager()->createTexture(image);
	texture->setName(ptexture->name);
	return texture;
}

}
}
}
