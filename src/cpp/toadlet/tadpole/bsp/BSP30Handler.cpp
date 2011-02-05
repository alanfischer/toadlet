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
#include <toadlet/tadpole/bsp/BSP30Handler.h>
#include <toadlet/tadpole/handler/WADArchive.h>
#include <stdlib.h>
#include <string.h> // memset

using namespace toadlet::egg;
using namespace toadlet::egg::io;
using namespace toadlet::egg::image;
using namespace toadlet::peeper;
using namespace toadlet::tadpole::handler;

namespace toadlet{
namespace tadpole{
namespace bsp{

extern tbyte Quake1Palette[];

BSP30Handler::BSP30Handler(Engine *engine):ResourceHandler(),
	mEngine(NULL)
{
	mEngine=engine;
}

BSP30Handler::~BSP30Handler(){}

Resource::ptr BSP30Handler::load(Stream::ptr stream,const ResourceHandlerData *handlerData){
	DataStream::ptr dataStream(new DataStream(stream));
	int version=dataStream->readLittleInt32();
	dataStream->reset();

	// Q1 and HL do not have magic number, unlike Q2+ and HL2
	if(version!=Q1BSPVERSION && version!=HLBSPVERSION){
		Error::unknown(Categories::TOADLET_TADPOLE_BSP,
			String("incorrect bsp version:")+version);
		return NULL;
	}

	Logger::debug(Categories::TOADLET_TADPOLE,"Reading map");

	BSP30Map::ptr map(new BSP30Map());

	stream->read((tbyte*)&map->header,sizeof(map->header));

	readLump(stream,&map->header.lumps[LUMP_MODELS],		(void**)&map->models,sizeof(bmodel),			&map->nmodels);
	readLump(stream,&map->header.lumps[LUMP_VERTEXES],		(void**)&map->vertexes,sizeof(bvertex),			&map->nvertexes);
	readLump(stream,&map->header.lumps[LUMP_PLANES],		(void**)&map->planes,sizeof(bplane),			&map->nplanes);
	readLump(stream,&map->header.lumps[LUMP_LEAFS],			(void**)&map->leafs,sizeof(bleaf),				&map->nleafs);
	readLump(stream,&map->header.lumps[LUMP_NODES],			(void**)&map->nodes,sizeof(bnode),				&map->nnodes);
	readLump(stream,&map->header.lumps[LUMP_TEXINFO],		(void**)&map->texinfos,sizeof(btexinfo),		&map->ntexinfos);
	readLump(stream,&map->header.lumps[LUMP_CLIPNODES],		(void**)&map->clipnodes,sizeof(bclipnode),		&map->nclipnodes);
	readLump(stream,&map->header.lumps[LUMP_FACES],			(void**)&map->faces,sizeof(bface),				&map->nfaces);
	readLump(stream,&map->header.lumps[LUMP_MARKSURFACES],	(void**)&map->marksurfaces,sizeof(bmarksurface),&map->nmarksurfaces);
	readLump(stream,&map->header.lumps[LUMP_SURFEDGES],		(void**)&map->surfedges,sizeof(bsurfedge),		&map->nsurfedges);
	readLump(stream,&map->header.lumps[LUMP_EDGES],			(void**)&map->edges,sizeof(bedge),				&map->nedges);
	readLump(stream,&map->header.lumps[LUMP_VISIBILITY],	(void**)&map->visibility,1,						&map->nvisibility);
	readLump(stream,&map->header.lumps[LUMP_TEXTURES],		(void**)&map->textures,1,						&map->ntextures);
	readLump(stream,&map->header.lumps[LUMP_LIGHTING],		(void**)&map->lighting,1,						&map->nlighting);
	readLump(stream,&map->header.lumps[LUMP_ENTITIES],		(void**)&map->entities,1,						&map->nentities);

	map->miptexlump=(bmiptexlump*)map->textures;

	parseVisibility(map);
	parseEntities(map);
	parseWADs(map);
	parseTextures(map);
	buildBuffers(map);
	buildMaterials(map);

	Logger::debug(Categories::TOADLET_TADPOLE,"Reading map finished");

	return map;
}

void BSP30Handler::readLump(Stream *stream,blump *lump,void **data,int size,int *count){
	int length=lump->filelen;
	int ofs=lump->fileofs;

	stream->seek(ofs);
	*data=malloc(length);
	stream->read((tbyte*)*data,length);

	if(count!=NULL){
		*count=length/size;
	}
}

void BSP30Handler::parseVisibility(BSP30Map *map){
	Logger::debug(Categories::TOADLET_TADPOLE,"Parsing visibility");

	int count;
	int i,c,index;
	unsigned char bit;

	if(map->nvisibility==0){
		return; // No vis data to decompress
	}

	map->parsedVisibility.resize(map->nleafs);

	// First allocate space for each leaf
	for(i=0;i<map->nleafs;i++){
		int v=map->leafs[i].visofs;
		if(v>=0){
			count=0;

			// We enumerate through all possible leafs
			for(c=1;c<map->models[0].visleafs;v++){
				// If the whole byte is zero, that means the nextvis*8 leafs are invisible, so skip them
				if(((tbyte*)map->visibility)[v]==0){
					v++;
					c+=(((tbyte*)map->visibility)[v]<<3);
				}
				// Otherwise, we need to check each bit to see if that leaf is visible
				else{
					for(bit=1;bit;bit<<=1,c++){
						if(((tbyte*)map->visibility)[v]&bit){
							count++;
						}
					}
				}
			}

			map->parsedVisibility[i].resize(count);
		}
	}

	// Now actually store the leaf vis info
	for(i=0;i<map->nleafs;i++){
		int v=map->leafs[i].visofs;
		if(v>=0){
			index=0;

			// We enumerate through all possible leafs
			for(c=1;c<map->models[0].visleafs;v++){
				// If the whole byte is zero, that means the nextvis*8 leafs are invisible, so skip them
				if(((tbyte*)map->visibility)[v]==0){
					v++;
					c+=(((tbyte*)map->visibility)[v]<<3);
				}
				// Otherwise, we need to check each bit to see if that leaf is visible
				else{
					for(bit=1;bit;bit<<=1,c++){
						if(((tbyte*)map->visibility)[v]&bit){
							map->parsedVisibility[i][index]=c;
							index++;
						}
					}
				}
			}
		}
	}
}

void BSP30Handler::parseEntities(BSP30Map *map){
	Logger::debug(Categories::TOADLET_TADPOLE,"Parsing entities");

	char *data=map->entities;
	Map<String,String> keyValues;

	while(*data!=0){
		if(*data=='{'){
			keyValues.clear();
		}
		else if(*data=='}'){
			map->parsedEntities.add(keyValues);
		}
		else if(*data=='\"'){
			char *key=++data;
			while(*data!='\"') data++;
			*data=0;

			while(*data!='\"') data++;

			char *value=++data;
			while(*data!='\"') data++;
			*data=0;

			keyValues[key]=value;
		}
		data++;
	}
}

void BSP30Handler::parseWADs(BSP30Map *map){
	Logger::debug(Categories::TOADLET_TADPOLE,"Parsing WADs");

	if(map->parsedEntities.size()>0 && map->parsedEntities[0]["classname"].equals("worldspawn")){
		String wad=map->parsedEntities[0]["wad"];
		int start=0,end=0;
		while((end=wad.find(".wad",end+1))!=-1){
			start=Math::maxVal(wad.rfind('\\',end)+1,wad.rfind(';',end)+1);
			String file=wad.substr(start,(end-start)+4);
			Archive::ptr archive=mEngine->getArchiveManager()->findArchive(file);
			if(archive!=NULL){
				mEngine->getTextureManager()->addResourceArchive(archive);
			}
		}
	}
}

void BSP30Handler::parseTextures(BSP30Map *map){
	Logger::debug(Categories::TOADLET_TADPOLE,"Parsing textures");

	map->parsedTextures.resize(map->miptexlump->nummiptex);
	int i;
	for(i=0;i<map->miptexlump->nummiptex;i++){
		Texture::ptr texture;
		int miptexofs=map->miptexlump->dataofs[i];
		if(miptexofs!=-1){
			WADArchive::wmiptex *miptex=(WADArchive::wmiptex*)(&((tbyte*)map->textures)[miptexofs]);
			texture=WADArchive::createTexture(mEngine->getTextureManager(),miptex,map->header.version==Q1BSPVERSION?Quake1Palette:NULL);
			if(texture==NULL){
				texture=mEngine->getTextureManager()->findTexture(miptex->name);
			}
			if(texture!=NULL){
				texture->retain();
			}
		}
		map->parsedTextures[i]=texture;
	}
}

void BSP30Handler::buildBuffers(BSP30Map *map){
	Logger::debug(Categories::TOADLET_TADPOLE,"Building buffers and lightmaps");

	int i,j;

	VertexFormat::ptr vertexFormat=mEngine->getBufferManager()->createVertexFormat();
	vertexFormat->addElement(VertexFormat::Semantic_POSITION,0,VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_3);
	vertexFormat->addElement(VertexFormat::Semantic_NORMAL,0,VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_3);
	vertexFormat->addElement(VertexFormat::Semantic_TEX_COORD,0,VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_2);
	vertexFormat->addElement(VertexFormat::Semantic_TEX_COORD,1,VertexFormat::Format_BIT_FLOAT_32|VertexFormat::Format_BIT_COUNT_2);
	VertexBuffer::ptr vertexBuffer=mEngine->getBufferManager()->createVertexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,vertexFormat,map->nsurfedges);

