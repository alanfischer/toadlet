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

#include <toadlet/egg/io/DataStream.h>
#include <toadlet/tadpole/studio/StudioStreamer.h>

namespace toadlet{
namespace tadpole{
namespace studio{

StudioStreamer::StudioStreamer(Engine *engine){
	mEngine=engine;
}

StudioStreamer::~StudioStreamer(){}

Resource::ptr StudioStreamer::load(Stream::ptr stream,ResourceData *data,ProgressListener *listener){
	if(stream==NULL){
		Error::nullPointer(Categories::TOADLET_TADPOLE_STUDIO,
			"null stream");
		return NULL;
	}

	DataStream::ptr dataStream=new DataStream(stream);
	int id=dataStream->readLInt32();
	dataStream->reset();

	if(id!=IDST && id!=IDSQ){
		Error::unknown(Categories::TOADLET_TADPOLE_STUDIO,
			String("incorrect studio id:")+id);
		return NULL;
	}

	StudioModel::ptr model=new StudioModel();

	int length=dataStream->length();
	model->data=new tbyte[length];

	dataStream->read(model->data,length);

	model->header=(studiohdr*)model->data;
	buildBuffers(model);
	buildTextures(model);
	buildMaterials(model);

	return model;
}

void StudioStreamer::buildBuffers(StudioModel *model){
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
				while((l=*(tricmds++))!=0){
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

				model->meshdatas[meshCount].vertexStart=vertexCount;

				short *tricmds=(short*)(model->data+smesh->triindex);
				while((l=*(tricmds++))!=0){
					IndexData::ptr indexData;
					/// @todo: Pack all these indexes into 1 IndexBuffer to speed up rendering
					if(l>0){
						indexData=new IndexData(IndexData::Primitive_TRISTRIP,NULL,vertexCount,l);
					}
					else{
						l=-l;
						int numedges=l;
						int firstedge=vertexCount;
						if(mEngine->getRenderCaps().triangleFan){
							indexData=new IndexData(IndexData::Primitive_TRIFAN,NULL,firstedge,numedges);
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
							indexData=new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,indexes);
						}
					}

					model->meshdatas[meshCount].indexDatas.add(indexData);

					for(;l>0;l--,tricmds+=4){
						// Setting these manually, they dont appear to be set in the mdl
						if((stexture->flags&STUDIO_NF_CHROME)!=0){
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

void StudioStreamer::buildTextures(StudioModel *model){
	if(model->header->textureindex>0 && model->header->numtextures<=MAXSTUDIOSKINS){
		int i;
		for(i=0;i<model->header->numtextures;i++){
			studiotexture *stexture=model->texture(i);
			model->textures.add(createTexture(stexture,model->data+stexture->index,model->data+stexture->index+stexture->width*stexture->height));
		}
	}
}

void StudioStreamer::buildMaterials(StudioModel *model){
	model->materials.resize(model->header->numtextures);
	int i;
	for(i=0;i<model->header->numtextures;i++){
		studiotexture *stexture=model->texture(i);

		RenderState::ptr renderState=mEngine->getMaterialManager()->createRenderState();
		renderState->setRasterizerState(RasterizerState(RasterizerState::CullType_FRONT));
		renderState->setMaterialState(MaterialState(true,false,(stexture->flags&STUDIO_NF_FLATSHADE)!=0?MaterialState::ShadeType_FLAT:MaterialState::ShadeType_GOURAUD));

		model->materials[i]=mEngine->createDiffuseMaterial(model->textures[i],renderState);
		model->materials[i]->setName(stexture->name);
	}
}

Texture::ptr StudioStreamer::createTexture(studiotexture *t,tbyte *data,tbyte *pal){
	TextureFormat::ptr textureFormat=new TextureFormat(TextureFormat::Dimension_D2,TextureFormat::Format_RGB_8,t->width,t->height,1,0);
	tbyte *textureData=new tbyte[textureFormat->getDataSize()];

	int i,j;
	for(j=0;j<t->height;++j){
		for(i=0;i<t->width;++i){
			tbyte *d=textureData+(j*t->width+i)*3;
			tbyte *c=pal+data[j*t->width+i]*3;
				
			d[0]=c[0];
			d[1]=c[1];
			d[2]=c[2];
		}
	}

	Texture::ptr texture=mEngine->getTextureManager()->createTexture(textureFormat,textureData);
	texture->setName(t->name);

	delete[] textureData;

	return texture;
}

}
}
}
