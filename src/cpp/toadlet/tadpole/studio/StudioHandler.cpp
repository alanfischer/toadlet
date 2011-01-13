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
#include <toadlet/peeper/CapabilitySet.h>
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
	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE_STUDIO,
			"null stream");
		return NULL;
	}

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

	IndexBufferAccessor iba;
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
	model->vertexCount=vertexCount;

	meshCount=0;
	vertexCount=0;
	for(i=0;i<model->header->numbodyparts;++i){
		studiobodyparts *sbodyparts=model->bodyparts(i);
		for(j=0;j<sbodyparts->nummodels;++j){
			studiomodel *smodel=model->model(sbodyparts,j);
			tbyte *snormbone=((tbyte*)model->data+smodel->norminfoindex);
			for(k=0;k<smodel->nummesh;++k){
				studiomesh *smesh=model->mesh(smodel,k);

				short sskin=model->skin(smesh->skinref);
				studiotexture *stexture=model->texture(sskin);

				short *tricmds=(short*)(model->data+smesh->triindex);
				while(l=*(tricmds++)){
					IndexData::ptr indexData;
					if(l>0){
						indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRISTRIP,NULL,vertexCount,l));
					}
					else{
						l=-l;
						int numedges=l;
						int firstedge=vertexCount;
						if(mEngine->getRenderer()==NULL || mEngine->getRenderer()->getCapabilitySet().triangleFan){
							indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIFAN,NULL,firstedge,numedges));
						}
						else{
							int indexes=(numedges-2)*3;
							IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,indexes);
							iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);
							for(int edge=1;edge<numedges-1;++edge){
								iba.set((edge-1)*3+0,firstedge);
								iba.set((edge-1)*3+1,firstedge+edge);
								iba.set((edge-1)*3+2,firstedge+edge+1);
							}
							iba.unlock();
							indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,indexes));
						}
					}
					model->meshdatas[meshCount].indexDatas.add(indexData);

					for(;l>0;l--,tricmds+=4){
						// Setting these manually, they dont appear to be set in the mdl
						if((stexture->flags&STUDIO_NF_CHROME)>0){
							model->bone(snormbone[tricmds[1]])->flags|=STUDIO_HAS_CHROME;
						}

						vertexCount++;
					}
				}

				meshCount++;
			}
		}
	}
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
		material->setLighting(true);
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