	int width=0,height=0;
	float iwidth=0,iheight=0;
	float s=0,t=0,ls=0,lt=0;
	int surfmins[2],surfmaxs[2];
	int lmwidth=0,lmheight=0;
	int lightmapCoord[2];

	map->facedatas.resize(map->nfaces);
	VertexBufferAccessor vba;
	IndexBufferAccessor iba;
	vba.lock(vertexBuffer,Buffer::Access_BIT_WRITE);
	for(i=0;i<map->nfaces;i++){
		bface *face=&map->faces[i];
		BSP30Map::facedata *faced=&map->facedatas[i];

		faced->index=i;

		// Find texture width & height for this face.
		// We have to use the information from the actual BSP here, instead of the found texture,
		//  since the found texture may be of different dimensions than what was used when the BSP was compiled.
		btexinfo *texinfo=&map->texinfos[face->texinfo];
		int miptexofs=map->miptexlump->dataofs[texinfo->miptex];
		if(miptexofs!=-1){
			WADArchive::wmiptex *miptex=(WADArchive::wmiptex*)(&((tbyte*)map->textures)[miptexofs]);
			width=miptex->width;
			height=miptex->height;
			if(width>0 && height>0){
				iwidth=1.0f/(float)width;
				iheight=1.0f/(float)height;
			}
		}

		if((texinfo->flags&TEX_SPECIAL)==0){
			map->findSurfaceExtents(face,surfmins,surfmaxs);
			lmwidth=((surfmaxs[0]-surfmins[0])>>4)+1;
			lmheight=((surfmaxs[1]-surfmins[1])>>4)+1;

			if((faced->lightmapIndex=map->allocLightmap(lightmapCoord,lmwidth,lmheight))<0){
				map->uploadLightmap(mEngine->getTextureManager());
				map->initLightmap();
				if((faced->lightmapIndex=map->allocLightmap(lightmapCoord,lmwidth,lmheight))<0){
					Error::unknown("unable to allocate lightmap");
					return;
				}
			}

			tbyte *dst=map->lightmapImage->getData();
			int pixelSize=map->lightmapImage->getPixelSize();
			tbyte *src=(tbyte*)map->lighting + face->lightofs;
			for(j=0;j<lmheight;++j){
				memcpy(dst + ((lightmapCoord[1]+j)*BSP30Map::LIGHTMAP_SIZE + lightmapCoord[0])*pixelSize,src + lmwidth*j*pixelSize,lmwidth*pixelSize);
			}
		}

		/// @todo: Pack all these indexes into 1 IndexBuffer to speed up rendering
		IndexData::ptr indexData;
		if(mEngine->getBufferManager()->useTriFan()){
			indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIFAN,NULL,face->firstedge,face->numedges));
		}
		else{
			int indexes=(face->numedges-2)*3;
			IndexBuffer::ptr indexBuffer=mEngine->getBufferManager()->createIndexBuffer(Buffer::Usage_BIT_STATIC,Buffer::Access_BIT_WRITE,IndexBuffer::IndexFormat_UINT16,indexes);
			iba.lock(indexBuffer,Buffer::Access_BIT_WRITE);
			for(int edge=1;edge<face->numedges-1;++edge){
				iba.set((edge-1)*3+0,face->firstedge);
				iba.set((edge-1)*3+1,face->firstedge+edge);
				iba.set((edge-1)*3+2,face->firstedge+edge+1);
			}
			iba.unlock();
			indexData=IndexData::ptr(new IndexData(IndexData::Primitive_TRIS,indexBuffer,0,indexes));
		}
		faced->indexData=indexData;

		for(j=0;j<face->numedges;j++){
			int faceedge=face->firstedge+j;
			int surfedge=map->surfedges[faceedge];
			bvertex *v=&map->vertexes[surfedge<0?map->edges[-surfedge].v[1]:map->edges[surfedge].v[0]];
			bplane *p=&map->planes[face->planenum];
			int side=-2*face->side+1;

			vba.set3(faceedge,0,v->point);

			// Flip the face normal around if necessary
			vba.set3(faceedge,1,side*p->normal[0],side*p->normal[1],side*p->normal[2]);

			// Project the texture using the projection vectors to find our texture coordinates
			s=Math::dot((Vector3)v->point,texinfo->vecs[0]) + texinfo->vecs[0][3];
			t=Math::dot((Vector3)v->point,texinfo->vecs[1]) + texinfo->vecs[1][3];
			// The texture projection information is not normalized when stored.  So normalize it here.
			vba.set2(faceedge,2,s*iwidth,t*iheight);

			// Calculate lightmap coordinates
			if((texinfo->flags&TEX_SPECIAL)==0){
				ls=s-surfmins[0] + lightmapCoord[0]*16 + 8;
				ls/=BSP30Map::LIGHTMAP_SIZE*16;

				lt=t-surfmins[1] + lightmapCoord[1]*16 + 8;
				lt/=BSP30Map::LIGHTMAP_SIZE*16;

				vba.set2(faceedge,3,ls,lt);
			}
		}
	}
	vba.unlock();

	map->uploadLightmap(mEngine->getTextureManager());

	map->vertexData=VertexData::ptr(new VertexData(vertexBuffer));
}

void BSP30Handler::buildMaterials(BSP30Map *map){
	Logger::debug(Categories::TOADLET_TADPOLE,"Building materials");

	map->materials.resize(map->miptexlump->nummiptex);
	int i;
	for(i=0;i<map->miptexlump->nummiptex;i++){
		Material::ptr material=mEngine->getMaterialManager()->createMaterial();
		material->retain();
		material->setLighting(false);
		material->setFaceCulling(Renderer::FaceCulling_FRONT);
		material->setDepthWrite(true);

		TextureStage::ptr primary=mEngine->getMaterialManager()->createTextureStage(map->parsedTextures[i]);
		material->setTextureStage(0,primary);

		map->materials[i]=material;
	}
}

tbyte Quake1Palette[]={
	0x0,0x0,0x0,
	0xf,0xf,0xf,
	0x1f,0x1f,0x1f,
	0x2f,0x2f,0x2f,
	0x3f,0x3f,0x3f,
	0x4b,0x4b,0x4b,
	0x5b,0x5b,0x5b,
	0x6b,0x6b,0x6b,
	0x7b,0x7b,0x7b,
	0x8b,0x8b,0x8b,
	0x9b,0x9b,0x9b,
	0xab,0xab,0xab,
	0xbb,0xbb,0xbb,
	0xcb,0xcb,0xcb,
	0xdb,0xdb,0xdb,
	0xeb,0xeb,0xeb,
	0xf,0xb,0x7,
	0x17,0xf,0xb,
	0x1f,0x17,0xb,
	0x27,0x1b,0xf,
	0x2f,0x23,0x13,
	0x37,0x2b,0x17,
	0x3f,0x2f,0x17,
	0x4b,0x37,0x1b,
	0x53,0x3b,0x1b,
	0x5b,0x43,0x1f,
	0x63,0x4b,0x1f,
	0x6b,0x53,0x1f,
	0x73,0x57,0x1f,
	0x7b,0x5f,0x23,
	0x83,0x67,0x23,
	0x8f,0x6f,0x23,
	0xb,0xb,0xf,
	0x13,0x13,0x1b,
	0x1b,0x1b,0x27,
	0x27,0x27,0x33,
	0x2f,0x2f,0x3f,
	0x37,0x37,0x4b,
	0x3f,0x3f,0x57,
	0x47,0x47,0x67,
	0x4f,0x4f,0x73,
	0x5b,0x5b,0x7f,
	0x63,0x63,0x8b,
	0x6b,0x6b,0x97,
	0x73,0x73,0xa3,
	0x7b,0x7b,0xaf,
	0x83,0x83,0xbb,
	0x8b,0x8b,0xcb,
	0x0,0x0,0x0,
	0x7,0x7,0x0,
	0xb,0xb,0x0,
	0x13,0x13,0x0,
	0x1b,0x1b,0x0,
	0x23,0x23,0x0,
	0x2b,0x2b,0x7,
	0x2f,0x2f,0x7,
	0x37,0x37,0x7,
	0x3f,0x3f,0x7,
	0x47,0x47,0x7,
	0x4b,0x4b,0xb,
	0x53,0x53,0xb,
	0x5b,0x5b,0xb,
	0x63,0x63,0xb,
	0x6b,0x6b,0xf,
	0x7,0x0,0x0,
	0xf,0x0,0x0,
	0x17,0x0,0x0,
	0x1f,0x0,0x0,
	0x27,0x0,0x0,
	0x2f,0x0,0x0,
	0x37,0x0,0x0,
	0x3f,0x0,0x0,
	0x47,0x0,0x0,
	0x4f,0x0,0x0,
	0x57,0x0,0x0,
	0x5f,0x0,0x0,
	0x67,0x0,0x0,
	0x6f,0x0,0x0,
	0x77,0x0,0x0,
	0x7f,0x0,0x0,
	0x13,0x13,0x0,
	0x1b,0x1b,0x0,
	0x23,0x23,0x0,
	0x2f,0x2b,0x0,
	0x37,0x2f,0x0,
	0x43,0x37,0x0,
	0x4b,0x3b,0x7,
	0x57,0x43,0x7,
	0x5f,0x47,0x7,
	0x6b,0x4b,0xb,
	0x77,0x53,0xf,
	0x83,0x57,0x13,
	0x8b,0x5b,0x13,
	0x97,0x5f,0x1b,
	0xa3,0x63,0x1f,
	0xaf,0x67,0x23,
	0x23,0x13,0x7,
	0x2f,0x17,0xb,
	0x3b,0x1f,0xf,
	0x4b,0x23,0x13,
	0x57,0x2b,0x17,
	0x63,0x2f,0x1f,
	0x73,0x37,0x23,
	0x7f,0x3b,0x2b,
	0x8f,0x43,0x33,
	0x9f,0x4f,0x33,
	0xaf,0x63,0x2f,
	0xbf,0x77,0x2f,
	0xcf,0x8f,0x2b,
	0xdf,0xab,0x27,
	0xef,0xcb,0x1f,
	0xff,0xf3,0x1b,
	0xb,0x7,0x0,
	0x1b,0x13,0x0,
	0x2b,0x23,0xf,
	0x37,0x2b,0x13,
	0x47,0x33,0x1b,
	0x53,0x37,0x23,
	0x63,0x3f,0x2b,
	0x6f,0x47,0x33,
	0x7f,0x53,0x3f,
	0x8b,0x5f,0x47,
	0x9b,0x6b,0x53,
	0xa7,0x7b,0x5f,
	0xb7,0x87,0x6b,
	0xc3,0x93,0x7b,
	0xd3,0xa3,0x8b,
	0xe3,0xb3,0x97,
	0xab,0x8b,0xa3,
	0x9f,0x7f,0x97,
	0x93,0x73,0x87,
	0x8b,0x67,0x7b,
	0x7f,0x5b,0x6f,
	0x77,0x53,0x63,
	0x6b,0x4b,0x57,
	0x5f,0x3f,0x4b,
	0x57,0x37,0x43,
	0x4b,0x2f,0x37,
	0x43,0x27,0x2f,
	0x37,0x1f,0x23,
	0x2b,0x17,0x1b,
	0x23,0x13,0x13,
	0x17,0xb,0xb,
	0xf,0x7,0x7,
	0xbb,0x73,0x9f,
	0xaf,0x6b,0x8f,
	0xa3,0x5f,0x83,
	0x97,0x57,0x77,
	0x8b,0x4f,0x6b,
	0x7f,0x4b,0x5f,
	0x73,0x43,0x53,
	0x6b,0x3b,0x4b,
	0x5f,0x33,0x3f,
	0x53,0x2b,0x37,
	0x47,0x23,0x2b,
	0x3b,0x1f,0x23,
	0x2f,0x17,0x1b,
	0x23,0x13,0x13,
	0x17,0xb,0xb,
	0xf,0x7,0x7,
	0xdb,0xc3,0xbb,
	0xcb,0xb3,0xa7,
	0xbf,0xa3,0x9b,
	0xaf,0x97,0x8b,
	0xa3,0x87,0x7b,
	0x97,0x7b,0x6f,
	0x87,0x6f,0x5f,
	0x7b,0x63,0x53,
	0x6b,0x57,0x47,
	0x5f,0x4b,0x3b,
	0x53,0x3f,0x33,
	0x43,0x33,0x27,
	0x37,0x2b,0x1f,
	0x27,0x1f,0x17,
	0x1b,0x13,0xf,
	0xf,0xb,0x7,
	0x6f,0x83,0x7b,
	0x67,0x7b,0x6f,
	0x5f,0x73,0x67,
	0x57,0x6b,0x5f,
	0x4f,0x63,0x57,
	0x47,0x5b,0x4f,
	0x3f,0x53,0x47,
	0x37,0x4b,0x3f,
	0x2f,0x43,0x37,
	0x2b,0x3b,0x2f,
	0x23,0x33,0x27,
	0x1f,0x2b,0x1f,
	0x17,0x23,0x17,
	0xf,0x1b,0x13,
	0xb,0x13,0xb,
	0x7,0xb,0x7,
	0xff,0xf3,0x1b,
	0xef,0xdf,0x17,
	0xdb,0xcb,0x13,
	0xcb,0xb7,0xf,
	0xbb,0xa7,0xf,
	0xab,0x97,0xb,
	0x9b,0x83,0x7,
	0x8b,0x73,0x7,
	0x7b,0x63,0x7,
	0x6b,0x53,0x0,
	0x5b,0x47,0x0,
	0x4b,0x37,0x0,
	0x3b,0x2b,0x0,
	0x2b,0x1f,0x0,
	0x1b,0xf,0x0,
	0xb,0x7,0x0,
	0x0,0x0,0xff,
	0xb,0xb,0xef,
	0x13,0x13,0xdf,
	0x1b,0x1b,0xcf,
	0x23,0x23,0xbf,
	0x2b,0x2b,0xaf,
	0x2f,0x2f,0x9f,
	0x2f,0x2f,0x8f,
	0x2f,0x2f,0x7f,
	0x2f,0x2f,0x6f,
	0x2f,0x2f,0x5f,
	0x2b,0x2b,0x4f,
	0x23,0x23,0x3f,
	0x1b,0x1b,0x2f,
	0x13,0x13,0x1f,
	0xb,0xb,0xf,
	0x2b,0x0,0x0,
	0x3b,0x0,0x0,
	0x4b,0x7,0x0,
	0x5f,0x7,0x0,
	0x6f,0xf,0x0,
	0x7f,0x17,0x7,
	0x93,0x1f,0x7,
	0xa3,0x27,0xb,
	0xb7,0x33,0xf,
	0xc3,0x4b,0x1b,
	0xcf,0x63,0x2b,
	0xdb,0x7f,0x3b,
	0xe3,0x97,0x4f,
	0xe7,0xab,0x5f,
	0xef,0xbf,0x77,
	0xf7,0xd3,0x8b,
	0xa7,0x7b,0x3b,
	0xb7,0x9b,0x37,
	0xc7,0xc3,0x37,
	0xe7,0xe3,0x57,
	0x7f,0xbf,0xff,
	0xab,0xe7,0xff,
	0xd7,0xff,0xff,
	0x67,0x0,0x0,
	0x8b,0x0,0x0,
	0xb3,0x0,0x0,
	0xd7,0x0,0x0,
	0xff,0x0,0x0,
	0xff,0xf3,0x93,
	0xff,0xf7,0xc7,
	0xff,0xff,0xff,
	0x9f,0x5b,0x53,
};

}
}
}

